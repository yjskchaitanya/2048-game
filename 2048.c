#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GRID_SIZE 4
#define TILE_SIZE 100
#define TILE_PADDING 10
#define SCREEN_WIDTH (GRID_SIZE * (TILE_SIZE + TILE_PADDING) + TILE_PADDING)
#define SCREEN_HEIGHT SCREEN_WIDTH

int grid[GRID_SIZE][GRID_SIZE] = {0};
int score = 0;

void initGame();
void spawnTile();
int moveTiles(int direction);
void drawGrid(SDL_Renderer *renderer, TTF_Font *font);
int checkGameOver();
SDL_Color getTileColor(int value);

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("2048 Game", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!win) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(win);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("Roboto-Regular.ttf", 48); 
    if (!font) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    srand(time(NULL));
    initGame();

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                int moved = 0;
                switch (event.key.keysym.sym) {
                    case SDLK_UP:    moved = moveTiles(0); break;
                    case SDLK_DOWN:  moved = moveTiles(1); break;
                    case SDLK_LEFT:  moved = moveTiles(2); break;
                    case SDLK_RIGHT: moved = moveTiles(3); break;
                }
                if (moved) {
                    spawnTile();
                    if (checkGameOver()) {
                        printf("Game Over! Final Score: %d\n", score);
                        running = 0;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
        SDL_RenderClear(renderer);
        drawGrid(renderer, font);
        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

void initGame() {
    spawnTile();
    spawnTile();
}

void spawnTile() {
    int emptyTiles[GRID_SIZE * GRID_SIZE][2];
    int count = 0;

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (grid[i][j] == 0) {
                emptyTiles[count][0] = i;
                emptyTiles[count][1] = j;
                count++;
            }
        }
    }

    if (count > 0) {
        int randIndex = rand() % count;
        grid[emptyTiles[randIndex][0]][emptyTiles[randIndex][1]] = (rand() % 10 < 9) ? 2 : 4;
    }
}

int moveTiles(int direction) {
    int moved = 0;

    for (int i = 0; i < GRID_SIZE; ++i) {
        int temp[GRID_SIZE] = {0};
        int index = (direction == 0 || direction == 2) ? 0 : GRID_SIZE - 1;
        int step = (direction == 0 || direction == 2) ? 1 : -1;

        for (int j = 0; j < GRID_SIZE; ++j) {
            int value = (direction < 2) ? grid[(direction == 0 ? j : GRID_SIZE - 1 - j)][i] : grid[i][(direction == 2 ? j : GRID_SIZE - 1 - j)];
            if (value) {
                if (temp[index] == value) {
                    temp[index] *= 2;
                    score += temp[index];
                    index += step;
                } else if (temp[index]) {
                    index += step;
                    temp[index] = value;
                } else {
                    temp[index] = value;
                }
            }
        }

        for (int j = 0; j < GRID_SIZE; ++j) {
            int newValue = (direction < 2) ? temp[(direction == 0 ? j : GRID_SIZE - 1 - j)] : temp[(direction == 2 ? j : GRID_SIZE - 1 - j)];
            if (direction < 2) {
                if (grid[(direction == 0 ? j : GRID_SIZE - 1 - j)][i] != newValue) moved = 1;
                grid[(direction == 0 ? j : GRID_SIZE - 1 - j)][i] = newValue;
            } else {
                if (grid[i][(direction == 2 ? j : GRID_SIZE - 1 - j)] != newValue) moved = 1;
                grid[i][(direction == 2 ? j : GRID_SIZE - 1 - j)] = newValue;
            }
        }
    }

    return moved;
}

void drawGrid(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_Color textColor = {0, 0, 0, 255};

    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            SDL_Rect tile = {
                j * (TILE_SIZE + TILE_PADDING) + TILE_PADDING,
                i * (TILE_SIZE + TILE_PADDING) + TILE_PADDING,
                TILE_SIZE,
                TILE_SIZE
            };

            SDL_SetRenderDrawColor(renderer, 205, 193, 180, 255);
            if (grid[i][j] > 0) {
                SDL_Color tileColor = getTileColor(grid[i][j]);
                SDL_SetRenderDrawColor(renderer, tileColor.r, tileColor.g, tileColor.b, 255);
            }
            SDL_RenderFillRect(renderer, &tile);

            if (grid[i][j] > 0) {
                char buffer[8];
                sprintf(buffer, "%d", grid[i][j]);
                SDL_Surface *textSurface = TTF_RenderText_Solid(font, buffer, textColor);
                SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                int textW, textH;
                SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
                SDL_Rect textRect = {tile.x + (TILE_SIZE - textW) / 2, tile.y + (TILE_SIZE - textH) / 2, textW, textH};
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);
            }
        }
    }
}

int checkGameOver() {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (grid[i][j] == 0) return 0;
            if (i > 0 && grid[i][j] == grid[i - 1][j]) return 0;
            if (j > 0 && grid[i][j] == grid[i][j - 1]) return 0;
        }
    }
    return 1;
}

SDL_Color getTileColor(int value) {
    switch (value) {
        case 2: return (SDL_Color){238, 228, 218, 255};
        case 4: return (SDL_Color){237, 224, 200, 255};
        case 8: return (SDL_Color){242, 177, 121, 255};
        case 16: return (SDL_Color){245, 149, 99, 255};
        case 32: return (SDL_Color){246, 124, 95, 255};
        case 64: return (SDL_Color){246, 94, 59, 255};
        case 128: return (SDL_Color){237, 207, 114, 255};
        case 256: return (SDL_Color){237, 204, 97, 255};
        case 512: return (SDL_Color){237, 200, 80, 255};
        case 1024: return (SDL_Color){237, 197, 63, 255};
        case 2048: return (SDL_Color){237, 194, 46, 255};
        default: return (SDL_Color){60, 58, 50, 255};
    }
}

	

