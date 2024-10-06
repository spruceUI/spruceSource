#include <stdio.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define FIXED_WIDTH		640
#define FIXED_HEIGHT	480

int main(int argc , char* argv[]) {
	if (argc<3) {
		puts("Usage: display_text image.png text delay textsize height side width R G B font_path bg_R bg_G bg_B bg_alpha image_scaling (next-image.png scale height side)*");
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
    uint8_t bgtrans = argc>15 ? (uint8_t)((float) atoi(argv[15]) * 255.0/(float)100.0) : 160;

    float relative_scaling = argc>16 ? strtof(argv[16], NULL) : 1.0;

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
    new_h = new_h * relative_scaling;
    new_w = new_w * relative_scaling;
    
    SDL_Rect rtimg = {0};
    rtimg.x = (480 - new_w) / 2;
    rtimg.y = (640 - new_h) / 2;
    rtimg.w = new_w;
    rtimg.h = new_h;
    
	SDL_RenderCopyEx(renderer, texture, NULL, &rtimg, 270, NULL, SDL_FLIP_NONE);
    SDL_DestroyTexture(texture);

    int max_arg = 17;
    while (argc>max_arg){
	    strncpy(path,argv[max_arg],512);
	    if (access(path, F_OK)!=0) return 0;
        max_arg++;
        relative_scaling = argc>max_arg ? strtof(argv[max_arg], NULL) : 1.0;
        max_arg++;
        img = IMG_Load(path);
        texture = SDL_CreateTextureFromSurface(renderer,img);
        SDL_FreeSurface(img);
        h_ratio = ((float)img->h/FIXED_HEIGHT);
        w_ratio = ((float)img->w/FIXED_WIDTH);
        if (h_ratio > w_ratio){
            new_h = FIXED_HEIGHT;
            new_w = img->w / h_ratio;
        }
        else{
            new_h = img->h / w_ratio;
            new_w = FIXED_WIDTH;
        }
        new_h = new_h * relative_scaling;
        new_w = new_w * relative_scaling;
        rtimg.w = new_w;
        rtimg.h = new_h;
        if (argc > max_arg){
            if (strcmp( argv[max_arg], "top" ) == 0){
                rtimg.x = (new_h - new_w) / 2;
            }
            else if (strcmp( argv[max_arg], "bottom" ) == 0){
                rtimg.x = (480 - new_w) + ((new_w - new_h)/2);
            }
            else {
                rtimg.x = (480 - new_w) / 2;
            }     
        }
        max_arg++;
        if (argc > max_arg){
            if (strcmp( argv[max_arg], "left" ) == 0){
                rtimg.y = (640 - new_h) - ((new_w - new_h)/2);
            }
            else if (strcmp( argv[max_arg], "right" ) == 0){
                rtimg.y = ((new_w - new_h)/2);
            }
            else {
                rtimg.y = (640 - new_h) / 2;
            }
        }
        max_arg++;
        SDL_RenderCopyEx(renderer, texture, NULL, &rtimg, 270, NULL, SDL_FLIP_NONE);
        SDL_DestroyTexture(texture);
    }

    TTF_Init();
    TTF_Font* Font = TTF_OpenFont(font_path, textsize);
    if(Font == NULL){ printf("Font loading failed: %s\n", TTF_GetError()); }
    int test_w, test_h;
    if ((TTF_SizeUTF8(Font, message, &test_w, &test_h) < 0) || !width) {
        printf("Text has zero width\n");
        return 0;
    }
    int lineheight = test_h;
    char * start_line = message;
    char * end_line = start_line;
    char * next_line = start_line;
    char * test_ptr = start_line;
    char tmp_char;
    
    int numLines = 0;
    do {
        while (*test_ptr != ' ' && *test_ptr != '\n' && *test_ptr != '\0'){
            test_ptr++;
        }
        tmp_char = *test_ptr;
        *test_ptr = '\0';
        TTF_SizeText(Font, start_line, &test_w, &test_h);
        *test_ptr = tmp_char;
        if (test_w > width){
            if (start_line != end_line){
                *end_line = '\n';
                numLines++;
                end_line++;
                start_line = end_line;
            }
            else {
                if (*test_ptr != '\0'){
                    *test_ptr = '\n';
                    test_ptr++;
                    start_line = test_ptr;
                }
                numLines++;
                end_line = test_ptr;
            }
        }
        else {
            switch ( *test_ptr ) {
                case ' ':
                    end_line = test_ptr;
                    test_ptr++;
                    break;
                case '\n':
                    numLines++;
                    test_ptr++;
                    end_line = test_ptr;
                    start_line = test_ptr;
                    break;
                case '\0':
                    numLines++;
                    end_line = test_ptr;
                    break;
            }
        }
    } while (*end_line != '\0');
    int lineskip;
    lineskip = TTF_FontLineSkip(Font);
    int text_height = numLines * lineheight;
    // Create a new surface for centered text
    SDL_Surface* centeredSurface = SDL_CreateRGBSurface(0, width, text_height, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    SDL_Color color = {red, green, blue, 255};
    int y = 0;
    int new_text_width;
    char * next;
    char * line = message;
    next = strchr(line, (int)'\n');
    if (next != NULL) *next++ = '\0';
    while (line != NULL) {
        SDL_Surface* lineSurface = TTF_RenderText_Blended(Font, line, color);
        if ((width) < lineSurface->w){
            new_text_width = (width);
        }
        else{
            new_text_width = lineSurface->w;
        }
        SDL_Rect lineRect = {(width - new_text_width) / 2, y, new_text_width, lineSurface->h};
        SDL_BlitScaled(lineSurface, NULL, centeredSurface, &lineRect);
        do{
            y += lineSurface->h;
            line = next;
            if (line != NULL){
                next = strchr(line, (int)'\n');
                if (next != NULL) *next++ = '\0';
            }
        } while (line != NULL && strlen(line) == 0 ) ;
        SDL_FreeSurface(lineSurface);
    }

    SDL_Surface* surfaceMessage = centeredSurface;
    surfaceMessage->h = y;

    //SDL_Surface* surfaceMessage = TTF_RenderText_Blended_Wrapped(Font, message, color, width);

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
	
	
    SDL_DestroyTexture(messageBGTexture);
    SDL_DestroyTexture(messageTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
