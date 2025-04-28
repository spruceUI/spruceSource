import sdl2
import sdl2.ext
import os
import time
from PIL import Image
import ctypes
from ctypes import byref

class BitmapFont:
    def __init__(self, font_surface, glyph_width, glyph_height):
        self.font_surface = font_surface
        self.glyph_width = glyph_width
        self.glyph_height = glyph_height
        self.chars_per_row = font_surface.w // glyph_width

    def render_text(self, renderer, text, x, y, color=(255, 255, 255)):
        # Apply color tint to font texture
        sdl2.SDL_SetTextureColorMod(self.font_surface.texture, *color)
        for i, char in enumerate(text):
            ascii_val = ord(char)
            if 32 <= ascii_val < 128:
                char_index = ascii_val - 32
                src_x = (char_index % 16) * self.glyph_width
                src_y = (char_index // 16) * self.glyph_height
                src_rect = sdl2.SDL_Rect(src_x, src_y, self.glyph_width, self.glyph_height)
                dst_rect = sdl2.SDL_Rect(x + i * self.glyph_width, y, self.glyph_width, self.glyph_height)
                sdl2.SDL_RenderCopy(renderer.sdlrenderer, self.font_surface.texture, src_rect, dst_rect)

def load_png_as_surface(path):
    pil_img = Image.open(path).convert("RGBA")
    width, height = pil_img.size
    data = pil_img.tobytes("raw", "RGBA")

    pixel_buffer = ctypes.create_string_buffer(data)

    sdl_surface = sdl2.SDL_CreateRGBSurfaceWithFormatFrom(
        pixel_buffer,
        width,
        height,
        32,
        width * 4,
        sdl2.SDL_PIXELFORMAT_RGBA32
    )
    return sdl_surface, pixel_buffer  # keep buffer alive

    
ROM_DIR = "/mnt/sdcard/Roms/"

os.environ["SDL_VIDEODRIVER"] = "KMSDRM"
os.environ["SDL_RENDER_DRIVER"] = "kmsdrm"

sdl2.ext.init(controller=True)

display_mode = sdl2.SDL_DisplayMode()
if sdl2.SDL_GetCurrentDisplayMode(0, display_mode) != 0:
    print("Failed to get display mode, using fallback 640x480")
    width, height = 640, 480
else:
    width, height = display_mode.w, display_mode.h
    print(f"Display size: {width}x{height}")

window = sdl2.ext.Window("Minimal SDL2 GUI", size=(width, height), flags=sdl2.SDL_WINDOW_FULLSCREEN)
window.show()

# Use default renderer flags
renderer = sdl2.ext.Renderer(window, flags=sdl2.SDL_RENDERER_ACCELERATED)

sdl2.SDL_SetHint(sdl2.SDL_HINT_RENDER_SCALE_QUALITY, b"2")

# Quickly present something, helps on ArkOS.
renderer.clear()
renderer.present()



# Fill screen with white
renderer.color = sdl2.ext.Color(255, 255, 255)
renderer.clear()
renderer.present()

# Open first connected game controller
# Load controller.
print("Checking for a controller")
count = sdl2.SDL_NumJoysticks()
for index in range(count):
    print("Checking index")
    is_game_controller = sdl2.SDL_IsGameController(index)
    print(f"{index}: {is_game_controller}")
    if is_game_controller == sdl2.SDL_TRUE:
        pad = sdl2.SDL_GameControllerOpen(index)
        if pad is not None:
            print(f"Opened GameController {index}: {sdl2.SDL_GameControllerName(pad)}")
            print(f" {sdl2.SDL_GameControllerMapping(pad)}")
            

def run_option_toggle_ui(renderer, font):
    line_height = font.glyph_height + 10  # add 5px padding between lines
    options = ["01234", "56789", "ABCDE" , "FGHIJ", "abcde", "fghij"]
    toggles = [False] * len(options)

    running = True
    event = sdl2.SDL_Event()

    print("Use D-pad or left stick to navigate. Press A to confirm. Press Start to exit.")
    selected = 0;
    while running:
        while sdl2.SDL_PollEvent(byref(event)) != 0:
            if event.type == sdl2.SDL_QUIT:
                running = False
            elif event.type == sdl2.SDL_CONTROLLERBUTTONDOWN:
                btn = event.cbutton.button
                if btn == sdl2.SDL_CONTROLLER_BUTTON_DPAD_UP:
                    print("⬆️ Up")
                    selected-=1
                elif btn == sdl2.SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                    print("⬇️ Down")
                    selected+=1
                elif btn == sdl2.SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                    print("⬅️ Left")
                elif btn == sdl2.SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                    print("➡️ Right")
                elif btn == sdl2.SDL_CONTROLLER_BUTTON_A:
                    print("✅ Confirm (A)")
                    toggles[selected] = not toggles[selected];
                    print(f"Toggling {options[selected]}")

                elif btn == sdl2.SDL_CONTROLLER_BUTTON_START:
                    print("🛑 Exit (Start)")
                    running = False

        renderer.clear(sdl2.ext.Color(20, 20, 20))
        selected = max(0, selected)
        selected = min(len(options)-1, selected)

        for i, (label, state) in enumerate(zip(options, toggles)):
            text = f"{label.ljust(5)} [{'y' if state else 'n'}]"
            if(i == selected) :
                font.render_text(renderer, text, 50, 50 + i * line_height , color=(255, 255, 0))
            else :
                font.render_text(renderer, text, 50, 50 + i * line_height , color=(255, 255, 255))


        renderer.present()
        sdl2.SDL_Delay(16)


# Load font image
sdl_surface, pixel_buffer = load_png_as_surface("font.png")  # keep pixel_buffer alive
texture = sdl2.SDL_CreateTextureFromSurface(renderer.sdlrenderer, sdl_surface)

w = sdl_surface.contents.w
h = sdl_surface.contents.h

class Sprite:
    def __init__(self, texture, w, h):
        self.texture = texture
        self.w = w
        self.h = h

font_sprite = Sprite(texture, w, h)
bitmap_font = BitmapFont(font_sprite, glyph_width=31, glyph_height=31)

run_option_toggle_ui(renderer, bitmap_font);
    

