BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= ds_deyellower
GAME_TITLE	:= DS_deyellower
GAME_SUBTITLE	:=
GAME_AUTHOR	:= by derivativeoflog7
GAME_ICON     	:= icon.bmp

CFLAGS	+= "-Iarm7/include"

ifdef SKIP_CONSOLE_CHECK
	CFLAGS	+= -DSKIP_CONSOLE_CHECK
endif

export CFLAGS

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9arm7/Makefile
