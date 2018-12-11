#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

#define CONSTANTS
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CARD_WIDTH = 50;
const int CARD_HEIGHT = 100;
const int HAND_WIDTH = SCREEN_WIDTH / 2;
double HAND_SPACE = 0.1;
const int HAND_HEIGHT = CARD_HEIGHT * (1 + 3 * HAND_SPACE);

#define FLAGS
const int DONE = 0;
const int CLICKEDONCARD = 7919;

#define GLOBALS
int UID = 0;
SDL_Window* gameWindow = NULL;
SDL_Surface* gameSurface = NULL;
SDL_Surface* cardFace = NULL;
SDL_Renderer* gameRenderer = NULL;
SDL_Texture* cardTexture = NULL;
SDL_Texture* card2Texture = NULL;
SDL_Texture* container1Texture = NULL;
SDL_Texture* container2Texture = NULL;
SDL_Event e;
SDL_Rect CARD_RECTANGLE = { -CARD_WIDTH,-CARD_HEIGHT,CARD_WIDTH,CARD_HEIGHT };
SDL_Rect HAND_RECTANGLE = { SCREEN_WIDTH / 4,SCREEN_HEIGHT - HAND_HEIGHT,HAND_WIDTH, HAND_HEIGHT };
SDL_Point mousePosition;

#define FUNCTIONS
bool init();
bool loadMedia();
void close();
int generateUID();
SDL_Surface* loadSurface(std::string path);
SDL_Texture* loadTexture(std::string path);


class card {
private:
	SDL_Rect geometry;
	SDL_Texture* texture;
	bool beingDragged = false;
	int deltax, deltay;
	SDL_Point position;
	int UID;
public:
	card()
	{
		geometry = CARD_RECTANGLE;
		texture = cardTexture;
		UID = generateUID();
	}
	card(SDL_Rect cardRectangle,SDL_Texture* cardTexture)
	{
		geometry = cardRectangle;
		texture = cardTexture;
		UID = generateUID();
	}
	void setTexture(SDL_Texture* newTexture)
	{
		texture = newTexture;
	}
	void render(SDL_Renderer* renderer)
	{
		SDL_RenderSetViewport(renderer, &geometry);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
	}
	void setPosition(int newx, int newy)
	{
		geometry.x = position.x = newx;
		geometry.y = position.y = newy;
	}
	SDL_Rect* getCardRectangle()
	{
		return &geometry;
	}
	int getXCoordinate()
	{
		return geometry.x;
	}
	int getYCoordinate()
	{
		return geometry.y;
	}
	int getUID()
	{
		return UID;
	}
	~card()
	{
		
	}
};

class cardHolder
{
private:
	card * cards;
	int elements, size;
public:
	cardHolder()
	{
		size = 2;
		elements = 0;
		cards = (card*)malloc(2 * sizeof(card));
	}
	bool loadTextures()
	{
		return true;
	}
	card* getMember(int k)
	{
		if (k >= 0 & k < elements)
		{
			return &cards[k];
		}
		else
		{
			printf("ERROR: Tried to access %d element of %d-element object, cardContainer UID : %d\n", k, elements, UID);
			return NULL;
		}
	}
	card * createCard(SDL_Texture* texture, int x, int y)
	{
		if (elements >= size)
		{
			card* temp = (card*)malloc(size * 2 * sizeof(card));
			for (int i = 0; i < elements; i++)
			{
				temp[i] = cards[i];
			}
			free(cards);
			cards = temp;
			size *= 2;
		}
		cards[elements].setTexture(texture);
		cards[elements].setPosition(x, y);
		elements++;
		return cards + elements - 1;
	}
};

cardHolder CARDBASE;

