#include <nds/debug.h>
#include <nds.h>
#include "common.h"
#include "menu.h"
#include "process.h"
#include "settings.h"

int main(int argc, char **argv) {
    //Initialize things
    initMaxBacklightLevel();
    systemSetBacklightLevel(5);

    PrintConsole top_console, bottom_console;
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);
    consoleInit(&top_console, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(&bottom_console, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);

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
