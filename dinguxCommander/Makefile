#
#	DinguxCommander Makefile for MiyooMini
#

export PATH=/opt/a30/bin:$(shell echo $$PATH)

CROSS_COMPILE = arm-linux-
CXX := $(CROSS_COMPILE)g++


PLATFORM ?= $(UNION_PLATFORM)
ifeq (,$(PLATFORM))
PLATFORM=linux
endif

ifeq ($(PLATFORM),miyoomini)
CXXFLAGS := -Os -marm -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7ve+simd
else
CXXFLAGS := -Os -DUSE_SDL2
endif

SDL_CONFIG := /opt/a30/usr/arm-a30-linux-gnueabihf/sysroot/usr/bin/sdl2-config
CXXFLAGS += -I/opt/a30/arm-a30-linux-gnueabihf/sysroot/usr/include/SDL2

CXXFLAGS += -DPATH_DEFAULT=\"/mnt/SDCARD\"
CXXFLAGS += -DFILE_SYSTEM=\"/dev/mmcblk0p1\"
CXXFLAGS += -DCMDR_KEY_UP=SDLK_UP
CXXFLAGS += -DCMDR_KEY_RIGHT=SDLK_RIGHT
CXXFLAGS += -DCMDR_KEY_DOWN=SDLK_DOWN
CXXFLAGS += -DCMDR_KEY_LEFT=SDLK_LEFT
CXXFLAGS += -DCMDR_KEY_OPEN=SDLK_LCTRL		# A
CXXFLAGS += -DCMDR_KEY_PARENT=SDLK_LALT	# B
CXXFLAGS += -DCMDR_KEY_OPERATION=SDLK_SPACE	# X
CXXFLAGS += -DCMDR_KEY_SYSTEM=SDLK_LSHIFT		# Y
CXXFLAGS += -DCMDR_KEY_PAGEUP=SDLK_PAGEUP		# L1 / L2 = SDLK_TAB
CXXFLAGS += -DCMDR_KEY_PAGEDOWN=SDLK_PAGEDOWN		# R1 / R2 = SDLK_BACKSPACE
CXXFLAGS += -DCMDR_KEY_SELECT=SDLK_ESCAPE	# SELECT
CXXFLAGS += -DCMDR_KEY_TRANSFER=SDLK_RETURN	# START
CXXFLAGS += -DCMDR_KEY_MENU=SDLK_HOME		# MENU (added)
CXXFLAGS += -DOSK_KEY_SYSTEM_IS_BACKSPACE=ON
CXXFLAGS += -DSCREEN_WIDTH=640
CXXFLAGS += -DSCREEN_HEIGHT=480
CXXFLAGS += -DPPU_X=1.66666
CXXFLAGS += -DPPU_Y=1.66666
CXXFLAGS += -DSCREEN_BPP=32
CXXFLAGS += -DFONTS='{"nunwen.ttf",22},{"SmallLcd.ttf",16},{"/mnt/SDCARD/Themes/SPRUCE/nunwen.ttf",22},{"/mnt/SDCARD/miyoo/res/wqy-microhei.ttc",16}'
#CXXFLAGS += -D_REENTRANT
ifeq ($(PLATFORM),miyoomini)
CXXFLAGS += -DMIYOOMINI
endif

RESDIR := res
CXXFLAGS += -DRESDIR="\"$(RESDIR)\""

LINKFLAGS += 
LINKFLAGS += -L/opt/a30/usr/arm-a30-linux-gnueabihf/sysroot/usr/lib -lSDL2 -lSDL2_image -lSDL2_ttf -static-libstdc++
ifeq ($(PLATFORM),miyoomini)
LINKFLAGS += -lmi_sys -lmi_gfx
endif

CMD := 
SUM := @echo

OUTDIR := ./output

EXECUTABLE := $(OUTDIR)/DinguxCommander

OBJS :=	main.o commander.o config.o dialog.o fileLister.o fileutils.o keyboard.o panel.o resourceManager.o \
	screen.o sdl_ttf_multifont.o sdlutils.o text_edit.o utf8.o text_viewer.o image_viewer.o  window.o \
	SDL_rotozoom.o
ifeq ($(PLATFORM),miyoomini)
OBJS += gfx.o
endif

DEPFILES := $(patsubst %.o,$(OUTDIR)/%.d,$(OBJS))

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(addprefix $(OUTDIR)/,$(OBJS))
	echo ${CXX}
	echo $(LINKFLAGS)
	$(SUM) "  LINK    $@"
	$(CMD)$(CXX) $(LINKFLAGS) -I/opt/a30/arm-a30-linux-gnueabihf/sysroot/usr/include/SDL2 -o $@ $^ $(LINKFLAGS) -I/opt/a30/arm-a30-linux-gnueabihf/sysroot/usr/include/SDL2

$(OUTDIR)/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(SUM) "  CXX     $@"
	$(CMD)$(CXX) $(CXXFLAGS) -MP -MMD -MF $(@:%.o=%.d) -c $< -o $@
	@touch $@ # Force .o file to be newer than .d file.

$(OUTDIR)/%.o: src/%.c
	@mkdir -p $(@D)
	$(SUM) "  CXX     $@"
	$(CMD)$(CXX) $(CXXFLAGS) -MP -MMD -MF $(@:%.o=%.d) -c $< -o $@
	@touch $@ # Force .o file to be newer than .d file.

clean:
	$(SUM) "  RM      $(OUTDIR)"
	$(CMD)rm -rf $(OUTDIR)

# Load dependency files.
-include $(DEPFILES)

# Generate dependencies that do not exist yet.
# This is only in case some .d files have been deleted;
# in normal operation this rule is never triggered.
$(DEPFILES):
