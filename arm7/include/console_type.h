#pragma once

#include <nds/ndstypes.h>

typedef enum {
    DS_PHAT_WITHOUT_BACKLIGHT_CONTROL,
    DS_PHAT_WITH_BACKLIGHT_CONTROL,
    DS_LITE,
    DSI,
    _3DS,
    UNKNOWN,
    WAITING_RESPONSE = 0xFF
} ConsoleType;

