//---------------------------------------------------------------------------
//Program: Skeleton for Task 1c � group assignment
//Author: Pascale Vacher
//Last updated: 24 February 2017
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//----- include libraries
//---------------------------------------------------------------------------

//include standard libraries
#include <iostream>	
#include <iomanip> 
#include <conio.h> 
#include <cassert> 
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <windows.h>		// Required for the sound commands to work
#include <MMSystem.h>		// As above, required for the sound command
using namespace std;

//include our own libraries
#include "ConsoleUtils.h"	//for Clrscr, Gotoxy, etc.

//---------------------------------------------------------------------------
//----- define constants
//---------------------------------------------------------------------------

//defining the size of the grid
const int  SIZEX(16);    	//horizontal dimension
const int  SIZEY(11);		//vertical dimension
//defining symbols used for display of the grid and content
const char BEAR('@');   	//bear
const char PROTECTEDBEAR('£');
const char DRILLBEAR('€');
const char TUNNEL(' ');    	//tunnel
const char WALL('#');    	//border
const char WAL('+');		//wall for level 3
const char BOMB('0');		//bomb
const char PILL('P');		//pill
const char DRILL('D');		//drill
const char DETONATOR('T');	//detonator
const char LOCK('&');
const char KEY('F');
const char EXIT('X');
//defining the command letters to move the bear on the maze
const int  UP(72);			//up arrow
const int  DOWN(80); 		//down arrow
const int  RIGHT(77);		//right arrow
const int  LEFT(75);		//left arrow
//defining the other command letters
const char QUIT('Q');		//to end the game
const char CHEAT('C');		//disable bombs
const char RULES('R');      //display menu
const char BACK('B');

struct Item {
	int x, y;
	char symbol;
	bool isProtected;
	bool drillMode;
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main() {
	//function declarations (prototypes)
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], vector<Item>& bears, vector<Item>& bombs, vector<Item>& pills, vector<Item>& locks, string playerName, int& level, int& scoreMove, int& rescued, bool& onBomb, bool& onWal);
	void paintGame(const char g[][SIZEX], string mess, string name, int scoreMove, int rescued, int previousScore, int level, const vector<Item>& bears);
	bool wantsToQuit(const int key);
	bool enableCheatMode(const int key);
	bool isArrowKey(const int k);
	bool readSave(string);
	int  getKeyPress();
	void floatRules(int);
	void mainMenu(int, string, string, string&, int&, int&, bool&, int&);
	bool displayRules(const int);
	void entryScreen(string, string&);
	void levelSelection(string, string, string&, int&, int&, bool&);
	void sortBears(vector<Item>& bears, const int key, const int rescued);
	void updateGameData(char g[][SIZEX], char maze[][SIZEX], int, vector<Item>& bears, vector<Item>& bombs, vector<Item>& pills, vector<Item>& locks, const int key, string& mess, bool& finishGame, bool& levelCompleted, bool& onBomb, bool& onWal, bool cheatMode, int& rescued, int level, bool& drillMode);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], const vector<Item> bears, const vector<Item> bombs, const vector<Item> pills, const vector<Item> locks);
	void readScoreTxt(string, string, int&);
	void recordPlayerTxt(string, string, int, int, int);
	void resetStats(vector<Item>& bombs, vector<Item>& locks, int& scoreMove, int& rescued);
	void saveGame(char g[][SIZEX], string, int, int, int, bool, bool);
	void endLevel(string levelString);
	void endProgram(bool finishGame, vector<Item>& bears);

	//Colors in game
	HANDLE  hconsole;
	hconsole = GetStdHandle(STD_OUTPUT_HANDLE);

	//Reset the random function
	srand(time(NULL));

	//local variable declarations
	bool finishGame = false;
	bool levelCompleted = false;
	bool cheatMode = false;
	bool unlockMode = false;
	bool drillMode = false;
	bool onBomb = false;
	bool onWal = false;
	int key;
	int level(1), scoreMove(0), rescued(0), moves(0), previousScore(500), levelRecord(level);
	char grid[SIZEY][SIZEX];	//grid for display
	char maze[SIZEY][SIZEX];	//structure of the maze
	string message;	//current message to player
	string playerName;
	string levelString = to_string(level);
	vector<Item> bears;
	vector<Item> bombs;
	vector<Item> pills;
	vector<Item> locks;

	//action...
	entryScreen(message, playerName);
	mainMenu(moves, message, playerName, levelString, level, levelRecord, unlockMode, key);
	if (!readSave(playerName)) {
		levelSelection(message, playerName, levelString, level, levelRecord, unlockMode);
		readScoreTxt(playerName, levelString, previousScore);
	}
	initialiseGame(grid, maze, bears, bombs, pills, locks, playerName, level, scoreMove, rescued, onBomb, onWal);	//initialise grid (incl. walls & bear)
	paintGame(grid, message, playerName, scoreMove, rescued, previousScore, level, bears);			//display game info, modified grid & messages
	showMessage(clBlack, clWhite, 40, 8, "LET'S START!          ");
	key = getKeyPress(); 			//read in  selected key: arrow or letter command
	do {
		while ((!wantsToQuit(key)) && (finishGame == false)) {
			//while user does not want to quit
			if (isArrowKey(key))
			{
				sortBears(bears, key, rescued);
				scoreMove++;
				moves++;
				updateGameData(grid, maze, moves, bears, bombs, pills, locks, key, message, finishGame, levelCompleted, onBomb, onWal, cheatMode, rescued, level, drillMode);		//move bear in that direction
				//updateGrid(grid, maze, bears, bombs);			//update grid information - disabled for testing, updateGrid exist in updateGameData
			}
			else{
				if (enableCheatMode(key)) {
					if (cheatMode == false) {
						cheatMode = true;
						PlaySound("cheat.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
						scoreMove = 500;
						showMessage(clBlack, clWhite, 40, 7, "CHEAT MODE ACTIVATED!");
						showMessage(clBlack, clWhite, 40, 8, "Bombs Deactivated!   ");

						Beep(523, 200);				// Beep sounds when cheatmode activated.
						Beep(523, 200);				// Beep sounds when cheatmode activated.
						Beep(523, 200);				// Beep sounds when cheatmode activated.
					}
					else {
						cheatMode = false;
						PlaySound(NULL, NULL, NULL);
						showMessage(clBlack, clBlack, 40, 7, "                     ");
						showMessage(clBlack, clBlack, 40, 8, "                     ");
					}
				}
				else {
					if (displayRules(key)) {
						floatRules(level);
					}
					else {
						message = "INVALID KEY!         ";	//set 'Invalid key' message
					}
				}
				message = "                     ";
			}
			paintGame(grid, message, playerName, scoreMove, rescued, previousScore, level, bears);	//display game info, modified grid & messages
			key = getKeyPress(); 		// display menu & read in next option

			// initialise the next level
			if (levelCompleted == true) {
				levelCompleted = false;	// reset the state of a completed level in the next one
				cheatMode = false;		// reset cheat mode in the next level
				showMessage(clBlack, clBlack, 40, 7, "                     ");
				showMessage(clBlack, clBlack, 40, 8, "                     ");

				// record the player's highest level completion
				if ((scoreMove <= previousScore) && (unlockMode == false) && (bears.empty())) {
					recordPlayerTxt(playerName, levelString, scoreMove, level + 1, levelRecord);
				}

				endLevel(levelString);	// display level completion message
				resetStats(bombs, locks, scoreMove, rescued);	// reset the scores to 0
				level++;	// increment the level count
				levelString = to_string(level);	// create a string version of the level count for the filename
				readScoreTxt(playerName, levelString, previousScore);	// read the player's highest score of the next level
				initialiseGame(grid, maze, bears, bombs, pills, locks, playerName, level, scoreMove, rescued, onBomb, onWal);	// reinitialise the game with the next level
				paintGame(grid, message, playerName, scoreMove, rescued, previousScore, level, bears);	// update the next level's descriptions
			}
		}
		PlaySound(NULL, NULL, NULL);

		endProgram(finishGame, bears);	//display final message
		Clrscr();

		if (finishGame == true) {
			finishGame = false;
			moves = 0;
			scoreMove = 0;
			level = 1;
			levelString = to_string(level);
		}

		mainMenu(moves, message, playerName, levelString, level, levelRecord, unlockMode, key);

		// record the player's highest record after game completion
		if ((scoreMove <= previousScore) && (finishGame == true) && (unlockMode == false) && (bears.empty())) {
			recordPlayerTxt(playerName, levelString, scoreMove, level, levelRecord);
		}

		// save current playthrough
		if ((key == 4) && (finishGame == false)) {
			saveGame(grid, playerName, level, scoreMove, rescued, onBomb, onWal);
		}
	} while (key != 4);
	return 0;
}