class cardContainer
{
private:
	card * * container;
	int UID, elements, capacity, cardDragged, dx, dy, x, y, ox, oy;
	double spacing;
	SDL_Rect geometry;
	SDL_Point position;
	SDL_Texture* texture;
	
public:
	cardContainer()
	{
		dx = dy = x = y = ox = oy = 0;
		cardDragged = -1;
		elements = 0;
		capacity = 10;
		UID = generateUID();
		container = (card**) malloc (10 * sizeof(card*));
		spacing = HAND_SPACE;
		geometry = HAND_RECTANGLE;
		position = { geometry.x,geometry.y };
	}
	bool notFull()
	{
		if (elements >= capacity)
		{
			capacity *= 2;
			card** temporaryContainer = (card**)malloc(capacity * sizeof(card*));
			if (temporaryContainer == NULL)
			{
				printf("CANNOT ALLOCATE MORE MEMORY :(\n");
				return false;
			}
			for (int i = 0; i < elements; i++)
			{
				temporaryContainer[i] = container[i];
			}
			free(container);
			// THIS PROGRAM MAY CORRUPT HEAP HERE, I DONT KNOW WHY THIS HAPPENS
			container = temporaryContainer;
		}
		return true;
	}
	int size()
	{
		return elements;
	}
	void setTexture(SDL_Texture* newTexture)
	{
		texture = newTexture;
	}
	void render(SDL_Renderer* renderer)
	{
		SDL_RenderSetViewport(renderer, &geometry);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
	}
	void positionElements()
	{
		//printf("0\n");
		int c = (double)(geometry.h - CARD_HEIGHT * (double)(spacing)) / (double)(CARD_HEIGHT * (1 + (double)(spacing)));
		//printf("%d\n", geometry.h);
		int inrow = elements/c;
		//printf("1\n");
		for (int i = 0; i < c; i++)
		{
			//printf("2\n");
			if (inrow*CARD_WIDTH + (inrow + 1)*CARD_WIDTH*(double)(spacing) <= geometry.w)
			{
				//printf("3\n");
				for (int j = i*inrow; j < (i+1)*inrow; j++)
				{
					container[j]->setPosition(position.x + geometry.w / 2 - (inrow * CARD_WIDTH * (1 + (double)(spacing)) - CARD_WIDTH * (double)(spacing)) / 2 + CARD_WIDTH * (1 + (double)(spacing)) * j, position.y + CARD_HEIGHT * (double)(spacing) + CARD_HEIGHT * (1 + (double)(spacing)) * i);
				}
			}
			else
			{

			}
		}
	}
	int insert(card* input)	//potential bug - inserting same card twice
	{
		//if (input not already in container)
		{
			if (notFull())
			{
				container[elements] = input;
				elements++;
				return elements;
			}
		}
		return -1;
	}
	int getElementUID(int k)
	{
		return container[k]->getUID();
	}
	card* takeOutByUID(int uid)
	{
		card* cardTaken = NULL;
		bool found = false;
		for (int i = 0; i < elements; i++)
		{
			if (found)
			{
				container[i-1] = container[i];
			}
			if (container[i]->getUID() == uid)
			{
				cardTaken = container[i];
				printf("Card succesfully removed\n");
				found = true;
			}
		}
		if (found)
		{
			container[elements--] = 0;
		}
		return cardTaken;
	}
	void moveToFront(int k)
	{
		if (k >= 0 && k < elements)
		{
			card * temp = container[k];
			for (int i = k; i > 0; i--)
			{
				container[i] = container[i - 1];
			}
			container[0] = temp;
		}
	}
	void setPosition(int newx, int newy)
	{
		position.x = newx;
		position.y = newy;
		geometry.x = newx;
		geometry.y = newy;
	}
	card* getElementByUID(int uid)
	{
		card* cardFound = NULL;
		for (int i = 0; i <= elements; i++)
		{
			if (container[i]->getUID() == uid)
			{
				cardFound = container[i];
				break;
			}
		}
		return cardFound;
	}
	card* getElement(int k)
	{
		if (k >= 0 && k < elements)
		{
			return container[k];
		}
		else return NULL;
	}
	SDL_Rect* getContainerRectangle()
	{
		return &geometry;
	}
	int eventHandler(SDL_Event* e)
	{
		switch (e->type)
		{
		case SDL_MOUSEBUTTONDOWN:
			//printf("MOUSEBUTTONDOWN IN CARDCONTAINER\n");
			for (int i = 0; i < elements && cardDragged == -1; i++)
			{
				if (SDL_PointInRect(&mousePosition, container[i]->getCardRectangle()))
				{
					dx = container[i]->getXCoordinate() - mousePosition.x;
					dy = container[i]->getYCoordinate() - mousePosition.y;
					ox = container[i]->getXCoordinate();
					oy = container[i]->getYCoordinate();
					cardDragged = i;
					moveToFront(cardDragged);
					cardDragged = 0;
					//printf("CLICKED ON CARD IN CARDCONTAINER\n");
					return CLICKEDONCARD;
				}
			}
			return DONE;
		case SDL_MOUSEBUTTONUP:
			//printf("MOUSEBUTTONUP IN CARDCONTAINER\n");
			cardDragged = -1;
			return DONE;
		case SDL_MOUSEMOTION:
			//printf("MOVING IN CARDCONTAINER\n");
			if (cardDragged!=-1)
			{
				//printf("Dragging card!\n");
				container[cardDragged]->setPosition(mousePosition.x + dx, mousePosition.y + dy);
			}
			return DONE;
		}
		return DONE;
	}
	int getCardDraggedIndex()
	{
		return cardDragged;
	}
	void renderContent(SDL_Renderer* renderer)
	{
		for (int i = elements - 1; i >= 0; i--)
		{
			container[i]->render(renderer);
		}
	}
	int getUID ()
	{
		return UID;
	}
	~cardContainer()
	{
		free(container);
	}
};

