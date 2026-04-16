#include "common.h"
#include "settings.h"
#include <assert.h>
#include <nds.h>
#include <stdbool.h>
#include <stdio.h>

// Initialize default settings (apart from backlight level which is done at runtime)
static Settings current_settings = {
	.screen_on_duration_mins = 120,
	.screen_off_duration_mins = 10,
	.cycle_count = 12,
	.mode = WHITE_SCREEN,
	.screens = BOTH,
	.cycling_colors_settings = {
		.delay = 0
	}
};

static SettingsMenuStatus settings_menu_status = {
	.menu_position = 0,
	.submenu_position = 0
};

const char* const MODE_NAMES[] = {
	[WHITE_SCREEN] = "White screen",
	[CYCLING_COLORS] = "Cycling colors" 
};

typedef struct {
    const SettingScreen target;
    const char *text;
} SettingsEntry;

typedef struct {
    const Screens target;
    const char *text;
} ScreenEntry;

typedef struct {
    const Mode target;
    const char *text;
} ModeEntry;

static const SettingsEntry SETTING_ENTRIES[] = {
    {.target = SELECT_SCREENS, .text = "Select screens"},
    {.target = SCREEN_ON_LENGTH, .text = "Screen on length"},
    {.target = SCREEN_OFF_LENGTH, .text = "Screen off length"},
    {.target = CYCLE_COUNT, .text = "Cycle count"},
    {.target = MODE, .text = "Mode"},
    {.target = BACKLIGHT_LEVEL, .text = "Backlight level"}
};

static const SettingsEntry CYCLING_COLORS_SETTING_ENTRIES[] = {
	{.target = CYCLING_COLORS_SPEED, .text = "Cycling colors speed"}
};

// The following two must be in the same order as the enums for input code to properly work
static const ScreenEntry SCREEN_ENTRIES[] = {
    {.target = BOTH, .text = "Both screens"},
    {.target = TOP, .text = "Top screen"},
    {.target = BOTTOM, .text = "Bottom screen"},
};

static const ModeEntry MODE_ENTRIES[] = {
    {.target = WHITE_SCREEN, .text = MODE_NAMES[WHITE_SCREEN]},
    {.target = CYCLING_COLORS, .text = MODE_NAMES[CYCLING_COLORS]}
};

/**
 * Prints a single line of a list input, with arrow and different color for the current selection
 * @param is_selected boolean indicating to print the arrow and in different color
 * @param text string to print
*/
static void printListLine (
    const bool is_selected,
    const char* const text
) {
    if (is_selected)
        printf("-> ");
    else {
        consoleSetColor(NULL, CONSOLE_LIGHT_GRAY);
        printf("   ");
    }
    printf("%s\n", text);

    // Make sure color is reset to white at the end
    consoleSetColor(NULL, CONSOLE_WHITE);
}


/** 
 * Prints a number input and an arrow indicating the currently highlighted digit
 * Printing is hardcoded to be done at lines 2 and 3
 */
static void printNumberInput() {
	consoleSetCursor(NULL, 0, 2);

    for (int i = 0; i < NUMBER_INPUT_BUFFER_SIZE; i++) {
        printf("%d", settings_menu_status.number_input_buffer[i]);
        if (settings_menu_status.submenu_position == i) {
			//Print arrow below highlighted digit
            consoleAddToCursor(NULL, -1, 1);
            printf("^");
            consoleAddToCursor(NULL, 0, -1);
        }
    }
}

/**
 * Puts the individual digits of an unsigned int into a number input buffer of size NUMBER_INPUT_BUFFER_SIZE
 * @param val unsigned int input value
 * @param buf output buffer of size NUMBER_INPUT_BUFFER_SIZE
 */
static void uintToBuffer(unsigned int val, unsigned int buf[NUMBER_INPUT_BUFFER_SIZE]) {
    for (int i = NUMBER_INPUT_BUFFER_SIZE - 1; i >= 0; i--) {
		printf("%d", i);
        buf[i] = val % 10;
        val /= 10;
    }
    assert(val == 0); // ensure that val is not >= 10**NUMBER_INPUT_BUFFER_SIZE
}

/**
 * Converts the content of a number input buffer of size NUMBER_INPUT_BUFFER_SIZE to an unsigned int value
 * @param buf input buffer of size NUMBER_INPUT_BUFFER_SIZE
 * @return output unsigned int value
 */
static unsigned int bufferToUint(const unsigned int buf[NUMBER_INPUT_BUFFER_SIZE]) {
    unsigned int ret = 0, mult = 1;
    for (int i = NUMBER_INPUT_BUFFER_SIZE - 1; i >= 0; i--) {
        assert(buf[i] <= 9);
        ret += buf[i] * mult;
        mult *= 10;
    }
    return ret;
}

