#pragma once
#include "common.h"
#include "settings.h"

void printStatus();
void printMainMenu();
void handleMainMenuInput(
	const u16 keys_down,
	const u16 keys_held
);
