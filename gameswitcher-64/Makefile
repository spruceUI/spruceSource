TARGET  = switcher fbfixcolor
CXXFLAGS  = -I/usr/include/SDL2 
CXXFLAGS += -pthread -Ofast -std=gnu++17
LDFLAGS = -L$(PREFIX)
LDFLAGS += -lSDL2 -lSDL2_image -lSDL2_ttf -static-libstdc++
WARMINGS = -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef
WARMINGS += -Wold-style-cast -Wmissing-declarations 

export PATH=/opt/$(UNION_PLATFORM)/bin:$(shell echo $$PATH)

all: $(TARGET)

fbfixcolor: fbfixcolor.c
	$(CROSS_COMPILE)g++  fbfixcolor.c -o fbfixcolor $(CXXFLAGS) $(LDFLAGS)

SDL_rotozoom.o: SDL_rotozoom.c
	$(CROSS_COMPILE)g++ -c SDL_rotozoom.c $(CXXFLAGS) $(LDFLAGS)

switcher: SDL_rotozoom.o $(wildcard *.cpp) $(wildcard *.h)
	$(CROSS_COMPILE)g++ *.cpp *.o -o switcher $(CXXFLAGS) $(LDFLAGS) $(WARMINGS)

clean:
	rm -rf $(TARGET) *.o