class containerHolder
{
private:
	int UID, capacity, elements, cardDraggedFrom;
	cardContainer** containers;
	bool notFull()
	{
		if (elements >= capacity)
		{
			capacity *= 2;
			cardContainer** temporaryContainer = (cardContainer**)malloc(capacity * sizeof(cardContainer*));
			if (temporaryContainer == NULL)
			{
				return false;
			}
			for (int i = 0; i < elements; i++)
			{
				temporaryContainer[i] = containers[i];
			}
			free(containers);
			containers = temporaryContainer;
		}
		return true;
	}
public:
	containerHolder()
	{
		UID = generateUID();
		capacity = 2;
		elements = 0;
		cardDraggedFrom = -1;
		containers = (cardContainer**)malloc(2 * sizeof(cardContainer*));
		
	}
	void moveToFront(int k)
	{
		if (k >= 0 && k < elements)
		{
			cardContainer * temp = containers[k];
			for (int i = k; i > 0; i--)
			{
				containers[i] = containers[i - 1];
			}
			containers[0] = temp;
		}
	}
	int insert(cardContainer* input)
	{
		if (notFull())
		{
			containers[elements] = input;
			elements++;
			return elements;
		}
		return -1;
	}
	int getUID()
	{
		return UID;
	}
	int eventHandler(SDL_Event* e)
	{
		switch (e->type)
		{
		case SDL_MOUSEBUTTONDOWN:
			//printf("MOUSEBUTTONDOWN IN CONTAINERHOLDER %d\n",cardDraggedFrom);
			for (int i = 0; i < elements && cardDraggedFrom == -1; i++)
			{
				if (SDL_PointInRect(&mousePosition, containers[i]->getContainerRectangle()) && containers[i]->eventHandler(e) == CLICKEDONCARD)
				{
					/*
					dx = container[i]->getXCoordinate() - mousePosition.x;
					dy = container[i]->getYCoordinate() - mousePosition.y;
					ox = container[i]->getXCoordinate();
					oy = container[i]->getYCoordinate();
					*/
					cardDraggedFrom = i;
					printf("CARD DRAGGING IN CONTAINERHOLDER %d\n", cardDraggedFrom);
					//moveToFront(cardDraggedFrom);
					//cardDraggedFrom = 0;
					return CLICKEDONCARD;
				}
			}
			return DONE;
		case SDL_MOUSEBUTTONUP:
			//printf("MOUSEBUTTONUP IN CONTAINERHOLDER %d\n",cardDraggedFrom);
			if (cardDraggedFrom != -1)
			{
				//printf("STOP CARD DRAGGING IN CONTAINERHOLDER %d\n",cardDraggedFrom);
				for (int i = 0; i < elements; i++)
				{
					if (i != cardDraggedFrom && SDL_PointInRect(&mousePosition, containers[i]->getContainerRectangle()))
					{
						printf("\nRELEASED CARD IN ANOTHER CONTAINER %d\n\n",containers[i]->size());
						int tempI = containers[cardDraggedFrom]->getCardDraggedIndex();
						int tempUID = containers[cardDraggedFrom]->getElementUID(tempI);
						card * temp = containers[cardDraggedFrom]->takeOutByUID(tempUID);
						containers[i]->insert(temp);
						containers[i]->positionElements();
						containers[cardDraggedFrom]->eventHandler(e);
						containers[cardDraggedFrom]->positionElements();
						cardDraggedFrom = -1;
						return DONE;
					}
				}
				containers[cardDraggedFrom]->eventHandler(e);
				containers[cardDraggedFrom]->positionElements();
				cardDraggedFrom = -1;
				return DONE;
			}
			return DONE;
		case SDL_MOUSEMOTION:
			if (cardDraggedFrom != -1)
			{
				//printf("MOVING IN CONTAINERHOLDER %d\n",cardDraggedFrom);
				containers[cardDraggedFrom]->eventHandler(e);
			}
			return DONE;
		}
		return DONE;
	}
	cardContainer* getMember(int k)
	{
		if (k >= 0 && k < elements)
		{
			return containers[k];
		}
		else
		{
			printf("ERROR - tried to access element %d from %d-element object, containerHolder UID: %d\n", k, elements, UID);
			return NULL;
		}
	}
	void render(SDL_Renderer* renderer)
	{
		for (int i = 0; i < elements; i++)
		{
			containers[i]->render(renderer);
			containers[i]->renderContent(renderer);
		}
	}
	~containerHolder()
	{
		free(containers); 
	}
};

