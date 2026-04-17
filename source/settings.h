#pragma once
#include <nds.h>

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

Settings getCurrentSettings();
void printSettingsMenu(const PrintConsole *p_console);
void handleSettingsInput(u16 keys_down);
void initSettingsBacklightLevel(unsigned int backlight_level);

/**
 * Mode names, defined in settings.c
 */
extern const char* const MODE_NAMES[];