/**
 * Prints the settings menu
 * @param p_console pointer to console that is being printed
 */
void printSettingsMenu(PrintConsole *p_console) {
	GeneralStatus general_status = getGeneralStatus();
	
	switch (settings_menu_status.current_screen) {
		case MAIN_SETTINGS_MENU:
			int adj_menu_position = settings_menu_status.menu_position - ARRAY_LENGTH(SETTING_ENTRIES);
			//assert(settings_menu_status.menu_position < ARRAY_LENGTH(SETTING_ENTRIES));
			// Print common settings
			for (int i = 0; i < ARRAY_LENGTH(SETTING_ENTRIES); i++) {
				printListLine(
					i == settings_menu_status.menu_position,
					SETTING_ENTRIES[i].text
				);
			}
			// Print mode specific settings
			switch (current_settings.mode) {
				case CYCLING_COLORS:
					for (int i = 0; i < ARRAY_LENGTH(CYCLING_COLORS_SETTING_ENTRIES); i++)
						printListLine(
							i == adj_menu_position,
							CYCLING_COLORS_SETTING_ENTRIES[i].text
						);
					break;
				case WHITE_SCREEN:
			}
			break;
		case SELECT_SCREENS:
			assert(settings_menu_status.submenu_position < ARRAY_LENGTH(SCREEN_ENTRIES));
			printf("Screens:\n");
			for (int i = 0; i < ARRAY_LENGTH(SCREEN_ENTRIES); i++) {
				printListLine(
					i == settings_menu_status.submenu_position,
					SCREEN_ENTRIES[i].text
				);
			}
			break;
		case MODE:
			printf("Mode:\n");
			for (int i = 0; i < ARRAY_LENGTH(MODE_ENTRIES); i++) {
				printListLine(
					i == settings_menu_status.submenu_position,
					MODE_ENTRIES[i].text
				);
			}

			printf("\n");
			consoleSetColor(NULL, CONSOLE_RED);
			printf("The cycling colors mode is\n");
			printf("experimental, and testing is\n");
			printf("required to understand it's\n");
			printf("effectiveness in various\n");
			printf("circumstances\n");
			break;
		case BACKLIGHT_LEVEL:
			assert(settings_menu_status.backlight_level_buffer <= general_status.max_backlight_level);
			printf("Backlight level:\n");
			printBacklightString(settings_menu_status.backlight_level_buffer, general_status.max_backlight_level);

			printf("\n\n");
			consoleSetColor(NULL, CONSOLE_LIGHT_GRAY);
			printf("This will not take effect\n");
			printf("until the process is started\n\n");
			printf("Level 0 means that the screen\n");
			printf("will still produce an image,\n");
			printf("but the backlight LEDs will be\n");
			printf("completely off\n\n");
			consoleSetColor(NULL, CONSOLE_YELLOW);
			printf("Even if the console type has\n");
			printf("been misdetected, setting the\n");
			printf("level to max will work as\n");
			printf("expected on all models\n\n");
			consoleSetColor(NULL, CONSOLE_RED);
			printf("This is for experimenting only,\n");
			printf("the current consensus is that\n");
			printf("max brightness works best for\n");
			printf("the process");
			break;
		// Number input is printed at the end of the function
		case SCREEN_ON_LENGTH: 
			printf("Screen on duration (minutes):\n");
			break;
		case SCREEN_OFF_LENGTH: 
			printf("Screen off duration (minutes):\n");
			break;
		case CYCLE_COUNT: 
			printf("Cycle count:\n");
			break;
		case CYCLING_COLORS_SPEED:
			printf("Cycling colors delay (frames): \n\n\n\n\n");
			consoleSetColor(NULL, CONSOLE_GRAY);
			printf("Controls how many frames to wait");
			printf("before fading the color, as\n");
			printf("normally it runs at 30fps");
			break;
	}

	// Print instructions on the bottom of the screen
	consoleSetColor(NULL, CONSOLE_WHITE);
    consoleSetCursor(NULL, 0, p_console->consoleHeight - 1);
	printf("Press B to go back");

    consoleSetCursor(NULL, 0, p_console->consoleHeight - 2);
	if (settings_menu_status.current_screen == MAIN_SETTINGS_MENU)
		printf("Press A to enter");
	else
		printf("Press A to confirm");

	if (settings_menu_status.current_screen >= BACKLIGHT_LEVEL) {
		consoleSetCursor(NULL, 0, p_console->consoleHeight - 3);
		printf("Use dpad UP/DOWN to incr/decr");
	}

	// Print number input and extra instructions on the bottom of the screen
	if (settings_menu_status.current_screen >= SCREEN_ON_LENGTH) {
		printNumberInput();
		// Show invalid value warning if number input is 0 for screen on duration or cycles count
		if (
			(settings_menu_status.current_screen == SCREEN_ON_LENGTH ||
			settings_menu_status.current_screen == CYCLE_COUNT) &&
			bufferToUint(settings_menu_status.number_input_buffer) == 0
		) {
			printf("\n\n\n\n"); // Leave space for number input
			consoleSetColor(NULL, CONSOLE_RED);
			printf("Invalid value");
			consoleSetColor(NULL, CONSOLE_DEFAULT);
		}

		consoleSetCursor(NULL, 0, p_console->consoleHeight - 4);
		printf("Use dpad LEFT/RIGHT to move");
	}
}

