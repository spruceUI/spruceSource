#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cstdio>
#include <cmath>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "global.h"
#include "image_item.h"
#include "text_texture.h"
#include "fileutils.h"

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::list;

// settings
int scrollingFrames = 20; // how many frames used for image scrolling
bool isMultilineTitle = false;
bool isShowDescription = true;
bool isSwapLeftRight = false;
bool isAllowDeletion = true;
bool isShowItemIndex = true;
string deleteCommand = "";
int scrollingSpeed = 4;	  // title scrolling speed in pixel per frame

// global variables used in main.cpp
string programName;
list<ImageItem *> imageItems;				  // all loaded images
list<ImageItem *>::iterator currentIter; // iterator point to current image item
int fontSize = 28;
SDL_Color text_color = {235, 219, 178, 255};
SDL_Color delete_mode_text_color = {255, 50, 50, 255};
TTF_Font *fontInstruction = nullptr;
TTF_Font *fontTitle = nullptr;
string fontPath = "res/nunwen.ttf";
SDL_Texture *messageBGTexture = nullptr;
TextTexture *titleTexture = nullptr;
TextTexture *instructionTexture = nullptr;
TextTexture *deleteInstructionTexture = nullptr;
TextTexture *indexTexture = nullptr;
SDL_Rect overlay_bg_render_rect = {0, 0, 0, 0};
bool isScrollingTitle = false;
bool isDeleteMode = false;
int scrollingOffset = 0;  // current title scrolling offset
int scrollingLength = 0;  // length of scrolling title with space
int scrollingPause = 10;  // number of frames to pause when text touch left screen boundary
string instructionText = " \u2190/\u2192 Scroll   \u24B6 Load   \u24B7 Exit   \u24CD Settings";
string shortInstructionText = "\u24B6 Load  \u24B7 Exit  \u24CD Settings";
string deleteAddonText = "  \u24CE Remove";
string deleteInstructionText = "Remove Item?   \u24B6 Confirm   \u24B7 Cancel";
string argumentPlaceholder = "INDEX";

