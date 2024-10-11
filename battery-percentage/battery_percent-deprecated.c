#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


int main(int argc , char* argv[]) {
	if (argc<4) {
		puts("Usage: battery_percent battery.png text-percent output.png (font) (fontsize)");
		return 0;
	}
	
	char path[512];
	strncpy(path,argv[1],512);
	if (access(path, F_OK)!=0) return 0;
	
	char message[32];
	strncpy(message, argv[2], 32);

	char output_path[512];
	strncpy(output_path,argv[3],512);

	char font_path[512];
    if (argc>4){
    	strncpy(font_path, argv[4], 512);
    }
    else{
        strncpy(font_path, "/mnt/SDCARD/Themes/SPRUCE/nunwen.ttf", 512);
    }
    int textsize = argc>5 ? atoi(argv[5]) : 20;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to init SDL\n");
        exit(1);
    }

	SDL_Surface* img = IMG_Load(path);
   
    TTF_Init();
    TTF_Font* Font = TTF_OpenFont(font_path, textsize);
    if(Font == NULL){ printf("Font loading failed: %s\n", TTF_GetError()); }
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* surfaceMessage = TTF_RenderText_Blended(Font, message, color);
    SDL_Rect lineRect = {(img->w - surfaceMessage->w) / 2, (img->h - surfaceMessage->h) / 2, surfaceMessage->w, surfaceMessage->h};
    SDL_BlitSurface(surfaceMessage, NULL, img, &lineRect);

    IMG_SavePNG(img, output_path);

    SDL_FreeSurface(img);
    SDL_FreeSurface(surfaceMessage);
	SDL_Quit();
	return 0;
}
