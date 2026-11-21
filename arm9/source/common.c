#include <nds.h>
#include <stdio.h>
#include "common.h"
#include "process.h"
#include "settings.h"

char debug_buffer[DEBUG_BUFFER_SIZE];

// Init general status (except backlight which is done at runtime)
GeneralStatus general_status = {
	.current_status = MAIN_MENU,
	.do_reprint_bottom = true,
	.do_reprint_top = true
};

/**
 * Prints the backlight level, along with " (off)" or " (max)" when needed
 * @param backlight_level backlight level
 * @param max_backlight_level backlight level that prints " (max)"
 */
void printBacklightString(
	const unsigned int backlight_level,
	const unsigned int max_backlight_level
) {
	assert(backlight_level <= max_backlight_level);
	printf("%u", backlight_level);
	if (backlight_level == 0)
		printf(" (off)");
	else if (backlight_level == max_backlight_level)
		printf(" (max)");
}

/**
 * Returns a copy of the general status
 * @return GeneralStatus instance
 */
GeneralStatus getGeneralStatus() {
	return general_status;
}

/**
 * Sets reprint for bottom screen
 * @param val boolean value to set
 */
void setReprintBottom(
	const bool val
) {
	general_status.do_reprint_bottom = val;
}

/**
 * Sets reprint for top screen
 * @param val boolean value to set
 */
void setReprintTop(
	const bool val
) {
	general_status.do_reprint_top = val;
}

/**
 * Sets the current status in the general status
 * @param status Status to set
 */
void setCurrentStatus(
	const Status status
) {
	general_status.current_status = status;
}

/**
 * Detects, SETS and inits max backlight level in structs
 */
void initGeneralStatus(ConsoleType console_type) {
	general_status.console_type = console_type;
	switch (console_type) {
		case DSI:
			general_status.max_backlight_level = 5;
			break;
		case DS_PHAT_WITH_BACKLIGHT_CONTROL:
		case DS_LITE:
			general_status.max_backlight_level = 4;
			break;
		case _3DS:
		case UNKNOWN:
		case DS_PHAT_WITHOUT_BACKLIGHT_CONTROL:
			general_status.max_backlight_level = 1;
			break;
		default:
			assert(false);
	}
	initSettingsBacklightLevel(general_status.max_backlight_level);
}