namespace
{
	// trim from start (in place)
	inline void ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
			return !std::isspace(ch);
		}));
	}

	// trim from end (in place)
	inline void rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}

	double easeInOutQuart(double x)
	{
		return x < 0.5 ? 8 * x * x * x * x : 1 - pow(-2 * x + 2, 4) / 2;
	}

	void printUsage()
	{
		cout << endl
			 << "Usage: switcher image_list title_list [-s speed] [-b on|off] [-m on|off] [-t on|off] [-ts speed] [-n on|off] [-d command]" << endl
			 << endl
			 << "-s:\timage scrolling speed in frames (default is 20), larger value means slower." << endl
			 << "-b:\tswap left/right buttons for image scrolling (default is off)." << endl
			 << "-m:\tdisplay title in multiple lines (default is off)." << endl
			 << "-t:\tdisplay title at start (default is on)." << endl
			 << "-ts:\ttitle scrolling speed in pixel per frame (default is 4)." << endl
			 << "-n:\tdisplay item index (default is on)." << endl
			 << "-d:\tenable item deletion with the deletion command provided (default is disable)." << endl
			 << "\tUse TITLE in command to take the selected title as input. e.g. \"echo TITLE\"" << endl
			 << "\tPass \"\" as argument if no command is provided." << endl
			 << "-h,--help\tshow this help message." << endl
			 << endl
			 << "Control: Left/Right: Switch games, A: Confirm, B: Cancel, R1: Toggle title" << endl
			 << endl
			 << "return value: the 1-based index of the selected image" << endl
			 << endl;
	}

	void printErrorAndExit(const char *message, const char *extraMessage = nullptr)
	{
		cerr << programName << ": " << message;
		if (extraMessage != nullptr)
			cerr << extraMessage;
		cerr << endl
			 << endl;
		exit(0);
	}

	void printErrorUsageAndExit(const char *message, const char *extraMessage = nullptr)
	{
		cerr << programName << ": " << message;
		if (extraMessage != nullptr)
			cerr << extraMessage;
		cerr << endl;
		printUsage();
		exit(0);
	}

	void handleOptions(int argc, char *argv[])
	{
		programName = File_utils::getFileName(argv[0]);

		// ensuer enough number of arguments
		if (argc < 3)
			printErrorUsageAndExit("Arguments missing");

		// handle options
		int i = 3;
		while (i < argc)
		{
			auto option = argv[i];
			if (strcmp(option, "-s") == 0)
			{
				if (i == argc - 1)
					printErrorUsageAndExit("-s: Missing option value");
				int s = atoi(argv[i + 1]);
				if (s <= 0)
					printErrorUsageAndExit("-s: Invalue scrolling speed");
				scrollingFrames = s;
				i += 2;
			}
			else if (strcmp(option, "-b") == 0)
			{
				if (i == argc - 1)
					printErrorUsageAndExit("-b: Missing option value");
				if (strcmp(argv[i + 1], "on") == 0)
					isSwapLeftRight = true;
				else if (strcmp(argv[i + 1], "off") == 0)
					isSwapLeftRight = false;
				else
					printErrorUsageAndExit("-m: Invalue option value, expects on/off\n");
				i += 2;
			}
			else if (strcmp(option, "-m") == 0)
			{
				if (i == argc - 1)
					printErrorUsageAndExit("-m: Missing option value");
				if (strcmp(argv[i + 1], "on") == 0)
					isMultilineTitle = true;
				else if (strcmp(argv[i + 1], "off") == 0)
					isMultilineTitle = false;
				else
					printErrorUsageAndExit("-m: Invalue option value, expects on/off\n");
				i += 2;
			}
			else if (strcmp(option, "-t") == 0)
			{
				if (i == argc - 1)
					printErrorUsageAndExit("-t: Missing option value");
				if (strcmp(argv[i + 1], "on") == 0)
					isShowDescription = true;
				else if (strcmp(argv[i + 1], "off") == 0)
					isShowDescription = false;
				else
					printErrorUsageAndExit("-t: Invalue option value, expects on/off\n");
				i += 2;
			}
			else if (strcmp(option, "-ts") == 0)
			{
				if (i == argc - 1)
					printErrorUsageAndExit("-ts: Missing option value");
				int s = atoi(argv[i + 1]);
				if (s <= 0)
					printErrorUsageAndExit("-ts: Invalue scrolling speed");
				scrollingSpeed = s;
				i += 2;
			}
			else if (strcmp(option, "-n") == 0)
			{
				if (i == argc - 1)
					printErrorUsageAndExit("-n: Missing option value");
				if (strcmp(argv[i + 1], "on") == 0)
					isShowItemIndex = true;
				else if (strcmp(argv[i + 1], "off") == 0)
					isShowItemIndex = false;
				else
					printErrorUsageAndExit("-m: Invalue option value, expects on/off\n");
				i += 2;
			}
			else if (strcmp(option, "-d") == 0)
			{
				if (i == argc - 1)
					printErrorUsageAndExit("-d: Missing option value");
				if (strcmp(argv[i + 1], "on") == 0)
					isAllowDeletion = true;
				else if (strcmp(argv[i + 1], "off") == 0)
					isAllowDeletion = false;
				else
					printErrorUsageAndExit("-m: Invalue option value, expects on/off\n");
				i += 2;
			}			
			else if (strcmp(option, "-dc") == 0)
			{
				if (i == argc - 1)
					printErrorUsageAndExit("-dc: Missing option value");
				string cmd = argv[i + 1];
				ltrim(cmd);
				rtrim(cmd);
				deleteCommand = cmd;
				i += 2;
			}
			else if (strcmp(option, "-h") == 0 || strcmp(option, "--help") == 0)
			{
				printUsage();
				exit(0);
			}
			else
				printErrorUsageAndExit("Invalue option: ", option);
		}
	}

	void loadImageFiles(const char *filename)
	{
		// open file
		std::ifstream file;
		file.open(filename);
		
		if (file.is_open())
		{
			string line;
			int index = 1;
			bool rotation = true;

			// iterate all input line
			while (std::getline(file, line))
			{
				// trim input line
				rtrim(line);
				ltrim(line);

				// skip empty line
				if (line.empty()) continue;

				if (line == "__NO_ROTATE__") {
					rotation = false;
					continue;
				}

				// create imageItem and add to list
				imageItems.push_back(new ImageItem(index, line, rotation));
				
				// increace index
				index++;

				// reset rotation flag
				rotation = true;
			}
		}
		else
		{
			printErrorAndExit("cannot open file: ", filename);
		}

		// close file
		file.close();
	}

	int loadImageDescriptions(const char *filename)
	{
		// open file
		std::ifstream file;
		file.open(filename);

		if (file.is_open())
		{
			string line;
			auto iter = imageItems.begin();

			// iterate all input line
			while (std::getline(file, line))
			{
				// trim input line
				rtrim(line);
				ltrim(line);

				// skip empty line
				if (line.empty()) continue;

				// set description
				(*iter)->setDescription(line);

				// move to next imageItem
				iter++;

				// exit loop after reading enough lines
				if (iter == imageItems.end()) break;
			}
		}
		else
		{
			printErrorAndExit("cannot open file: ", filename);
		}

		// close file
		file.close();

		return 0;
	}

	int loadAllImages(void *)
	{
		auto front = imageItems.begin();
		auto back = --imageItems.end();

		// load images from both directions,
		// make sure the images close to the first shown image will be loaded earlier.
		while (true)
		{
			(*front)->loadImage();
			(*back)->loadImage();

			front++;
			if (back != imageItems.begin())
				back--;
			if (front == imageItems.end())
				break;
		}

		return 0;
	}

	void prepareTextures()
	{
		// create message overlay background texture
		int overlay_height = fontSize + fontSize / 2;
		SDL_Rect overlay_bg_rect = {0, 0, overlay_height, global::SCREEN_HEIGHT};
		overlay_bg_render_rect.x = 0;//global::SCREEN_WIDTH - overlay_height;
		overlay_bg_render_rect.y = 0;
		overlay_bg_render_rect.w = overlay_height;
		overlay_bg_render_rect.h = global::SCREEN_HEIGHT;
		SDL_Surface *surfacebg = SDL_CreateRGBSurface(
			0,
			overlay_height,
			global::SCREEN_HEIGHT,
			32, 0, 0, 0, 0);
		SDL_FillRect(
			surfacebg,
			&overlay_bg_rect,
			SDL_MapRGB(surfacebg->format, 0, 0, 0));
		SDL_SetSurfaceBlendMode(surfacebg, SDL_BLENDMODE_BLEND);
		messageBGTexture = SDL_CreateTextureFromSurface(
			global::renderer,
			surfacebg);
		SDL_SetTextureAlphaMod(messageBGTexture, 160);
		SDL_FreeSurface(surfacebg);

		// create texture for instruction text
		string text = shortInstructionText; //isShowItemIndex ? shortInstructionText : instructionText;
		if (isAllowDeletion) text += deleteAddonText; 
		instructionTexture = new TextTexture(
			text,
			fontInstruction,
			text_color,
			isShowItemIndex ? TextTextureAlignment::bottomLeft : TextTextureAlignment::bottomCenter
		);

		// create texture for delete instruction text
		deleteInstructionTexture = new TextTexture(
			deleteInstructionText,
			fontInstruction,
			delete_mode_text_color,
			TextTextureAlignment::bottomCenter
		);
	}

	void updateMessageTexture(string message)
	{
		// create new message text texture
		if (isMultilineTitle)
		{
			titleTexture = new TextTexture(
				message.c_str(),
				fontTitle,
				text_color,
				TextTextureAlignment::topLeft,
				global::SCREEN_HEIGHT - 20
			);
		}
		else
		{
			titleTexture = new TextTexture(
				message.c_str(),
				fontTitle,
				text_color,
				TextTextureAlignment::topCenter
			);
		}

		// initial variables for scrolling title
		isScrollingTitle = false;
		if (!isMultilineTitle && titleTexture->getWidth() > global::SCREEN_HEIGHT)
		{
			isScrollingTitle = true;
			scrollingPause = 10;
			scrollingOffset = 0;
			scrollingLength = titleTexture->getWidth() + 40;
			scrollingLength -= scrollingLength % scrollingSpeed;
			titleTexture->updateTargetRect(TextTextureAlignment::topLeft);
		}

		// initial variables for multiline title
		if (isMultilineTitle)
		{
			overlay_bg_render_rect.x = 0; //global::SCREEN_WIDTH - titleTexture->getHeight();
			overlay_bg_render_rect.y = 0;
			overlay_bg_render_rect.w = titleTexture->getHeight();
			overlay_bg_render_rect.h = global::SCREEN_HEIGHT;
		}
	}

	void updateIndexTexture() {
		// find the index of current item in list
		// note that this may not be the same as currentItem->getIndex()
		auto index = imageItems.size();
		auto iter = imageItems.begin();
		while (iter != imageItems.end())
		{
			if (iter == currentIter) break;
			index--;
			iter++;
		}

		// create texture
		std::stringstream ss;
		ss << index << '/' << imageItems.size(); 
		indexTexture = new TextTexture(
			ss.str(),
			fontInstruction,
			text_color,
			TextTextureAlignment::bottomRight
		);

		// shift left 5 pixels
		indexTexture->scrollLeft(5);
	}

	void renderInstruction()
	{
		int overlay_height = fontSize + fontSize / 2;
		auto rect = overlay_bg_render_rect;
		rect.x = global::SCREEN_WIDTH - overlay_height;
		rect.w = overlay_height;

		if (isDeleteMode) 
		{
			// deelete mode - render dimmer background and delete instruction at top of screen

			// render a less transparent background
			Uint8 old_alpha;
			SDL_GetTextureAlphaMod(messageBGTexture, &old_alpha);
			SDL_SetTextureAlphaMod(messageBGTexture, 200);
			SDL_RenderCopy(global::renderer, messageBGTexture, nullptr, &rect);
			SDL_SetTextureAlphaMod(messageBGTexture, old_alpha);
			// render delete instruction text
			deleteInstructionTexture->render();
		}
		else if (isShowDescription)
		{
			// normal case - render background and instruction at top of screen
			SDL_RenderCopy(global::renderer, messageBGTexture, nullptr, &rect);
			instructionTexture->render();
			if (isShowItemIndex) indexTexture->render();
		}
		else if (isShowItemIndex)
		{
			indexTexture->render();
		}
	}

	void renderTitle(Uint8 alpha)
	{
		if (!isShowDescription)return;

		SDL_RenderCopy(global::renderer, messageBGTexture, nullptr, &overlay_bg_render_rect);
		SDL_SetTextureAlphaMod(titleTexture->getTexture(), alpha);
		titleTexture->render();

		// render the second message if using rolling message
		if (isScrollingTitle && scrollingLength - scrollingOffset < global::SCREEN_HEIGHT)
		{
			titleTexture->render(scrollingLength);
		}
	}

	void scrollingDescription()
	{
		// pause few frames in the begining
		if (scrollingPause > 0)
		{
			scrollingPause--;
			return;
		}

		// update offset and texture target y coordinate
		scrollingOffset += scrollingSpeed;
		titleTexture->scrollLeft(scrollingSpeed);

		// reset if the text is completely scrolled outside of screen
		if (scrollingOffset >= scrollingLength)
		{
			scrollingPause = 10;
			scrollingOffset = 0;
			titleTexture->updateTargetRect(TextTextureAlignment::topLeft);
		}
	}

	void scrollRight(bool showCurrent = true)
	{
		// get current and previous images
		auto prevIter = currentIter;
		prevIter = (prevIter != imageItems.begin()) ? --prevIter : --(imageItems.end());
		ImageItem *curr = *currentIter;
		ImageItem *prev = *prevIter;

		// update new text first
		updateMessageTexture(prev->getDescription());

		// scroll images
		double offset = 0;
		double step = 1.0 / scrollingFrames;
		for (int i = 0; i < scrollingFrames; i++)
		{
			double easing = easeInOutQuart(offset);
			SDL_RenderClear(global::renderer);
			if (showCurrent) curr->renderOffset(0, easing);
			prev->renderOffset(0, easing - 1);
			int text_alpha = static_cast<int>((i * 255.0) / scrollingFrames);
			renderTitle(text_alpha);
			renderInstruction();
			offset += step;
			SDL_RenderPresent(global::renderer);
			SDL_Delay(30);
		}
		prev->renderOffset(0, 0);
		renderTitle(255);
		renderInstruction();
		SDL_RenderPresent(global::renderer);

		// update iterator
		currentIter = prevIter;
		if (isShowItemIndex) updateIndexTexture();
	}

	void scrollLeft(bool showCurrent = true)
	{
		// get current and next images
		auto nextIter = currentIter;
		nextIter++;
		if (nextIter == imageItems.end())
			nextIter = imageItems.begin();
		ImageItem *curr = *currentIter;
		ImageItem *next = *nextIter;

		// update new text first
		updateMessageTexture(next->getDescription());

		// scroll images
		double offset = 1.0;
		double step = 1.0 / scrollingFrames;
		for (int i = 0; i < scrollingFrames; i++)
		{
			double easing = easeInOutQuart(offset);
			SDL_RenderClear(global::renderer);
			if (showCurrent) curr->renderOffset(0, easing - 1);
			next->renderOffset(0, easing);
			int text_alpha = static_cast<int>((i * 255.0) / scrollingFrames);
			renderTitle(text_alpha);
			renderInstruction();
			offset -= step;
			SDL_RenderPresent(global::renderer);
			SDL_Delay(30);
		}
		next->renderOffset(0, 0);
		renderTitle(255);
		renderInstruction();
		SDL_RenderPresent(global::renderer);

		// update iterator
		currentIter = nextIter;
		if (isShowItemIndex) updateIndexTexture();
	}

	void removeCurrentItem()
	{
		// fade out current item
		Uint8 old_alpha;
		auto texture = (*currentIter)->getTexture();
		SDL_GetTextureAlphaMod(texture, &old_alpha);
		for (int alpha = 255; alpha >= 0; alpha-=20)
		{
			SDL_SetTextureAlphaMod(texture, alpha);
			(*currentIter)->renderOffset(0, 0);
			SDL_RenderPresent(global::renderer);
			SDL_Delay(30);
		}
		SDL_SetTextureAlphaMod(texture, old_alpha);

		// get current iterator
		auto iter = currentIter;

		// if no more item in list after removing current item, exit with value 0
		if (imageItems.size() == 1) exit(0);

		// scroll the next item without showing the current item
		scrollRight(false);

		// remove current item from list
		imageItems.remove(*iter);

		// update index for display
		if (isShowItemIndex) updateIndexTexture();
	}

	void runDeleteCommand(int currentIndex)
	{
		if (deleteCommand.empty()) return;

		// convert current index to string
		auto currentIndexText = std::to_string(currentIndex);

		// get the provided command
		string cmd = deleteCommand;

		// replace placeholder with current index
		unsigned int index = 0;
		while (true)
		{
			// locate the substring to replace
			index = cmd.find(argumentPlaceholder, index);
			if (index == std::string::npos) break;
			// make the replacement
			cmd.replace(index, argumentPlaceholder.length(), currentIndexText);
			// advance index forward
			index += currentIndexText.length();
		}
		// run the cmd
		system(cmd.c_str());
	}

	void keyPress(const SDL_Event &event)
	{
		if (event.type != SDL_KEYDOWN)
			return;
		const auto sym = event.key.keysym.sym;
		switch (sym)
		{
		// button A (Space key)
		case SDLK_SPACE:
			if (isDeleteMode) {
				// delete mode: remove current item when A is pressed
				isDeleteMode = false; // reset flag to end delete mode
				int currentIndex = (*currentIter)->getIndex(); // get title of current item first
				runDeleteCommand(currentIndex);
				removeCurrentItem();
			}
			else {
				// normal case: exit and return index of current item 
				exit((*currentIter)->getIndex());
			}
			break;
		// button LEFT (Left arrow key)
		case SDLK_LEFT:
			if (isDeleteMode) return; // disable in delete mode
			if (isSwapLeftRight) scrollRight(); else scrollLeft();
			break;
		// button RIGHT (Right arrow key)
		case SDLK_RIGHT:
			if (isDeleteMode) return; // disable in delete mode
			if (isSwapLeftRight) scrollLeft(); else scrollRight();
			break;
		// button X (Left Shift key)
		case SDLK_LSHIFT:
			if (isDeleteMode) return; // disable in delete mode
			exit(255);
			break;
		// button Y (Left Alt key)
		case SDLK_LALT:
			if (!isAllowDeletion) return; // disable if option -d is not set
			if (isDeleteMode) return; // disable in delete mode
			isDeleteMode = true;
			isShowDescription = true; // ensure instruction & title is shown
			break;
		// button R1 (Backspace key)
		case SDLK_BACKSPACE:
			if (isDeleteMode) return; // disable in delete mode
			isShowDescription = !isShowDescription;
			break;
		}

		// button B (Left control key)
		if (event.key.keysym.mod == KMOD_LCTRL)
		{
			if (isDeleteMode) {
				// delete mode: cancel delete mode when B is pressed
				isDeleteMode = false;
			} else {
				// normal case: exit with return value 0
				exit(0);
			}
		}
	}

}

