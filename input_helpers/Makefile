TARGET  = getevent sendevent joystickinput joypad setlcdbl devlogger setsharedmem

WARMINGS = -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs  -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-overflow=5 -Wundef
WARMINGS += -Wmissing-declarations 

export PATH=/opt/a30/bin:$(shell echo $$PATH)

all: $(TARGET)

setsharedmem: setsharedmem.c
	$(CROSS_COMPILE)gcc  setsharedmem.c -o setsharedmem -O3 $(WARMINGS)

devlogger: devlogger.c
	$(CROSS_COMPILE)gcc  devlogger.c -o devlogger -O3 $(WARMINGS)

setlcdbl: setlcdbl.c
	$(CROSS_COMPILE)gcc  setlcdbl.c -o setlcdbl -O3 $(WARMINGS)

joypad: joypad.c
	$(CROSS_COMPILE)gcc joypad.c -o joypad -O3 $(WARMINGS)

joystickinput: joystickinput.c
	$(CROSS_COMPILE)gcc joystickinput.c -o joystickinput -O3 $(WARMINGS)

getevent: getevent.c
	$(CROSS_COMPILE)gcc getevent.c -o getevent -O3 $(WARMINGS)

sendevent: sendevent.c
	$(CROSS_COMPILE)gcc sendevent.c -o sendevent -O3 $(WARMINGS)

clean:
	rm -rf $(TARGET) *.o
