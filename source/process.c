#include "common.h"
#include "process.h"
#include "process_typedefs_internal.h"
#include "settings.h"
#include <nds.h>
#include <stdio.h>

#define WHITE  0b0111111111111111
#define BLUE   0b0111110000000000
#define YELLOW 0b0000001111111111
#define BLACK  0

#define BLUE_INCREMENT   0b0000010000000000
#define YELLOW_INCREMENT 0b0000000000100001
#define WHITE_INCREMENT (BLUE_INCREMENT | YELLOW_INCREMENT)

// init process status, backdrop color is set black since that's the color in the menus
static ProcessStatus process_status = {
    .current_backdrop_color = BLACK,
    .do_print_time = false,
    .cycling_colors_status = {
        .backdrop_color = BLACK,
        .current_phase = BLACK_TO_BLUE
    }
};

/**
 * Timer handler called every second
 */
void timer_handler() {
    Settings current_settings = getCurrentSettings();
    
    // Tick
    process_status.remaining_seconds--;

    // Reprint screen if time is being printed
    if (process_status.do_print_time) {
        setReprintBottom(true);
        setReprintTop(true);
    }

    // Check if time ran out for current phase
    if (!process_status.remaining_seconds) {
        // Invert screen on phase and set reprint, except if screen off phase has a duration of 0
        if (current_settings.screen_off_duration_mins > 0) {
            process_status.is_screen_on_phase = !process_status.is_screen_on_phase;
            setReprintBottom(true);
            setReprintTop(true);
        }

        // If no more cycles remain, shutdown
        if (!process_status.remaining_cycles)
            systemShutDown();

        // Handle phase switching
        if (process_status.is_screen_on_phase) {

            // Decrease remaining cycles
            process_status.remaining_cycles--;

            // Init timer
            process_status.remaining_seconds = current_settings.screen_on_duration_mins * 60;
        } else {
            // Init timer
            process_status.remaining_seconds = current_settings.screen_off_duration_mins * 60;
        }
    }
}

/**
 * Set the backlight level accounting for console type
 * @param backlight_level backlight level to set
 * @param max_backlight_level max backlight level, used to adjust for console type
 */
void setBacklightAdjusted(
    const unsigned int backlight_level,
    const unsigned int max_backlight_level
) {
    /*
     * https://blocksds.skylyrac.net/libnds/system_8h.html#a9bd93bee5409c05451447034b250959b
     * On DSi, just set the level as is
     * On DS (lite) with backlight control, add 1 to the backlight level, as they are shifted by one (except 0)
     * On DS without backlight control, set to 5 if backlight level is 1
     * This ensures max backlight level works even if the console gets misidentified
     */
    assert(max_backlight_level <= 5);
    assert(backlight_level <= max_backlight_level);
	systemSetBacklightLevel(backlight_level + 5 - max_backlight_level);
}

/**
 * Set backdrop on both screens
 * @param col color to set
 */
void setBackdropBoth(u16 col) {
    setBackdropColor(col);
    setBackdropColorSub(col);
}

/**
 * Handle backlight
 * @param keys_held held keys 
 */
