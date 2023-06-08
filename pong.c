#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// VirtualC IDE runs your Code in a 32Bit Virtual Maschine and compile your Code with the SimpleC Compiler
// The Preprocessor of the SimpleC Compiler don't set any common Operating System Define.
// So we can check these Defines, to set the value of the virtualc Define.
#if defined _WIN32 || defined _WIN64 || defined __APPLE__ || defined __linux__ || defined __unix__ || defined _POSIX_VERSION
#include <SDL/SDL.h> // Only needed if you want to play the game outside of Virtual-C IDE
#else
#include <sdlite.h> // Only needed if you want to play the game inside of Virtual-C IDE
#endif

#define FIELDWITH 1028	// Window width
#define FIELDHEIGHT 720 // Window height
#define NUMBERLENGTH 10 // number of possible scores
#define PLAYERMARGIN 20 // Distance of the player to the edge
#define BITSPERPIXEL 32

// Global variables
// Surfaces are the individual layers of the window
SDL_Surface *screen, *background, *ball, *playerOne, *playerTwo, *middleLine, *number[NUMBERLENGTH];

// The positions of each item in the window
SDL_Rect middleLinePosition, playerOnePosition, playerTwoPosition, ballPosition, numberPlayerOnePosition, numberPlayerTwoPosition;

// Is the game running?
bool run = true;

// Points of players
int pointsPlayerOne = 0, pointsPlayerTwo = 0;

// speed of the ball
float xspeed = 5;
float yspeed = 3;

// Force acting on the speed of the ball. This will make him faster over time!
const float force = (float)1.0005;

/**
 * @brief Helper function to convert an integer to a char
 *
 * @param i int from 0 to 9
 * @return char
 */
char intToChar(int i)
{
	return (char)i + '0';
}

/**
 * @brief Draws items on the window
 *
 * @return int
 * 0 if success
 */
int render(void)
{
	// Draw background image
	SDL_BlitSurface(background, 0, screen, 0);

	// Copy surface to window with center line
	SDL_BlitSurface(middleLine, 0, screen, &middleLinePosition);

	// Copy surfaces with player 1 and 2 to window
	SDL_BlitSurface(playerOne, 0, screen, &playerOnePosition);
	SDL_BlitSurface(playerTwo, 0, screen, &playerTwoPosition);

	// Number
	SDL_BlitSurface(number[pointsPlayerOne], 0, screen, &numberPlayerOnePosition);
	SDL_BlitSurface(number[pointsPlayerTwo], 0, screen, &numberPlayerTwoPosition);

	// Ball
	SDL_BlitSurface(ball, 0, screen, &ballPosition);

	// redraw screen
	SDL_Flip(screen);

	return EXIT_SUCCESS;
}

/**
 * @brief This function creates all the necessary conditions to run the program
 *
 * @return int
 * 0 if success
 */
