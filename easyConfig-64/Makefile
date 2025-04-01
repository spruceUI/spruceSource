TARGET  = easyConfig
# CXXFLAGS  = -I/opt/staging_dir/target/usr/include/SDL2 
CXXFLAGS = -pthread -O3 -std=gnu++17
LDFLAGS = -L/opt/staging_dir/target/rootfs/usr/miyoo/lib
LDFLAGS += -lSDL2 -lSDL2_image -lSDL2_ttf -static-libstdc++
WARMINGS = -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wundef
WARMINGS += -Wold-style-cast -Wmissing-declarations 

export PATH=/opt/a30/bin:$(shell echo $$PATH)

all: $(TARGET)

$(TARGET): $(wildcard *.cpp) $(wildcard *.h)
	$(CROSS_COMPILE)g++ *.cpp -o $(TARGET) $(CXXFLAGS) $(LDFLAGS) $(WARMINGS)

clean:
	rm -rf $(TARGET) *.o
