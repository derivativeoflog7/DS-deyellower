#include <ctype.h>
#include <nds.h>
#include "common.h"
#include "menu.h"
#include "process.h"
#include "settings.h"

/**
 * Prints info (including current settings)
 */
void printStatus () {
	GeneralStatus general_status = getGeneralStatus();
	Settings current_settings = getCurrentSettings();

	//Banner
	consoleSetColor(NULL, CONSOLE_YELLOW);
	printf("DS de");
	consoleSetColor(NULL, CONSOLE_LIGHT_YELLOW);
	printf("yell");
	consoleSetColor(NULL, CONSOLE_DEFAULT);
	printf("ower");
	consoleSetColor(NULL, CONSOLE_LIGHT_CYAN);
	printf(" v%s\n", VERSION);
	consoleSetColor(NULL, CONSOLE_DEFAULT);
	printf("by derivativeoflog7\n\n");

	//Current settings
	consoleSetColor(NULL, CONSOLE_LIGHT_GRAY);
	printf("Detected console type:\n");
	consoleSetColor(NULL, CONSOLE_DEFAULT);
	switch (general_status.max_backlight_level) {
		case 5:
			printf("DSi\n");
			break;
		case 4:
			printf("DS/DSLite with backlight control");
			break;
		case 1:
			printf("DS without backlight control\n");
			break;
	}
	printf("\n");
	consoleSetColor(NULL, CONSOLE_LIGHT_GRAY);
	printf("Current settings:\n");
	consoleSetColor(NULL, CONSOLE_DEFAULT);
	printf("Top screen: %s\n", current_settings.screens == BOTTOM ? "NO" : "YES");
	printf("Bottom screen: %s\n", current_settings.screens == TOP ? "NO" : "YES");
	printf("Screen on duration: %umin\n", current_settings.screen_on_duration_mins);
	printf("Screen off duration: %umin\n", current_settings.screen_off_duration_mins);
	printf("Cycle count: %u\n", current_settings.cycle_count);
	printf("Mode: %c%s\n", tolower(MODE_NAMES[current_settings.mode][0]), &MODE_NAMES[current_settings.mode][1]);
	printf("Backlight level: ");
	printBacklightString(current_settings.backlight_level, general_status.max_backlight_level);
	printf("\n");
	// Print additional settings if required by current mode
	switch (current_settings.mode) {
		case CYCLING_COLORS:
			printf("Cycling colors delay: %ufrm\n", current_settings.cycling_colors_settings.delay);
			break;
		case WHITE_SCREEN:
	}
}

/**
 * Prints main menu
 */
void printMainMenu() {
	printf("Press A to begin\n");
	printf("Press X for settings\n");
	printf("Press START to power off\n");
	printf("\t(at any time)\n");
}

/**
 * Handle input on main manu
 * @param keys_down u16 with pressed keys
 * @param keys_held u16 with held keys
 */
void handleMainMenuInput(
	const u16 keys_down,
	const u16 keys_held
) {
	if (keys_down & KEY_X)
		setCurrentStatus(SETTINGS_MENU);
	else if (keys_down & KEY_A) {
		setCurrentStatus(RUNNING_PROCESS);
		startProcess(keys_held);
	} else {
		return;
	}
	setReprintBottom(true);
	//Technically unneded when going to settings, but not worrying about that simplifies the code
	setReprintTop(true);
}