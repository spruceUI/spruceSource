#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


int main(int argc , char* argv[]) {
	//if (argc<3) {
	//	puts("Usage: battery_percent theme_path text-percent");
	//	return 0;
	//}


    char *icons[] = { "ic-power-charge-0\%",
    "ic-power-charge-25\%",
    "ic-power-charge-50\%",
    "ic-power-charge-75\%",
    "ic-power-charge-100\%",
    "power-0\%-icon",
    "power-20\%-icon",
    "power-50\%-icon",
    "power-80\%-icon",
    "power-full-icon"};

    TTF_Init();
    TTF_Font* Font = TTF_OpenFont("/mnt/SDCARD/Themes/SPRUCE/nunwen.ttf", 20);
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* surfaceMessage = TTF_RenderText_Blended(Font, argv[2], color);


    char tmp_path_target[128];
    char tmp_path_destination[128];
    for(uint8_t i = 0; i < 10; i++)
    {
        strcpy(tmp_path_target, argv[1]);
        strcat(tmp_path_target, icons[i]);
        strcat(tmp_path_target, "-backup.png");
        if (access(tmp_path_target, F_OK)!=0) continue;
        strcpy(tmp_path_destination, argv[1]);
        strcat(tmp_path_destination, icons[i]);
        strcat(tmp_path_destination, ".png");
        SDL_Surface* img = IMG_Load(tmp_path_target);
        SDL_Rect lineRect = {(img->w - surfaceMessage->w)>>1, (img->h - surfaceMessage->h)>>1, surfaceMessage->w, surfaceMessage->h};
        SDL_BlitSurface(surfaceMessage, NULL, img, &lineRect);
        IMG_SavePNG(img, tmp_path_destination);
        SDL_FreeSurface(img);
    }
	return 0;
}
