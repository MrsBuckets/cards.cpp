#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

#ifndef CONSTANTS
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CARD_WIDTH = 50;
const int CARD_HEIGHT = 100;
const int HAND_WIDTH = SCREEN_WIDTH / 2;
const int HAND_HEIGHT = CARD_HEIGHT * (1 + 3 * HAND_SPACE);
#endif

#ifndef GLOBALS
SDL_Window* gameWindow = NULL;
SDL_Surface* gameSurface = NULL;
SDL_Surface* cardFace = NULL;
SDL_Renderer* gameRenderer = NULL;
SDL_Texture* cardTexture = NULL;
SDL_Texture* card2Texture = NULL;
SDL_Event e;
SDL_Rect CARD_RECTANGLE = { -CARD_WIDTH,-CARD_HEIGHT,CARD_WIDTH,CARD_HEIGHT };
SDL_Rect HAND_RECTANGLE = { SCREEN_WIDTH / 4,SCREEN_HEIGHT - HAND_HEIGHT,HAND_WIDTH, HAND_HEIGHT };
SDL_Point mousePosition;
int UID = 0;
double HAND_SPACE = 0.1;
#endif

#ifndef CLASSES
class card {
private:
	SDL_Rect geometry;
	SDL_Texture* texture;
	int deltax, deltay;
	SDL_Point position;
	int UID;
public:
	card()
	{
		beingDragged = false;
		geometry = CARD_RECTANGLE;
		texture = cardTexture;
		UID = generateUID();
	}
	card(SDL_Rect cardRectangle, SDL_Texture* cardTexture)
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
	card * cardBase;
	int elements, size;
public:
	cardHolder()
	{
		size = 2;
		elements = 0;
		cardBase = (card*)malloc(2 * sizeof(card));
	}
	bool loadTextures()
	{
		return true;
	}
	card* getMember(int k)
	{
		return &cardBase[k];
	}
	card * createCard(SDL_Texture* texture, int x, int y)
	{
		if (elements >= size)
		{
			card* temp = (card*)malloc(size * 2 * sizeof(card));
			for (int i = 0; i < elements; i++)
			{
				temp[i] = cardBase[i];
			}
			free(cardBase);
			cardBase = temp;
			size *= 2;
		}
		cardBase[elements].setTexture(texture);
		cardBase[elements].setPosition(x, y);
		elements++;
		return cardBase + elements - 1;
	}
};
class cardContainer
{
private:
	card * * container;
	int UID, elements, capacity, cardDragged, dx, dy, x, y, ox, oy;
	double spacing;
	SDL_Rect geometry;
	SDL_Point position;
	bool notFull()
	{
		if (elements >= capacity)
		{
			capacity *= 2;
			card** temporaryContainer = (card**)malloc(capacity * sizeof(card*));
			if (temporaryContainer == NULL)
			{
				return false;
			}
			for (int i = 0; i < elements; i++)
			{
				temporaryContainer[i] = container[i];
			}
			free(container);
			container = temporaryContainer;
		}
		return true;
	}
public:
	cardContainer()
	{
		dx = dy = x = y = ox = oy = 0;
		cardDragged = -1;
		elements = 0;
		capacity = 2;
		UID = generateUID();
		container = (card**)malloc(capacity * sizeof(card*));
		spacing = HAND_SPACE;
		geometry = HAND_RECTANGLE;
		position = { geometry.x,geometry.y };
	}
	int size()
	{
		return elements;
	}
	void positionElements()
	{
		printf("0\n");
		int c = (double)(geometry.h - CARD_HEIGHT * (double)(spacing)) / (double)(CARD_HEIGHT * (1 + (double)(spacing)));
		printf("%d\n", geometry.h);
		int inrow = elements / c;
		printf("1\n");
		for (int i = 0; i < c; i++)
		{
			printf("2\n");
			if (inrow*CARD_WIDTH + (inrow + 1)*CARD_WIDTH*(double)(spacing) <= geometry.w)
			{
				printf("3\n");
				for (int j = i * inrow; j < (i + 1)*inrow; j++)
				{
					container[j]->setPosition(position.x + geometry.w / 2 - (inrow * CARD_WIDTH * (1 + (double)(spacing)) - CARD_WIDTH * (double)(spacing)) / 2 + CARD_WIDTH * (1 + (double)(spacing)) * j, position.y + CARD_HEIGHT * (double)(spacing)+CARD_HEIGHT * (1 + (double)(spacing)) * i);
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
	card* takeOutByUID(int uid)
	{
		card* cardTaken = NULL;
		bool found = false;
		for (int i = 0; i < elements; i++)
		{
			if (found)
			{
				container[i - 1] = container[i];
			}
			if (container[i]->getUID() == uid)
			{
				cardTaken = container[i];
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
	void eventHandler(SDL_Event* e)
	{
		switch (e->type)
		{
		case SDL_MOUSEBUTTONDOWN:
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
					break;
				}
			}
			break;
		case SDL_MOUSEBUTTONUP:
			cardDragged = -1;
			break;
		case SDL_MOUSEMOTION:
			if (cardDragged != -1)
			{
				container[cardDragged]->setPosition(mousePosition.x + dx, mousePosition.y + dy);
			}
			break;
		}
	}
	void renderContent(SDL_Renderer* renderer)
	{
		for (int i = elements - 1; i >= 0; i--)
		{
			container[i]->render(renderer);
		}
	}
	int getUID()
	{
		return UID;
	}
	~cardContainer()
	{
		free(container);
	}
};
cardHolder CARDBASE;

#endif

