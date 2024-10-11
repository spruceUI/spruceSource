TARGET = battery_percent
PRODUCT = $(TARGET).elf
CROSS_ROOT=/opt/a30
COMPILER_PREFIX=arm-linux-
CC=${CROSS_ROOT}/bin/${COMPILER_PREFIX}gcc
FLAGS	= -O3 -lSDL2 -lSDL2_image -lSDL2_ttf -lrt -ldl -Wl,--gc-sections -s

all:
	$(CC) $(TARGET).c -o $(PRODUCT) $(FLAGS)
clean:
	rm -rf $(PRODUCT)