#include "header.h"

// ! helper functions

// function selects the game mode to be played
int chooseGameMode() {
    // declares the necessary variables
    const int NUM_GAME_MODES = 3;
    const char validGameModes[NUM_GAME_MODES] = {'1', '2', '3'};
    string gameMode;
    bool isValidGameMode = false;

    // asks user to pick a game mode
    cout << "Choose a game mode:\n";
    cout << "1. Player vs Player\n";
    cout << "2. Player vs Computer\n";
    cout << "3. Computer vs Computer\n";

    // checks if the game mode is valid
    while (!isValidGameMode) {
        cout << "Choose a game mode (1, 2, or 3): ";
        cin >> gameMode;

        // loops through the valid game modes, and if the chosen game mode
        // is valid, we set 'isValidGameMode' to true
        for (int gameModeIndex = 0; gameModeIndex < NUM_GAME_MODES; gameModeIndex++) {
            if (gameMode.length() == 1 && gameMode[0] == validGameModes[gameModeIndex]) {
                isValidGameMode = true;
            }
        }

        // if the game mode is valid, we break from the loop, else we
        // display an error message
        if (isValidGameMode) {
            break;
        } else {
            cout << "Invalid game mode. Please enter 1, 2 or 3.\n";
        }
    }

    // we return the 'gameMode' converted to an integer using 'stoi()'
    return stoi(gameMode);
}

// function prints the column header
void printColumnHeader() {
    const string spaceBetweenNumber = "   ";

    cout << "    ";

    for (int colIndex = 1; colIndex <= BOARD_COL_SIZE; colIndex++) {
        // if the 'colIndex' is 1, we dont want to print a space
        if (colIndex == 1) {
            cout << colIndex;
        } else {
            cout << spaceBetweenNumber << colIndex;
        }
    }
}

// function displays a player's board
void displayPlayerBoard(char board[][BOARD_COL_SIZE], int rowIndex, bool isGameStart) {
    const char separator = '|';

    // prints out the row header by using the ASCII value
    cout << char('A' + rowIndex) << " " << separator;

    // loop through the board and prints out whatever is on the board
    // at 'rowIndex', 'colIndex'
    for (int columnIndex = 0; columnIndex < BOARD_COL_SIZE; columnIndex++) {
        if (!isGameStart) {
            cout << " " << board[rowIndex][columnIndex] << " ";
        } else {
            char loc = board[rowIndex][columnIndex];

            if (loc != 'X' && loc != 'O') {
                loc = ' ';
            }

            cout << " " << loc << " ";
        }

        cout << separator;
    }
}

// function loops through a player's fleet and calls 'placeShip' for each
// ship, filling the board with ships
void startShipPlacement(Player& player1, Player& player2, string player, bool isGameStart) {
    cout << player << " set your board\n";

    // display the initial board
    displayBoards(player1.board, player2.board, isGameStart);

    // loops through every ship in the fleet
    for (int shipIndex = 0; shipIndex < FLEET_SIZE; shipIndex++) {
        // places the ship
        placeShip(player == "Player 1" ? player1 : player2, shipIndex);

        // display the boards after ship is placed
        displayBoards(player1.board, player2.board, isGameStart);
    }
}

// function checks if the ship will go out of bounds, if so
// displays the appropriate error message
bool isShipOutOfBounds(char orientation, int shipRowIndex, int shipColIndex, int shipSize) {
    const char vertical = 'V';

    // initializes the 'startingPosition' and 'bounds' depending on the orientation
    int startingPosition = (orientation == vertical) ? shipRowIndex : shipColIndex;
    int bounds = (orientation == vertical) ? BOARD_ROW_SIZE : BOARD_COL_SIZE;

    // we would know if the ship is out of bounds if the place where we place the ship
    // plus the ship's size is greater than the bounds, meaning parts of the ship is
    // out of the map
    if (startingPosition + shipSize > bounds) {
        return true;
    }

    return false;
}

// function checks if the coordinate is valid
bool isValidCoordinate(string coordinate, int& rowIndex, int& colIndex) {
    const int maxCoordinateLength = 2;

    // checks if the coordinate is a string of length 2 in letter number format
    if (coordinate.length() == maxCoordinateLength && isalpha(coordinate[0]) && isdigit(coordinate[1])) {
        // uppercases first index
        coordinate[0] = toupper(coordinate[0]);

        // convert the coordinate into int form
        rowIndex = coordinate[0] - 'A';
        colIndex = stoi(coordinate.substr(1)) - 1;

        // checks if 'rowIndex' and 'colIndex' is in range of 0-6 if it is,
        // the coordinate is valid it returns true
        return (rowIndex >= 0 && rowIndex < BOARD_ROW_SIZE && colIndex >= 0 && colIndex < BOARD_COL_SIZE);
    }

    return false;
}