/**
 * Handle input in the settings screen
 * @param keys_down pressed keys
 */
void handleSettingsInput (
	const u16 keys_down
) {
	SettingScreen current_screen = settings_menu_status.current_screen;
	int submenu_position = settings_menu_status.submenu_position;

	if (keys_down & (KEY_A | KEY_B | KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT))
		setReprintBottom(true);

	// Handle A input
	if (keys_down & KEY_A) {
		switch (current_screen) {
			case MAIN_SETTINGS_MENU:
				SettingScreen target = 0; //init to shutup gcc
				// Get highlighted option in main settings menu
				// If the cursor is over a common setting, get the value from the array as is
				if (settings_menu_status.menu_position < ARRAY_LENGTH(SETTING_ENTRIES))
					target = SETTING_ENTRIES[settings_menu_status.menu_position].target;
				// Otherwise, calculate the actual offset, and get the setting from the array corresponding to the current mode
				else {
					int adj_menu_position = settings_menu_status.menu_position - ARRAY_LENGTH(SETTING_ENTRIES);
					switch (current_settings.mode) {
						case CYCLING_COLORS:
							target = CYCLING_COLORS_SETTING_ENTRIES[adj_menu_position].target;
						case WHITE_SCREEN:
					}
				}

				// Prepare variables for submenu, by highlighting the current option
				// or copying a number value to the input buffer
				settings_menu_status.submenu_position = 0; //Reset for number input
				switch (target) {
					case BACKLIGHT_LEVEL:
						settings_menu_status.backlight_level_buffer = current_settings.backlight_level; 
						break;
					case SCREEN_ON_LENGTH:
						uintToBuffer(
							current_settings.screen_on_duration_mins, 
							settings_menu_status.number_input_buffer
						);
						break;
					case SCREEN_OFF_LENGTH:
						uintToBuffer(
							current_settings.screen_off_duration_mins,
							settings_menu_status.number_input_buffer
						);
						break;
					case CYCLE_COUNT:
						uintToBuffer(
							current_settings.cycle_count,
							settings_menu_status.number_input_buffer
						);
						break;
					case CYCLING_COLORS_SPEED:
						uintToBuffer(
							current_settings.cycling_colors_settings.delay,
							settings_menu_status.number_input_buffer
						);
					case SELECT_SCREENS:
						settings_menu_status.submenu_position = current_settings.screens;
						break;
					case MODE:
						settings_menu_status.submenu_position = current_settings.mode;
						break;
					case MAIN_SETTINGS_MENU:
						assert(false);
				}

				// Enter submenu and immediately return
				settings_menu_status.current_screen = target;
				return;
			case SELECT_SCREENS:
				assert(submenu_position < ARRAY_LENGTH(SCREEN_ENTRIES));
				current_settings.screens = SCREEN_ENTRIES[submenu_position].target;
				break;
			case MODE:
				assert(submenu_position < ARRAY_LENGTH(MODE_ENTRIES));
				current_settings.mode = MODE_ENTRIES[submenu_position].target;
				break;
			case BACKLIGHT_LEVEL:
				assert(settings_menu_status.backlight_level_buffer <= getGeneralStatus().max_backlight_level);
				current_settings.backlight_level = settings_menu_status.backlight_level_buffer;
				break;
			case SCREEN_ON_LENGTH:
				// Reject user input if it's 0
				unsigned int inp = bufferToUint(settings_menu_status.number_input_buffer);
				if (inp > 0)
					current_settings.screen_on_duration_mins = inp;
				break;
			case SCREEN_OFF_LENGTH:
				current_settings.screen_off_duration_mins = bufferToUint(settings_menu_status.number_input_buffer);
				break;
			case CYCLE_COUNT:
				// Reject user input if it's 0
				inp = bufferToUint(settings_menu_status.number_input_buffer);
				if (inp > 0)
					current_settings.cycle_count = inp;
				break;
			case CYCLING_COLORS_SPEED:
				current_settings.cycling_colors_settings.delay = bufferToUint(settings_menu_status.number_input_buffer);
				break;
		}
		
		// Return to main settings screen (unless already there) and reprint top screen
		setReprintTop(true);
		settings_menu_status.current_screen = MAIN_SETTINGS_MENU;
	} 

	// Handle B input
	else if (keys_down & KEY_B) {
		// Return to main settings menu if in submenu
		if (current_screen != MAIN_SETTINGS_MENU)
			settings_menu_status.current_screen = MAIN_SETTINGS_MENU;
		else
			setCurrentStatus(MAIN_MENU);
	} 
	
	// Handle dpad input
	else {
		// Also doubles as flag indicating if we're in a list SUBmenu (>0 => true)
		int list_entries_count = 0;

		switch(current_screen) {
			// Handle inputs on number inputs
			case CYCLE_COUNT:
			case SCREEN_ON_LENGTH:
			case SCREEN_OFF_LENGTH:
			case CYCLING_COLORS_SPEED:
				assert(submenu_position < 5);
				unsigned int *highlighted_digit = &settings_menu_status.number_input_buffer[submenu_position];
			
				if (keys_down & KEY_UP && *highlighted_digit < 9)
					(*highlighted_digit)++;
				else if (keys_down & KEY_DOWN && *highlighted_digit > 0)
					(*highlighted_digit)--;
				
				if (keys_down & KEY_LEFT && submenu_position > 0)
					settings_menu_status.submenu_position--;
				else if (keys_down & KEY_RIGHT && submenu_position < NUMBER_INPUT_BUFFER_SIZE - 1)
					settings_menu_status.submenu_position++;
				break;
				
			// Handle input on backlight level screen
			case BACKLIGHT_LEVEL:
				if (
					keys_down & KEY_UP && 
					settings_menu_status.backlight_level_buffer < getGeneralStatus().max_backlight_level
				)
					settings_menu_status.backlight_level_buffer++;
				else if (
					keys_down & KEY_DOWN && 
					settings_menu_status.backlight_level_buffer > 0
				)
					settings_menu_status.backlight_level_buffer--;
				break;
			
			// Handle screens with lists (along with the if below)
			case MAIN_SETTINGS_MENU:
				int menu_position = settings_menu_status.menu_position;
				// Calculate list length based on current mode, adding mode-specific settings to the count
				int setting_entires_count = ARRAY_LENGTH(SETTING_ENTRIES);
				switch (current_settings.mode) {
					case CYCLING_COLORS:
						setting_entires_count += ARRAY_LENGTH(CYCLING_COLORS_SETTING_ENTRIES);
						break;
					case WHITE_SCREEN:
				}

				// MAIN menu dpad handling
				if (keys_down & KEY_UP) {
					if (menu_position == 0)
						settings_menu_status.menu_position = setting_entires_count - 1;
					else
						settings_menu_status.menu_position--;
				} else if (keys_down & KEY_DOWN) {
					if (menu_position == setting_entires_count - 1)
						settings_menu_status.menu_position = 0;
					else
						settings_menu_status.menu_position++;
				}
				break;
			case SELECT_SCREENS:
				list_entries_count = ARRAY_LENGTH(SCREEN_ENTRIES);
				break;
			case MODE:
				list_entries_count = ARRAY_LENGTH(MODE_ENTRIES);
				break;
		}

		// SUBmenus dpad handling
		if (list_entries_count) {
			if (keys_down & KEY_UP) {
				if (submenu_position == 0)
					settings_menu_status.submenu_position = list_entries_count - 1;
				else
					settings_menu_status.submenu_position--;
			} else if (keys_down & KEY_DOWN) {
				if (submenu_position == list_entries_count - 1)
					settings_menu_status.submenu_position = 0;
				else
					settings_menu_status.submenu_position++;
			}
		}
	}
}

/**
 * Sets the backlight level in the current settings
 * To be called at launch after detecting the console type
 * @param backlight_level unsigned int backlight level
 */
void initSettingsBacklightLevel(
	unsigned int backlight_level
) {
	current_settings.backlight_level = backlight_level;
}

/**
 * Returns a copy of the current settings
 * @return Settings instance
 */
Settings getCurrentSettings() {
	return current_settings;
}
