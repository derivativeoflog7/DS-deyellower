/*
 * Console detection code from Godmode9i, licensed under GPL-3.0
 * https://github.com/DS-Homebrew/GodMode9i/blob/6dc548625240b63dcac1058afeaba04bb60cd26b/arm9/source/consoleDetection.h#L6
 * https://github.com/DS-Homebrew/GodMode9i/blob/6dc548625240b63dcac1058afeaba04bb60cd26b/arm7/source/main.c#L296
 */
/*---------------------------------------------------------------------------------
 *
 d efault ARM7 core                                                                 *

 Copyright (C) 2005 - 2010
 Michael Noland (joat)
 Jason Rogers (dovoto)
 Dave Murphy (WinterMute)

 This software is provided 'as-is', without any express or implied
 warranty.  In no event will the authors be held liable for any
 damages arising from the use of this software.

 Permission is granted to anyone to use this software for any
 purpose, including commercial applications, and to alter it and
 redistribute it freely, subject to the following restrictions:

 1.	The origin of this software must not be misrepresented; you
 must not claim that you wrote the original software. If you use
 this software in a product, an acknowledgment in the product
 documentation would be appreciated but is not required.

 2.	Altered source versions must be plainly marked as such, and
 must not be misrepresented as being the original software.

 3.	This notice may not be removed or altered from any source
 distribution.

 ---------------------------------------------------------------------------------*/

#include <nds.h>
#include "console_type.h"

void vblank_handler(void) {
    inputGetAndSend();
}

int main(int argc, char *argv[]) {
    irqInit();
    fifoInit();
    installSystemFIFO();
    irqSet(IRQ_VBLANK, vblank_handler);
    irqEnable(IRQ_VBLANK);
    *(volatile u16*)0x04000304 |= (1 << 1);

    u16 wifi_chip_ID = *(volatile u16*)0x04808000;
    u16 wifi_version = 0xFF;
    wifi_version = readFirmware(0x2F, &wifi_version, 1);
    consoleType ct = _DS_WITHOUT_BACKLIGHT_CONTROL;

    if (readPowerManagement(PM_BACKLIGHT_LEVEL) & 0xF0)
        ct = _DS_WITH_BACKLIGHT_CONTROL;
    if (isDSiMode() || (wifi_chip_ID != 0x1440 && readPowerManagement(5) == 0))
        ct = _DSI;
    if (wifi_version != 15 && wifi_version != 24)
        ;//ct = _3DS; //FIXME: does not work
    fifoSendValue32(FIFO_USER_01, (u32)ct);
    *(volatile u16*)0x04000304 |= (0 << 1);
    while (1) {}
    return 0;
}
