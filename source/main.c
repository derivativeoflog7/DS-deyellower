#include <nds/debug.h>
#include <nds.h>
#include <stdbool.h>
#include <stdio.h>
#include "common.h"
#include "menu.h"
#include "process.h"
#include "settings.h"

int remaining_seconds;
bool do_reprint_top_screen = true, do_reprint_bottom_screen = true, do_print_progress = false;

/**
 * Allocates and initializes consoles
 * @return Consoles struct containing pointers to top and bottom console
 */
Consoles initConsoles() {
    Consoles ret;

    //Allocate
    ret.bottom = (PrintConsole*)alloca(sizeof(PrintConsole));
    ret.top = (PrintConsole*)alloca(sizeof(PrintConsole));
    
    //Set properties
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);
    consoleInit(ret.top, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
    consoleInit(ret.bottom, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, false, true);
    return ret;
}

int main(int argc, char **argv) {
    //Initialize things
    initMaxBacklightLevel();
    systemSetBacklightLevel(5);
    Consoles consoles = initConsoles();

    // Main loop
    while (1) {
        GeneralStatus general_status = getGeneralStatus();
        swiWaitForVBlank();
        
        // Read keys
        scanKeys();
        int keys_down = keysDown();
        int keys_held = keysHeld();
        int keys_up = keysUp(); 

        // Start to shutdown works anywhere anytime
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

        // Reprint top screen if needed
        if(general_status.do_reprint_top) {
            nocashMessage("Reprint top");
            consoleSelect(consoles.top);
            consoleClear();
            switch(general_status.current_status) {
                case MAIN_MENU:
                case SETTINGS_MENU:
                    printStatus();
                    break;
                case RUNNING_PROCESS:
                    printProcess(consoles.top);
                    break;
            }
            
            setReprintTop(false);
        }

        // Reprint bottom screen if needed
        if (general_status.do_reprint_bottom) {
            nocashMessage("Reprint bottom");
            consoleSelect(consoles.bottom);
            consoleClear();
            switch (general_status.current_status) {
                case MAIN_MENU:
                    printMainMenu();
                    break;
                case SETTINGS_MENU:
                    printSettingsMenu(consoles.bottom);
                    break;
                case RUNNING_PROCESS:
                    printProcess(consoles.bottom);
                    break;
            }

            setReprintBottom(false);
        }
    }
}
