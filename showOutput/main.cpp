#include <string>
#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>
#include <mutex>

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "global.h"
#include "fileutils.h"
#include "text_texture.h"

using namespace std;

class IncomingLine {
public:
    IncomingLine(const string & line) : line_(line) { }

    // once text is updated, existing texture will become outdated 
    void setText(const string & text) { line_ = text; needRebuiltTexture_ = true; }
    void appendText(const string & text) { line_.append(text); needRebuiltTexture_ = true; }
    const string & getText() const { return line_; }
    const TextTexture * getTexture() 
    {
        // delete outdated texture if required
        if (needRebuiltTexture_) {
            if (texture_ != nullptr) { 
                delete texture_;
                texture_ = nullptr;
            }
            needRebuiltTexture_ = false;
        }

        // build new texture if not found
        if (texture_ == nullptr) {
            if (line_.empty()) {
                texture_ = new TextTexture(" ", global::font, global::text_color);
            } else {
                texture_ = new TextTexture(line_, global::font, global::text_color);
            }
        }

        return texture_;
    }
private:
    string line_;
    bool needRebuiltTexture_ = true;
    TextTexture * texture_ = nullptr;
};

// global variables used in main.cpp
string programName;
string resourcePath = "res/";
int fontSize = 20;
size_t columns = 53; // unused
size_t rows = 10;   // unused
TextTexture* titleTexture = nullptr;
vector<IncomingLine> incomingLines;
mutex incomingLines_mutex;
size_t startIndex = 0;
size_t endIndex = 0;
size_t maxDisplayIndex = 0;
SDL_KeyCode downButton = SDLK_UNKNOWN;
int keyHoldWaitingTick = 0;

namespace {
    void printErrorAndExit(string message, string extraMessage = "")
	{
		cerr << programName << ": " << message;
		if (!extraMessage.empty())
			cerr << extraMessage;
		cerr << endl
			 << endl;
		exit(0);
	}

	void handleOptions(int argc, char *argv[])
    {
        // get program name
		programName = File_utils::getFileName(argv[0]);

    }
	void prepareTextures()
    {
        titleTexture = new TextTexture(
            "< Output >", 
            global::font,
            global::text_color,
            TextureAlignment::topCenter
        );
    }

    // try read text from input stream beffer until no input, or read '\n' or '\r' 
    bool tryGetText(istream & is, string & text, bool & newLine, bool & carriageReturn) {
        char ch;
        bool readAny = false;

        text.clear();
        newLine = false;
        carriageReturn = false;

        while (true)
        {
            auto charsRead = is.readsome(&ch, 1);
            if (charsRead != 1) break;
            readAny = true;
            if (ch == '\n') { newLine = true; break; }
            if (ch == '\r') { carriageReturn = true; break; }
            text.append(1, ch);
        }

        return readAny;
    }

    int readInput(void *) 
    {
        // init a lone string as buffer
        static string line;
        line.reserve(4000);

        bool isNewLine = false;
        bool isCarriageReturn =false;

        // handle input from stdin
        while (true)
        {
            // try read some text
            if (tryGetText(cin, line, isNewLine, isCarriageReturn))
            {
                cout << line;

                // add new text to line
                incomingLines_mutex.lock();
                incomingLines.back().appendText(line);
                incomingLines_mutex.unlock();

                // add new line if '\n' is read
                if (isNewLine) 
                {
                    cout << endl;
                    incomingLines_mutex.lock();
                    incomingLines.push_back(IncomingLine(""));
                    incomingLines_mutex.unlock();
                }

                // clear last line if '\r' is read
                if (isCarriageReturn) 
                {
                    cout << '\r';
                    incomingLines_mutex.lock();
                    incomingLines.back().setText("");
                    incomingLines_mutex.unlock();
                }
            }
            // sleep 0.01s to prevents busy looping
            usleep(100);
        }

        return 0;
    }

    void printScreen()
    {
        int marginTop = 10;
        int offsetY = marginTop;

        // render title
        titleTexture->render(0, offsetY);
        offsetY += titleTexture->getHeight();

        // get total line number
        incomingLines_mutex.lock();
        size_t size = incomingLines.size();
        incomingLines_mutex.unlock();

        // render all lines
        for (size_t i=startIndex; i<size; i++)
        {
            // lock mutex
            incomingLines_mutex.lock();

            // render line
            auto texture = incomingLines[i].getTexture();
            texture->render(0, offsetY);

            // adjust offset and starting index
            offsetY += texture->getHeight();

            // unlock mutex
            incomingLines_mutex.unlock();

            // update indices
            endIndex = i;
            if (i > maxDisplayIndex) {
                maxDisplayIndex = i;
            }

            // update start index if more new lines to show
            if (offsetY > global::SCREEN_WIDTH) {
                if (maxDisplayIndex < size - 1 && i < size - 1) {
                    startIndex++;
                }
                break;
            }
        }
    }

