#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <stdio.h>
#include <limits.h>

#if defined(__APPLE__) || defined(__linux__)
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#if __has_include("SDL2/SDL.h")
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#define SGL_IMPLEMENTATION
#include "sgl.h"

#include "logger.h"

sglBuffer* buf = NULL;
sglFont* font = NULL;

const uint8_t* keyboard;

struct UIState {
  int mx;
  int my;
  bool ml;
  bool mr;

  int hotitem;
  int activeitem;

  int kbditem;
  const uint8_t* keyentered;
  int keymod;

  int lastwidget;
} uistate = {0,0,false,false,0,0,0,0,0,0};

bool isMouseOverRegion(int x, int y, int w, int h)
{
	return uistate.mx >= x && uistate.my >= y &&
		uistate.mx < x + w && uistate.my < y + h;
}

void imgui_prepare(void)
{
	uistate.hotitem = 0;
}

void imgui_finish(void)
{
	if (!uistate.ml) {
		uistate.activeitem = 0;
	}
	if (uistate.keyentered && uistate.keyentered[SDL_SCANCODE_TAB]) {
		uistate.kbditem = 0;
	}
	uistate.keyentered = 0;
	// else if (uistate.activeitem == 0) {
	// 	uistate.activeitem = -1;
	// }
}

const int BTN_WIDTH = 64;
const int BTN_HEIGHT = 48;
bool button(int id, int x, int y)
{
	bool isHot = isMouseOverRegion(x, y, BTN_WIDTH, BTN_HEIGHT);
	if (isHot) {
		uistate.hotitem = id;
		if (uistate.activeitem == 0 && uistate.ml) {
			uistate.activeitem = id;
		}
	}

	bool isActive = uistate.activeitem == id;

	if (uistate.kbditem == 0) {
		uistate.kbditem = id;
	}

	if (uistate.kbditem == id) {
		sglDrawRectangle(buf, 0xff0000ff, x-1, y-1, BTN_WIDTH+1, BTN_HEIGHT+1);
	}

	if (isHot) {
		if (isActive) {
			sglFillRectangle(buf, 0x777777ff, x, y, BTN_WIDTH, BTN_HEIGHT);
		} else {
			sglFillRectangle(buf, 0xffffffff, x, y, BTN_WIDTH, BTN_HEIGHT);
		}
	} else {
		sglFillRectangle(buf, 0xaaaaaaff, x, y, BTN_WIDTH, BTN_HEIGHT);
	}

	// keyboard focus
	if (uistate.kbditem == id && uistate.keyentered) {

		if (uistate.keyentered[SDL_SCANCODE_TAB]) {
			uistate.kbditem = 0;

			if (uistate.keyentered[SDL_SCANCODE_TAB] && uistate.keyentered[SDL_SCANCODE_LSHIFT]) {
				uistate.kbditem = uistate.lastwidget;
			}

			uistate.keyentered = NULL; // absorb keyboard input so that next
									   // widget doesn't register tab
		} else if (uistate.keyentered[SDL_SCANCODE_RETURN]) {
			return true;
		}
	}
	uistate.lastwidget = id;

	return (!uistate.ml && uistate.hotitem == id && uistate.activeitem == id);
}

bool slider(int id, int x, int y, int max, int* value) {
	int ypos = ((256 - 16) * (*value)) / max;

	if (isMouseOverRegion(x + 8, y + 8, 16, 255)) {
		uistate.hotitem = id;
		if (uistate.activeitem == 0 && uistate.ml) {
			uistate.activeitem = id;
		}
	}

	sglFillRectangle(buf, 0x777777ff, x, y, 32, 256+16);

	if (uistate.activeitem == id || uistate.hotitem == id) {
		sglFillRectangle(buf, 0xffffffff, x + 8, y + 8 + ypos, 16, 16);
	} else {
		sglFillRectangle(buf, 0xaaaaaaff, x + 8, y + 8 + ypos, 16, 16);
	}

	if (uistate.activeitem == id) {
		int mousepos = uistate.my - (y + 8);
		if (mousepos < 0) mousepos = 0;
		if (mousepos > 255) mousepos = 255;
		int v = (mousepos * max) / 255;
		if (v != *value) {
			*value = v;
			return true;
		}
	}

	return false;
}

