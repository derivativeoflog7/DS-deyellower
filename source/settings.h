#pragma once
#include <nds.h>
#include <stdio.h>

#define NUMBER_INPUT_BUFFER_SIZE 5

typedef enum {
	BOTH,
	TOP,
	BOTTOM
} Screens;

typedef enum {
	WHITE_SCREEN,
	CYCLING_COLORS
} Mode;

//TODO move in .c?
typedef enum {
	MAIN_SETTINGS_MENU,
	SELECT_SCREENS,
	MODE,
	BACKLIGHT_LEVEL,
	SCREEN_ON_LENGTH,
	SCREEN_OFF_LENGTH,
	CYCLE_COUNT,
	CYCLING_COLORS_SPEED
} SettingScreen;

/** @struct CyclingColorsSettings
 * Current settings for cycling colors mode
 * 
 * @var CyclingColorsSettings::delay
 * Color transition delay
 */
typedef struct {
	unsigned int delay;
} CyclingColorsSettings;

/** @struct Settings
 * Current settings
 * 
 * @var Settings::screen_on_duration_mins
 * Screen on duration in minutes
 * 
 * @var Settings::screen_off_duration_mins
 * Screen off duration in minutes
 * 
 * @var Settings::cycle_count
 * Cycle count
 * 
 * @var Settings::backlight_level
 * Backlight level (set once process starts)
 * 
 * @var Settings::mode
 * Mode
 * 
 * @var Settings::screens
 * Screens to run the process on
 */
typedef struct {
	unsigned int screen_on_duration_mins,
		screen_off_duration_mins,
		cycle_count,
		backlight_level;
	Mode mode;
	Screens screens;
	CyclingColorsSettings cycling_colors_settings;
} Settings;

/** @struct SettingsMenuStatus
 * Working variables for settings menu
 * 
 * @var SettingsMenuStatus::menu_position
 * Cursor position on settings main menu
 * 
 * @var SettingsMenuStatus::submenu_menu_position
 * Cursor position on settings submenu (list option or highlighted digit)
 * 
 * @var SettingsMenuStatus::current_screen
 * Current settings screen
 * 
 * @var SettingsMenuStatus::number_input_buffer
 * Array buffer for number input
 * 
 * @var SettingsMenuStatus::backlight_level_buffer
 * Buffer for backlight level
 */
typedef struct {
	int menu_position, submenu_position;
	SettingScreen current_screen;
	unsigned int number_input_buffer[NUMBER_INPUT_BUFFER_SIZE],
		backlight_level_buffer;

} SettingsMenuStatus;


Settings getCurrentSettings();
void printSettingsMenu(PrintConsole *p_console);
void handleSettingsInput(const u16 keys_down);
void initSettingsBacklightLevel(unsigned int backlight_level);

// Defined in settings.c
extern const char* const MODE_NAMES[];