void handleBacklight(
	const u16 keys_held
) {
    /*
     * This monster handles changing backlight levels on the screens and turning them on or off
     * It exists because you definitely don't want to rapidly toggle the LEDs on and off
     * or change their light level; it also avoids setting the backlight level and/or
     * turning the screens off constantly, which is probably for the better even when
     * the backlight level hasn't actually changed
     */
	GeneralStatus general_status = getGeneralStatus();
	Settings current_settings = getCurrentSettings();
    bool has_backlight_changed = 0; //flag
	const bool is_screen_on_phase = process_status.is_screen_on_phase;
	int *current_backlight_bottom = &process_status.current_backlight_bottom,
		*current_backlight_top = &process_status.current_backlight_top,
		*last_backlight_bottom = &process_status.last_backlight_bottom,
		*last_backlight_top = &process_status.last_backlight_top;
	const unsigned int backlight_level = current_settings.backlight_level,
        max_backlight_level = general_status.max_backlight_level;
	const Screens screens = current_settings.screens;

    // Copy current backlights to last
    *last_backlight_bottom = *current_backlight_bottom;
	*last_backlight_top = *current_backlight_top;

    // If in the screen on phase, set both current levels to the one set by the user
    if (is_screen_on_phase) {
        *current_backlight_bottom = backlight_level;
        *current_backlight_top = backlight_level;
    // Otherwise, set both current levels to off (-1)
    } else {
        *current_backlight_bottom = -1;
        *current_backlight_top = -1;
    }

    // If dpad is being held, set both screens to max backlight
    if (keys_held & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT)) {
        *current_backlight_bottom = general_status.max_backlight_level;
        *current_backlight_top = general_status.max_backlight_level;
    }
    // If X is being held (and not dpad) and backlight or LCD is off, set both screens to min
    else if (keys_held & KEY_X && (!is_screen_on_phase || backlight_level == 0)) {
        *current_backlight_bottom = PM_BACKLIGHT_MIN;
        *current_backlight_top = PM_BACKLIGHT_MIN;
    }

    // Set to zero if the corresponding backlight is off, but only if the screen and backlight are on
    if (screens == TOP && *current_backlight_top > PM_BACKLIGHT_OFF)
        *current_backlight_bottom = PM_BACKLIGHT_OFF;
    else if (screens == BOTTOM && *current_backlight_bottom > PM_BACKLIGHT_OFF)
        *current_backlight_top = PM_BACKLIGHT_OFF;

    // Turn on screens if they were off...
    if (*last_backlight_bottom == -1 && *current_backlight_bottom != -1) {
        assert(*last_backlight_top == -1);
        assert(*current_backlight_bottom != -1);

        powerOn(POWER_LCD);
        nocashMessage("Turned ON the screens");
        has_backlight_changed = 1;
    }
    // ...and vice versa
    else if (*last_backlight_bottom != -1 && *current_backlight_bottom == -1) {
        assert(*last_backlight_top != -1);
        assert(*current_backlight_bottom == -1);

        powerOff(POWER_LCD);
        nocashMessage("Turned OFF the screens");
        has_backlight_changed = 1;
    }

    // If both screens have backlight turned on...
    if (*current_backlight_bottom > PM_BACKLIGHT_OFF && *current_backlight_top > PM_BACKLIGHT_OFF) {
        // Assert that levels are equal
        assert(*current_backlight_bottom == *current_backlight_top);

        // If either screen had another level last time, change backlight level
        if (*last_backlight_bottom != *current_backlight_bottom || *last_backlight_top != *current_backlight_top) {
            setBacklightAdjusted(*current_backlight_bottom, max_backlight_level);
            snprintf(debug_buffer, DEBUG_BUFFER_SIZE, "Set backlight to %d", *current_backlight_bottom);
            nocashMessage(debug_buffer);
            has_backlight_changed = 1;
        }
    // ...if both screens have backlight off, turn them off if they weren't last time...
    } else if (
        (*current_backlight_bottom == PM_BACKLIGHT_OFF && *current_backlight_top == PM_BACKLIGHT_OFF) &&
        (*last_backlight_bottom != PM_BACKLIGHT_OFF || *last_backlight_top != PM_BACKLIGHT_OFF)
    ) {
        systemSetBacklightLevel(PM_BACKLIGHT_OFF);
        nocashMessage("Set backlight to 0");
        has_backlight_changed = 1;
    // ...if bottom screen is on and top is off, and if their level is different from last time, set backlight level then turn top off...
    } else if (
        (*current_backlight_bottom > PM_BACKLIGHT_OFF && *current_backlight_top == PM_BACKLIGHT_OFF) &&
        (*current_backlight_bottom != *last_backlight_bottom || *current_backlight_top != *last_backlight_top)
    ) {
        setBacklightAdjusted(*current_backlight_bottom, max_backlight_level);
        powerOff(PM_BACKLIGHT_TOP);
        snprintf(debug_buffer, DEBUG_BUFFER_SIZE, "Set backlight to %d, then turned OFF top screen", *current_backlight_bottom);
        nocashMessage(debug_buffer);
        has_backlight_changed = 1;
    // ...vice versa of the above
    } else if (
        (*current_backlight_top > PM_BACKLIGHT_OFF && *current_backlight_bottom == PM_BACKLIGHT_OFF) &&
        (*current_backlight_bottom != *last_backlight_bottom || *current_backlight_top != *last_backlight_top)
    ) {
        setBacklightAdjusted(*current_backlight_top, max_backlight_level);
        powerOff(PM_BACKLIGHT_BOTTOM);
        snprintf(debug_buffer, DEBUG_BUFFER_SIZE, "Set backlight to %d, then turned OFF bottom screen", *current_backlight_top);
        nocashMessage(debug_buffer);
        has_backlight_changed = 1;
    }

    if (has_backlight_changed) {
        snprintf(
            debug_buffer, DEBUG_BUFFER_SIZE, "Last top %d bot %d; curr top %d bot %d",
            *last_backlight_top, *last_backlight_bottom, *current_backlight_top, *current_backlight_bottom
        );
        nocashMessage(debug_buffer);
    }
}

