# Game Switcher for SpruceOS on Miyoo A30

Game switcher is a SDL2 program run on Miyoo A30 game console. It is used for fast switching between list of games.

```
Usage: switcher image_list title_list [-s speed] [-b on|off] [-m on|off] [-t on|off] [-ts speed] [-n on|off] [-d command]
-s: scrolling speed in frames (default is 20), larger value means slower.
-b: swap left/right buttons for image scrolling (default is off).
-m: display title in multiple lines (default is off).
-t: display title at start (default is on).
-ts: title scrolling speed in pixel per frame (default is 4).
-n: display item index (default is on).
-d: enable item deletion (default is on).
-dc: additional deletion command runs when an item is deleted (default is none).
     Use INDEX in command to take the selected index as input. e.g. "echo INDEX"
-h,--help show this help message.
# return value: the 1-based index of the selected image
```

# Links
Original repositories
https://github.com/oscarkcau/game-switcher-A30

SpruceOS
https://github.com/spruceUI/spruceOS

Miyoo webpage
https://www.lomiyoo.com/
