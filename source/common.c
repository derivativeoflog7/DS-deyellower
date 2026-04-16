#include "common.h"
#include "process.h"
#include "settings.h"
#include <nds.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
 * Detects the console type by SETTING the backlight to the minimum level
 * @return ConsoleType with detected type
 */
ConsoleType detectConsoleType() {
    /*
    * https://blocksds.skylyrac.net/libnds/system_8h.html#a9bd93bee5409c05451447034b250959b
    * It's probably possible to do this in a more elegant way by reading some registers directly (which seemingly
    * is how the library determines if the DS has backlight control or not), but this will suffice for now
    */
    u32 test = systemSetBacklightLevel(1);
    if (isDSiMode() || test == 1)
        return DSI;
    if (test == 2)
        return DS_WITH_BACKLIGHT_CONTROL;
    return DS_WITHOUT_BACKLIGHT_CONTROL;
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
void setReprintBottom(bool val) {
	general_status.do_reprint_bottom = val;
}

/**
 * Sets reprint for top screen
 * @param val boolean value to set
 */
void setReprintTop(bool val) {
	general_status.do_reprint_top = val;
}

/**
 * Sets the current status in the general status
 * @param status Status to set
 */
void setCurrentStatus(Status status) {
	general_status.current_status = status;
}

/**
 * Detects, SETS and inits max backlight level in structs
 */
void initMaxBacklightLevel() {
	ConsoleType type = detectConsoleType();
	switch (type) {
		case DSI:
			general_status.max_backlight_level = 5;
			break;
		case DS_WITH_BACKLIGHT_CONTROL:
			general_status.max_backlight_level = 4;
			break;
		case DS_WITHOUT_BACKLIGHT_CONTROL:
			general_status.max_backlight_level = 1;
			break;
		default:
			assert(false);
	}
	initSettingsBacklightLevel(general_status.max_backlight_level);
	initCurrentBacklights(general_status.max_backlight_level);
}