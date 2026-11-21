#pragma once
#include <nds.h>
#include <stdbool.h>

void startProcess(u16 keys_held);
void initCurrentBacklights(int level);
void printProcess(const PrintConsole *p_console);
void fadeColors();
void handleProcessInput(
    u16 keys_held,
    u16 keys_down,
    u16 keys_up
);