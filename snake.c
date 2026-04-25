#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL3/SDL.h>

#define WINDOW_WIDTH		 800
#define WINDOW_HEIGHT		 600
#define SQUARE_SIZE		 20
#define FPS			 60
#define SNAKE_SPEED		 5
#define DELTA_TIME_MS		 1000 / FPS
#define DELTA_SNAKE_TIME_MS	 1000 / SNAKE_SPEED
#define SNAKE_MAX_LEN            ((WINDOW_HEIGHT / SQUARE_SIZE) * (WINDOW_WIDTH / SQUARE_SIZE))
#define PADDING                  1

typedef enum {
	PLAY = 1,
	END,
} GameStatus;

typedef struct {
	int x, y;
} Vec2;

int mod(int a, int b)
{
	return (a%b+b)%b;
}

int fill_snake(Vec2 *snake, int snake_len, int x, int y)
{
	if (snake_len < SNAKE_MAX_LEN) {
		snake[snake_len].x = x;
		snake[snake_len].y = y;
		++snake_len;
	}

	return snake_len;
}

void new_apple(Vec2 *apple)
{
	apple->x = mod(rand(), WINDOW_WIDTH / SQUARE_SIZE) * SQUARE_SIZE;
	apple->y = mod(rand(), WINDOW_HEIGHT / SQUARE_SIZE) * SQUARE_SIZE;
}

int main(void)
{
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
	}

	SDL_Window *window;
	SDL_Renderer *renderer;
	if (!SDL_CreateWindowAndRenderer("snake", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
		fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
	}

	bool quit = false;
	GameStatus status = PLAY;
	SDL_Event event = {0};
	SDL_Keycode current_direction = SDLK_RIGHT;
	int snake_len = 0;
	Vec2 snake[SNAKE_MAX_LEN] = {0};
	SDL_FRect snake_front[SNAKE_MAX_LEN] = {0};
	Vec2 apple = {0};
	SDL_FRect apple_front = {0};

	const int center_coordinate_x = WINDOW_WIDTH / 2 - SQUARE_SIZE;
	const int center_coordinate_y = WINDOW_HEIGHT / 2 - SQUARE_SIZE;

	snake_len = fill_snake(snake, snake_len, center_coordinate_x, center_coordinate_y);
	new_apple(&apple);

	Uint32 snake_next_move = SDL_GetTicks();
	while (!quit) {
		const Uint32 start = SDL_GetTicks();

		while (SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_EVENT_QUIT: {
				quit = true;
			} break;
			case SDL_EVENT_KEY_DOWN: {
				switch (current_direction) {
				case SDLK_UP: {
					if (event.key.key != SDLK_DOWN) {
						current_direction = event.key.key;
					}
				} break;
				case SDLK_DOWN: {
					if (event.key.key != SDLK_UP) {
						current_direction = event.key.key;
					}
				} break;
				case SDLK_LEFT: {
					if (event.key.key != SDLK_RIGHT) {
						current_direction = event.key.key;
					}
				} break;
				case SDLK_RIGHT: {
					if (event.key.key != SDLK_LEFT) {
						current_direction = event.key.key;
					}
				} break;
				}
			} break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0);
		SDL_RenderClear(renderer);

		if (status == PLAY && snake_next_move < SDL_GetTicks()) {
			for (int i = 1; i < snake_len; ++i) {
				switch (current_direction) {
				case SDLK_UP: {
					if (snake[0].x == snake[i].x && snake[0].y == (snake[i].y + SQUARE_SIZE))
						status = END;
				} break;
				case SDLK_DOWN: {
					if (snake[0].x == snake[i].x && snake[0].y + SQUARE_SIZE == snake[i].y)
						status = END;
				} break;
				case SDLK_LEFT: {
					if (snake[0].x == (snake[i].x + SQUARE_SIZE) && snake[0].y == snake[i].y)
						status = END;
				} break;
				case SDLK_RIGHT: {
					if ((snake[0].x + SQUARE_SIZE) == snake[i].x && snake[0].y == snake[i].y)
						status = END;
				} break;
				}
			}

			if (apple.x == snake[0].x && apple.y == snake[0].y) {
				snake_len = fill_snake(snake, snake_len, apple.x, apple.y);
				new_apple(&apple);
			}

			for (int i = snake_len; i >= 0; --i) {
				snake_next_move = SDL_GetTicks() + (DELTA_SNAKE_TIME_MS - snake_len);
				if (i == 0) {
					switch (current_direction) {
					case SDLK_UP: {
						snake[0].y = mod(snake[0].y - SQUARE_SIZE, WINDOW_HEIGHT);
					} break;
					case SDLK_DOWN: {
						snake[0].y = mod(snake[0].y + SQUARE_SIZE, WINDOW_HEIGHT);
					} break;
					case SDLK_LEFT: {
						snake[0].x = mod(snake[0].x - SQUARE_SIZE, WINDOW_WIDTH);
					} break;
					case SDLK_RIGHT: {
						snake[0].x = mod(snake[0].x + SQUARE_SIZE, WINDOW_WIDTH);
					} break;
					}
				} else {
					snake[i].x = snake[i-1].x;
					snake[i].y = snake[i-1].y;
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 0xFF, 0x0, 0x0, 0);

		apple_front.x = apple.x + PADDING;
		apple_front.y = apple.y + PADDING;
		apple_front.w = SQUARE_SIZE - PADDING;
		apple_front.h = SQUARE_SIZE - PADDING;

		SDL_RenderFillRect(renderer, &apple_front);

		for (int i = 0; i < snake_len; ++i) {
			snake_front[i].x = snake[i].x + PADDING;
			snake_front[i].y = snake[i].y + PADDING;
			snake_front[i].w = SQUARE_SIZE - PADDING;
			snake_front[i].h = SQUARE_SIZE - PADDING;
		}

		SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0);
		SDL_RenderFillRects(renderer, snake_front, snake_len);

		SDL_RenderPresent(renderer);

		const Uint32 duration = SDL_GetTicks() - start;
		if (duration < DELTA_TIME_MS) {
			SDL_Delay(DELTA_TIME_MS - duration);
		}
	}

	SDL_Quit();
	return 0;
}
