#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

#ifndef FUNCTIONS
bool init();
bool loadMedia();
void close();
int generateUID();
SDL_Surface* loadSurface(std::string path);
SDL_Texture* loadTexture(std::string path);

bool init()
{
	bool success = true;
	if (SDL_INIT_VIDEO < 0)
	{
		printf("SDL could not initialise! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		gameWindow = SDL_CreateWindow("Card Game Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gameWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			gameRenderer = SDL_CreateRenderer(gameWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gameRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(gameRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				gameSurface = SDL_GetWindowSurface(gameWindow);
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags)& imgFlags))
				{
					printf("SDL_image could not initialise! SDL_image error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}
	return success;
}

bool loadMedia()
{
	bool success = true;
	std::string path = "cardtexture.png";
	cardTexture = loadTexture(path.c_str());
	if (cardTexture == NULL)
	{
		printf("File %s could not be loaded! SDL_Error: %s\n", path.c_str(), SDL_GetError());
		success = false;
	}
	path = "card 2.png";
	card2Texture = loadTexture(path.c_str());
	if (cardTexture == NULL)
	{
		printf("File %s could not be loaded! SDL_Error: %s\n", path.c_str(), SDL_GetError());
		success = false;
	}
	return success;
}

void close()
{
	SDL_FreeSurface(cardFace);
	cardFace = NULL;
	SDL_DestroyWindow(gameWindow);
	gameWindow = NULL;
	SDL_Quit();
}

SDL_Surface* loadSurface(std::string path)
{
	SDL_Surface* finalSurface = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load surface from %s. IMG Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		finalSurface = SDL_ConvertSurface(loadedSurface, gameSurface->format, NULL);
		if (finalSurface == NULL)
		{
			printf("Unable to optimaze image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		SDL_FreeSurface(loadedSurface);
	}
	return finalSurface;
}

SDL_Texture* loadTexture(std::string path)
{
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! IMG Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		newTexture = SDL_CreateTextureFromSurface(gameRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		SDL_FreeSurface(loadedSurface);
	}
	return newTexture;
}


int generateUID()
{
	return ++UID;
}
#endif