int setup(void)
{
	// Initialize SDL with the graphics system.
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		printf("Couldn't init SDL! Fehler: %s\n", SDL_GetError());
		return -1;
	}

	// create window
	SDL_Surface *newScreen = SDL_SetVideoMode(FIELDWIDTH, FIELDHEIGHT, BITSPERPIXEL, SDL_SWSURFACE);
	if (!newScreen)
	{
		printf("Couldn't create SDL window! Error: %s\n", SDL_GetError());
		return -1;
	}
	else
	{
		screen = newScreen;
	}

	// set title of the window
	SDL_WM_SetCaption("SDL-Pong", "SDL-Pong");

	// Load background image
	SDL_Surface *newBackground = SDL_LoadBMP("img/Field.bmp");
	if (!newBackground)
	{
		fprintf(stderr, "Couldn't load bmp! Error: %s\n", SDL_GetError());
		return -1;
	}
	else
	{
		background = newBackground;
	}

	// load middle line
	SDL_Surface *newMiddleLine = SDL_LoadBMP("img/Middleline.bmp");
	if (!newMiddleLine)
	{
		fprintf(stderr, "Couldn't load Bmp! Error: %s\n", SDL_GetError());
		return -1;
	}
	else
	{
		middleLine = newMiddleLine;
	}
	// Set the position of the center line
	middleLinePosition.x = (int)(FIELDWIDTH / 2 - middleLine->w / 2);
	middleLinePosition.y = 0;

	// load player one
	SDL_Surface *newPlayerOne = SDL_LoadBMP("img/Player.bmp");
	if (!newPlayerOne)
	{
		fprintf(stderr, "Couldn't load Bmp! Error: %s\n", SDL_GetError());
		return -1;
	}
	else
	{
		playerOne = newPlayerOne;
	}

	// Set position of player one
	playerOnePosition.x = PLAYERMARGIN;
	playerOnePosition.y = FIELDHEIGHT / 2 - playerOne->h / 2;

	// Load player two
	SDL_Surface *newPlayerTwo = SDL_LoadBMP("img/Player.bmp");
	if (!newPlayerTwo)
	{
		fprintf(stderr, "Couldn't load Bmp! Error: %s\n", SDL_GetError());
		return -1;
	}
	else
	{
		playerTwo = newPlayerTwo;
	}

	// Set position of player two
	playerTwoPosition.x = FIELDWIDTH - PLAYERMARGIN - playerTwo->w;
	playerTwoPosition.y = FIELDHEIGHT / 2 - playerTwo->h / 2;

	// load ball
	SDL_Surface *newBall = SDL_LoadBMP("img/Ball.bmp");
	if (!newPlayerTwo)
	{
		fprintf(stderr, "Couldn't load Bmp! Error: %s\n", SDL_GetError());
		return -1;
	}
	else
	{
		ball = newBall;
	}

	// Set start position of the ball
	ballPosition.x = FIELDWIDTH / 2 - ball->w / 2 + PLAYERMARGIN;
	ballPosition.y = FIELDHEIGHT / 2 - ball->h / 2;

	// Load the score numbers
	for (int i = 0; i < NUMBERLENGTH; i++)
	{
		char string[20] = "img/";
		char count[2] = {intToChar(i)};
		strcat(string, count);
		strcat(string, ".bmp");
		SDL_Surface *newNumber = SDL_LoadBMP(string);
		if (!newNumber)
		{
			fprintf(stderr, "Couldn't load Bmp! Error: %s\n", SDL_GetError());
			return -1;
		}
		else
		{
			number[i] = newNumber;
		}
	}

	// Set position of the score for player one
	numberPlayerOnePosition.x = FIELDWIDTH / 2 - number[0]->w - 2 * PLAYERMARGIN;
	numberPlayerOnePosition.y = PLAYERMARGIN;

	// Set position of the score for player one
	numberPlayerTwoPosition.x = FIELDWIDTH / 2 + 2 * PLAYERMARGIN;
	numberPlayerTwoPosition.y = PLAYERMARGIN;

	// Render the game
	render();

	return EXIT_SUCCESS;
}

/**
 * @brief Is responsible for the game logic.
 *
 * @return int
 * 0 if success
 */
