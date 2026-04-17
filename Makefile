BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= ds_deyellower
GAME_TITLE	:= DS_deyellower
GAME_SUBTITLE	:=
GAME_AUTHOR	:= by derivativeoflog7
GAME_ICON     	:= icon.bmp

CFLAGS		:= --std=gnu23 -Wimplicit-fallthrough


include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
