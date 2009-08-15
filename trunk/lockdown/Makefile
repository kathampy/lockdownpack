TARGET = vshmain
OBJS = main.o graphics.o framebuffer.o exports.o

INCDIR =
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

#USE_KERNEL_LIBC=1
#USE_KERNEL_LIBS=1

PSP_FW_VERSION = 500
PSP_LARGE_MEMORY = 0

LIBDIR =
LIBS = -lpspgu -lpng -lz -lm
LDFLAGS =

#EXTRA_TARGETS = EBOOT.PBP
#PSP_EBOOT_TITLE = Password
#PSP_EBOOT_ICON="icon0.png"
#PSP_EBOOT_PIC1="pic1.png"
#PSP_EBOOT_SND0="snd0.at3"

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