/**
 * Prints the time and/or screen off warning when the process is running
 * @param p_console console to print on
 */
void printProcess(const PrintConsole *const p_console) {
    // Only print time when X is being held
    if (process_status.do_print_time) {
        // Print phase, remaining time for phase, and remaining cycles
        consoleSetColor(NULL, CONSOLE_LIGHT_MAGENTA);
        printf("Screen %s phase\n\n", process_status.is_screen_on_phase ? "ON" : "OFF");
        printf("Remaining time for phase:\n");
        printf(
            "%d:%02d:%02d\n\n", 
            process_status.remaining_seconds / 60 / 60, 
            process_status.remaining_seconds / 60 % 60, 
            process_status.remaining_seconds % 60
        );
        printf("Remaining cycles: %d\n\n", process_status.remaining_cycles);
        if (process_status.remaining_cycles == 0) {
            printf("This is the last cycle, the\n");
            printf("console will power off after\n");
            printf("the time has elapsed\n");
        }
    }

    // Print warning if in screen off phase (always printed, as it wouldn't be seen if screen is off)
    if (!process_status.is_screen_on_phase) {
        consoleSetCursor(NULL, 0, p_console->consoleHeight - 3);
        consoleSetColor(NULL, CONSOLE_LIGHT_RED);
        printf("Avoid turning the screen on\n");
        printf("repeatedly and/or rapidly while\n");
        printf("in the screen off phase");
    }
    consoleSetColor(NULL, CONSOLE_DEFAULT);
}

/**
 * Handle input when process is running
 * @param keys_held held keys
 * @param keys_down pressed keys
 * @param keys_up released keys
 */
void handleProcessInput(
    const u16 keys_held,
    const u16 keys_down,
    const u16 keys_up
) {
    u16 *current_backdrop_color = &process_status.current_backdrop_color;
    process_status.last_backdrop_color = *current_backdrop_color;
    const Settings current_settings = getCurrentSettings();

    // Handle colors when holding a dpad direction
    if (keys_held & KEY_UP)
        *current_backdrop_color = BLUE;
    else if (keys_held & KEY_RIGHT)
        *current_backdrop_color = YELLOW;
    else if (keys_held & KEY_LEFT)
        *current_backdrop_color = BLACK;
    // Also if dpad is not held, and screen is off
    else if (keys_held & KEY_DOWN || !process_status.is_screen_on_phase)
        *current_backdrop_color = WHITE;
    // Otherwise, set according to current phase
    else {
        switch (current_settings.mode) {
            case WHITE_SCREEN:
                *current_backdrop_color = WHITE;
                break;
            case CYCLING_COLORS:
                *current_backdrop_color = process_status.cycling_colors_status.backdrop_color;
                break;
        }
    } 
        
    // Set reprint of both screens if X has just been pressed or released
    if (keys_down & (KEY_X) || keys_up & (KEY_X)) {
        setReprintBottom(true);
        setReprintTop(true);
    }

    // Flag that time is to be printed if X is being held
    if (keys_held & KEY_X)
        process_status.do_print_time = true; 
    else
        process_status.do_print_time = false;

    if (process_status.last_backdrop_color != *current_backdrop_color)
        setBackdropBoth(*current_backdrop_color);

    handleBacklight(keys_held);
}