void entryScreen(string message, string& playerName) {
	//Show the first info screen to the player with score

	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);

	//display game title and bear image
	showMessage(clDarkGrey, clGreen, 15, 2, "	   _      _                                 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _  ");
	showMessage(clDarkGrey, clGreen, 15, 3, "   : `.--.' ;              _....,_          /                                  /| ");
	showMessage(clDarkGrey, clGreen, 15, 4, "   .'      `.      _..--'''       `-._     /_ _ _ _ _ _ _ _  _ _ _ _ _ _ _ _ _/ | ");
	showMessage(clDarkGrey, clGreen, 15, 5, "  :          :_.-''                  .`.   |                                 |  | ");
	showMessage(clDarkGrey, clGreen, 15, 6, "  :  6    6  :                     :  '.;  |     THREE  BEARS  GAME - FOP    |  | ");
	showMessage(clDarkGrey, clGreen, 15, 7, "  :          :                      `..';  |            GROUP RR             |  | ");
	showMessage(clDarkGrey, clGreen, 15, 8, "  `: .----. :'                          ;  |       Greg, Andy and Irek       |  | ");
	showMessage(clDarkGrey, clGreen, 15, 9, "    `._Y _.'               '           ;   |      Computer Science - 2017    | /  ");
	showMessage(clDarkGrey, clGreen, 15, 10, "      'U'      .'          `.         ;    |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|/   ");
	showMessage(clDarkGrey, clGreen, 15, 11, "         `:   ;`-..___       `.     .'`.                    ||                    ");
	showMessage(clDarkGrey, clGreen, 15, 12, "          :   :  :    ```'''''``.    `.  `.                 ||                    ");
	showMessage(clDarkGrey, clGreen, 15, 13, "       .'     ;..'            .'       `.'`                 ||                    ");
	showMessage(clDarkGrey, clGreen, 15, 14, "      `.......'              `........-'`                   ||                    ");

	showMessage(clBlack, clWhite, 55, 18, "______________________");

	//print auxiliary messages if any
	showMessage(clBlack, clWhite, 47, 16, message);	//display current message

	showMessage(clBlack, clWhite, 40, 18, "Enter your name: ");
	getline(cin, playerName);								// Reads in the name of the player and stores it in "String name".
	
	// ensure that string length does not exceed 20 characters
	if (playerName.length() > 20) {
		playerName.resize(20);
	}
}

void levelSelection(string message, string playerName, string& levelString, int& level, int& levelRecord, bool& unlockMode) {

	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	void readPlayerRecord(string, int&);
	void reDrawBear();
	void displayLevels(int);

	// read the player's highest reached level
	readPlayerRecord(playerName, levelRecord);

	// display available levels and bear image
	showMessage(clDarkGrey, clGreen, 59, 5, "                                ");
	showMessage(clDarkGrey, clGreen, 59, 6, "                                ");
	showMessage(clDarkGrey, clGreen, 59, 7, "                                ");
	showMessage(clDarkGrey, clGreen, 59, 8, "                                ");
	showMessage(clDarkGrey, clGreen, 59, 9, "                                ");

	// display the level options if unlocked
	reDrawBear();
	displayLevels(levelRecord);

	showMessage(clBlack, clWhite, 40, 18, "Please select the desired level: ");
	getline(cin, levelString);	// Read the level option

	// unlock all levels if the "u" button has been pressed
	if (levelString == "u") {
		unlockMode = true;	// prevent file recording
		levelRecord = 4;	// unlock all levels in unlock mode
		displayLevels(levelRecord);	// display all levels in unlock mode
		showMessage(clBlack, clWhite, 40, 19, "All levels unlocked. Choose a level.");
		Beep(523, 200);		// Beep sounds when cheatmode activated
		Beep(523, 200);		// Beep sounds when cheatmode activated
		Beep(523, 200);		// Beep sounds when cheatmode activated
		showMessage(clBlack, clWhite, 40, 18, "Please select the desired level: ");
		getline(cin, levelString);	// Re-read a new level
	}

	// convert the string to int to declare the newly selected level as the current one
	level = atoi(levelString.c_str());	

	// set level to 1 if the selection does not represent any levels given
	if (!((level >= 1) && (level <= levelRecord))) {
		level = 1;
		levelString = to_string(level);
	}

	// Clear the level selection screen
	Clrscr(); 
}

//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], vector<Item>& bears, vector<Item>& bombs, vector<Item>& pills, vector<Item>& locks, string player, int& level, int& scoreMove, int& rescued, bool& onBomb, bool& onWal) {
	//initialise grid & place bear in middle
	void loadGame(string, vector<Item>& bears, char m[][SIZEX], int&, int&, int&, bool&, bool&);
	void setInitialMazeStructure(char maze[][SIZEX]);
	void setMazeStructureLevel2(char maze[][SIZEX]);
	void setMazeStructureLevel3(char maze[][SIZEX]);
	void setMazeStructureLevel4(char maze[][SIZEX]);
	void setInitialDataFromMaze(char maze[][SIZEX], vector<Item>& bears, vector<Item>& bombs, vector<Item>& locks, bool, bool);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], const vector<Item> bears, const vector<Item> bombs, const vector<Item> pills, const vector<Item> locks);
	bool readSave(string);

	if (readSave(player))
	{
		Clrscr();
		loadGame(player, bears, maze, level, scoreMove, rescued, onBomb, onWal);
	}
	else {
		// draw the maze dependent on the level count
		switch (level)
		{
		case 1:
			setInitialMazeStructure(maze);		//initialise maze
			break;
		case 2:
			setMazeStructureLevel2(maze);		//Level 2
			break;
		case 3:
			setMazeStructureLevel3(maze);		//Level 3
			break;
		case 4:
			setMazeStructureLevel4(maze);		//Level 4
			break;
		default:
			break;
		}
	}

	setInitialDataFromMaze(maze, bears, bombs, locks, onBomb, onWal);	//initialise bear's position
	updateGrid(grid, maze, bears, bombs, pills, locks);		//prepare grid
}

