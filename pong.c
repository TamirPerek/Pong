#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// VirtualC IDE runs your Code in a 32Bit Virtual Maschine and compile your Code with the SimpleC Compiler
// The Preprocessor of the SimpleC Compiler don't set any common Operating System Define.
// So we can check these Defines, to set the value of the virtualc Define.
#if defined _WIN32 || defined _WIN64 || defined __APPLE__ || defined __linux__ || defined __unix__ || defined _POSIX_VERSION
#include <SDL/SDL.h>	// Wird nur benoetigt wenn man das Spiel ausserhalb von Virtual-C IDE spielen will
#else
#include <sdlite.h> 	// Wird nur benoetigt wenn man das Spiel innerhalb von Virtual-C IDE spielen will
#endif

#define FIELDWITH 1028  // Fensterbreite
#define FIELDHEIGHT 720 // Fensterhoehe
#define NUMBERLENGTH 10 // Anzahl an moeglichen Zahlen
#define PLAYERMARGIN 20 // Abstand des Spielers zum Rand
#define BITSPERPIXEL 32

// Globale Variablen
// Surfaces sind die einzelnen Layer des Fensters
SDL_Surface *screen, *background, *ball, *playerOne, *playerTwo, *middleLine, *number[NUMBERLENGTH];

// Die Positionen der einzelnen Elemente im Fenster
SDL_Rect middleLinePosition, playerOnePosition, playerTwoPosition, ballPosition, numberPlayerOnePosition, numberPlayerTwoPosition;

// Laeuft das Spiel?
bool run = true;

// Spieler Punkte
int pointsPlayerOne = 0, pointsPlayerTwo = 0;

// Geschwindigkeit des Balls
float xspeed = 5;
float yspeed = 3;

// Kraft, welche auf die Geschwindigkeit des Balles wirkt. Damit wird er schneller ueber Zeit!
const float force = (float)1.0005;

// Hilfsfunktion um ein Integer in ein Char zu wandeln
char intToChar(int i)
{
	return (char)i + '0';
}

// Zeichnet Elemente auf das Fenster
int render(void)
{
	// Background Image zeichnen
	SDL_BlitSurface(background, 0, screen, 0);

	// Surface mit Mittellinie auf Fensterkopieren
	SDL_BlitSurface(middleLine, 0, screen, &middleLinePosition);

	// Surfaces mit Spieler 1 und 2 auf Fensterkopieren
	SDL_BlitSurface(playerOne, 0, screen, &playerOnePosition);
	SDL_BlitSurface(playerTwo, 0, screen, &playerTwoPosition);

	// Nummer
	SDL_BlitSurface(number[pointsPlayerOne], 0, screen, &numberPlayerOnePosition);
	SDL_BlitSurface(number[pointsPlayerTwo], 0, screen, &numberPlayerTwoPosition);

	// Ball
	SDL_BlitSurface(ball, 0, screen, &ballPosition);

	// Screen neu zeichen
	SDL_Flip(screen);

	return EXIT_SUCCESS;
}

