#include <stdio.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define FIXED_WIDTH		640
#define FIXED_HEIGHT	480

int main(int argc , char* argv[]) {
	if (argc<3) {
		puts("Usage: display_text image.png text delay textsize height side width R G B font_path bg_R bg_G bg_B bg_alpha");
		return 0;
	}
	
	char path[512];
	strncpy(path,argv[1],512);
	if (access(path, F_OK)!=0) return 0;
	
	char message[1024];
	strncpy(message, argv[2], 1024);

	int delay = argc>3 ? atoi(argv[3]) : 0;

    int textsize = argc>4 ? atoi(argv[4]) : 20;

    int width = argc>7 ? atoi(argv[7]) : 320;

    char * endPtr;
    uint8_t red = argc>8 ? strtoul(argv[8], &endPtr, 16 ) : 255;
    uint8_t green = argc>9 ? strtoul(argv[9], &endPtr, 16 ) : 255;
    uint8_t blue = argc>10 ? strtoul(argv[10], &endPtr, 16 ) : 255;

	char font_path[512];
    if (argc>11){
    	strncpy(font_path, argv[11], 512);
    }
    else{
        strncpy(font_path, "/mnt/SDCARD/Themes/SPRUCE/nunwen.ttf", 512);
    }

    uint8_t bgred = argc>12 ? strtoul(argv[12], &endPtr, 16 ) : 16;
    uint8_t bggreen = argc>13 ? strtoul(argv[13], &endPtr, 16 ) : 16;
    uint8_t bgblue = argc>14 ? strtoul(argv[14], &endPtr, 16 ) : 16;
    uint8_t bgtrans = argc>15 ? strtoul(argv[15], &endPtr, 16 ) : 160;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_ShowCursor(0);

    SDL_Window* window = SDL_CreateWindow("Main", 0, 0, 480, 640, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
	SDL_Surface* img = IMG_Load(path);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,img);
    SDL_FreeSurface(img);
    float h_ratio = ((float)img->h/FIXED_HEIGHT);
    float w_ratio = ((float)img->w/FIXED_WIDTH);
    int new_h, new_w;
    if (h_ratio > w_ratio){
        new_h = FIXED_HEIGHT;
        new_w = img->w / h_ratio;
    }
    else{
        new_h = img->h / w_ratio;
        new_w = FIXED_WIDTH;
    }
    
    SDL_Rect rtimg = {0};
    rtimg.x = (480 - new_w) / 2;
    rtimg.y = (640 - new_h) / 2;
    rtimg.w = new_w;
    rtimg.h = new_h;
    
	SDL_RenderCopyEx(renderer, texture, NULL, &rtimg, 270, NULL, SDL_FLIP_NONE);

    TTF_Init();
    TTF_Font* Font = TTF_OpenFont(font_path, textsize);
    if(Font == NULL){ printf("Font loading failed: %s\n", TTF_GetError()); }
    printf("color %u %u %u\n", red, green, blue);
    SDL_Color color = {red, green, blue, 255};
    SDL_Surface* surfaceMessage = TTF_RenderText_Blended_Wrapped(Font, message, color, width);

    int textheight;
    if (argc > 5){
        if (strcmp( argv[5], "top" ) == 0){
            textheight = 0 - ((surfaceMessage->w - surfaceMessage->h)/2);
        }
        else if (strcmp( argv[5], "bottom" ) == 0){
            textheight = (480 - surfaceMessage->w) + ((surfaceMessage->w - surfaceMessage->h)/2);
        }
        else {
            textheight = (480 - surfaceMessage->w) / 2;
        }
    }
    else {
    textheight = (480 - surfaceMessage->w) / 2;
    }
	
    int textside;
    if (argc > 6){
        if (strcmp( argv[6], "left" ) == 0){
            textside = (640 - surfaceMessage->h) - ((surfaceMessage->w - surfaceMessage->h)/2);
        }
        else if (strcmp( argv[6], "right" ) == 0){
            textside = ((surfaceMessage->w - surfaceMessage->h)/2);
        }
        else {
            textside = (640 - surfaceMessage->h) / 2;
        }
    }
    else {
        textside = (640 - surfaceMessage->h) / 2;
    }

    SDL_Rect rt = {0};
    rt.x = textheight;
    rt.y = textside;
    rt.w = surfaceMessage->w;
    rt.h = surfaceMessage->h;

    SDL_Rect rtorigin = {0};
    rtorigin.x = 0;
    rtorigin.y = 0;
    rtorigin.w = surfaceMessage->w;
    rtorigin.h = surfaceMessage->h;


    SDL_Surface* surfacebg = SDL_CreateRGBSurface( 0 , surfaceMessage->w , surfaceMessage->h, 32 , 0 , 0 , 0 , 0 );
    SDL_FillRect(surfacebg, &rtorigin, SDL_MapRGB(surfacebg->format, bgred, bggreen, bgblue));
    SDL_SetSurfaceBlendMode(surfacebg, SDL_BLENDMODE_BLEND);
    SDL_Texture* messageBGTexture = SDL_CreateTextureFromSurface(renderer, surfacebg);
    SDL_SetTextureAlphaMod(messageBGTexture, bgtrans);
    SDL_FreeSurface(surfacebg);
    SDL_RenderCopyEx(renderer, messageBGTexture, NULL, &rt, 270, NULL, SDL_FLIP_NONE);

    SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_FreeSurface(surfaceMessage);

    printf("x %d y %d w %d h %d\n", rt.x,rt.y,rt.w,rt.h);

    SDL_RenderCopyEx(renderer, messageTexture, NULL, &rt, 270, NULL, SDL_FLIP_NONE);

	SDL_RenderPresent(renderer);
	
    if (delay > 0){
        sleep(delay);
    }
    else{
        while (1){
            sleep(1);
        }
    }
	
	SDL_DestroyTexture(texture);
    SDL_DestroyTexture(messageBGTexture);
    SDL_DestroyTexture(messageTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