    void keyPress(const SDL_Event &event)
	{
        switch (event.key.keysym.sym)
        {
            // button A (Space key)
            case SDLK_SPACE:
                break;

            // button UP (Up arrow key)
            case SDLK_UP:
                if (startIndex > 0) startIndex--;
                downButton = SDLK_UP;
                keyHoldWaitingTick = 5;
                break;

            // button DOWN (Down arrow key)
            case SDLK_DOWN:
                if (endIndex < maxDisplayIndex) startIndex++;
                downButton = SDLK_DOWN;
                keyHoldWaitingTick = 5;
                break;

            // button LEFT (Left arrow key)
            case SDLK_LEFT:
                break;

            // button RIGHT (Right arrow key)
            case SDLK_RIGHT:
                break;

            // button X (Left Shift key)
            case SDLK_LSHIFT:
                break;

            // button Y (Left Alt key)
            case SDLK_LALT:
                break;

            // button R1 (Backspace key)
            case SDLK_BACKSPACE:
                break;
        }

        // button B (Left control key)
        if (event.key.keysym.mod == KMOD_LCTRL)
        {
            SDL_SetRenderDrawColor(global::renderer, 40, 40, 40, 255);
            SDL_RenderClear(global::renderer);
            SDL_RenderPresent(global::renderer);
            exit(0);
        }
    
    }

    void keyRelease(const SDL_Event &event) {
        switch (event.key.keysym.sym)
        {
            // button UP (Up arrow key)
            case SDLK_UP:
                downButton = SDLK_UNKNOWN;
                break;

            // button DOWN (Down arrow key)
            case SDLK_DOWN:
                downButton = SDLK_UNKNOWN;
                break;
        }
    }

    void checkKeyHold() {
        // wait for some ticks 
        if (keyHoldWaitingTick > 0) {
            keyHoldWaitingTick--;
            return;
        }

        switch (downButton) {
            // button UP (Up arrow key)
            case SDLK_UP:
                if (startIndex > 0) startIndex--;
                break;

            // button DOWN (Down arrow key)
            case SDLK_DOWN:
                if (endIndex < maxDisplayIndex) startIndex++;
                break;

            default:
                break;
        }
    }
}

int main(int argc, char *argv[])
{
    // this line is important for async non-blocking reading in background thread
    ios::sync_with_stdio(false);

    // handle arguments
    handleOptions(argc, argv);

	// Init SDL
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

	// Init font
	if (TTF_Init() == -1)
		printErrorAndExit("TTF_Init failed: ", SDL_GetError());

	global::font = TTF_OpenFont((resourcePath + "./SourceCodePro-Regular.ttf").c_str(), fontSize * 2);
	if (global::font == nullptr)
		printErrorAndExit("Font loading failed: ", TTF_GetError());

	// Hide cursor before creating the output surface.
	SDL_ShowCursor(SDL_DISABLE);

	// Create window and renderer
	SDL_Window *window = SDL_CreateWindow("Main", 0, 0, global::SCREEN_WIDTH, global::SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	global::renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (global::renderer == nullptr)
		printErrorAndExit("Renderer creation failed");

    prepareTextures();

    // initialize the first line
    incomingLines.push_back(IncomingLine(""));

    // read input from background thread
	SDL_CreateThread(readInput, "readInput", nullptr);

    int tick = 0;
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
			case SDL_KEYUP:
				keyRelease(event);
				break;
			case SDL_QUIT:
				return 0;
				break;
			}
		}

        // render setting items
        SDL_SetRenderDrawColor(global::renderer, 40, 40, 40, 255);
        SDL_RenderClear(global::renderer);
        printScreen();
        SDL_RenderPresent(global::renderer);

		// delay for around 30 fps
		SDL_Delay(20);
        
        // handle key hold every 5 ticks
        tick++;
        if (tick == 5) {
            tick = 0;
            checkKeyHold();
        }
    }

	SDL_DestroyRenderer(global::renderer);
	TTF_CloseFont(global::font);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}