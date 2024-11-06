TARGET  = switcher
CROSS   = arm-linux-
CXXFLAGS  = -I/opt/staging_dir/target/usr/include/SDL2 
CXXFLAGS += -pthread -Ofast
LDFLAGS = -L/opt/staging_dir/target/rootfs/usr/miyoo/lib
LDFLAGS += -lSDL2 -lSDL2_image -lSDL2_ttf -static-libstdc++
WARMINGS = -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef
WARMINGS += -Wold-style-cast -Wmissing-declarations 

export PATH=/opt/a30/bin:$(shell echo $$PATH)

all: $(TARGET) fbfixcolor

fbfixcolor: fbfixcolor.c
	$(CROSS)g++  fbfixcolor.c -o fbfixcolor $(CXXFLAGS) $(LDFLAGS)

SDL_rotozoom.o: SDL_rotozoom.c
	$(CROSS)g++ -c SDL_rotozoom.c $(CXXFLAGS) $(LDFLAGS)

$(TARGET): SDL_rotozoom.o $(wildcard *.cpp) $(wildcard *.h)
	$(CROSS)g++ *.cpp *.o -o $(TARGET) $(CXXFLAGS) $(LDFLAGS) $(WARMINGS)

clean:
	rm -rf $(TARGET) *.o
