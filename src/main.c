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
		}

		uint32_t tic = SDL_GetTicks();


		//////////////////////////////////////////////////////////////////////

		// clear pixel buffer
		memset(pixels, 0, CANVAS_WIDTH * CANVAS_HEIGHT * sizeof(uint32_t));

		pixels[10 + 10 * CANVAS_WIDTH] = 0x00ff00ff;

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

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}

// nnoremap <leader>b <cmd>wa<cr><cmd>!pushd build/ && ./build.sh; popd<cr>
// nnoremap <leader>b <cmd>wa<cr><cmd>!pushd build/ && make demo; popd<cr>
// nnoremap <leader>b <cmd>wa<cr><cmd>make -C build<CR>