// function checks if the current ship will intersect with any existing ships
bool isIntersect(Player player, char orientation, int shipRowIndex, int shipColIndex, int shipSize) {
    const char vertical = 'V';
    const char horizontal = 'H';

    // sets the starting position based on the orientation
    const int startingPosition = (orientation == vertical) ? shipRowIndex : shipColIndex;

    // loops through the indices that the ship will take up
    for (int shipIndices = startingPosition; shipIndices < startingPosition + shipSize; shipIndices++) {
        // loops through the player's fleet and checks if the current ship's points
        // will intersect an existing ship's points
        for (int shipIndex = 0; shipIndex < FLEET_SIZE; shipIndex++) {
            Ship ship = player.fleet[shipIndex];

            // checks every point in the ship
            for (Point point : ship.points) {
                int row = point.rowIndex;
                int col = point.colIndex;

                // depending on the orientation we check if the ship will intersect another
                // ship. if so, we will return true
                if (orientation == vertical && shipIndices == row && shipColIndex == col) {
                    return true;
                } else if (orientation == horizontal && shipIndices == col && shipRowIndex == row) {
                    return true;
                }
            }
        }
    }

    return false;
}

// function removes a ship from the fleet at a specified index
void removeShip(Ship fleet[], int& fleetSize, int shipIndex) {
    // loop starts at 'shipIndex', the index of the ship we want to remove.
    // we then set that index value to the value of the next index,
    // shifting every indexed value to the left
    for (int i = shipIndex; i < fleetSize - 1; i++) {
        fleet[i] = fleet[i + 1];
    }

    // since we removed a ship, we also need to decrease the fleet size
    fleetSize--;
}

// function handles the shot of the current player
void handleShot(Player player, bool& shotIsValid, int& shotRowIndex, int& shotColIndex, char hitSymbol, char missSymbol) {
    string coordinate;

    // loops until 'shotIsValid' is true
    while (!shotIsValid) {
        cout << "Fire a shot: ";
        cin >> coordinate;

        // checks if the coordinate is valid with the 'isValidCoordinate()'
        // function, if it is, we set 'shotIsValid' to true, otherwise
        // display error message and continues
        if (isValidCoordinate(coordinate, shotRowIndex, shotColIndex)) {
            // checks if the coordinate has already been used and display
            // the appropriate error message.
            if (player.board[shotRowIndex][shotColIndex] == hitSymbol || player.board[shotRowIndex][shotColIndex] == missSymbol) {
                cout << "Coordinate has already been used\n";
                continue;
            }

            // if the coordinate has not been used, we set 'shotIsValid' to true
            shotIsValid = true;
        } else {
            cout << "Invalid format or coordinates out of range, try again.\n";
        }
    }
}

// function checks if a shot is a hit or miss
bool checkForHit(Player& player, int& fleetSize, int shotRowIndex, int shotColIndex, char hitSymbol, char missSymbol, bool isComputer, bool& hasShipSunk, vector<Ship>& sunkenShips) {
    // we loop through every ship in the fleet and checks if the ship's
    // points matches the location of the shot. we also need to check if
    // the ship has sunk
    for (int shipIndex = 0; shipIndex < fleetSize; shipIndex++) {
        // assigns a referenced 'Ship' of 'currentShip' to the current
        // indexed fleet
        Ship& currentShip = player.fleet[shipIndex];

        // we loop through every point in the current ship
        for (Point point : currentShip.points) {
            // assigns the appropriate 'row' and 'col' values
            int row = point.rowIndex;
            int col = point.colIndex;

            // checks if the row and column value matches. if so we
            // print out 'Hit!' and then assign the board at the current
            // 'shotRowIndex', 'shotColIndex' to be a 'hitSymbol'. we
            // also increment the hit count of the current ship
            if (row == shotRowIndex && col == shotColIndex) {
                cout << "Hit!\n";

                player.board[shotRowIndex][shotColIndex] = hitSymbol;

                currentShip.hitCount++;

                // if the hit count of the ship is equal to its size,
                // we know that the ship has sunk. we display the appropriate
                // message and remove the ship from the array with the
                // 'removeShip()' function

                if (currentShip.hitCount == currentShip.size) {
                    if (isComputer) {
                        hasShipSunk = true;
                        sunkenShips.push_back(currentShip);
                    }

                    cout << currentShip.name << " has sunken!\n";

                    removeShip(player.fleet, fleetSize, shipIndex);
                }

                // we early return because we found the ship that we hit,
                // no need to loop through the other ships
                return true;
            }
        }
    }

    // if we did not hit, then we missed, displaying 'Miss!' and
    // then assign the board at the current 'shotRowIndex', 'shotColIndex'
    // to be a 'missSymbol'.
    cout << "Miss!\n";
    player.board[shotRowIndex][shotColIndex] = missSymbol;

    return false;
}

// returns a random row/col number
int generateRandomCoordinates() {
    return rand() % BOARD_COL_SIZE;
}

