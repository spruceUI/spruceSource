#
# DinguxCommander Makefile for MiyooMini
#

PLATFORM ?= $(UNION_PLATFORM)
ifeq (,$(PLATFORM))
PLATFORM=linux
endif

# Compiler settings
CXX := $(CROSS_COMPILE)g++
CXXFLAGS += -Os -I/usr/include/SDL2
LINKFLAGS = -L/usr/lib -pthread -lSDL2 -lSDL2_image -lSDL2_ttf

ifeq ($(PLATFORM),miyoomini)
CXXFLAGS += -marm -mtune=cortex-a55 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv8.2-a+simd
else
CXXFLAGS += -mtune=cortex-a55 -march=armv8.2-a+simd -DUSE_SDL2
endif

# SDL Configuration
CXXFLAGS += -DPATH_DEFAULT=\"/mnt/SDCARD\"
CXXFLAGS += -DFILE_SYSTEM=\"/dev/mmcblk0p1\"
CXXFLAGS += -DCMDR_KEY_UP=SDLK_UP -DCMDR_KEY_RIGHT=SDLK_RIGHT -DCMDR_KEY_DOWN=SDLK_DOWN -DCMDR_KEY_LEFT=SDLK_LEFT
CXXFLAGS += -DCMDR_KEY_OPEN=SDLK_LCTRL -DCMDR_KEY_PARENT=SDLK_SPACE -DCMDR_KEY_OPERATION=SDLK_LALT -DCMDR_KEY_SYSTEM=SDLK_LSHIFT
CXXFLAGS += -DCMDR_KEY_PAGEUP=SDLK_PAGEUP -DCMDR_KEY_PAGEDOWN=SDLK_PAGEDOWN -DCMDR_KEY_SELECT=SDLK_RCTRL -DCMDR_KEY_TRANSFER=SDLK_RETURN
CXXFLAGS += -DCMDR_KEY_MENU=SDLK_HOME -DOSK_KEY_SYSTEM_IS_BACKSPACE=ON -DSCREEN_WIDTH=640 -DSCREEN_HEIGHT=480 -DPPU_X=1.66666
CXXFLAGS += -DPPU_Y=1.66666 -DSCREEN_BPP=32 -DFONTS='{"nunwen.ttf",22},{"SmallLcd.ttf",16},{"/mnt/SDCARD/Themes/SPRUCE/nunwen.ttf",22},{"/mnt/SDCARD/miyoo/res/wqy-microhei.ttc",16}'

ifeq ($(PLATFORM),miyoomini)
CXXFLAGS += -DMIYOOMINI
LINKFLAGS += -lmi_sys -lmi_gfx
endif

RESDIR := res
CXXFLAGS += -DRESDIR="\"$(RESDIR)\""

# File and directory settings
CMD := 
SUM := @echo
OUTDIR := ./output
EXECUTABLE := $(OUTDIR)/DinguxCommander

OBJS :=	main.o commander.o config.o dialog.o fileLister.o fileutils.o keyboard.o panel.o resourceManager.o \
	screen.o sdl_ttf_multifont.o sdlutils.o text_edit.o utf8.o text_viewer.o image_viewer.o window.o SDL_rotozoom.o
ifeq ($(PLATFORM),miyoomini)
OBJS += gfx.o
endif

DEPFILES := $(patsubst %.o,$(OUTDIR)/%.d,$(OBJS))

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(addprefix $(OUTDIR)/,$(OBJS))
	$(SUM) "  LINK    $@"
	$(CMD)$(CXX) $(LINKFLAGS) -o $@ $^ $(LINKFLAGS)

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

