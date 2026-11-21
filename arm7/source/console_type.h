#pragma once

#include <nds/ndstypes.h>

typedef enum {
    _DS_WITHOUT_BACKLIGHT_CONTROL,
    _DS_WITH_BACKLIGHT_CONTROL,
    _DSI,
    _3DS,
    WAITING_RESPONSE = 0xFF
} consoleType;