// returns a random even row/col number
int generateRandomEvenCoordinates() {
    int randomEvenCoordinate;

    // keeps generating a random coordinate until it is even
    do {
        randomEvenCoordinate = generateRandomCoordinates();
    } while (randomEvenCoordinate % 2 != 0);

    return randomEvenCoordinate;
}

// returns a random orientation
char generateRandomOrientation() {
    return (rand() % 2 == 0) ? 'V' : 'H';
}

// function randomly places the ships for the computer
void computerStartShipPlacement(Player& player1, Player& computer) {
    // provides a seed value
    srand((unsigned)time(NULL));

    const char vertical = 'V';
    const char horizontal = 'H';

    // loops through every ship in the fleet
    for (int shipIndex = 0; shipIndex < FLEET_SIZE; shipIndex++) {
        Ship& ship = computer.fleet[shipIndex];

        while (true) {
            // generates a random location
            int randRowIndex = generateRandomCoordinates();
            int randColIndex = generateRandomCoordinates();

            // generate a random orientation
            int randOrientation = generateRandomOrientation();

            // checks if the randomly generated coordinate and orientation
            // is out of bounds or if it will intersect an existing ship,
            // if so, we continue
            if (isShipOutOfBounds(randOrientation, randRowIndex, randColIndex, ship.size) ||
                isIntersect(computer, randOrientation, randRowIndex, randColIndex, ship.size)) {
                continue;
            }

            // checks for the orientation and pushes the correct ship points
            if (randOrientation == vertical) {
                for (int boardRow = randRowIndex; boardRow < randRowIndex + ship.size; boardRow++) {
                    Point shipLocation = {boardRow, randColIndex};

                    ship.points.push_back(shipLocation);

                    computer.board[boardRow][randColIndex] = ship.name[0];
                }
            } else if (randOrientation == horizontal) {
                for (int boardCol = randColIndex; boardCol < randColIndex + ship.size; boardCol++) {
                    Point shipLocation = {randRowIndex, boardCol};

                    ship.points.push_back(shipLocation);

                    computer.board[randRowIndex][boardCol] = ship.name[0];
                }
            }

            break;
        }
    }
}

// function generates a random valid shot coordinate
void randomlyGenerateShot(Player& player, int& randRowIndex, int& randColIndex, bool isTargeting, vector<Point>& potentialPoints, char hitSymbol, char missSymbol, double probabilityDensity[][BOARD_COL_SIZE], vector<Point> highestProbability) {
    // generates a random location if the mode is not targeting,
    // else gets a random point from the vector of 'potentialPoints'
    if (!isTargeting) {
        do {
            //// generates a random even coordinate
            // randRowIndex = generateRandomEvenCoordinates();
            // randColIndex = generateRandomEvenCoordinates();

            // picks the highest probabilty from the probablityDensity
            vector<Point> highestProbabilityIndices;

            double highestProbability = 0;

            Point randomPoint;

            for (int row = 0; row < BOARD_ROW_SIZE; row++) {
                for (int col = 0; col < BOARD_COL_SIZE; col++) {
                    if (probabilityDensity[row][col] > highestProbability) {
                        highestProbability = probabilityDensity[row][col];

                        highestProbabilityIndices.clear();
                        highestProbabilityIndices.emplace_back(Point{row, col});
                    } else if (probabilityDensity[row][col] == highestProbability) {
                        highestProbabilityIndices.emplace_back(Point{row, col});
                    }
                }
            }

            // shuffle the vector using shuffle
            random_device rd;
            mt19937 gen(rd());
            shuffle(highestProbabilityIndices.begin(), highestProbabilityIndices.end(), gen);

            // get a random element from the vector
            randomPoint = highestProbabilityIndices[rand() % highestProbabilityIndices.size()];

            randRowIndex = randomPoint.rowIndex;
            randColIndex = randomPoint.colIndex;

        } while (player.board[randRowIndex][randColIndex] == hitSymbol || player.board[randRowIndex][randColIndex] == missSymbol);
    } else {
        if (!potentialPoints.empty()) {
            // // gets the first point in the vector of 'potentialPoints'
            // int nextPointIndex = 0;

            // // get the row index and column index
            // randRowIndex = potentialPoints[nextPointIndex].rowIndex;
            // randColIndex = potentialPoints[nextPointIndex].colIndex;
            // if (!highestProbability.empty()) {
            //     while (true) {
            //         Point randomPoint;

            //         // shuffle the vector using shuffle
            //         random_device rd;
            //         mt19937 gen(rd());
            //         shuffle(highestProbability.begin(), highestProbability.end(), gen);

            //         // get a random element from the vector
            //         randomPoint = highestProbability[rand() % highestProbability.size()];

            //         randRowIndex = randomPoint.rowIndex;
            //         randColIndex = randomPoint.colIndex;

            //         if (player.board[randRowIndex][randColIndex] == 'X' || player.board[randRowIndex][randColIndex] == 'O') {
            //             continue;
            //         } else {
            //             break;
            //         }
            //     }
            // }
        }

        Point randomPoint;

        // shuffle the vector using shuffle
        random_device rd;
        mt19937 gen(rd());
        shuffle(highestProbability.begin(), highestProbability.end(), gen);

        // get a random element from the vector
        randomPoint = highestProbability[rand() % highestProbability.size()];

        randRowIndex = randomPoint.rowIndex;
        randColIndex = randomPoint.colIndex;
    }
}

