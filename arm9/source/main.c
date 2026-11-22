#include <nds/debug.h>
#include <nds.h>
#include "common.h"
#include "error.h"
#include "menu.h"
#include "process.h"
#include "settings.h"
#include "console_type.h"

volatile ConsoleType console_type = WAITING_RESPONSE;

void FIFO_handler(u32 value, void *userdata) {
    console_type = (ConsoleType)value;
}

int main(int argc, char **argv) {
    //Initialize things
    PrintConsole top_console, bottom_console;
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);
    consoleInit(&top_console, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottom_console, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
    fifoSetValue32Handler(FIFO_USER_01, FIFO_handler, NULL);

    // Check console type, and error out if necessary
    while (console_type == WAITING_RESPONSE) {}
#ifndef SKIP_CONSOLE_CHECK
    assert(console_type != UNKNOWN);
    bool error = false;
    if (console_type == DSI && !isDSiMode()) {
        error = true;
        consoleSelect(&top_console);
        printDSiError();
        consoleSelect(&bottom_console);
        printDSiError();
    } else if (console_type == _3DS) {
        error = true;
        consoleSelect(&top_console);
        print3DSError();
        consoleSelect(&bottom_console);
        print3DSError();
    }

    if (error) {
        while (1) {
            swiWaitForVBlank();
            scanKeys();
            const u16 keys_down = keysDown();

            if (keys_down & KEY_START)
                return 0;
        }
    }
#endif
#if defined(SKIP_CONSOLE_CHECK) || defined(SELECT_TO_STOP)
    // Tint the top screen red if test build
    setBackdropColor(0b0000000000001111);
#endif

    initGeneralStatus(console_type);

    // Main loop
    while (1) {
        GeneralStatus general_status = getGeneralStatus();
        swiWaitForVBlank();
        
        // Read keys
        scanKeys();
        const u16 keys_down = keysDown(),
            keys_held = keysHeld(),
            keys_up = keysUp();

        // Start to shut down works anywhere anytime
        if (keys_down & KEY_START)
            systemShutDown();

        // Handle inputs
        switch (general_status.current_status) {
            case MAIN_MENU:
                handleMainMenuInput(
                    keys_down,
                    keys_held
                );
                break;
            case SETTINGS_MENU:
                handleSettingsInput(
                    keys_down
                );
                break;
            case RUNNING_PROCESS:
                handleProcessInput(
                    keys_held,
                    keys_down,
                    keys_up
                );
                break;
        }

        if (general_status.current_status == RUNNING_PROCESS)
            fadeColors();


        // Reprint top screen if needed
        if(general_status.do_reprint_top) {
            nocashMessage("Reprint top");
            consoleSelect(&top_console);
            consoleClear();
            switch(general_status.current_status) {
                case MAIN_MENU:
                case SETTINGS_MENU:
                    printBanner();
                    break;
                case RUNNING_PROCESS:
                    printProcess(&top_console);
                    break;
            }
            
            setReprintTop(false);
        }

        // Reprint bottom screen if needed
        if (general_status.do_reprint_bottom) {
            nocashMessage("Reprint bottom");
            consoleSelect(&bottom_console);
            consoleClear();
            switch (general_status.current_status) {
                case MAIN_MENU:
                    printMainMenu();
                    break;
                case SETTINGS_MENU:
                    printSettingsMenu(&bottom_console);
                    break;
                case RUNNING_PROCESS:
                    printProcess(&bottom_console);
                    break;
            }
            setReprintBottom(false);
        }
    }
}
