#include <stdio.h>
#include <nds.h>

void printDSiError() {
    consoleSetColor(NULL, CONSOLE_RED);
    printf("ERROR\n");
    consoleSetColor(NULL, CONSOLE_DEFAULT);
    printf("This program has detected that\n");
    printf("it's running on a DSi, but in\n");
    printf("DS mode; this is not supported,\n");
    printf("as software running in DS mode\n");
    printf("may not be able to correctly set");
    printf("backlight levels on a DSi.\n\n");
    printf("This program cannot be run from\n");
    printf("a DS mode flashcart or forced\n");
    printf("into DS mode via TwilightMenu++\n");
    printf("on a DSi.\n\n");
    printf("Press START to exit");
}
