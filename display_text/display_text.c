#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    puts("Usage: display_text screen_width screen_height rotation image.png "
         "text delay textsize height side width R "
         "G B font_path bg_R bg_G bg_B bg_alpha image_scaling (next-image.png "
         "scale height side)* ");
    return 0;
  }

  int screenWidth = atoi(argv[1]);
  int screenHeight = atoi(argv[2]);
  int rotation = atoi(argv[3]);

  printf("screenWidth: %d\n", screenWidth);
  printf("screenHeight: %d\n", screenHeight);
  printf("rotation: %d\n", rotation);

  char path[512];
  strncpy(path, argv[4], 512);
  if (access(path, F_OK) != 0)
    return 0;

  char message[1024];
  strncpy(message, argv[5], 1024);

  int delay = argc > 6 ? atoi(argv[6]) : 0;

  int textsize = argc > 7 ? atoi(argv[7]) : 20;

  int width = argc > 10 ? atoi(argv[10]) : 320;

  char *endPtr;
  uint8_t red = argc > 11 ? strtoul(argv[11], &endPtr, 16) : 255;
  uint8_t green = argc > 12 ? strtoul(argv[12], &endPtr, 16) : 255;
  uint8_t blue = argc > 13 ? strtoul(argv[13], &endPtr, 16) : 255;

  char font_path[512];
  if (argc > 14) {
    strncpy(font_path, argv[14], 512);
  } else {
    strncpy(font_path, "/mnt/SDCARD/Themes/SPRUCE/nunwen.ttf", 512);
  }

  uint8_t bgred = argc > 15 ? strtoul(argv[15], &endPtr, 16) : 16;
  uint8_t bggreen = argc > 16 ? strtoul(argv[16], &endPtr, 16) : 16;
  uint8_t bgblue = argc > 17 ? strtoul(argv[17], &endPtr, 16) : 16;
  uint8_t bgtrans =
      argc > 18 ? (uint8_t)((float)atoi(argv[18]) * 255.0 / (float)100.0) : 160;

  float relative_scaling = argc > 19 ? strtof(argv[19], NULL) : 1.0;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_ShowCursor(0);

  SDL_Window *window = SDL_CreateWindow("Main", 0, 0, screenWidth, screenHeight,
                                        SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  SDL_Surface *img = IMG_Load(path);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, img);
  SDL_FreeSurface(img);
  float h_ratio = ((float)img->h / screenHeight);
  float w_ratio = ((float)img->w / screenWidth);
  int new_h, new_w;
  if (h_ratio > w_ratio) {
    new_h = screenHeight;
    new_w = img->w / h_ratio;
  } else {
    new_h = img->h / w_ratio;
    new_w = screenWidth;
  }
  new_h = new_h * relative_scaling;
  new_w = new_w * relative_scaling;

  SDL_Rect rtimg = {0};
  rtimg.x = (screenWidth - new_w) / 2;
  rtimg.y = (screenHeight - new_h) / 2;
  rtimg.w = new_w;
  rtimg.h = new_h;

  SDL_RenderCopyEx(renderer, texture, NULL, &rtimg, rotation, NULL,
                   SDL_FLIP_NONE);
  SDL_DestroyTexture(texture);

  int max_arg = 20;
  while (argc > max_arg) {
    strncpy(path, argv[max_arg], 512);
    if (access(path, F_OK) != 0)
      return 0;
    max_arg++;
    relative_scaling = argc > max_arg ? strtof(argv[max_arg], NULL) : 1.0;
    max_arg++;
    img = IMG_Load(path);
    texture = SDL_CreateTextureFromSurface(renderer, img);
    SDL_FreeSurface(img);
    h_ratio = ((float)img->h / screenHeight);
    w_ratio = ((float)img->w / screenWidth);
    if (h_ratio > w_ratio) {
      new_h = screenHeight;
      new_w = img->w / h_ratio;
    } else {
      new_h = img->h / w_ratio;
      new_w = screenWidth;
    }
    new_h = new_h * relative_scaling;
    new_w = new_w * relative_scaling;
    rtimg.w = new_w;
    rtimg.h = new_h;

    if (argc > max_arg) {
      if (rotation == 0) { // Normal orientation
        if (strcmp(argv[max_arg], "top") == 0) {
          rtimg.y = 0;
        } else if (strcmp(argv[max_arg], "bottom") == 0) {
          rtimg.y = screenHeight - new_h;
        } else {
          rtimg.y = (screenHeight - new_h) / 2;
        }
      } else if (rotation == 90) { // Rotated 90 degrees
        if (strcmp(argv[max_arg], "top") == 0) {
          rtimg.x = screenWidth - new_w;
        } else if (strcmp(argv[max_arg], "bottom") == 0) {
          rtimg.x = 0;
        } else {
          rtimg.x = (screenWidth - new_w) / 2;
        }
      } else if (rotation == 180) { // Rotated 180 degrees
        if (strcmp(argv[max_arg], "top") == 0) {
          rtimg.y = screenHeight - new_h;
        } else if (strcmp(argv[max_arg], "bottom") == 0) {
          rtimg.y = 0;
        } else {
          rtimg.y = (screenHeight - new_h) / 2;
        }
      } else if (rotation == 270) { // Rotated 270 degrees (original behavior)
        if (strcmp(argv[max_arg], "top") == 0) {
          rtimg.x = (new_h - new_w) / 2;
        } else if (strcmp(argv[max_arg], "bottom") == 0) {
          rtimg.x = (screenWidth - new_w) + ((new_w - new_h) / 2);
        } else {
          rtimg.x = (screenWidth - new_w) / 2;
        }
      }
    }
    max_arg++;

    if (argc > max_arg) {
      if (rotation == 0) { // Normal orientation
        if (strcmp(argv[max_arg], "left") == 0) {
          rtimg.x = 0;
        } else if (strcmp(argv[max_arg], "right") == 0) {
          rtimg.x = screenWidth - new_w;
        } else {
          rtimg.x = (screenWidth - new_w) / 2;
        }
      } else if (rotation == 90) { // Rotated 90 degrees
        if (strcmp(argv[max_arg], "left") == 0) {
          rtimg.y = screenHeight - new_h;
        } else if (strcmp(argv[max_arg], "right") == 0) {
          rtimg.y = 0;
        } else {
          rtimg.y = (screenHeight - new_h) / 2;
        }
      } else if (rotation == 180) { // Rotated 180 degrees
        if (strcmp(argv[max_arg], "left") == 0) {
          rtimg.x = screenWidth - new_w;
        } else if (strcmp(argv[max_arg], "right") == 0) {
          rtimg.x = 0;
        } else {
          rtimg.x = (screenWidth - new_w) / 2;
        }
      } else if (rotation == 270) { // Rotated 270 degrees (original behavior)
        if (strcmp(argv[max_arg], "left") == 0) {
          rtimg.y = (screenHeight - new_h) - ((new_w - new_h) / 2);
        } else if (strcmp(argv[max_arg], "right") == 0) {
          rtimg.y = ((new_w - new_h) / 2);
        } else {
          rtimg.y = (screenHeight - new_h) / 2;
        }
      }
    }

    max_arg++;
    SDL_RenderCopyEx(renderer, texture, NULL, &rtimg, rotation, NULL,
                     SDL_FLIP_NONE);
    SDL_DestroyTexture(texture);
  }

  TTF_Init();
  TTF_Font *Font = TTF_OpenFont(font_path, textsize);
  if (Font == NULL) {
    printf("Font loading failed: %s\n", TTF_GetError());
  }
  int test_w, test_h;
  if ((TTF_SizeUTF8(Font, message, &test_w, &test_h) < 0) || !width) {
    printf("Text has zero width\n");
    return 0;
  }
  int lineheight = test_h;
  char *start_line = message;
  char *end_line = start_line;
  char *next_line = start_line;
  char *test_ptr = start_line;
  char tmp_char;

  int numLines = 0;
  do {
    while (*test_ptr != ' ' && *test_ptr != '\n' && *test_ptr != '\0') {
      test_ptr++;
    }
    tmp_char = *test_ptr;
    *test_ptr = '\0';
    TTF_SizeText(Font, start_line, &test_w, &test_h);
    *test_ptr = tmp_char;
    if (test_w > width) {
      if (start_line != end_line) {
        *end_line = '\n';
        numLines++;
        end_line++;
        start_line = end_line;
      } else {
        if (*test_ptr != '\0') {
          *test_ptr = '\n';
          test_ptr++;
          start_line = test_ptr;
        }
        numLines++;
        end_line = test_ptr;
      }
    } else {
      switch (*test_ptr) {
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
  SDL_Surface *centeredSurface =
      SDL_CreateRGBSurface(0, width, text_height, 32, 0x000000FF, 0x0000FF00,
                           0x00FF0000, 0xFF000000);
  SDL_Color color = {red, green, blue, 255};
  int y = 0;
  int new_text_width;
  char *next;
  char *line = message;
  next = strchr(line, (int)'\n');
  if (next != NULL)
    *next++ = '\0';
  while (line != NULL) {
    SDL_Surface *lineSurface = TTF_RenderText_Blended(Font, line, color);
    if ((width) < lineSurface->w) {
      new_text_width = (width);
    } else {
      new_text_width = lineSurface->w;
    }
    SDL_Rect lineRect = {(width - new_text_width) / 2, y, new_text_width,
                         lineSurface->h};
    SDL_BlitScaled(lineSurface, NULL, centeredSurface, &lineRect);
    do {
      y += lineSurface->h;
      line = next;
      if (line != NULL) {
        next = strchr(line, (int)'\n');
        if (next != NULL)
          *next++ = '\0';
      }
    } while (line != NULL && strlen(line) == 0);
    SDL_FreeSurface(lineSurface);
  }

  SDL_Surface *surfaceMessage = centeredSurface;
  surfaceMessage->h = y;

  // SDL_Surface* surfaceMessage = TTF_RenderText_Blended_Wrapped(Font, message,
  // color, width);

  // begin chad

  int textheight, textside;
  int percentY = 50; // Default to center if no argument is provided

  if (argc > 8) {
    percentY = atoi(argv[8]); // Convert argument to integer percentage
    if (percentY < 0)
      percentY = 0;
    if (percentY > 100)
      percentY = 100;
  }

  if (rotation == 0) { // Normal orientation
    textheight = (screenHeight - surfaceMessage->h) * percentY / 100;

    printf("rotation: 0, percentY: %d, screenHeight: %d, surfaceMessage->h: "
           "%d, textheight: %d\n",
           percentY, screenHeight, surfaceMessage->h, textheight);

    if (argc > 9) {
      if (strcmp(argv[9], "left") == 0) {
        textside = 0;
      } else if (strcmp(argv[9], "right") == 0) {
        textside = screenWidth - surfaceMessage->w;
      } else {
        textside = (screenWidth - surfaceMessage->w) / 2;
      }
    } else {
      textside = (screenWidth - surfaceMessage->w) / 2;
    }
  } else if (rotation == 90) { // Rotated 90 degrees
    textheight = (screenWidth - surfaceMessage->w) * percentY / 100;

    if (argc > 9) {
      if (strcmp(argv[9], "left") == 0) {
        textside = 0;
      } else if (strcmp(argv[9], "right") == 0) {
        textside = screenHeight - surfaceMessage->h;
      } else {
        textside = (screenHeight - surfaceMessage->h) / 2;
      }
    } else {
      textside = (screenHeight - surfaceMessage->h) / 2;
    }
  } else if (rotation == 180) { // Rotated 180 degrees
    textheight = (screenHeight - surfaceMessage->h) * percentY / 100;

    if (argc > 9) {
      if (strcmp(argv[9], "left") == 0) {
        textside = screenWidth - surfaceMessage->w;
      } else if (strcmp(argv[9], "right") == 0) {
        textside = 0;
      } else {
        textside = (screenWidth - surfaceMessage->w) / 2;
      }
    } else {
      textside = (screenWidth - surfaceMessage->w) / 2;
    }
  } else if (rotation == 270) { // Rotated 270 degrees
    textheight = (screenHeight - surfaceMessage->w) * percentY / 100;

    if (argc > 9) {
      if (strcmp(argv[9], "left") == 0) {
        textside = (screenWidth - surfaceMessage->h) -
                   ((surfaceMessage->w - surfaceMessage->h) / 2);
      } else if (strcmp(argv[9], "right") == 0) {
        textside = ((surfaceMessage->w - surfaceMessage->h) / 2);
      } else {
        textside = (screenWidth - surfaceMessage->h) / 2;
      }
    } else {
      textside = (screenWidth - surfaceMessage->h) / 2;
    }
  }

  // Update SDL_Rect positioning
  SDL_Rect rt = {0};
  rt.x = textside;
  rt.y = textheight;
  rt.w = surfaceMessage->w;
  rt.h = surfaceMessage->h;

  SDL_Rect rtorigin = {0};
  rtorigin.x = 0;
  rtorigin.y = 0;
  rtorigin.w = surfaceMessage->w;
  rtorigin.h = surfaceMessage->h;

  // end chad

  SDL_Surface *surfacebg = SDL_CreateRGBSurface(
      0, surfaceMessage->w, surfaceMessage->h, 32, 0, 0, 0, 0);
  SDL_FillRect(surfacebg, &rtorigin,
               SDL_MapRGB(surfacebg->format, bgred, bggreen, bgblue));
  SDL_SetSurfaceBlendMode(surfacebg, SDL_BLENDMODE_BLEND);
  SDL_Texture *messageBGTexture =
      SDL_CreateTextureFromSurface(renderer, surfacebg);
  SDL_SetTextureAlphaMod(messageBGTexture, bgtrans);
  SDL_FreeSurface(surfacebg);
  SDL_RenderCopyEx(renderer, messageBGTexture, NULL, &rt, rotation, NULL,
                   SDL_FLIP_NONE);

  SDL_Texture *messageTexture =
      SDL_CreateTextureFromSurface(renderer, surfaceMessage);
  SDL_FreeSurface(surfaceMessage);

  printf("x %d y %d w %d h %d\n", rt.x, rt.y, rt.w, rt.h);

  SDL_RenderCopyEx(renderer, messageTexture, NULL, &rt, rotation, NULL,
                   SDL_FLIP_NONE);

  SDL_RenderPresent(renderer);

  if (delay > 0) {
    sleep(delay);
  } else {
    while (1) {
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