void setInitialMazeStructure(char maze[][SIZEX]) {
	//set the position of the walls in the maze
	//initialise maze configuration
	int initialMaze[SIZEY][SIZEX] 	//local array to store the maze structure
	= { { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 2, 3, 0, 3, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 1 },
		{ 1, 2, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1 },
		{ 1, 2, 1, 0, 1, 5, 0, 0, 0, 3, 0, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 1, 3, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
		{ 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, };

	// with 1 for wall, 0 for tunnel, etc. 
	//copy into maze structure
	for (int row(0); row < SIZEY; ++row) {
		for (int col(0); col < SIZEX; ++col) {
			switch (initialMaze[row][col])
			{
				case 0: maze[row][col] = TUNNEL; break;
				case 1: maze[row][col] = WALL; break;
				case 2: maze[row][col] = BEAR; break;
				case 3: maze[row][col] = BOMB; break;
				case 4:	maze[row][col] = DETONATOR; break;
				case 5: maze[row][col] = EXIT; break;
			}
		}
	}
}

void setMazeStructureLevel2(char maze[][SIZEX]) {
	//set the position of the walls in the maze
	//initialise maze configuration
	int maze2[SIZEY][SIZEX] 	//local array to store the maze structure
	= { { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 2, 3, 7, 0, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 1 },
		{ 1, 2, 1, 0, 1, 1, 1, 1, 3, 1, 6, 1, 0, 1, 0, 1 },
		{ 1, 2, 1, 0, 1, 5, 0, 0, 0, 3, 0, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1 },
		{ 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
		{ 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, };

	// with 1 for wall, 0 for tunnel, etc. 
	//copy into maze structure
	for (int row(0); row < SIZEY; ++row) {
		for (int col(0); col < SIZEX; ++col) {
			switch (maze2[row][col])
			{
			case 0: maze[row][col] = TUNNEL; break;
			case 1: maze[row][col] = WALL; break;
			case 2: maze[row][col] = BEAR; break;
			case 3: maze[row][col] = BOMB; break;
			case 5: maze[row][col] = EXIT; break;
			case 6: maze[row][col] = LOCK; break;
			case 7:	maze[row][col] = KEY; break;
			}
		}
	}
}

void setMazeStructureLevel3(char maze[][SIZEX]) {
	//set the position of the walls in the maze
	//initialise maze configuration
	int maze3[SIZEY][SIZEX] 	//local array to store the maze structure
		= { { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1 },
		{ 1, 0, 0, 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1 },
		{ 1, 3, 0, 0, 3, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 2, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1 },
		{ 1, 3, 0, 0, 3, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1 },
		{ 1, 0, 0, 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, };

	// with 1 for wall, 0 for tunnel, etc. 
	//copy into maze structure
	for (int row(0); row < SIZEY; ++row) {
		for (int col(0); col < SIZEX; ++col) {
			switch (maze3[row][col])
			{
			case 0: maze[row][col] = TUNNEL; break;
			case 1: maze[row][col] = WALL; break;
			case 2: maze[row][col] = BEAR; break;
			case 3: maze[row][col] = BOMB; break;
			case 5:	maze[row][col] = EXIT; break;
			}
		}
	}
}

void setMazeStructureLevel4(char maze[][SIZEX]) {
	//set the position of the walls in the maze
	//initialise maze configuration
	int maze4[SIZEY][SIZEX] 	//local array to store the maze structure
	= { { 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 2, 0, 0, 0, 3, 0,  0, 0, 9, 0, 0, 0, 0, 0, 1 },
		{ 1, 2, 9, 0, 9, 9, 0,  9, 0, 9, 0, 3, 3, 3, 0, 1 },
		{ 1, 2, 9, 0, 0, 0, 0,  9, 0, 9, 0, 3, 5, 3, 0, 1 },
		{ 1, 0, 9, 3, 9, 9, 9,  9, 0, 9, 0, 3, 3, 3, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0,  3, 0, 0, 0, 0, 9, 0, 0, 1 },
		{ 1, 9, 0, 9, 9, 9, 9,  9, 9, 9, 0, 9, 9, 9, 0, 1 },
		{ 1, 0, 0, 0, 9, 4, 9, 10, 0, 9, 0, 9, 8, 9, 0, 1 },
		{ 1, 3, 9, 0, 9, 9, 9,  9, 6, 9, 0, 9, 6, 9, 0, 1 },
		{ 1, 7, 0, 0, 9, 3, 0,  0, 0, 0, 0, 3, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1 }, };

	// with 1 for wall, 0 for tunnel, etc. 
	//copy into maze structure
	for (int row(0); row < SIZEY; ++row) {
		for (int col(0); col < SIZEX; ++col) {
			switch (maze4[row][col])
			{
			case 0: maze[row][col] = TUNNEL; break;
			case 1: maze[row][col] = WALL; break;
			case 2: maze[row][col] = BEAR; break;
			case 3: maze[row][col] = BOMB; break;
			case 4: maze[row][col] = DETONATOR; break;
			case 5: maze[row][col] = EXIT; break;
			case 6: maze[row][col] = LOCK; break;
			case 7:	maze[row][col] = KEY; break;
			case 8:	maze[row][col] = DRILL; break;
			case 9:	maze[row][col] = WAL; break;
			case 10: maze[row][col] = PILL; break;
			}
		}
	}
}

void setInitialDataFromMaze(char maze[][SIZEX], vector<Item>& bears, vector<Item>& bombs, vector<Item>& locks, bool onBomb, bool onWal) {
	//extract bear's coordinates from initial maze info
	for (int row(0); row < SIZEY; ++row) {
		for (int col(0); col < SIZEX; ++col) {
			switch (maze[row][col])
			{
				case BEAR:
				{
					Item bear = { col, row, BEAR, false, false };
					bears.push_back(bear);
					maze[row][col] = TUNNEL;
					break;
				}
				case PROTECTEDBEAR:
				{
					Item bear = { col, row, PROTECTEDBEAR, true, false };
					bears.push_back(bear);
					if (onBomb != true) {
						maze[row][col] = TUNNEL;
					}
					else {
						maze[row][col] = BOMB;
					}
					break;
				}
				case DRILLBEAR:
				{
					Item bear = { col, row, DRILLBEAR, false, true };
					bears.push_back(bear);
					if (onWal != true) {
						maze[row][col] = TUNNEL;
					}
					else {
						maze[row][col] = WAL;
					}
					break;
				}
				case BOMB:
				{
					const Item bomb = { col, row, BOMB };
					bombs.push_back(bomb);
					maze[row][col] = TUNNEL;
					break;
				}
				case LOCK:
				{
					const Item lock = { col, row, LOCK };
					locks.push_back(lock);
					maze[row][col] = TUNNEL;
					break;
				}
				default:
					break;
					//will work for other bombs too
			}
		}
	}
}

//---------------------------------------------------------------------------
//----- update grid state
//---------------------------------------------------------------------------

void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], const vector<Item> bears, const vector<Item> bombs, const vector<Item> pills, const vector<Item> locks) {
	//update grid configuration after each move
	void setMaze(char g[][SIZEX], const char b[][SIZEX]);
	void placeItems(char g[][SIZEX], const vector<Item>& bombs, const vector<Item>& locks, const vector<Item>& pills);
	void placeBears(char g[][SIZEX], const vector<Item>& bears);

	setMaze(grid, maze);	//reset the empty maze configuration into grid
	placeItems(grid, bombs, locks, pills);	// set bombs in grid - remove them if they're deleted
	placeBears(grid, bears);	// set bears in grid
}

void setMaze(char grid[][SIZEX], const char maze[][SIZEX]) {
	//reset the empty/fixed maze configuration into grid
	for (int row(0); row < SIZEY; ++row) {
		for (int col(0); col < SIZEX; ++col) {
			grid[row][col] = maze[row][col];
		}
	}
}

void placeBears(char grid[][SIZEX], const vector<Item>& bears) {
	//place bear at its new position in grid
	for (size_t pos(0); pos < bears.size(); ++pos) {
		grid[bears.at(pos).y][bears.at(pos).x] = bears.at(pos).symbol;
	}
}

void placeItems(char grid[][SIZEX], const vector<Item>& bombs, const vector<Item>& locks,  const vector<Item>& pills) {
	for (size_t pos(0); pos < bombs.size(); ++pos)
	{
		grid[bombs.at(pos).y][bombs.at(pos).x] = bombs.at(pos).symbol;
	}

	for (size_t pos(0); pos < locks.size(); ++pos)
	{
		grid[locks.at(pos).y][locks.at(pos).x] = locks.at(pos).symbol;
	}

	for (size_t pos(0); pos < pills.size(); ++pos)
	{
		grid[pills.at(pos).y][pills.at(pos).x] = pills.at(pos).symbol;

	}
}

//---------------------------------------------------------------------------
//----- move the bear
//---------------------------------------------------------------------------
void updateGameData(char g[][SIZEX], char maze[][SIZEX], int moves, vector<Item>& bears, vector<Item>& bombs, vector<Item>& pills, vector<Item>& locks, const int key, string& mess, bool& finishGame, bool& levelCompleted, bool& onBomb, bool& onWal, bool cheatMode, int& rescued, int level, bool& drillMode) {
	//move bear in required direction
	bool isArrowKey(const int k);
	void setKeyDirection(int k, int& dx, int& dy);
	void createThePill(char g[][SIZEX], const int, vector<Item>& pills);
	void paintGrid(const char g[][SIZEX], const vector<Item>& bears);
	assert(isArrowKey(key));

	//calculate direction of movement for given key
	int dx(0), dy(0);
	setKeyDirection(key, dx, dy);

	if ((moves % 10 == 0) && (level != 3)){
		pills.clear();
		createThePill(g, moves, pills);
		moves = 0;
	}

	for (size_t pos(0); pos < bears.size(); ++pos) {
		//check new target position in grid and update game data (incl. bear coordinates) if move is possible
		switch (g[bears.at(pos).y + dy][bears.at(pos).x + dx])
		{			//...depending on what's on the target position in grid...
			case TUNNEL:		//can move
				bears.at(pos).y += dy;	//go in that Y direction
				bears.at(pos).x += dx;	//go exitin that X direction
				onBomb = false;
				onWal = false;
				mess = "                                     ";
				break;
			case WALL:  		//hit a wall and stay there
				//cout << '\a';	//beep the alarm - Disabled as the alarm is annoying
				break;
			case WAL:
				if (bears.at(pos).drillMode != true) {
					onWal = true;
					cout << '\a';
				}
				else {
					bears.at(pos).y += dy;	//go in that Y direction
					bears.at(pos).x += dx;	//go in that X direction
				}
				break;
			case BEAR:
				bears.at(pos).y += 0;	//go in that Y direction
				bears.at(pos).x += 0;	//go in that X direction
				break;
			case BOMB:			//hit a bomb and the bear disappears and the game ends
				if (!cheatMode && !bears.at(pos).isProtected) {
					cout << '\a';		//beep the alarm
					mess = "BEAR DIES!                           ";
					PlaySound("bomb.wav", NULL, SND_ASYNC | SND_FILENAME);
					onBomb = true;
					bears.at(pos).symbol = ' ';
					finishGame = true;
					break;
				}
				else {
					bears.at(pos).y += dy;	//go in that Y direction
					bears.at(pos).x += dx;	//go in that X direction
					break;
				}
			case DETONATOR:
				PlaySound("detonator.wav", NULL, SND_ASYNC | SND_FILENAME);
				bears.at(pos).y += dy;	//go in that Y direction
				bears.at(pos).x += dx;	//go in that X direction	
				if (cheatMode == false)
				{
					bombs.clear();
					mess = "BOMBS DEACTIVATED!   ";	//set 'Invalid key' message
				}
				break;
			case KEY:
				bears.at(pos).y += dy;	//go in that Y direction
				bears.at(pos).x += dx;	//go in that X direction	
				locks.clear();
				mess = "LOCK REMOVED!        ";	//set 'Invalid key' message
				break;
			case PILL:
				PlaySound("pill.wav", NULL, SND_ASYNC | SND_FILENAME);
				bears.at(pos).isProtected = true;
				bears.at(pos).y += dy;
				bears.at(pos).x += dx;
				bears.at(pos).symbol = PROTECTEDBEAR;
				pills.clear();
				break;
			case DRILL:
				bears.at(pos).drillMode = true;
				bears.at(pos).y += dy;	//go in that Y direction
				bears.at(pos).x += dx;	//go in that X direction
				bears.at(pos).symbol = DRILLBEAR;
				mess = "DRILL MODE ACTIVATED!                ";
				break;
			case EXIT:
				cout << '\a';		//beep the alarm
				rescued++;
				mess = "BEAR RESCUED!                        ";
				bears.erase(bears.begin() + pos);
				if (rescued == 3){
					bears.clear();
				}
				if (bears.empty() == true) {
					if (level != 4) {
						levelCompleted = true;
						PlaySound("exit.wav", NULL, SND_ASYNC | SND_FILENAME);
					}
					else {
						finishGame = true;	// confirms the game's completion
					}
				}
		}
		updateGrid(g, maze, bears, bombs, pills, locks);
	}
}

void createThePill(char g[][SIZEX], const int moves, vector<Item>& pills) {

	int col = rand() % 16 + 1;
	int row = rand() % 11 + 1;

	while (pills.empty())
	{
		if (g[row][col] == TUNNEL){
			Item pill = { col, row, PILL };
			pills.push_back(pill);
			g[row][col] = PILL;
		}
		else{
			col = rand() % 16 + 1;
			row = rand() % 11 + 1;
		}
	}

}
//---------------------------------------------------------------------------
//----- process key
//---------------------------------------------------------------------------
void setKeyDirection(const int key, int& dx, int& dy) {
	//calculate direction indicated by key
	bool isArrowKey(const int k);
	//assert(isArrowKey(key));
	switch (key)	//...depending on the selected key...
	{
	case LEFT:  	//when LEFT arrow pressed...
		dx = -1;	//decrease the X coordinate
		dy = 0;
		break;
	case RIGHT: 	//when RIGHT arrow pressed...
		dx = +1;	//increase the X coordinate
		dy = 0;
		break;
	case UP: 		//when UP arrow pressed...
		dx = 0;
		dy = -1;	//decrease the Y coordinate
		break;
	case DOWN: 		//when DOWN arrow pressed...
		dx = 0;
		dy = +1;	//increase the Y coordinate
		break;
	}
}

int getKeyPress() {
	//get key or command (in uppercase) selected by user
	//KEEP THIS FUNCTION AS GIVEN
	int keyPressed;
	keyPressed = _getch();			//read in the selected arrow key or command letter
	while (keyPressed == 224) { 	//ignore symbol following cursor key
		keyPressed = _getch();
	}
	return toupper(keyPressed);		//return it in uppercase 
}

bool isArrowKey(const int key) {
	//check if the key pressed is an arrow key (also accept 'K', 'M', 'H' and 'P')
	return (key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN);
}
bool wantsToQuit(const int key) {
	//check if the user wants to quit (when key is 'Q' or 'q')
	return toupper(key) == QUIT;
}

bool enableCheatMode(const int key) {
	// Checks if user has enabled cheat mode (disable bombs)
	return toupper(key) == CHEAT;
}

bool displayRules(const int key) {
	// Checks if user has enabled cheat mode (disable bombs)
	return toupper(key) == RULES;
}

bool goBack(const int key) {
	// Checks if user has enabled cheat mode (disable bombs)
	return toupper(key) == BACK;
}

//---------------------------------------------------------------------------
//----- display info on screen
//---------------------------------------------------------------------------

string tostring(char x) {
	std::ostringstream os;
	os << x;
	return os.str();
}

void paintGame(const char g[][SIZEX], string mess, string playerName, int scoreMove, int rescued, int previousScore, int level, const vector<Item>& bears) {
	//display game title, messages, maze, bear and other bombs on screen
	string tostring(char x);
	string prevScore = to_string(previousScore);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	void paintGrid(const char g[][SIZEX], const vector<Item>& bears);
	void reduceVisibility(const char g[][SIZEX], const vector<Item>& bears);

	// retrieve current time and date.
	auto t = time(nullptr);
	auto tm = *localtime(&t);

	ostringstream oss;
	oss << put_time(&tm, "%H:%M:%S %d/%m/%Y");
	auto str = oss.str();

	// clear level completion messages after transition
	showMessage(clBlack, clWhite, 40, 22, "                                   ");
	showMessage(clBlack, clWhite, 40, 23, "                                   ");

	// display game title
	showMessage(clYellow, clBlack, 0, 0, "THREE BEARS GAME - GROUP 1RR");

	// display number of rescued bears
	showMessage(clDarkGrey, clYellow, 0, 2, "RESCUED:        ");
	switch (rescued)
	{
		case 1: showMessage(clDarkGrey, clGreen, 9, 2, "@      ");
			break;
		case 2: showMessage(clDarkGrey, clGreen, 9, 2, "@@     ");
			break;
		case 3: showMessage(clDarkGrey, clGreen, 9, 2, "@@@    ");
			break;
		default:
			break;
	}

	// calculate length of current player line and then output the correct number of blank spaces
	string nameSpaces;
	for (int spaces = 37 - (playerName.length() + 17); spaces > 0; spaces--) {
		nameSpaces = nameSpaces + " ";
	}

	// calculate length of score line and then output the correct number of blank spaces
	string scoreSpaces;
	for (int spaces = 37 - (prevScore.length() + 17); spaces > 0; spaces--) {
		scoreSpaces = scoreSpaces + " ";
	}

	// display details
	showMessage(clDarkGrey, clYellow, 40, 0, " CURRENT PLAYER: " + playerName + nameSpaces);
	showMessage(clDarkGrey, clYellow, 40, 1, " PREVIOUS SCORE: " + prevScore + scoreSpaces);
	showMessage(clDarkGrey, clYellow, 40, 2, " DATE AND TIME : " + str + " ");

	// display moves and rescued bears
	showMessage(clBlack, clWhite, 40, 5, "MOVES SO FAR: ");
	cout << scoreMove;
	showMessage(clBlack, clWhite, 40, 6, "BEARS OUT: ");
	cout << rescued;
	showMessage(clDarkGrey, clYellow, 40, 17, "For help press 'R'");
	showMessage(clDarkGrey, clYellow, 40, 18, "To activate cheat mode, press 'C'");
	showMessage(clDarkGrey, clYellow, 40, 19, "To return to main menu, press 'Q'");




	//Print current level
	string currentLevelDisplay = "Current level: " + to_string(level);
	showMessage(clDarkGrey, clWhite, 40, 4, currentLevelDisplay);

	// print auxiliary messages if any
	showMessage(clBlack, clWhite, 40, 8, mess);	//display current message

	// display grid contents
	if (level > 2) {
		reduceVisibility(g, bears);
	}
	else {
		paintGrid(g, bears);
	}

	// symbol description
	showMessage(clBlack, clWhite, 0, 16, "BEAR         ");
	showMessage(clBlack, clGreen, 10, 16, "@     ");
	showMessage(clBlack, clWhite, 0, 17, "BOMB          ");
	showMessage(clBlack, clRed, 10, 17, "0     ");
	showMessage(clBlack, clWhite, 0, 18, "DETONATOR      ");
	showMessage(clBlack, clYellow, 10, 18, "T     ");
	showMessage(clBlack, clWhite, 0, 19, "EXIT         ");
	showMessage(clBlack, clWhite, 10, 19, "X     ");
	if (level == 2) {
		showMessage(clBlack, clWhite, 0, 18, "KEY            ");
		showMessage(clBlack, clCyan, 10, 18, "F     ");
		showMessage(clBlack, clWhite, 0, 19, "LOCK         ");
		showMessage(clBlack, clCyan, 10, 19, "&     ");
		showMessage(clBlack, clWhite, 0, 20, "EXIT         ");
		showMessage(clBlack, clWhite, 10, 20, "X     ");
	}
	else if (level == 3) {
		showMessage(clBlack, clWhite, 0, 16, "BEAR         ");
		showMessage(clBlack, clGreen, 10, 16, "@     ");
		showMessage(clBlack, clWhite, 0, 17, "BOMB          ");
		showMessage(clBlack, clRed, 10, 17, "0     ");
		showMessage(clBlack, clWhite, 0, 18, "DETONATOR      ");
		showMessage(clBlack, clYellow, 10, 18, "T     ");
		showMessage(clBlack, clWhite, 0, 19, "EXIT         ");
		showMessage(clBlack, clWhite, 10, 19, "X     ");
		showMessage(clBlack, clWhite, 0, 20, "             ");
		showMessage(clBlack, clBlack, 10, 20, "      ");
	}
	else if (level == 4) {
		showMessage(clBlack, clWhite, 0, 19, "KEY            ");
		showMessage(clBlack, clCyan, 10, 19, "F     ");
		showMessage(clBlack, clWhite, 0, 20, "LOCK         ");
		showMessage(clBlack, clCyan, 10, 20, "&     ");
		showMessage(clBlack, clWhite, 0, 21, "DRILL          ");
		showMessage(clBlack, clMagenta, 10, 21, "D     ");
		showMessage(clBlack, clWhite, 0, 22, "PILL         ");
		showMessage(clBlack, clCyan, 10, 22, "P     ");
		showMessage(clBlack, clWhite, 0, 23, "EXIT         ");
		showMessage(clBlack, clWhite, 10, 23, "X     ");
	}
}

void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message) {
	Gotoxy(x, y);
	SelectBackColour(backColour);
	SelectTextColour(textColour);
	cout << message;
}

void paintGrid(const char g[][SIZEX], const vector<Item>& bears) {
	//display grid content on screen
	void setColor(int);
	void resetColor();

	Gotoxy(0, 4);

	int pos(0);

	// Paint the whole Grid, item by item
	for (int row(0); row < SIZEY; ++row) {
		for (int col(0); col < SIZEX; ++col) {
			SelectBackColour(clBlack);	//	Sets the background colour to black
			SelectTextColour(clWhite);	//	Sets the walls colour to white

			switch (g[row][col])		//	Looks for special characters and paint them for chosen colours
			{
			case BEAR:
			{
				setColor(10);			// Paint the Bear green
				cout << g[row][col];
				resetColor();			// Reset the colour back to white
				break;
			}
			case DRILLBEAR:
				setColor(13);			// Paint the Bear magenta
				cout << BEAR;
				resetColor();			// Reset the colour back to white
				break;
			case PROTECTEDBEAR:
				setColor(2);
				cout << BEAR;
				resetColor();
				break;
			case BOMB:
			{
				setColor(12);			// Paint the Bomb red
				cout << g[row][col];
				resetColor();			// Reset the colour back to white
				break;
			}
			case DETONATOR:
			{
				setColor(14);			// Paint the Detonator yellow
				cout << g[row][col];
				resetColor();			// Reset the colour back to white
				break;
			}
			case PILL:
				setColor(11);
				cout << g[row][col];
				resetColor();
				break;
			case LOCK:
			{
				setColor(11);			// Paint the Lock cyan
				cout << g[row][col];
				resetColor();			// Reset the colour back to white
				break;
			}
			case KEY:
			{
				setColor(11);			// Paint the Key cyan
				cout << g[row][col];
				resetColor();			// Reset the colour back to white
				break;
			}
			case DRILL:
			{
				setColor(13);			// Paint the Drill magenta
				cout << g[row][col];
				resetColor();			// Reset the colour back to white
				break;
			}
			case EXIT:
				SelectBackColour(clWhite);
				SelectTextColour(clBlack);			
				cout << g[row][col];
				break;

			default:
				cout << g[row][col];
				break;
			}
		}
		cout << endl;
	}
}

void reduceVisibility(const char g[][SIZEX], const vector<Item>& bears) {
	//display grid content on screen
	void setColor(int);
	void resetColor();
	bool visible(false);

	Gotoxy(0, 4);

	int pos(0);

	// Paint the whole Grid, item by item
	for (int row(0); row < SIZEY; ++row) {
		for (int col(0); col < SIZEX; ++col) {
			SelectBackColour(clBlack);	//	Sets the background colour to black
			SelectTextColour(clWhite);	//	Sets the walls colour to white

			int pos(0);
			while ((pos < bears.size()) && (visible == false))
			{
				if ((row > bears.at(pos).y - 2 && row < bears.at(pos).y + 2) && (col > bears.at(pos).x - 2 && col < bears.at(pos).x + 2))
				{
					visible = true;
				}
				else
				{
					visible = false;
				}
				pos++;
			}

			if (visible == true)
			{
				switch (g[row][col])		//	Looks for special characters and paint them for chosen colours
				{
				case BEAR:
				{
					setColor(10);			// Paint the Bear green
					cout << g[row][col];
					resetColor();			// Reset the colour back to white
					break;
				}
				case DRILLBEAR:
					setColor(13);			// Paint the Bear magenta
					cout << BEAR;
					resetColor();			// Reset the colour back to white
					break;
				case PROTECTEDBEAR:
					setColor(2);
					cout << BEAR;
					resetColor();
					break;
				case BOMB:
				{
					setColor(12);			// Paint the Bomb red
					cout << g[row][col];
					resetColor();			// Reset the colour back to white
					break;
				}
				case DETONATOR:
				{
					setColor(14);			// Paint the Detonator yellow
					cout << g[row][col];
					resetColor();			// Reset the colour back to white
					break;
				}
				case PILL:
					setColor(11);
					cout << g[row][col];
					resetColor();
					break;
				case LOCK:
				{
					setColor(11);			// Paint the Lock cyan
					cout << g[row][col];
					resetColor();			// Reset the colour back to white
					break;
				}
				case KEY:
				{
					setColor(11);			// Paint the Key cyan
					cout << g[row][col];
					resetColor();			// Reset the colour back to white
					break;
				}
				case DRILL:
				{
					setColor(13);			// Paint the Drill magenta
					cout << g[row][col];
					resetColor();			// Reset the colour back to white
					break;
				}
				case EXIT:
					SelectBackColour(clWhite);
					SelectTextColour(clBlack);
					cout << g[row][col];
					break;
				default:
					cout << g[row][col];
					break;
				}
			}
			else
			{
				setColor(0);
				cout << g[row][col];
				resetColor();
			}
			visible = false;
		}
		cout << endl;
	}
}

void displayLevels(int levelRecord) {
	showMessage(clDarkGrey, clGreen, 59, 6, "  1.  Maze with detonator       ");
	if (levelRecord > 1) {
		showMessage(clDarkGrey, clGreen, 59, 7, "  2.  Maze with lock and key    ");
		if (levelRecord > 2) {
			showMessage(clDarkGrey, clGreen, 59, 8, "  3.  Maze with short visibility");
			if (levelRecord > 3) {
				showMessage(clDarkGrey, clGreen, 59, 9, "  4.  Maze with drill           ");
			}
		}
	}
}

bool readSave(string player) {
	ifstream loadProgress;
	loadProgress.open(player + "_saveGame.txt", ios::in);
	if (loadProgress.fail())
		return false;
	else {
		return true;
	}
}

void floatRules(int level) {

	int getKeyPress();

	showMessage(clDarkBlue, clGreen, 7, 2, "      _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _    ");
	showMessage(clDarkBlue, clGreen, 7, 3, "     /          Rules of the game           /|   ");
	showMessage(clDarkBlue, clGreen, 7, 4, "    /_ _ _ _ _ _ _ _  _ _ _ _ _ _ _ _ _ _ _/ |   ");
	showMessage(clDarkBlue, clGreen, 7, 5, "    |                                     |  |   ");
	showMessage(clDarkBlue, clGreen, 7, 6, "    |                                     |  |   ");
	showMessage(clDarkBlue, clGreen, 7, 7, "    |                                     |  |   ");
	showMessage(clDarkBlue, clGreen, 7, 8, "    |                                     |  |   ");
	showMessage(clDarkBlue, clGreen, 7, 9, "    |                                     |  |   ");
	showMessage(clDarkBlue, clGreen, 7, 10, "    |                                     |  |   ");
	showMessage(clDarkBlue, clGreen, 7, 11, "    |                                     |  |   ");
	showMessage(clDarkBlue, clGreen, 7, 12, "    |                                     |  |   ");
	showMessage(clDarkBlue, clGreen, 7, 13, "    |                                     |  |   ");
	showMessage(clDarkBlue, clGreen, 7, 14, "    |                                     |  |   ");
	showMessage(clDarkBlue, clGreen, 7, 15, "    |                                     | /    ");
	showMessage(clDarkBlue, clGreen, 7, 16, "    |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|/     ");
	showMessage(clDarkBlue, clGreen, 7, 17, "                        ||                       ");
	showMessage(clDarkBlue, clGreen, 7, 18, "                        ||                       ");
	showMessage(clDarkBlue, clGreen, 7, 19, "                        ||                       ");
	showMessage(clDarkBlue, clGreen, 7, 20, "                        ||                       ");

	// display level descriptions
	switch (level)
	{
	case 1:
		showMessage(clDarkBlue, clWhite, 13, 5, "          LEVEL 1 RULES:");
		showMessage(clDarkBlue, clWhite, 13, 7, " To disable bombs use detonator 'T'");
		break;
	case 2:
		showMessage(clDarkBlue, clWhite, 13, 5, "          LEVEL 2 RULES:");
		showMessage(clDarkBlue, clWhite, 13, 7, " To remove the lock use key 'F'");
		break;
	case 3:
		showMessage(clDarkBlue, clWhite, 13, 5, "          LEVEL 3 RULES: ");
		showMessage(clDarkBlue, clWhite, 13, 7, "Find the exit without seeing much");
		break;
	case 4:
		showMessage(clDarkBlue, clWhite, 13, 5, "          LEVEL 4 RULES: ");
		showMessage(clDarkBlue, clWhite, 13, 7, "Use key, detonate bombs, drill walls");
		break;
	}
	showMessage(clDarkBlue, clWhite, 13, 9, " Rescue all the bears '@' through");
	showMessage(clDarkBlue, clWhite, 13, 10, " exit 'X' avoiding bombs '0'");
	showMessage(clDarkBlue, clWhite, 13, 12, " TO MOVE USE KEYBOARD ARROWS");
	showMessage(clDarkBlue, clWhite, 13, 13, " TO QUIT ENTER 'Q'");

	showMessage(clDarkBlue, clWhite, 13, 15, " Press 'R' to close window");

	int key(getKeyPress());

	while (!displayRules(key)) {
		key = getKeyPress();
	}

	showMessage(clBlack, clGreen, 7, 2, "                                                 ");
	showMessage(clBlack, clGreen, 7, 3, "                                                 ");
	showMessage(clBlack, clGreen, 7, 4, "                                                 ");
	showMessage(clBlack, clGreen, 7, 5, "                                                 ");
	showMessage(clBlack, clGreen, 7, 6, "                                                 ");
	showMessage(clBlack, clGreen, 7, 7, "                                                 ");
	showMessage(clBlack, clGreen, 7, 8, "                                                 ");
	showMessage(clBlack, clGreen, 7, 9, "                                                 ");
	showMessage(clBlack, clGreen, 7, 10, "                                                 ");
	showMessage(clBlack, clGreen, 7, 11, "                                                 ");
	showMessage(clBlack, clGreen, 7, 12, "                                                 ");
	showMessage(clBlack, clGreen, 7, 13, "                                                 ");
	showMessage(clBlack, clGreen, 7, 14, "                                                 ");
	showMessage(clBlack, clGreen, 7, 15, "                                                 ");
	showMessage(clBlack, clGreen, 7, 16, "                                                 ");
	showMessage(clBlack, clGreen, 7, 17, "                                                 ");
	showMessage(clBlack, clGreen, 7, 18, "                                                 ");
	showMessage(clBlack, clGreen, 7, 19, "                                                 ");
	showMessage(clBlack, clGreen, 7, 20, "                                                 ");
}

void clearSign() {
	showMessage(clDarkGrey, clGreen, 59, 5, "                                ");
	showMessage(clDarkGrey, clGreen, 59, 6, "                                ");
	showMessage(clDarkGrey, clGreen, 59, 7, "                                ");
	showMessage(clDarkGrey, clGreen, 59, 8, "                                ");
	showMessage(clDarkGrey, clGreen, 59, 9, "                                ");
}

void mainMenu(int moves, string message, string playerName, string& levelString, int& level, int& levelRecord, bool& unlockMode, int& selection) {
	char testselection;
	do {
		void levelSelection(string message, string playerName, string& levelString, int& level, int& levelRecord, bool& unlockMode);
		void translateASCII(char& num);
		void reDrawBear();
		void readScoreTxt(string, string, int&);

		reDrawBear();
		
		if (moves > 0) {
			showMessage(clDarkGrey, clGreen, 59, 6, "     1. Continue game           ");
		}
		else {
			showMessage(clDarkGrey, clGreen, 59, 6, "     1. Start game              ");
		}
		showMessage(clDarkGrey, clGreen, 59, 7, "     2. See rules               ");
		showMessage(clDarkGrey, clGreen, 59, 8, "     3. See previous scores     ");
		showMessage(clDarkGrey, clGreen, 59, 9, "     4. Quit the game           ");

		showMessage(clBlack, clWhite, 40, 18, "                                                                                                         ");
		showMessage(clBlack, clWhite, 40, 18, "Choose an option: ");

		cin >> testselection;

		translateASCII(testselection);
		selection = static_cast<int>(testselection);
		switch (testselection) {
		case 1:
			if (moves > 0) {
				//Resume game
				system("CLS");
			}
			else {
				levelSelection(message, playerName, levelString, level, levelRecord, unlockMode);
			}
			break;
		case 2:
		{
			char num;

			do {
				//Ask which level to display rules for
				reDrawBear();
				clearSign();

				showMessage(clDarkGrey, clGreen, 61, 3, " Select the level to see rules: ");
				showMessage(clDarkGrey, clGreen, 59, 6, " 1.  Maze with detonator        ");
				showMessage(clDarkGrey, clGreen, 59, 7, " 2.  Maze with lock and key     ");
				showMessage(clDarkGrey, clGreen, 59, 8, " 3.  Maze with short visibility ");
				showMessage(clDarkGrey, clGreen, 59, 9, " 4.  Maze with drill            ");
				showMessage(clBlack, clWhite, 40, 20, "Press 'B' to go back                    ");

				showMessage(clBlack, clWhite, 41, 18, "                                       ");

				showMessage(clBlack, clWhite, 40, 18, "                                                                                                         ");
				showMessage(clBlack, clWhite, 40, 18, "Choose an option: ");


				cin >> num;

				while (((num < '1') || (num > '4')) && ((num != 'B') && (num != 'b'))) {
					showMessage(clBlack, clWhite, 58, 17, "                                                                           ");
					showMessage(clRed, clYellow, 58, 17, "INVALID COMMAND");
					showMessage(clBlack, clWhite, 40, 18, "                                                                                                         ");
					showMessage(clBlack, clWhite, 40, 18, "Choose an option: ");
					cin >> num;
				}
				if ((num != 'B') && (num != 'b')) {
					translateASCII(num);
					floatRules((int)num);
				}
				else {

				}
				showMessage(clBlack, clWhite, 58, 17, "                                                                           ");

			} while ((num != 'B') && (num != 'b'));
			break;
		}
		case 3:
		{
			clearSign();
			showMessage(clDarkGrey, clGreen, 61, 3, "        Previous scores:        ");
			showMessage(clDarkGrey, clGreen, 59, 6, "   Level 1:                  ");
			showMessage(clDarkGrey, clGreen, 59, 7, "   Level 2:                  ");
			showMessage(clDarkGrey, clGreen, 59, 8, "   Level 3:                  ");
			showMessage(clDarkGrey, clGreen, 59, 9, "   Level 4:                  ");
			showMessage(clBlack, clWhite, 40, 20, "Press 'B' to go back                    ");
			showMessage(clBlack, clWhite, 40, 18, "                                                                     ");

			int previousScore = 0;
			int levelCounter = 1;
			for (int i = 6; i <= 9; i++) {
				string levelString = to_string(levelCounter);
				readScoreTxt(playerName, levelString, previousScore);
				showMessage(clDarkGrey, clGreen, 72, i, to_string(previousScore));
				levelCounter++;
			}
			int key(getKeyPress());

			while (!goBack(key)) {
				key = getKeyPress();
			}
			break;
		}
		default:
			break;
		}
	} while ((selection != 1) && (selection != 4));
}


void readScoreTxt(string playerName, string level, int& previousScore) {
	ifstream fin;
	fin.open(playerName + "_LV" + level + ".txt", ios::in);
	fin >> previousScore;
	fin.close();
}

void readPlayerRecord(string playerName, int& levelRecord) {
	ifstream gameRecord;
	gameRecord.open(playerName + "_gameRecord.txt", ios::in);
	gameRecord >> levelRecord;
	gameRecord.close();
}

void recordPlayerTxt(string playerName, string levelString, int scoreMove, int level, int levelRecord) {
	ofstream fout(playerName + "_LV" + levelString + ".txt", ios::out);
	fout << scoreMove << endl;

	if (level >= levelRecord) {
		ofstream gameRecord(playerName + "_gameRecord.txt", ios::out);
		gameRecord << level << endl;
	}
}

void saveGame(char g[][SIZEX], string playerName, int level, int score, int rescued, bool onBomb, bool onWal) {
	ofstream saveProgress(playerName + "_saveGame.txt", ios::out);
	saveProgress << level << endl;
	saveProgress << score << endl;
	saveProgress << rescued << endl;
	saveProgress << onBomb << endl;
	saveProgress << onWal << endl;
	for (int row = 0; row < SIZEY; row++) {
		for (int col = 0; col < SIZEX; col++) {
			switch (g[row][col])
			{
			case TUNNEL:
				saveProgress << 0;
				break;
			case WALL:
				saveProgress << 1;
				break;
			case BEAR:
				saveProgress << 2;
				break;
			case PROTECTEDBEAR:
				saveProgress << 21;
				break;
			case DRILLBEAR:
				saveProgress << 22;
				break;
			case BOMB:
				saveProgress << 3;
				break;
			case DETONATOR:
				saveProgress << 4;
				break;
			case EXIT:
				saveProgress << 5;
				break;
			case LOCK:
				saveProgress << 6;
				break;
			case KEY:
				saveProgress << 7;
				break;
			case DRILL:
				saveProgress << 8;
				break;
			case WAL:
				saveProgress << 9;
				break;
			case PILL:
				saveProgress << 10;
				break;
			default:
				break;
			}
			saveProgress << " ";
		}
		saveProgress << endl;
	}
}

void loadGame(string player, vector<Item>& bears, char m[][SIZEX], int& level, int& score, int& rescued, bool& onBomb, bool& onWal) {
	ifstream loadProgress;
	int savedMaze[SIZEY][SIZEX];
	loadProgress.open(player + "_saveGame.txt", ios::in);
	loadProgress >> level;
	loadProgress >> score;
	loadProgress >> rescued;
	loadProgress >> onBomb;
	loadProgress >> onWal;
	for (int row = 0; row < SIZEY; row++) {
		for (int col = 0; col < SIZEX; col++) {
			loadProgress >> savedMaze[row][col];
		}
	}

	loadProgress.close();
	
	// delete the save file after reading it straightaway to prevent reloading after game completion
	if (remove((player + "_saveGame.txt").c_str()) != 0) {
		cout << endl;
		perror("Error deleting file");
	}
	else {
		cout << endl;
		puts("Previous ingame progress loaded");
	}

	for (int row(0); row < SIZEY; ++row) {
		for (int col(0); col < SIZEX; ++col) {
			switch (savedMaze[row][col])
			{
			case 0: m[row][col] = TUNNEL; break;
			case 1: m[row][col] = WALL; break;
			case 2: m[row][col] = BEAR; break;
			case 21:
				m[row][col] = PROTECTEDBEAR;
				break;
			case 22:
				m[row][col] = DRILLBEAR;
				break;
			case 3: m[row][col] = BOMB; break;
			case 4: m[row][col] = DETONATOR; break;
			case 5: m[row][col] = EXIT; break;
			case 6: m[row][col] = LOCK; break;
			case 7:	m[row][col] = KEY; break;
			case 8:	m[row][col] = DRILL; break;
			case 9: m[row][col] = WAL; break;
			case 10: m[row][col] = PILL; break;
			}
		}
	}
}

void resetStats(vector<Item>& bombs, vector<Item>& locks, int& scoreMove, int& rescued) {
	bombs.clear();	// remove residual bombs available on previous level for the next one
	locks.clear();	// remove residual locks available on previous level for the next one
	showMessage(clBlack, clWhite, 40, 5, "                   ");	// reset score count
	scoreMove = 0;	// reset score count
	rescued = 0;	// reset number of bears rescued (vital for bears movement)
}

//Translate the number input
void translateASCII(char& num) {
	switch (num) {
	case 49:
		num = 1;
		break;
	case 50:
		num = 2;
		break;
	case 51:
		num = 3;
		break;
	case 52:
		num = 4;
		break;
	}
}

void reDrawBear() {
	showMessage(clDarkGrey, clGreen, 15, 2, "	   _      _                                 _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _  ");
	showMessage(clDarkGrey, clGreen, 15, 3, "   : `.--.' ;              _....,_          /              MENU                /| ");
	showMessage(clDarkGrey, clGreen, 15, 4, "   .'      `.      _..--'''       `-._     /_ _ _ _ _ _ _ _  _ _ _ _ _ _ _ _ _/ | ");
	showMessage(clDarkGrey, clGreen, 15, 5, "  :          :_.-''                  .`.   |                                 |  | ");
	showMessage(clDarkGrey, clGreen, 15, 6, "  :  6    6  :                     :  '.;  |                                 |  | ");
	showMessage(clDarkGrey, clGreen, 15, 7, "  :          :                      `..';  |                                 |  | ");
	showMessage(clDarkGrey, clGreen, 15, 8, "  `: .----. :'                          ;  |                                 |  | ");
	showMessage(clDarkGrey, clGreen, 15, 9, "    `._Y _.'               '           ;   |                                 | /  ");
	showMessage(clDarkGrey, clGreen, 15, 10, "       'U'      .'          `.         ;   |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|/   ");
	showMessage(clDarkGrey, clGreen, 15, 11, "          `:   ;`-..___       `.     .'`.                    ||                   ");
	showMessage(clDarkGrey, clGreen, 15, 12, "           :   :  :    ```'''''``.    `.  `.                 ||                   ");
	showMessage(clDarkGrey, clGreen, 15, 13, "        .'     ;..'            .'       `.'`                 ||                   ");
	showMessage(clDarkGrey, clGreen, 15, 14, "       `.......'              `........-'`                   ||                   ");
	showMessage(clBlack, clWhite, 40, 20, "                                            ");
}

void endLevel(string level) {
	showMessage(clRed, clYellow, 40, 22, "LEVEL ");
	showMessage(clRed, clYellow, 46, 22, level);
	showMessage(clRed, clYellow, 47, 22, " COMPLETED!");
	showMessage(clBlack, clWhite, 40, 23, "");
	system("PAUSE");
}

void endProgram(bool finishGame, vector<Item>& bears) {
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	if (finishGame == false) {
		showMessage(clRed, clYellow, 40, 22, "END OF GAME: GAME STOPPED BY PLAYER");	//hold output screen until a keyboard key is hit
	}
	else {
		if (bears.empty() == true) {
			showMessage(clRed, clYellow, 40, 22, "END OF GAME: GAME WON!");
		}
		else {
			showMessage(clRed, clYellow, 40, 22, "END OF GAME: GAME OVER!");
		}
	}
	showMessage(clBlack, clWhite, 40, 23, "");
	system("pause");
}

// Changes the console colour to the value passed in
void setColor(int colour) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour);
}