int main(int argc, char *argv[])
{
	// handle CLI options
	handleOptions(argc, argv);

	// Init SDL
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
	if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP) == 0)
	{
		printErrorAndExit("IMG_Init failed");
	}
	else
	{
		// Clear the errors for image libraries that did not initialize.
		SDL_ClearError();
	}

	// Init font
	if (TTF_Init() == -1)
		printErrorAndExit("TTF_Init failed: ", SDL_GetError());

	fontInstruction = TTF_OpenFont(fontPath.c_str(), fontSize);
	fontTitle = TTF_OpenFont(fontPath.c_str(), fontSize + 4);
	if (fontInstruction == nullptr || fontTitle == nullptr)
		printErrorAndExit("Font loading failed: ", TTF_GetError());

	// Hide cursor before creating the output surface.
	SDL_ShowCursor(SDL_DISABLE);

	// Create window and renderer
	SDL_Window *window = SDL_CreateWindow("Main", 0, 0, global::SCREEN_WIDTH, global::SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	global::renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (global::renderer == nullptr)
		printErrorAndExit("Renderer creation failed");

	prepareTextures();

	// load all image filenames and create imageItem instances
	loadImageFiles(argv[1]);
	if (imageItems.size() == 0)
		printErrorAndExit("Cannot load image list");

	// load all image titles
	loadImageDescriptions(argv[2]);

	// load first texture
	imageItems.back()->loadImage();
	imageItems.back()->createTexture();

	// load all other image fiies in background thread
	SDL_CreateThread(loadAllImages, "load_images", nullptr);

	// set current image as last image in list
	currentIter = --imageItems.end();

	// create title text texture and index texture
	updateMessageTexture((*currentIter)->getDescription());
	if (isShowItemIndex) updateIndexTexture();

	// Execute main loop of the window
	while (true)
	{
		// handle input events
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				keyPress(event);
				break;
			case SDL_QUIT:
				return 0;
				break;
			}
		}

		// render current image and title
		(*currentIter)->renderOffset(0, 0);
		if (isScrollingTitle) scrollingDescription();
		renderTitle(255);
		renderInstruction();
		SDL_RenderPresent(global::renderer);

		// delay for around 30 fps
		SDL_Delay(30);
	}

	// the lines below should never reach, just for code completeness
	SDL_DestroyTexture(messageBGTexture);
	SDL_DestroyRenderer(global::renderer);
	TTF_CloseFont(fontInstruction);
	TTF_CloseFont(fontTitle);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
