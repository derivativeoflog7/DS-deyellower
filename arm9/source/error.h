#include <stdio.h>
#include <nds.h>

static inline void printDSiError() {
    consoleSetColor(NULL, CONSOLE_LIGHT_RED);
    printf("ERROR\n");
    consoleSetColor(NULL, CONSOLE_DEFAULT);
    printf("DS deyellower has detected that\n");
    printf("it's running on a DSi, but in\n");
    printf("DS mode; this is not supported,\n");
    printf("as software running in DS mode\n");
    printf("may not be able to correctly set");
    printf("backlight levels on a DSi.\n\n");
    printf("DS deyellower cannot be run from\n");
    printf("a DS mode flashcart or forced\n");
    printf("into DS mode via TwilightMenu++\n");
    printf("on a DSi.\n\n");
    printf("Press START to exit");
}

static inline void print3DSError() {
    consoleSetColor(NULL, CONSOLE_LIGHT_RED);
    printf("ERROR\n");
    consoleSetColor(NULL, CONSOLE_DEFAULT);
    printf("DS deyellower has detected that\n");
    printf("it's running on a 3DS; this is\n");
    printf("not supported, as DS/DSi\n");
    printf("software cannot set backlight\n");
    printf("levels on a 3DS.\n\n");
    printf("Use 3Deyellower instead.\n\n");
    printf("Press START to exit");

}