/**
 * Fade colors for modes that require it
 */
void fadeColors() {
    const Settings current_settings = getCurrentSettings();

    // Skip progressing fading colors if in screen off phase
    if (!process_status.is_screen_on_phase)
        return;

    // If there's some remaining delay, decrement it and skip progressing for this iteration
    if (process_status.general_timer-- > 0)
        return;

    switch (current_settings.mode) {
        case CYCLING_COLORS:
            CyclingColorsStatus *cycling_colors_status = &process_status.cycling_colors_status;

            // Increment backdrop color according to phase, and switch phase if necessary
            switch (cycling_colors_status->current_phase) {
                case BLACK_TO_BLUE:
                    cycling_colors_status->backdrop_color += BLUE_INCREMENT;
                    if (cycling_colors_status->backdrop_color == BLUE)
                        cycling_colors_status->current_phase = BLUE_TO_YELLOW;
                    break;
                case BLUE_TO_YELLOW:
                    cycling_colors_status->backdrop_color += YELLOW_INCREMENT - BLUE_INCREMENT;
                    if (cycling_colors_status->backdrop_color == YELLOW)
                        cycling_colors_status->current_phase = YELLOW_TO_WHITE;
                    break;
                case YELLOW_TO_WHITE:
                    cycling_colors_status->backdrop_color += BLUE_INCREMENT;
                    if (cycling_colors_status->backdrop_color == WHITE)
                        cycling_colors_status->current_phase = WHITE_TO_BLACK;
                    break;
                case WHITE_TO_BLACK:
                    cycling_colors_status->backdrop_color -= WHITE_INCREMENT;
                    if (cycling_colors_status->backdrop_color == BLACK)
                        cycling_colors_status->current_phase = BLACK_TO_BLUE;
                    break;
            }
            // Copy delay from settings to remaining delay
            process_status.general_timer = current_settings.cycling_colors_settings.delay;
        case WHITE_SCREEN:
            break;
    }
}

/**
 * Initialize parameters and start process 
 * @param keys_held held keys
 */
void startProcess(const u16 keys_held) {
    Settings current_settings = getCurrentSettings();
    setBackdropBoth(WHITE);

    switch (current_settings.mode) {
        case CYCLING_COLORS:
            process_status.general_timer = current_settings.cycling_colors_settings.delay;
        case WHITE_SCREEN:
            break;
    }

    process_status.is_screen_on_phase = true;
    process_status.remaining_seconds = current_settings.screen_on_duration_mins * 60;
    process_status.remaining_cycles = current_settings.cycle_count - 1;
    process_status.current_backdrop_color = WHITE;
    handleBacklight(keys_held);
    setCurrentStatus(RUNNING_PROCESS);
    setReprintBottom(true);
    setReprintTop(true);
    disableSleep();
    timerStart(0, ClockDivider_1024, timerFreqToTicks_1024(1), timer_handler);
}

/**
 * Initialize current backlight level in process_status 
 * @param level level to initialize to
 */
void initCurrentBacklights(const int level) {
    process_status.current_backlight_top = process_status.current_backlight_bottom = level;
}