// Diese Funktion stellt alle noetigen Bedingungen her um das Programm auszufuehren
int setup(void)
{
	// SDL mit dem Grafiksystem initialisieren.
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		printf("Konnte SDL nicht initialisieren! Fehler: %s\n", SDL_GetError());
		return -1;
	}

	// Fenster erstellen
	SDL_Surface *newScreen = SDL_SetVideoMode(FIELDWITH, FIELDHEIGHT, BITSPERPIXEL, SDL_SWSURFACE);
	if (!newScreen)
	{
		printf("Konnte SDL-Fenster nicht erzeugen! Fehler: %s\n", SDL_GetError());
		return -1;
	}
	else
	{
		screen = newScreen;
	}

	// Fenster Title anzeigen
	SDL_WM_SetCaption("SDL-Pong", "SDL-Pong");

	// Background Image laden
	SDL_Surface *newBackground = SDL_LoadBMP("img/Field.bmp");
	if (!newBackground)
	{
		fprintf(stderr, "Konnte Bmp nicht laden! Fehler: %s\n", SDL_GetError());
		return -1;
	}
	else
	{
		background = newBackground;
	}

	// Mittellinie laden
	SDL_Surface *newMiddleLine = SDL_LoadBMP("img/Middleline.bmp");
	if (!newMiddleLine)
	{
		fprintf(stderr, "Konnte Bmp nicht laden! Fehler: %s\n", SDL_GetError());
		return -1;
	}
	else
	{
		middleLine = newMiddleLine;
	}
	// Position der Mittellinie festlegen
	middleLinePosition.x = (int)(FIELDWITH / 2 - middleLine->w / 2);
	middleLinePosition.y = 0;

	// Spieler Eins laden
	SDL_Surface *newPlayerOne = SDL_LoadBMP("img/Player.bmp");
	if (!newPlayerOne)
	{
		fprintf(stderr, "Konnte Bmp nicht laden! Fehler: %s\n", SDL_GetError());
		return -1;
	}
	else
	{
		playerOne = newPlayerOne;
	}

	// Position des Spieler Eins festlegen
	playerOnePosition.x = PLAYERMARGIN;
	playerOnePosition.y = FIELDHEIGHT / 2 - playerOne->h / 2;

	// Spieler Zwei laden
	SDL_Surface *newPlayerTwo = SDL_LoadBMP("img/Player.bmp");
	if (!newPlayerTwo)
	{
		fprintf(stderr, "Konnte Bmp nicht laden! Fehler: %s\n", SDL_GetError());
		return -1;
	}
	else
	{
		playerTwo = newPlayerTwo;
	}

	// Position des Spieler Zwei festlegen
	playerTwoPosition.x = FIELDWITH - PLAYERMARGIN - playerTwo->w;
	playerTwoPosition.y = FIELDHEIGHT / 2 - playerTwo->h / 2;

	// Ball laden
	SDL_Surface *newBall = SDL_LoadBMP("img/Ball.bmp");
	if (!newPlayerTwo)
	{
		fprintf(stderr, "Konnte Bmp nicht laden! Fehler: %s\n", SDL_GetError());
		return -1;
	}
	else
	{
		ball = newBall;
	}

	// Intitiale Position des Balls festlegen
	ballPosition.x = FIELDWITH / 2 - ball->w / 2 + PLAYERMARGIN;
	ballPosition.y = FIELDHEIGHT / 2 - ball->h / 2;

	// Laden der Nummern fuer die Spielstandsanzeige
	for (int i = 0; i < NUMBERLENGTH; i++)
	{
		char string[20] = "img/";
		char count[2] = {intToChar(i)};
		strcat(string, count);
		strcat(string, ".bmp");
		SDL_Surface *newNumber = SDL_LoadBMP(string);
		if (!newNumber)
		{
			fprintf(stderr, "Konnte Bmp nicht laden! Fehler: %s\n", SDL_GetError());
			return -1;
		}
		else
		{
			number[i] = newNumber;
		}
	}

	// Positionierung des Punktestands von Spieler Eins
	numberPlayerOnePosition.x = FIELDWITH / 2 - number[0]->w - 2 * PLAYERMARGIN;
	numberPlayerOnePosition.y = PLAYERMARGIN;

	// Positionierung des Punktestands von Spieler Zwei
	numberPlayerTwoPosition.x = FIELDWITH / 2 + 2 * PLAYERMARGIN;
	numberPlayerTwoPosition.y = PLAYERMARGIN;

	// Zeichnen des Spiels auf das Fenster
	render();

	return EXIT_SUCCESS;
}

// Ist fuer die Spiellogik zustaendig.
int loop(void)
{
	// Speichert ab das eine Taste gedrueckt wurde
	bool keyPressed[SDLK_LAST]; 	// SDLK_LAST is deprecated and and replaced by SDL_NUM_SCANCODES with the value of 512
							// --> https://wiki.libsdl.org/MigrationGuide?highlight=%28SDLK_LAST%29
	memset(keyPressed, 0, sizeof(keyPressed));
	// Program Schleife
	while (run)
	{
		// Speichert das eingehende Event ab
		SDL_Event event;

		// die Event-Schleife
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

		// Ball bewegung
		ballPosition.x += (int)xspeed;
		ballPosition.y += (int)yspeed;

		// Wand abpraller
		if (ballPosition.y < 0 || ballPosition.y + ball->h > screen->h)
			yspeed *= -1;

		// Punkte Spieler Zwei
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
			// Resett Ball Position
			ballPosition.x = FIELDWITH / 2 - ball->w / 2 + PLAYERMARGIN;
			ballPosition.y = FIELDHEIGHT / 2 - ball->h / 2;
			// Resett Ball geschwindigkeit
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
		if (ballPosition.x > FIELDWITH)
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
			// Resett Ball Position
			ballPosition.x = FIELDWITH / 2 - ball->w / 2 - PLAYERMARGIN;
			ballPosition.y = FIELDHEIGHT / 2 - ball->h / 2;
			// Resett Ballspeed
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

		// Spieler Eins abpraller
		if (ballPosition.x < playerOnePosition.x + playerOne->w && ballPosition.y < playerOnePosition.y + playerOne->h && ballPosition.y + ball->h > playerOnePosition.y)
		{
			xspeed *= -1;
		}
		// Spieler Zwei abpraller
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
	if (setup())// != EXIT_SUCCESS)
	{
		fprintf(stderr, "Fehler im Setup()");
		return -1;
	}

	// Start Gameloop
	if (loop() != EXIT_SUCCESS)
	{
		fprintf(stderr, "Fehler in loop()");
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