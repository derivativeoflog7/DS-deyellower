/*
 * Console detection code from Godmode9i, licensed under GPL-3.0
 * https://github.com/DS-Homebrew/GodMode9i/blob/6dc548625240b63dcac1058afeaba04bb60cd26b/arm9/source/consoleDetection.h#L6
 * https://github.com/DS-Homebrew/GodMode9i/blob/6dc548625240b63dcac1058afeaba04bb60cd26b/arm7/source/main.c#L296
 *
 *
 * and by edo9300
 */
/*---------------------------------------------------------------------------------
 *
 default ARM7 core                                                                 *

 Copyright (C) 2005 - 2010
 Michael Noland (joat)
 Jason Rogers (dovoto)
 Dave Murphy (WinterMute)

 This software is provided 'as-is', without any express or implied
 warranty.  In no event will the authors be held liable for any
 damages arising from the use of this software.

 Permission is granted to anyone to use this software for any
 purpose, including commercial applications, and to alter it and
 redistribute it freely, subjeconsole_type to the following restrictions:

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
    PM_Bits things_to_power_off[] = {
        PM_SOUND_AMP,
        POWER_MATRIX,
        POWER_3D_CORE
    };

    irqInit();
    fifoInit();
    installSystemFIFO();
    irqSet(IRQ_VBLANK, vblank_handler);
    irqEnable(IRQ_VBLANK);

    int old_IME = enterCriticalSection();
    if (!(REG_POWERCNT & POWER_WIFI))
        powerOn(POWER_WIFI);
    u16 wifi_chip_ID = *(volatile u16*)0x04808000;
    u8 reg = readPowerManagement(PM_CONTROL_REG);
    writePowerManagement(PM_CONTROL_REG, reg | BIT(1));
    u8 readback = readPowerManagement(PM_CONTROL_REG);
    writePowerManagement(PM_CONTROL_REG, reg);
    powerOff(POWER_WIFI);
    for (int i = 0; i < sizeof(things_to_power_off)/sizeof(PM_Bits); i++)
        if (REG_POWERCNT & things_to_power_off[i])
            powerOff(things_to_power_off[i]);
    leaveCriticalSection(old_IME);
    u8 pm5 = readPowerManagement(5);
    u8 backlight_level = readPowerManagement(PM_BACKLIGHT_LEVEL);

    ConsoleType console_type = UNKNOWN;
    if (wifi_chip_ID == 0x1440) {
        if (backlight_level & 0xF0)
            console_type = DS_PHAT_WITH_BACKLIGHT_CONTROL;
        else
            console_type = DS_PHAT_WITHOUT_BACKLIGHT_CONTROL;
    }
    else if (pm5 != 0)
        console_type = DS_LITE;
    else if (isDSiMode() || pm5 == 0) {
        if (readback & BIT(1))
            console_type = DSI;
        else
            console_type = _3DS;
    }

    fifoSendValue32(FIFO_USER_01, (u32)console_type);

    while (1) {
        // If there's a better way to let the ARM7 idle, please do tell me
        swiWaitForVBlank();
    }
    return 0;
}
