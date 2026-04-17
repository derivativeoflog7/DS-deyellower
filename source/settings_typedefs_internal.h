#pragma once

#include "settings.h"

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

/**
 * Holds a target SettingScreen/text pair to be shown in the main settings menu
 */
typedef struct {
    const SettingScreen target;
    const char *const text;
} SettingsEntry;

/**
 * Holds an array of SettingsEntry and the number of elements in said array
 * Used for mode-specific setting entries
 */
typedef struct {
    const size_t count;
    const SettingsEntry *const entries;
} SettingEntriesCollection;

/**
 * Holds a target Screens value/text pair to be shown in the screens settings submenu
 */
typedef struct {
    const Screens target;
    const char *const text;
} ScreenEntry;

/**
 * Holds a target Mode value/text pair to be shown in the mode settings submenu
 */
typedef struct {
    const Mode target;
    const char *const text;
} ModeEntry;

/**
 * Macro to automatically create a SettingEntiresCollection with the correct count
 * To be used when initializing MODE_SPECIFIC_SETTING_ENTRIES, since it also specifies the array index as mode
 * Passing no SettingEntry will set count to 0 and fill entries with NULL
 * @param mode array index, mode for which to show the setting entries
 * @param ... collecton of SettingEntry
 */
#define MAKE_SETTING_ENTRIES_COLLECTION(\
    mode, ...\
) [mode] = (SettingEntriesCollection){\
    .count = 0 __VA_OPT__(+ (sizeof((SettingsEntry[]){ __VA_ARGS__ }) / sizeof(SettingsEntry))),\
    .entries = NULL __VA_OPT__(, .entries = (SettingsEntry[]){ __VA_ARGS__ })\
} // Above is a gross abuse of syntax, but hey it works

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
    size_t menu_position, submenu_position;
    SettingScreen current_screen;
    unsigned int number_input_buffer[NUMBER_INPUT_BUFFER_SIZE],
        backlight_level_buffer;
} SettingsMenuStatus;