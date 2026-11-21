#pragma once
#include <nds.h>
#include <stdbool.h>
#include "console_type.h"

#define VERSION "3.1"
#define ARRAY_LENGTH(x) (sizeof(x) / sizeof(x[0]))
#define DEBUG_BUFFER_SIZE 50
extern char debug_buffer[]; //for the nocash console, defined in common.c

typedef enum {
	MAIN_MENU,
	SETTINGS_MENU,
	RUNNING_PROCESS
} Status;

/** @struct GeneralStatus
 * Working variables used in all phases
 * 
 * @var GenralStatus::current_status
 * Current status (what the application is doing right now)
 * 
 * @var GenralStatus::max_backlight_level
 * Maximum backlight level (computer at runtime based on console type)
 * 
 * @var do_reprint_bottom
 * Reprint bottom screen on next main loop iteration
 * 
 * @var do_reprint_bottom
 * Reprint top screen on next main loop iteration
 */
typedef struct {
	Status current_status;
	//TODO figure this out
	/*const*/ unsigned int max_backlight_level;
	/*const*/ ConsoleType console_type;
	bool do_reprint_bottom, do_reprint_top;
} GeneralStatus;

/** @struct Consoles
 * Pointers to both consoles
 * 
 * @var Consoles::top
 * Pointer to top console
 * 
 * @var Consoles::bottom
 * Pointer to bottom console
 */
typedef struct {
	PrintConsole *top, *bottom;
} Consoles;

GeneralStatus getGeneralStatus();
void initGeneralStatus(
	ConsoleType type
);
void setCurrentStatus(
	Status status
);
void setReprintBottom(
	bool val
);
void setReprintTop(
	bool val
);
void printBacklightString(
	const unsigned int backlight_level,
	const unsigned int max_backlight_level
);
