#pragma once

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// constants
const int BOARD_ROW_SIZE = 6;
const int BOARD_COL_SIZE = 6;
const int FLEET_SIZE = 5;

// structs
struct Point {
    int rowIndex;
    int colIndex;

    bool operator==(const Point& point) const {
        return rowIndex == point.rowIndex && colIndex == point.colIndex;
    }
};

struct Ship {
    string name;
    int size;
    int hitCount;
    vector<Point> points;
};

struct Player {
    char board[BOARD_ROW_SIZE][BOARD_COL_SIZE];
    Ship fleet[FLEET_SIZE];
};

// functions
int chooseGameMode();

void displayBoards(char board1[][BOARD_COL_SIZE], char board2[][BOARD_COL_SIZE], bool isGameStart);

void initFleet(Player& player);

bool spaceOccupied(Player player, int shipRowIndices, int shipColIndices, char orientation, int shipSize);

void getValidShipInfo(Player& player, int& rowIndex, int& colIndex, char& orientation, int shipIndex);

void placeShip(Player& player, int shipIndex);

void boardSetup(Player& player1, Player& player2, int gameMode);

void play(Player& player1, Player& player2, int gameMode);