containerHolder gameContainers;

int main(int argc, char* args[])
{
	if (!init())
	{
		printf("Failed to initialise!\n");
	}
	else
	{
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			bool quit = false;
			const int CONTAINERS = 2;
			cardContainer containers[CONTAINERS];
			containers[1].setPosition(SCREEN_WIDTH / 4,0);
			containers[0].setTexture(container1Texture);
			containers[1].setTexture(container2Texture);
			for (int i = 0; i < CONTAINERS; i++)
			{
				gameContainers.insert(containers + i);
			}
			const int CARDS = 4;
			card testCard[CARDS];
			for (int i = 0; i < CARDS; i++)
			{
				testCard[i].setPosition(100, 100);
				testCard[i].setTexture(cardTexture);
				if (i < 2) containers[0].insert(testCard + i);
				if (i >= 2) containers[1].insert(testCard + i);
			}
			for (int i = 0; i < CONTAINERS; i++)
			{
				gameContainers.getMember(i)->positionElements();
			}

			while (!quit)
			{
				while (SDL_PollEvent(&e) != 0)
				{
					//handle events
					switch (e.type) {
					case SDL_QUIT:
						quit = true;
						break;
					case SDL_MOUSEMOTION:
						SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
						//printf("Moving %d %d\n", mousePosition.x, mousePosition.y);
						break;
					case SDL_MOUSEBUTTONDOWN:
						SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
						printf("\n\nClick\n\n");
						break;
					case SDL_MOUSEBUTTONUP:
						SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
						printf("\n\nUnclick\n\n");
						break;
					}//handle events//handle events

					SDL_RenderClear(gameRenderer);
					gameContainers.eventHandler(&e);
					for (int i = 0; i < CONTAINERS; i++)
					{
						cardContainer * temp = gameContainers.getMember(i);
						temp->render(gameRenderer);
					}
					for (int i = 0; i < CONTAINERS; i++)
					{
						cardContainer * temp = gameContainers.getMember(i);
						temp->renderContent(gameRenderer);
					}
					SDL_RenderPresent(gameRenderer);
				}
				/*
				SDL_BlitSurface(cardFace, NULL, gameSurface, NULL);
				SDL_UpdateWindowSurface(gameWindow);
				SDL_Delay(2000);
				*/
			}

		}
		close();
		return 0;
	}
}

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
		gameWindow = SDL_CreateWindow("Card Game Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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
	path = "container1.png";
	container1Texture = loadTexture(path.c_str());
	if (container1Texture == NULL)
	{
		printf("File %s could not be loaded! SDL_Error: %s:\n", path.c_str(), SDL_GetError());
		success = false;
	}
	path = "container2.png";
	container2Texture = loadTexture(path.c_str());
	if (container2Texture == NULL)
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
