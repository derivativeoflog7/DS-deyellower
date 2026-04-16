#pragma once
#include <nds.h>
#include <stdbool.h>

typedef enum {
    BLACK_TO_BLUE,
    BLUE_TO_YELLOW,
    YELLOW_TO_WHITE,
    WHITE_TO_BLACK
} CyclingColorsPhase;

/** @struct CyclingColorsStatus
 * Working variables used when running the proces in cycling colors mode
 * 
 * @var CyclingColorsStatus::current_phase
 * Current color transition phase
 * 
 * @var CyclingColorsStatus::remaining_delay
 * Remaining frames of delay
 * 
 * @var CyclingColorsStatus::backdrop_color
 * Current backdrop color (buffered to not interfere with dpad)
 */
typedef struct {
	CyclingColorsPhase current_phase;
	unsigned int remaining_delay;
	u16 backdrop_color;
} CyclingColorsStatus;

/** @struct ProcessStatus
 * Working variables used when the process is running
 * 
 * @var ProcessStatus::is_screen_on_phase
 * Is currently in screen on phase
 * 
 * @var ProcessStatus::do_print_time
 * Print time on screen (X is being held)
 * 
 * @var ProcessStatus::do_print_warning
 * Print screen off phase on screen (X or DPAD is being held)
 * 
 * @var ProcessStatus::remaining_seconds
 * Remaining seconds for current phase
 * 
 * @var ProcessStatus::remaining_cycles
 * Remaining cycles (screen on + screen off phase)
 * 
 * @var ProcessStatus::current_backlight_top
 * Current backlight level for top screen
 * 
 * @var ProcessStatus::current_backlight_bottom
 * Current backlight level for bottom screen
 * 
 * @var ProcessStatus::last_backlight_top
 * Last backlight level (on last iteration) for top screen
 * 
 * @var ProcessStatus::last_backlight_bottom
 * Last backlight level (on last iteration) for bottom screen
 * 
 * @var ProcessStatus::current_backdrop_color
 * Current backdrop color
 * 
 * @var ProcessStatus::last_backdrop_color
 * Last backdrop color (on last iteration)
 * 
 * @var ProcessStatus::cycling_colors_status
 * CyclingColorStatus instance
 */
typedef struct {
	bool is_screen_on_phase, 
		do_print_time, 
		do_print_warning;
	unsigned int remaining_seconds,
		remaining_cycles;
	int current_backlight_top,
		current_backlight_bottom,
		last_backlight_top,
		last_backlight_bottom;
	u16 current_backdrop_color,
		last_backdrop_color;
	CyclingColorsStatus cycling_colors_status;
} ProcessStatus;

void startProcess(u16 keys_held);
void initCurrentBacklights(int level);
void printProcess(PrintConsole *p_console);
void handleProcessInput(
    u16 keys_held,
    u16 keys_down,
    u16 keys_up
);