// Resets the console colour to basic colour
void resetColor() {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

// This function rearranges the position of bears inside the vector
// Based on which key is pressed for the movement
void sortBears(vector<Item>& bears, const int key, const int rescued) {
	vector<Item> temp(1);
	int pos(0);

	switch (key)	//...depending on the selected key...
	{
	case LEFT:  	//when LEFT arrow pressed...
		if (rescued != 2) {
			if (bears[pos].x > bears[pos + 1].x) {
				swap(bears[pos], bears[pos + 1]);
			}
		}
		if (rescued == 0) {
			if (bears[pos].x > bears[pos + 2].x) {
				swap(bears[pos], bears[pos + 2]);
			}
			if (bears[pos + 1].x > bears[pos + 2].x) {
				swap(bears[pos + 1], bears[pos + 2]);
			}
		}
		break;
	case RIGHT: 	//when RIGHT arrow pressed...
		if (rescued != 2) {
			if (bears[pos].x < bears[pos + 1].x) {
				swap(bears[pos], bears[pos + 1]);
			}
		}
		if (rescued == 0) {
			if (bears[pos].x < bears[pos + 2].x) {
				swap(bears[pos], bears[pos + 2]);
			}
			if (bears[pos + 1].x < bears[pos + 2].x) {
				swap(bears[pos + 1], bears[pos + 2]);
			}
		}
		break;
	case UP: 		//when UP arrow pressed...
		if (rescued != 2) {
			if (bears[pos].y > bears[pos + 1].y) {
				swap(bears[pos], bears[pos + 1]);
			}
		}
		if (rescued == 0) {
			if (bears[pos].y > bears[pos + 2].y) {
				swap(bears[pos], bears[pos + 2]);
			}
			if (bears[pos + 1].y > bears[pos + 2].y) {
				swap(bears[pos + 1], bears[pos + 2]);
			}
		}
		break;
	case DOWN: 		//when DOWN arrow pressed...
		if (rescued != 2) {
			if (bears[pos].y < bears[pos + 1].y) {
				swap(bears[pos], bears[pos + 1]);
			}
		}
		if (rescued == 0) {
			if (bears[pos].y < bears[pos + 2].y) {
				swap(bears[pos], bears[pos + 2]);
			}
			if (bears[pos + 1].y < bears[pos + 2].y) {
				swap(bears[pos + 1], bears[pos + 2]);
			}
		}
		break;
	}
}