// function checks if the shot fired is already in the vector of 'surroundingPoints'
bool isShotInPotentialPoints(int shotRowIndex, int shotColIndex, vector<Point>& potentialPoints) {
    for (Point point : potentialPoints) {
        if (point.rowIndex == shotRowIndex && point.colIndex == shotColIndex) {
            return true;
        }
    }

    return false;
}

// functions adds the surrounding points of where the shot was fired to a vector of
// 'potentialPoints'
void addSurroundingPoints(Player player, int shotRowIndex, int shotColIndex, vector<Point>& potentialPoints, char hitSymbol, char missSymbol) {
    // defines an array of 'Point' called directions that defines the surround four areas
    Point directions[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    // iterates through each direction and adds it to our shot location
    for (Point direction : directions) {
        int surroundingRowIndex = shotRowIndex + direction.rowIndex;
        int surroundingColIndex = shotColIndex + direction.colIndex;

        // checks if the surrounding points are within range
        // if so, we add it to the vector of 'potentialPoints'
        if (surroundingRowIndex >= 0 && surroundingRowIndex < BOARD_ROW_SIZE && surroundingColIndex >= 0 && surroundingColIndex < BOARD_COL_SIZE) {
            Point surroundingPoint = {surroundingRowIndex, surroundingColIndex};

            // checks if the surrounding point is not already marked as a hit or a miss
            if (player.board[surroundingRowIndex][surroundingColIndex] != hitSymbol && player.board[surroundingRowIndex][surroundingColIndex] != missSymbol) {
                // we only add the point to the vector if it is not already in the vector
                if (find(potentialPoints.begin(), potentialPoints.end(), surroundingPoint) == potentialPoints.end()) {
                    potentialPoints.push_back(surroundingPoint);
                }
            }
        }
    }
}

// function calculates the probability density
void calculateProbabilityDensity(Player player, int fleetSize, double probabilityDensity[][BOARD_COL_SIZE], vector<Point>& hits, bool& isTargeting, vector<Point>& highestProbabilty, bool hasShipSunk, vector<Ship> sunkenShips) {
    if (fleetSize == 0) {
        return;
    }

    // reset the array to zero
    for (int row = 0; row < BOARD_ROW_SIZE; row++) {
        for (int col = 0; col < BOARD_COL_SIZE; col++) {
            probabilityDensity[row][col] = 0.0;
        }
    }

    Ship largestShip = player.fleet[0];

    char orientations[2] = {'V', 'H'};

    double highestProbabilityNum = 0;

    for (int shipIndex = 0; shipIndex < fleetSize; shipIndex++) {
        if (player.fleet[shipIndex].size > largestShip.size) {
            largestShip = player.fleet[shipIndex];
        }
    }

    if (hasShipSunk && hits.empty()) {
        isTargeting = false;
        // cout << "got here\n";
    }

    if (!isTargeting) {
        // cout << "and then i am here\n";
        //  fleet
        for (int shipIndex = 0; shipIndex < fleetSize; shipIndex++) {
            Ship currentShip = player.fleet[shipIndex];
            // row
            for (int row = 0; row < BOARD_ROW_SIZE; row++) {
                // col
                for (int col = 0; col < BOARD_COL_SIZE; col++) {
                    // orientation
                    for (char orientation : orientations) {
                        // ship length
                        if (!isShipOutOfBounds(orientation, row, col, currentShip.size)) {
                            // vertical
                            if (orientation == 'V') {
                                for (int i = 0; i < currentShip.size; i++) {
                                    bool isAdd = true;

                                    for (int j = row; j < row + currentShip.size; j++) {
                                        if (player.board[j][col] == 'O' || player.board[j][col] == 'X') {
                                            isAdd = false;
                                            break;
                                        }
                                    }

                                    for (int j = row; j < row + currentShip.size; j++) {
                                        if (isAdd && j >= 0 && j < BOARD_ROW_SIZE) {
                                            probabilityDensity[j][col]++;
                                        }
                                    }
                                }
                            }

                            // horizontal
                            if (orientation == 'H') {
                                for (int i = 0; i < currentShip.size; i++) {
                                    bool isAdd = true;

                                    for (int j = col; j < col + currentShip.size; j++) {
                                        if (player.board[row][j] == 'O' || player.board[row][j] == 'X') {
                                            isAdd = false;
                                            break;
                                        }
                                    }

                                    for (int j = col; j < col + currentShip.size; j++) {
                                        if (isAdd && j >= 0 && j < BOARD_COL_SIZE) {
                                            probabilityDensity[row][j]++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } else if (isTargeting) {
        // cout << "or am i here\n";

        // fleet
        for (int shipIndex = 0; shipIndex < fleetSize; shipIndex++) {
            Ship currentShip = player.fleet[shipIndex];

            // board
            for (int row = 0; row < BOARD_ROW_SIZE; row++) {
                for (int col = 0; col < BOARD_COL_SIZE; col++) {
                    // hits
                    for (Point hit : hits) {
                        int hitRow = hit.rowIndex;
                        int hitCol = hit.colIndex;

                        int startingRow = (hitRow + currentShip.size > BOARD_ROW_SIZE) ? (hitRow - ((hitRow + currentShip.size) - BOARD_ROW_SIZE)) : hitRow;
                        int startingCol = (hitCol + currentShip.size > BOARD_COL_SIZE) ? (hitCol - ((hitCol + currentShip.size) - BOARD_COL_SIZE)) : hitCol;

                        int verticalBias = 0, horizontalBias = 0;

                        for (int i = 0; i < currentShip.size; i++) {
                            for (int j = startingRow; j < startingRow + currentShip.size; j++) {
                                if (player.board[j][hitCol] == 'X') {
                                    verticalBias++;
                                }
                            }

                            for (int j = startingCol; j < startingCol + currentShip.size; j++) {
                                if (player.board[hitRow][j] == 'X') {
                                    horizontalBias++;
                                }
                            }
                        }

                        // vertically
                        for (int i = 0; i < currentShip.size; i++) {
                            bool isAdd = true;

                            for (int j = startingRow; j < startingRow + currentShip.size; j++) {
                                for (Ship ship : sunkenShips) {
                                    for (Point point : ship.points) {
                                        int sunkenRow = point.rowIndex;
                                        int sunkenCol = point.colIndex;

                                        if (j == sunkenRow && hitCol == sunkenCol) {
                                            isAdd = false;
                                        }
                                    }
                                }

                                if (player.board[j][hitCol] == 'O') {
                                    isAdd = false;
                                    break;
                                }
                            }

                            for (int j = startingRow; j < startingRow + currentShip.size; j++) {
                                if (isAdd && j >= 0 && j < BOARD_ROW_SIZE) {
                                    if (verticalBias > horizontalBias) {
                                        probabilityDensity[j][hitCol] += 2;
                                    } else {
                                        probabilityDensity[j][hitCol]++;
                                    }
                                }
                            }

                            startingRow--;

                            if (startingRow < 0) {
                                break;
                            }
                        }

                        // horizontally
                        for (int i = 0; i < currentShip.size; i++) {
                            bool isAdd = true;

                            for (int j = startingCol; j < startingCol + currentShip.size; j++) {
                                for (Ship ship : sunkenShips) {
                                    for (Point point : ship.points) {
                                        int sunkenRow = point.rowIndex;
                                        int sunkenCol = point.colIndex;

                                        if (j == sunkenCol && hitRow == sunkenRow) {
                                            isAdd = false;
                                        }
                                    }
                                }

                                if (player.board[hitRow][j] == 'O') {
                                    isAdd = false;
                                    break;
                                }
                            }

                            for (int j = startingCol; j < startingCol + currentShip.size; j++) {
                                if (isAdd && j >= 0 && j < BOARD_COL_SIZE) {
                                    if (horizontalBias > verticalBias) {
                                        probabilityDensity[hitRow][j] += 2;
                                    }
                                    probabilityDensity[hitRow][j]++;
                                }
                            }

                            startingCol--;

                            if (startingCol < 0) {
                                break;
                            }
                        }

                        for (int i = 0; i < currentShip.size; i++) {
                            ;
                        }
                    }
                }
            }
        }
    }

    highestProbabilty.clear();

    for (int row = 0; row < BOARD_ROW_SIZE; row++) {
        for (int col = 0; col < BOARD_COL_SIZE; col++) {
            for (Ship& sunkenShip : sunkenShips) {
                for (Point point : sunkenShip.points) {
                    probabilityDensity[point.rowIndex][point.colIndex] = 0;
                }
            }
        }
    }

    for (int row = 0; row < BOARD_ROW_SIZE; row++) {
        for (int col = 0; col < BOARD_COL_SIZE; col++) {
            if (player.board[row][col] == 'X' || player.board[row][col] == 'O') {
                probabilityDensity[row][col] = 0;
            }
        }
    }

    for (int row = 0; row < BOARD_ROW_SIZE; row++) {
        for (int col = 0; col < BOARD_COL_SIZE; col++) {
            if (probabilityDensity[row][col] > highestProbabilityNum) {
                highestProbabilityNum = probabilityDensity[row][col];
            }
        }
    }

    for (int row = 0; row < BOARD_ROW_SIZE; row++) {
        for (int col = 0; col < BOARD_COL_SIZE; col++) {
            if (player.board[row][col] != 'X' && player.board[row][col] != 'O' && probabilityDensity[row][col] == highestProbabilityNum) {
                highestProbabilty.push_back({row, col});
            }
        }
    }

    // // prints stuff out
    // for (int row = 0; row < BOARD_ROW_SIZE; row++) {
    //     for (int col = 0; col < BOARD_COL_SIZE; col++) {
    //         cout << setw(5) << probabilityDensity[row][col] << " ";
    //     }
    //     cout << "\n";
    // }
}

// ! main functions

// function displays both players boards, side by side
void displayBoards(char board1[][BOARD_COL_SIZE], char board2[][BOARD_COL_SIZE], bool isGameStart) {
    const string line = "\n  -------------------------     -------------------------\n";
    const string headerSpace = "     ";
    const string boardSpace = "   ";

    // board header
    printColumnHeader();
    cout << headerSpace;
    printColumnHeader();
    cout << line;

    // creates boards
    for (int rowIndex = 0; rowIndex < BOARD_ROW_SIZE; rowIndex++) {
        // creates first board
        displayPlayerBoard(board1, rowIndex, isGameStart);

        cout << boardSpace;

        // creates second board
        displayPlayerBoard(board2, rowIndex, isGameStart);

        cout << line;
    }
}

// An initFleet function that takes in a Player object
// as a parameter and initializes the board and all the ships
// in the fleet with the appropriate information. For example,
// the name and size of the ship should be initialized within
// the function.
void initFleet(Player& player) {
    // initialize the board with empty spaces
    for (int rowIndex = 0; rowIndex < BOARD_ROW_SIZE; rowIndex++) {
        for (int columnIndex = 0; columnIndex < BOARD_COL_SIZE; columnIndex++) {
            player.board[rowIndex][columnIndex] = ' ';
        }
    }

    // initializes neccessary variables to read in data from 'ships.txt'
    ifstream inStream;
    string line;
    int shipIndex = 0;

    inStream.open("ships.txt");

    // exit in the case that we can't open the file
    if (inStream.fail()) {
        cout << "Error opening file!";
        exit(1);
    }

    // loop through every line in 'ships.txt'
    while (getline(inStream, line)) {
        // if the line starts with a #, we skip it
        if (line[0] == '#') {
            continue;
        }

        istringstream lineStream(line);

        string shipName;
        int shipSize;
        int hitCount = 0;
        vector<Point> points = {};

        int num;

        // splits up every line into three variables
        lineStream >> num >> shipName >> shipSize;

        // creates a Ship called 'ship' with the collected data
        Ship ship = {
            shipName,
            shipSize,
            hitCount,
            points,
        };

        // assigns the ship to the player's fleet at 'shipIndex' index
        player.fleet[shipIndex] = ship;

        // moves to next ship
        shipIndex++;
    }

    inStream.close();
}

// A boardSetup function that takes in two Player objects
// by reference, and calls the placeShip function for each ship
// in the fleet.  After each ship is placed on the board the
// boards should be displayed.
void boardSetup(Player& player1, Player& player2, int gameMode, bool isGameStart) {
    // checks the game mode first, 1 for pvp, 2 for p vs. computer
    if (gameMode == 1) {
        // asks 'Player 1' for their ship placement
        startShipPlacement(player1, player2, "Player 1", isGameStart);

        cout << "\n";

        // asks 'Player 2' for their ship placement
        startShipPlacement(player1, player2, "Player 2", isGameStart);
    } else if (gameMode == 2) {
        // asks 'Player 1' for their ship placement
        startShipPlacement(player1, player2, "Player 1", isGameStart);

        cout << "\n";

        cout << "The computer will now randomly place their ships\n";

        computerStartShipPlacement(player1, player2);

        // displayBoards(player1.board, player2.board, isGameStart);
    } else if (gameMode == 3) {
        cout << "Computer 1 will now randomly place their ships\n";

        computerStartShipPlacement(player2, player1);

        cout << "Computer 2 will now randomly place their ships\n";

        computerStartShipPlacement(player1, player2);
    }
}

// A getValidShipInfo function that takes in four parameters
// by reference, two ints holding the row and the column of the
// starting coordinates of the ship, a char that will hold the
// orientation of the ship (horizontal or vertical), and the
// Player. The function will also take a fifth argument
// by value which is the index of the ship being placed.

// The function will prompt the user for the starting row and
// column coordinates of the ship which the user will enter as:
// letter number (i.e., a letter for the row and a number for the column.)
// These will then need to be converted to the proper row and
// column index of the array (i.e., [0]-[size-1])  The function
// will also prompt the user for the horizontal or vertical
// orientation of the ship.  The function will be responsible
// for performing the error checking so that a valid ship placement
// is received from the user.  The function will also call the
// function spaceOccupied to determine if any of the spaces the
// ship would take up if placed on the board are currently occupied.
void getValidShipInfo(Player& player, int& rowIndex, int& colIndex, char& orientation, int shipIndex) {
    const string shipName = player.fleet[shipIndex].name;
    const int shipSize = player.fleet[shipIndex].size;

    const char vertical = 'V';
    const char horizontal = 'H';

    // loops to confirm the ship placement is valid
    while (true) {
        string coordinate;

        // gets the coordinates
        cout << "Enter the starting coordinates of your " << shipName << " (size: " << shipSize << "): ";
        cin >> coordinate;

        // checks if the coordinate is in letter number format, if not, we continue
        if (!isValidCoordinate(coordinate, rowIndex, colIndex)) {
            cout << "Invalid format or coordinates out of range, try again.\n";
            continue;
        }

        // confirms the validity of 'orientation'
        while (true) {
            cout << "Enter the orientation of your " << shipName << " (horizontal(h) or vertical(v)): ";
            cin >> orientation;

            // uppercases 'orientation'
            orientation = toupper(orientation);

            // checks if 'orientation' is either 'V' or 'H'
            if (orientation == vertical || orientation == horizontal) {
                break;
            } else {
                cout << "Invalid orientation, try again.\n";
            }
        }

        // checks if ship placement is valid by calling 'spaceOccupied()', if it
        // is not occupied we break from the loop
        if (!spaceOccupied(player, rowIndex, colIndex, orientation, shipSize)) {
            break;
        }
    }
}

// A placeShip function that takes in a Player object by
// reference and an int variable that stores the index of the
// ship that is currently being placed, and places the ship onto
// the board. The placeShip function calls the getValidShipInfo
// function to determine which spots on the board the ship will
// occupy.
void placeShip(Player& player, int shipIndex) {
    // initializes the necessary variables to place the ship
    int rowIndex = 0, colIndex = 0;
    char orientation = ' ';

    const char vertical = 'V';
    const char horizontal = 'H';

    // checks if the ship is valid and if we can place it with 'getValidShipInfo()'
    // also updates 'rowIndex', 'colIndex' and 'orientation' through reference
    getValidShipInfo(player, rowIndex, colIndex, orientation, shipIndex);

    // creates a new Ship called 'ship' that references the ship in the
    // players board. this makes it easier when we need to access the
    // player's ship. instead of calling 'player.fleet[shipIndex]'
    // we can call 'ship' instead
    Ship& ship = player.fleet[shipIndex];

    // by checking the orientation of the ship, we can create points that
    // holds the location the ship takes up
    if (orientation == vertical) {
        for (int boardRow = rowIndex; boardRow < rowIndex + ship.size; boardRow++) {
            Point shipLocation = {boardRow, colIndex};

            ship.points.push_back(shipLocation);

            // gets the first character in the ship name and assigns it to
            // the board
            player.board[boardRow][colIndex] = ship.name[0];
        }
    } else if (orientation == horizontal) {
        for (int boardCol = colIndex; boardCol < colIndex + ship.size; boardCol++) {
            Point shipLocation = {rowIndex, boardCol};

            ship.points.push_back(shipLocation);

            // gets the first character in the ship name and assigns it to
            // the board
            player.board[rowIndex][boardCol] = ship.name[0];
        }
    }
}

// A spaceOccupied function that takes in the Player object,
// an int for the row and col placement of the ship, a character
// for the orientation, and the ship size. This function returns
// true if the placement of the ship would overlap an already
// existing ship placement or false if the space is not occupied.
bool spaceOccupied(Player player, int shipRowIndex, int shipColIndex, char orientation, int shipSize) {
    const char vertical = 'V';
    const char horizontal = 'H';

    // checks the orientation of the ship and loop accordingly,
    // if it is vertical, we only need to loop through the rows,
    // likewise, if it is horizontal, we only need to loop through
    // the columns
    if (orientation == vertical || orientation == horizontal) {
        // checks if the ship will go out of bounds
        if (isShipOutOfBounds(orientation, shipRowIndex, shipColIndex, shipSize)) {
            cout << "Error: Ship placement is outside the board.\n";
            return true;
        }

        // checks if ship will intersect another ship
        if (isIntersect(player, orientation, shipRowIndex, shipColIndex, shipSize)) {
            cout << "Error: Space already occupied.\n";
            return true;
        }
    }

    return false;
}

// function starts the game, and declares a winner when the
// opponent's fleet is destroyed
void play(Player& player1, Player& player2, int gameMode) {
    // declare the number of ships in each player's fleet
    int player1NumShips = FLEET_SIZE;
    int player2NumShips = FLEET_SIZE;

    // tracks if the game has started
    bool isGameStart = false;

    // hit and miss symbols
    const char hitSymbol = 'X';
    const char missSymbol = 'O';

    // tracks whose turn it is
    bool playerOneTurn = true;

    // defines the computer's mode
    // if it is not targeting it is firing at random, else
    // it searches the location around the successful shot
    bool isTargeting = false;

    // vector of potential points for the computer to hit
    vector<Point> potentialPoints;

    // defines the probability density
    double probabilityDensity[BOARD_ROW_SIZE][BOARD_COL_SIZE] = {};

    // defines a vector of hits
    vector<Point> hits;

    // defines a vector of highestProbablity
    vector<Point> highestProbabilty;

    // defines a vector of sunken ships
    vector<Ship> sunkenShips;

    // tracks of a ship has sunk
    bool hasShipSunk = false;

    // initializes the fleet's of player 1 and player 2
    initFleet(player1);
    initFleet(player2);

    // sets up the board by asking the user for ship positions
    boardSetup(player1, player2, gameMode, isGameStart);

    // game keeps running until either player's fleet is destroyed
    while (player1NumShips > 0 && player2NumShips > 0) {
        // declares the necessary variables
        string coordinate;
        int shotRowIndex, shotColIndex;
        bool shotIsValid = false;

        isGameStart = true;

        cout << "Player " << (playerOneTurn ? 1 : 2) << ":\n";

        if (gameMode == 1) {
            // calls 'handleShot(); to handle the shot of the current user
            handleShot((playerOneTurn ? player2 : player1), shotIsValid, shotRowIndex, shotColIndex, hitSymbol, missSymbol);

            // calls 'checkForHit()' to see if it was a hit or miss
            checkForHit((playerOneTurn ? player2 : player1), (playerOneTurn ? player2NumShips : player1NumShips), shotRowIndex, shotColIndex, hitSymbol, missSymbol, false, hasShipSunk, sunkenShips);

            playerOneTurn = !playerOneTurn;
        } else if (gameMode == 2) {
            // declares the necessary variables for the computer
            int randRowIndex, randColIndex;

            // handles the shots and hits of the user
            handleShot(player2, shotIsValid, shotRowIndex, shotColIndex, hitSymbol, missSymbol);
            checkForHit(player2, player2NumShips, shotRowIndex, shotColIndex, hitSymbol, missSymbol, false, hasShipSunk, sunkenShips);

            displayBoards(player1.board, player2.board, isGameStart);

            cout << "Computer: \n";

            // calculates the probability density before generating a shot
            calculateProbabilityDensity(player1, player1NumShips, probabilityDensity, hits, isTargeting, highestProbabilty, hasShipSunk, sunkenShips);

            // randomly generates a shot by the computer depending on the mode
            randomlyGenerateShot(player1, randRowIndex, randColIndex, isTargeting, potentialPoints, hitSymbol, missSymbol, probabilityDensity, highestProbabilty);

            hasShipSunk = false;

            // defines the point the computer shot at
            Point point = {randRowIndex, randColIndex};

            cout << "Computer shot at (" << char('A' + randRowIndex) << ", " << randColIndex + 1 << ") \n";

            // checks if the shot generated is a hit or not, if it is, we do something with it
            // if it is not, we check if there are any potential points and continue targeting
            if (checkForHit(player1, player1NumShips, randRowIndex, randColIndex, hitSymbol, missSymbol, true, hasShipSunk, sunkenShips)) {
                isTargeting = true;

                hits.push_back({randRowIndex, randColIndex});

                if (hasShipSunk) {
                    // dont clear all points but just remove the points of the ship that just sunk
                    Ship sunkenShip = sunkenShips.back();

                    for (Point& point : sunkenShip.points) {
                        hits.erase(remove(hits.begin(), hits.end(), point), hits.end());
                    }
                }

                // // checks if the shot is in the vector of 'potentialPoints'
                // // if it is we remove it from the vector
                // if (isShotInPotentialPoints(randRowIndex, randColIndex, potentialPoints)) {
                //     potentialPoints.erase(remove(potentialPoints.begin(), potentialPoints.end(), point), potentialPoints.end());
                // }

                // // adds the surrounding points of the shot fire
                // addSurroundingPoints(player1, randRowIndex, randColIndex, potentialPoints, hitSymbol, missSymbol);
            } else {
                // remove this point from the vector of potential points
                potentialPoints.erase(remove(potentialPoints.begin(), potentialPoints.end(), point), potentialPoints.end());

                // if there are no potential points we switch to hunting mode,
                // else we continue with targeting mode
                // if (potentialPoints.empty()) {
                //     isTargeting = false;
                // } else {
                //     isTargeting = true;
                // }
            }

            cout << "\n";
        }

        // breaks the loop
        if (player1NumShips == 0 || player2NumShips == 0) {
            isGameStart = false;

            // prints the ended board
            displayBoards(player1.board, player2.board, isGameStart);

            break;
        } else {
            // prints the current board
            displayBoards(player1.board, player2.board, isGameStart);
        }
    }

    // declares the winner
    if (gameMode == 1) {
        if (player1NumShips > player2NumShips) {
            cout << "Player 1 sunk the fleet! Player 1 wins!\n";
        } else {
            cout << "Player 2 sunk the fleet! Player 2 wins!\n";
        }
    } else if (gameMode == 2) {
        if (player1NumShips > player2NumShips) {
            cout << "Player 1 sunk the fleet! Player 1 wins!\n";
        } else {
            cout << "The computer sunk the fleet! The computer wins!\n";
        }
    }
}