int loop(void)
{
	// Saves if a key on the keyboard is pressed
	bool keyPressed[SDLK_LAST]; // SDLK_LAST is deprecated and and replaced by SDL_NUM_SCANCODES with the value of 512
								// --> https://wiki.libsdl.org/MigrationGuide?highlight=%28SDLK_LAST%29
	memset(keyPressed, 0, sizeof(keyPressed));
	// program loop
	while (run)
	{
		// Saves incomming events
		SDL_Event event;

		// event loop
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				run = false;
				break;
			case SDL_KEYDOWN:
				keyPressed[event.key.keysym.sym] = true;
				break;
			case SDL_KEYUP:
				keyPressed[event.key.keysym.sym] = false;
				break;
			}
		}

		// move the ball
		ballPosition.x += (int)xspeed;
		ballPosition.y += (int)yspeed;

		// wall bounce
		if (ballPosition.y < 0 || ballPosition.y + ball->h > screen->h)
			yspeed *= -1;

		// Points player two
		if (ballPosition.x + ball->w < 0)
		{
			if (pointsPlayerTwo == 9)
			{
				run = false;
			}
			else
			{
				pointsPlayerTwo++;
				xspeed *= -1;
			}
			// Reset Ball Position
			ballPosition.x = FIELDWIDTH / 2 - ball->w / 2 + PLAYERMARGIN;
			ballPosition.y = FIELDHEIGHT / 2 - ball->h / 2;
			// Reset Ball speed
			xspeed = 5;
			if (yspeed < 0)
			{
				yspeed = -3;
			}
			else
			{
				yspeed = 3;
			}
		}

		// Points Player One
		if (ballPosition.x > FIELDWIDTH)
		{
			if (pointsPlayerOne == 9)
			{
				run = false;
			}
			else
			{
				pointsPlayerOne++;
				xspeed *= -1;
			}
			// Reset Ball Position
			ballPosition.x = FIELDWIDTH / 2 - ball->w / 2 - PLAYERMARGIN;
			ballPosition.y = FIELDHEIGHT / 2 - ball->h / 2;
			// Reset Ball speed
			xspeed = -5;
			if (yspeed < 0)
			{
				yspeed = -3;
			}
			else
			{
				yspeed = 3;
			}
		}

		// Player one bounce
		if (ballPosition.x < playerOnePosition.x + playerOne->w && ballPosition.y < playerOnePosition.y + playerOne->h && ballPosition.y + ball->h > playerOnePosition.y)
		{
			xspeed *= -1;
		}
		// Player two bounce
		if (ballPosition.x + ball->w > playerTwoPosition.x && ballPosition.y < playerTwoPosition.y + playerTwo->h && ballPosition.y + ball->h > playerTwoPosition.y)
			xspeed *= -1;

		// Keycontrol Player One
		// Player One Up
		if (keyPressed[SDLK_w] && playerOnePosition.y > 0)
		{
			if (playerOnePosition.y <= 5)
			{
				playerOnePosition.y = 0;
			}
			else
			{
				playerOnePosition.y -= 5;
			}
		}
		// Player One Down
		if (keyPressed[SDLK_s] && playerOnePosition.y + playerOne->h < screen->h)
			playerOnePosition.y += 5;

		// Keycontrol Player Two
		// Player Two Up
		if (keyPressed[SDLK_UP] && playerTwoPosition.y > 0)
		{
			if (playerTwoPosition.y <= 5)
			{
				playerTwoPosition.y = 0;
			}
			else
			{
				playerTwoPosition.y -= 5;
			}
		}
		// Player Two Down
		if (keyPressed[SDLK_DOWN] && playerTwoPosition.y + playerTwo->h < screen->h)
			playerTwoPosition.y += 5;

		// Stop Game
		if (keyPressed[SDLK_ESCAPE])
			run = false;

		// Add Speed to the Ball
		xspeed = xspeed * force;
		yspeed = yspeed * force;

		// Render new states
		if (render() != EXIT_SUCCESS)
		{
			fprintf(stderr, "Fehler in render()");
			return -1;
		}
	}

	return EXIT_SUCCESS;
}

int main(void)
{
	// Start Setup for Game
	if (setup() != EXIT_SUCCESS)
	{
		fprintf(stderr, "Error in setup()");
		return -1;
	}

	// Start game loop
	if (loop() != EXIT_SUCCESS)
	{
		fprintf(stderr, "Error in loop()");
		return -1;
	}

	// Free Memory
	SDL_FreeSurface(screen);
	SDL_FreeSurface(ball);
	SDL_FreeSurface(playerOne);
	SDL_FreeSurface(playerTwo);
	SDL_FreeSurface(middleLine);

	for (int i = 0; i < NUMBERLENGTH; i++)
		SDL_FreeSurface(number[i]);

	// Close Engine
	SDL_Quit();

	return EXIT_SUCCESS;
}