void render(void) {
	// sglClear(buf);
	static uint32_t bgcolor = 0x112233ff;
	sglFillRectangle(buf, bgcolor, 0, 0, buf->width, buf->height);

	imgui_prepare();

	button(1, 50, 50);

	button(2, 150, 50);

	static int count = 0;
	if (button(3, 50, 150)) {
		// sglDrawText(buf, "button 3 pressed", 0xffffffff, 8, 8, font);
		logInfo("button 3 pressed %d times", count);
		count++;
	}

	if (button(4, 150, 150)) {
		exit(0);
	}

	static int red = 0;
	static int green = 0;
	static int blue = 0;

	// if (slider(5, 300, 8, 255, &red)) {
	// 	bgcolor = (bgcolor & 0x00ffffff) | red << 24;
	// }
	// if (slider(6, 350, 8, 255, &green)) {
	// 	bgcolor = (bgcolor & 0xff00ffff) | green << 16;
	// }
	// if (slider(7, 400, 8, 255, &blue)) {
	// 	bgcolor = (bgcolor & 0xffff00ff) | blue << 8;
	// }


	imgui_finish();

	///////////////

	logDraw(buf, font, 256, 8);
}

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	const int WINDOW_WIDTH = 640;
	const int WINDOW_HEIGHT = 480;

	const int CANVAS_WIDTH = 640;
	const int CANVAS_HEIGHT = 480;

	SDL_Window* window = SDL_CreateWindow("imgui", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
		SDL_RENDERER_ACCELERATED /*  | SDL_RENDERER_PRESENTVSYNC */);

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR32,
		SDL_TEXTUREACCESS_STREAMING, CANVAS_WIDTH, CANVAS_HEIGHT);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	uint32_t* pixels
		= (uint32_t*)malloc(CANVAS_WIDTH * CANVAS_HEIGHT * sizeof(pixels));
	memset(pixels, 0, CANVAS_WIDTH * CANVAS_HEIGHT * sizeof(uint32_t));

	buf = sglCreateBuffer(pixels, CANVAS_WIDTH, CANVAS_HEIGHT, SGL_PIXELFORMAT_ABGR32);
	font = sglCreateFont("../res/xterm7x14.png", 7, 14, true);

	SDL_Event event;
	bool alive = true;
	uint32_t delta = 0;

	while (alive) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				alive = false;
			}
			// TODO: reload demos on window focus
			switch (event.type) {
				case SDL_KEYDOWN:
					break;
				case SDL_KEYUP:
					break;
			}


			uint32_t buttons = SDL_GetMouseState(&uistate.mx, &uistate.my);
			uistate.mx /= WINDOW_WIDTH / (float)CANVAS_WIDTH;
			uistate.my /= WINDOW_HEIGHT / (float)CANVAS_HEIGHT;
			uistate.ml = (buttons & SDL_BUTTON_LMASK) != 0;
			uistate.mr = (buttons & SDL_BUTTON_RMASK) != 0;

			uistate.keyentered = SDL_GetKeyboardState(NULL);

		}

		uint32_t tic = SDL_GetTicks();


		//////////////////////////////////////////////////////////////////////


		// sglFillRectangle(buf, 0x00ff00ff | (0xff << 8 * uistate.ml), 10, 10, 32, 32);

		render();
		

		//////////////////////////////////////////////////////////////////////

		SDL_UpdateTexture(texture, NULL, pixels, CANVAS_WIDTH * sizeof(uint32_t));
		SDL_RenderClear(renderer);
		SDL_Rect srcRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
		SDL_Rect dstRect = { 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT };
		SDL_RenderCopy(renderer, texture, &dstRect, &srcRect);
		SDL_RenderPresent(renderer);

		uint32_t toc = SDL_GetTicks();
		delta += toc - tic;

		static uint32_t acc = 0;
		acc++;
		if (delta > 1000) {
			printf("elapsed time: %.2fms\n", delta / (float)acc);
			delta = 0;
			acc = 0;
		}
	}

	sglFreeBuffer(buf);

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}

// nnoremap <leader>b <cmd>wa<cr><cmd>!pushd build/ && ./build.sh; popd<cr>
// nnoremap <leader>b <cmd>wa<cr><cmd>!pushd build/ && make demo; popd<cr>
// nnoremap <leader>b <cmd>wa<cr><cmd>make -C build<CR>
