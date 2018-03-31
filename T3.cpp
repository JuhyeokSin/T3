// Programmer: Jay Sin
// Editor used: Sublime Text
// Compiler used: Apple LLVM version 8.0.0 (clang-800.0.38)

#include <iostream>
#include <iomanip>
#include <fstream>
using std::cout;
using std::cin;
using std::endl;
using std::setw;
using std::right;
using std::ios;
using std::ofstream;
using std::ifstream;

#include <cctype>
// the following C libarary is needed for srand(time(0)); rand();
#include <cstdlib>
#include <ctime>

// use LetterGrid enum to improve readability.
// (Q, W, E, ..., C) corresponds to (0, 1, 2, ..., 8) respectively
enum LetterGrid { Q, W ,E, A, S, D, Z, X, C };

// inputGuide is the array of upper-case letters to convert user-input
// into the corresponding grid index.
const char inputGuide[] = {'Q', 'W', 'E', 'A', 'S', 'D', 'Z', 'X', 'C'};

// Globally set the name of file for simple implementation of function
const char FILE_NAME[] = "t3.ini";

void printScores(int, int, int);
void readScoresData(int&, int&, int&);
void writeScoresData(int, int, int);

void printGrid(const char*);
void goXplayer(char*);

void goOplayer1(char*);
void goOplayer2(char*);
void goOplayer3(char*);

char check4Xwinner(const char*);
char check4Owinner(const char*);
char check4Tie(const char*);

// utility function
bool playAgain();
void resetGrid(char*);
void goOplayerByMode(int, char*);
bool isGameOver(const char*, int&, int&);
int readGameOption();
char readUserInput(char);
int isValidInput(char);
void assignCell(char, char&);
int smartPosition(char, const char*);
int optimalCell(int, int, int, char, const char*);
int randomEmptyCell(const char*);
int searchEmptyCells(int*, const char*);

int main() {

  // print my name and this assignment's title
  cout << "Lab10 part2" << endl;
  cout << "Programmer: Juhyeok Sin" << endl;
  cout << "Editor used: Apple LLVM version 8.0.0 (clang-800.0.38)" << endl;
  cout << "File: " << "T3.cpp" << endl;
  cout << "Compiled: " << "Nov 12nd, 2016" << "at" << "07:46PM" << endl;
  cout << endl;

  // Initialize the random number generator
  srand(time(0));

  // Initialize the 3x3 grid with white-spaces
  char grid[] = {' ',' ',' ',' ',' ',' ',' ',' ',' '};

  // Initialize scores: xwins, owins, and ties
  int xwins = 0, owins = 0, ties = 0;

  // Read scores through file input
  readScoresData(xwins, owins, ties);

  // Print the previous scores
  printScores(xwins, owins, ties);

  // Ask user to choose game option
  int gameMode = readGameOption();

  // if user chooses to exit, the game will not be started
  // Otherwise the user will start the first game.
  bool gameStatus = (gameMode != -1) ? true : false;
  while(gameStatus) {
    // print the current grid for three different cases:
    //   1) When the first game gets started (empty grid)
    //   2) After the playerO enters desired position on the grid
    //   3) After the current game is over, and the user wants to replay (empty grid)
    printGrid(grid);

    // Check if the current game is over (win or tie)
    if (!isGameOver(grid, owins, ties)) {
      // if the current game is not decided, playerX continues his/her turn
      goXplayer(grid);
    } else if (playAgain()) {
      // if the current game is over AND the user wants to replay,
      // then reset the grid and start a new game.
      resetGrid(grid);
      continue;
    } else {
      // if the current game is over AND the user wants to exit,
      // then terminates the game.
      break;
    }

    // Check if the current game is over (win or tie)
    if (!isGameOver(grid, xwins, ties)) {
      // if the current game is not decided, playerO continues his/her/its turn
      // with the same game-mode (goOplayer1, goOplayer2, or goOplayer3)
      goOplayerByMode(gameMode, grid);
    } else if (playAgain()) {
      // if the current game is over AND the user wants to replay,
      // then reset the grid and start a new game.
      resetGrid(grid);
      continue;
    } else {
      // if the current game is over AND the user wants to exit,
      // then terminates the game.
      break;
    }
  }

  // Write the current scores into t3.ini file
  writeScoresData(xwins, owins, ties);

  // Print the cumulated scores
  printScores(xwins, owins, ties);

  return 0;
}

void printScores(int xwins, int owins, int ties) {
  cout << "---------- Score Board ----------" << endl
    << "  X wins: " << xwins << endl
    << "  O wins: " << owins << endl
    << "    Ties: " << ties << endl << endl;
}

/**
 * Read a binary code from the file named 't3.ini' and parse the array
 * into xwins, owins, and ties respectively.
 * Note that writeScoresData function writes the array of these scores.
 * For further information, Please read the documentation for writeScoresData()
 * 
 * @param xwins variable to store the previous X's score
 * @param owins variable to store the previous O's score
 * @param ties  variable to store the previous ties score
 */
void readScoresData(int& xwins, int& owins, int& ties) {
  // t3.ini stores xwins, owins, and ties as binary code of int array
  int scores[3] = {0};

  // declare fin, type of ifstream object
  ifstream fin;
  // Open a file and set binary | in flag to manipulate binary code
  fin.open(FILE_NAME, ios::binary | ios::in);

  // Check if the file exists and is not empty
  if (!fin.good()) return;

  // Read binary code of 3 integers into scores array
  fin.read((char*) &scores, sizeof(scores));

  // Parse array of scores into corresponding scores
  xwins = scores[0]; owins = scores[1]; ties = scores[2];

  // close the opened file
  fin.close();
}

/**
 * Writes array of three scores as binary code into a file named 't3.ini'
 * Writing binary code of array is much more beneficial for two reasons:
 *   1) It requires only single procedure of reading a file
 *   2) No value-conversion is required (e.g. char* -> int | int -> char*)
 * 
 * @param xwins the current score of X
 * @param owins the current score of O
 * @param ties  the current ties
 */
void writeScoresData(int xwins, int owins, int ties) {
  // scores array will be written as binary code into t3.ini
  int scores[3] = {xwins, owins, ties};

  // declare fout, type of ofstream object
  ofstream fout;
  // Open a file and set binary | in flag to manipulate binary code
  fout.open(FILE_NAME, ios::binary | ios::out);

  // Write scores array into t3.ini as binary code
  fout.write((char*) &scores, sizeof(scores));

  // close the opened file
  fout.close();
}

/**
 * print the 3x3 grid according to each element in grid array
 * @param grid 3x3 character grid
 */
void printGrid(const char* grid) {
  // use setw to evenly distribute the width of column
  // note that the index of grid is replaced by the elements in LetterGrid enum
  // to improve the readability
  cout << setw(2) << grid[Q] << " | " << grid[W] << " | " << grid[E] << endl;
  cout << " - + - + -" << endl;
  cout << setw(2) << grid[A] << " | " << grid[S] << " | " << grid[D] << endl;
  cout << " - + - + -" << endl;
  cout << setw(2) << grid[Z] << " | " << grid[X] << " | " << grid[C] << endl;
}

/**
 * prompt the user if he/she wants to replay
 * @return [true]  if the user enters 'y' or 'Y'
 *         [false] if the user enters 'n' or 'N'
 */
bool playAgain() {
  // repeat until the user enters either 'y', 'Y', 'n', or 'N'
  while(true){
    // C-string buffer to store user input
    char buf[100];

    // Read the line of input to validate it easier
    // and to safely clear the keyboard buffer
    cout << "Continue to play? [y or n]: ";
    cin.getline(buf, 100);

    // Convert the FIRST character of buffer to lower case
    char userAnswer = tolower(buf[0]);

    // return proper value (true or false) depending on the userAnswer
    if (userAnswer == 'y') return true;
    if (userAnswer == 'n') return false;

    // At this point, user-input must be invalid.
    // repeat the loop after proper error message
    cout << "Invalid input. Please try again!" << endl;
  }
}

/**
 * The function executes three different types of playerO
 * based on the option chosen by the user. Note that the mode value
 * is initialized before starting the game, and it will be persistent
 * until the game terminates.
 * 
 * @param mode integer value represents three different mode.
 * @param grid 3x3 grid in array of characters
 */
void goOplayerByMode(int mode, char* grid) {
  // if the mode is 1, executes goOplayer1
  if (mode == 1) {
    // since the option 1 is (vs. human),
    // print grid again to improve the user experience
    printGrid(grid);
    goOplayer1(grid);
  } else if (mode == 2)
    // mode 2 executes goOplayer2
    goOplayer2(grid);
  else if (mode == 3)
    // mode 3 executes goOplayer3
    goOplayer3(grid);
}

/**
 * Check if the current game is over by evaluating two scenarios
 *   1) win (by X or O)
 *   2) tie
 * If the game has the winner, winScore will be increased by 1.
 * If the game is tie, ties will be increased by 1.
 * 
 * @param  grid     the grid of the current game (array of characters)
 * @param  winScore the current score of either X or O
 * @param  ties     the current score of ties
 * @return [true]   if the game has the winner, or is tie.
 *         [false]  if the game has not met any scenarios
 */
bool isGameOver(const char* grid, int& winScore, int& ties) {
  // check if the game has the winner, or is tie
  if (check4Xwinner(grid) == 'Y') {
    cout << "X is winner." << endl;
    winScore++;
  } else if (check4Owinner(grid) == 'Y') {
    cout << "O is winner." << endl;
    winScore++;
  } else if (check4Tie(grid) == 'Y') {
    cout << "Tie game! Everyone is winner." << endl;
    ties++;
  } else
    // if none of the conditions is satisfied,
    // the current game must be continued
    return false;

  // At this point the current game is over
  // because one of the if..else-if must be true
  return true;
}

/**
 * Prompt the user for the type of opponent,
 *   [1] vs. Human Player (goOplayer1)
 *   [2] vs. Dumb Computer (goOplayer2)
 *   [3] vs. Smart Computer (goOplayer3)
 * and returns his/her choice if it's valid.
 * 
 * @return [1..3] one of the user option
 *           [-1] when the user decides to exit
 */
int readGameOption() {
  // C-string buffer to store user input
  char buf[100];

  // repeat until the user enters the valid input
  while(true) {
    cout << setw(4) << right << "[1] vs. Human Player" << endl;
    cout << setw(4) << right << "[2] vs. Dumb Computer" << endl;
    cout << setw(4) << right << "[3] vs. Smart Computer" << endl;
    cout << "Choose an option 1, 2, or 3 [q to exit]: ";

    // Read the line of input to validate it easier
    // and to safely clear the keyboard buffer
    cin.getline(buf, 100);

    // Convert the FIRST character of buffer into lower case
    // return -1 if the user decides to exit (q or Q)
    if (tolower(buf[0]) == 'q') return -1;

    // Check if the user enters the valid option
    // by converting the buffer into integer
    int option = atoi(buf);
    if (option >= 1 && option <= 3)
      return option;

    // At this point, the user-input must be invalid.
    cout << "Invalid input. Please try again." << endl;
  }
}

/**
 * Prompt the user for the position on the grid to mark.
 * Note that if the user choose goOplayer1 (mode 1),
 * this function will be used to acquire playerO's position as well.
 * 
 * @param  player ['X'] if the function is prompting playerX
 *                ['O'] if the function is prompting playerO
 * @return        the first character entered by the current player
 */
char readUserInput(char player) {
  // C-string buffer to store the user-input
  char userInput[100];
  cout << "Enter a position [player: " << player << "]: ";

  // Read the line of input to validate it easier
  // and to safely clear the keyboard buffer
  cin.getline(userInput, 100);

  // return the first character of the buffer
  return userInput[0];
}

/**
 * Validates the position of grid entered by playerX,
 * and updates the value inside grid[position]
 * 
 * @param grid the current game grid
 */
void goXplayer(char* grid) {
  // repeat until the user enters the valid character AND
  // the corresponding index on the grid is not occupied.
  while (true) {
    // Read playerX's desired position with readUserInput function
    char userInput = readUserInput('X');
    // Check if the position value is valid
    int position = isValidInput(toupper(userInput));
    // If the position is invalid (in case -1),
    if (position == -1){
      // Throws an error message with the guide for grid indices
      cout << "Invalid input. Please try again." << endl;
      printGrid(inputGuide);
    }
    else if (grid[position] != ' ')
      // if the position on grid is already occupied,
      // throws an error message
      cout << userInput << " is already taken." << endl;
    else
      // At this point, the position must be valid and vacant.
      // Marking it with 'X'
      return assignCell('X', grid[position]);
  }
}

/**
 * Implementation for the 2nd human player. It basically shares the same logic
 * with goXplayer; however, it calls readUserInput with 'O'.
 * 
 * @param grid the current game grid
 */
void goOplayer1(char* grid) {
  while (true) {
    // Read playerO's desired position with readUserInput function
    char userInput = readUserInput('O');
    // Check if the position value is valid
    int position = isValidInput(toupper(userInput));
    // If the position is invalid (in case -1),
    if (position == -1){
      // Throws an error message with the guide for grid indices
      cout << "Invalid input. Please try again." << endl;
      printGrid(inputGuide);
    }
    else if (grid[position] != ' ')
      // if the position on grid is already occupied,
      // throws an error message
      cout << userInput << " is already taken." << endl;
    else
      // At this point, the position must be valid and vacant.
      // Marking it with 'O'
      return assignCell('O', grid[position]);
  }
}

/**
 * Implementation for a dumb computer. First, It calls randomEmptyCell function
 * to get empty cell in the grid. If the position is valid (!= -1),
 * it marks 'O' to the position.
 * (For more details please read the specification of randomEmptyCell)
 * 
 * @param grid the current game grid
 */
void goOplayer2(char* grid) {
  // Get the random empty cell on the current grid
  // by calling randomEmptyCell function.
  int position = randomEmptyCell(grid);
  // if the position is valid, marking 'O' to the cell
  if (position != -1)
    assignCell('O', grid[position]);
}

/**
 * Implementation for a smart computer. It basically follows the instruction
 * on the lab specification. However, to improve the readability and for better testing,
 * it utilizes smartPosition function by evaluating 1) defensive and 2) offensive
 * strategies.
 * 
 * @param grid the current game grid
 */
void goOplayer3(char* grid) {
  // Initialize the position with the invalid value
  int position = -1;

  // Evaluates the defensive strategy by counting
  // 'X' marks to search any lines to block
  position = smartPosition('X', grid);
  // if smartPosition finds any position for defense,
  // mark 'O' to block and terminates the function
  if (position > -1) {
    grid[position] = 'O';
    return;
  }

  // Evaluates the offensive strategy by counting
  // 'O' marks to search any lines to attack
  position = smartPosition('O', grid);
  if (position > -1) {
    // if smartPosition finds any position for offense,
    // mark 'O' to attack and terminates the function
    grid[position] = 'O';
    return;
  }

  // At this point, there is no position for effective offense or defense.
  // So let's mark 'O' on a random empty cell.
  position = randomEmptyCell(grid);
  grid[position] = 'O';
}

/**
 * Check if the playerX is winner for the current game.
 * Note that the members of LetterGrid(enum) are used for readability.
 * 
 * @param  grid the current game grid
 * @return      ['Y'] if the playerX is the winner
 *              ['N'] if the playerX cannot be the winner
 */
char check4Xwinner(const char* grid) {
  // Check the first row [Q-W-E]
  if ((grid[Q] == 'X') && (grid[W] == 'X') && (grid[E] == 'X'))
    return 'Y';
  // Check the second row [A-S-D]
  else if ((grid[A] == 'X') && (grid[S] == 'X') && (grid[D] == 'X'))
    return 'Y';
  // Check the third row [Z-X-C]
  else if ((grid[Z] == 'X') && (grid[X] == 'X') && (grid[C] == 'X'))
    return 'Y';
  // Check the first column [Q-A-Z]
  else if ((grid[Q] == 'X') && (grid[A] == 'X') && (grid[Z] == 'X'))
    return 'Y';
  // Check the second column [W-S-X]
  else if ((grid[W] == 'X') && (grid[S] == 'X') && (grid[X] == 'X'))
    return 'Y';
  // Check the third column [E-D-C]
  else if ((grid[E] == 'X') && (grid[D] == 'X') && (grid[C] == 'X'))
    return 'Y';
  // Check the diagonal from the left-top to the right-bottom [Q-S-C]
  else if ((grid[Q] == 'X') && (grid[S] == 'X') && (grid[C] == 'X'))
    return 'Y';
  // Check the diagonal from the right-top to the left-bottom [E-S-Z]
  else if ((grid[E] == 'X') && (grid[S] == 'X') && (grid[Z] == 'X'))
    return 'Y';

  // None of the winning conditions is met
  return 'N';
}

/**
 * Check if the playerO is winner for the current game.
 * Note that the members of LetterGrid(enum) are used for readability.
 * 
 * @param  grid the current game grid
 * @return      ['Y'] if the playerO is the winner
 *              ['N'] if the playerO cannot be the winner
 */
char check4Owinner(const char* grid) {
  // Check the first row [Q-W-E]
  if ((grid[Q] == 'O') && (grid[W] == 'O') && (grid[E] == 'O'))
    return 'Y';
  // Check the second row [A-S-D]
  else if ((grid[A] == 'O') && (grid[S] == 'O') && (grid[D] == 'O'))
    return 'Y';
  // Check the third row [Z-X-C]
  else if ((grid[Z] == 'O') && (grid[X] == 'O') && (grid[C] == 'O'))
    return 'Y';
  // Check the first column [Q-A-Z]
  else if ((grid[Q] == 'O') && (grid[A] == 'O') && (grid[Z] == 'O'))
    return 'Y';
  // Check the second column [W-S-X]
  else if ((grid[W] == 'O') && (grid[S] == 'O') && (grid[X] == 'O'))
    return 'Y';
  // Check the third column [E-D-C]
  else if ((grid[E] == 'O') && (grid[D] == 'O') && (grid[C] == 'O'))
    return 'Y';
  // Check the diagonal from the left-top to the right-bottom [Q-S-C]
  else if ((grid[Q] == 'O') && (grid[S] == 'O') && (grid[C] == 'O'))
    return 'Y';
  // Check the diagonal from the right-top to the left-bottom [E-S-Z]
  else if ((grid[E] == 'O') && (grid[S] == 'O') && (grid[Z] == 'O'))
    return 'Y';

  // None of the winning conditions is met
  return 'N';
}

/**
 * Check if the game can have no winner by counting the remaning empty cells.
 * Note that the members of LetterGrid(enum) are used for readability.
 * 
 * @param  grid the current game grid
 * @return      ['Y'] if there is no empty cell
 *              ['N'] if there is at least empty cell
 */
char check4Tie(const char* grid) {
  if (grid[Q] == ' ')
    return 'N';
  else if (grid[W] == ' ')
    return 'N';
  else if (grid[E] == ' ')
    return 'N';
  else if (grid[A] == ' ')
    return 'N';
  else if (grid[S] == ' ')
    return 'N';
  else if (grid[D] == ' ')
    return 'N';
  else if (grid[Z] == ' ')
    return 'N';
  else if (grid[X] == ' ')
    return 'N';
  else if (grid[C] == ' ')
    return 'N';

  return 'Y';
}

/**
 * Check if the user-input is a valid letter representing one of the grid indices.
 * 
 * @param  userInput the character entered by the user
 * @return [0,...,8] index number according to the userInput
 *              [-1] if the userInput is not valid (not convertable)
 */
int isValidInput(char userInput) {
  switch(userInput) {
    case 'Q' : return 0;
    case 'W' : return 1;
    case 'E' : return 2;
    case 'A' : return 3;
    case 'S' : return 4;
    case 'D' : return 5;
    case 'Z' : return 6;
    case 'X' : return 7;
    case 'C' : return 8;
  }

  // At this point, the user input must be invalid.
  return -1;
}

/**
 * Assign the given cell with the marking character.
 * 
 * @param mark ['X'] marking for playerX
 *             ['O'] marking for playerO
 * @param cell reference parameter receives one of the valid positons on grid
 */
void assignCell(char mark, char& cell) {
  cell = mark;
}

/**
 * It finds the optimal position to mark. It calls optimalCell function
 * for each rows, columns, and diagonals.
 * 
 * @param  mark ['X'] for defensive search by counting playerX's marks
 *              ['O'] for offensive search by counting playerO's marks
 * @param  grid the current game grid
 * @return      [0,...,8] when any optimal position has found
 *                   [-1] when there is no critical position to block or attack
 */
int smartPosition(char mark, const char* grid) {
  // Initialize the position with default value
  int bestPos = -1;

  // Check the first row
  bestPos = optimalCell(Q, W, E, mark, grid);
  if (bestPos > -1) return bestPos;

  // Check the second row
  bestPos = optimalCell(A, S, D, mark, grid);
  if (bestPos > -1) return bestPos;

  // Check the third row
  bestPos = optimalCell(Z, X, C, mark, grid);
  if (bestPos > -1) return bestPos;

  // Check the first column
  bestPos = optimalCell(Q, A, Z, mark, grid);
  if (bestPos > -1) return bestPos;

  // Check the second column
  bestPos = optimalCell(W, S, X, mark, grid);
  if (bestPos > -1) return bestPos;

  // Check the first column
  bestPos = optimalCell(E, D, C, mark, grid);
  if (bestPos > -1) return bestPos;

  // Check the diagonal from Q-S-C
  bestPos = optimalCell(Q, S ,C, mark, grid);
  if (bestPos > -1) return bestPos;

  // Check the diagonal from E-S-Z
  bestPos = optimalCell(E, S ,Z, mark, grid);
  if (bestPos > -1) return bestPos;

  // At this point, there is no critical point to block or attack
  return -1;
}

/**
 * Find the optimal position to mark among the three contiguous cells x, y, and z.
 * x, y, and z must be any sequence configuring either row, column, or diagonal.
 * If there is less than 2 marks (either 'O' or 'X' depending on the strategy),
 * there is no meaningful (or critical) position to mark so that
 * the function will return the default value, -1
 * 
 * @param  x    index of the first cell of the sequence
 * @param  y    index of the second cell of the sequence
 * @param  z    index of the third cell of the sequence
 * @param  mark 'X' or 'O' based on the strategy
 * @param  grid the current game grid
 * @return      optimal position if any, otherwise default value, -1
 */
int optimalCell(int x, int y, int z, char mark, const char* grid) {
  // Initialize counter for mark with 0
  // and the position of emptyCell with -1 (default value)
  int countMark = 0, emptyCell = -1;

  // Declare temporary cells array to iterate x, y, and z with for-loop
  int cells[3] = {x, y, z};
  for (int i = 0; i < 3; i++) {
    // for each position in order of (x -> y -> z)
    int pos = cells[i];
    if (grid[pos] == mark)
      // Increment countMark if the position on the grid contains the given mark
      countMark++;
    else if (grid[pos] == ' ')
      // Record the index if the position on the grid is empty
      emptyCell = pos;
  }

  // If the countMark is 2; in other words, the current sequence
  // must contain the optimal position to block or attack,
  if (countMark == 2)
    // the emptyCell in this sequence must be marked
    return emptyCell;
  
  // At this point, there is no critical position that requires action
  return -1;
}

/**
 * Returns a random index of empty cells on the current game grid.
 * First it builds emptyCells array to trace indices of all empty cells on the grid.
 * Second it executes rand() function to returns a random position of empty cell.
 * 
 * @param  grid the current game grid
 * @return      a random position of empty cell if any, otherwise, -1
 */
int randomEmptyCell(const char* grid) {
  // Initialize the array of indices of empty cells in grid
  int emptyCells[9];
  // Get the number of empty cells and fill emptyCells array
  // by calling searchEmptyCells.
  // (for further details, please read the specification for searchEmptyCells function)
  int size = searchEmptyCells(emptyCells, grid);
  
  // If there is no empty cell,
  if (size == 0)
    // return -1 to indicate the search is failed
    return -1;
  else if (size == 1)
    // if there is only empty cell, return the first element of emptyCells
    // because emptyCells array has only element.
    return emptyCells[0];
  else
    // At this point, there must be multiple empty cells.
    // Thus return the random position by executes rand() with the number of empty cells
    return emptyCells[rand() % size];
}

/**
 * Search empty cells on the current game grid. emptyCells paramter is
 * an array of 9 integers to store the indices of empty cells in grid.
 * 
 * @param  emptyCells an array of 9 integers to store the indices
 * @param  grid       the current game grid
 * @return            the number of empty cells on the grid
 */
int searchEmptyCells(int* emptyCells, const char* grid) {
  int size = 0;
  for (int i = 0; i < 9; i++) {
    if (grid[i] == ' ')
      emptyCells[size++] = i;
  }
  return size;
}

/**
 * Reset the grid with white-space for all positions
 * 
 * @param grid the current grid to reset
 */
void resetGrid(char* grid) {
  for (int i = 0; i < 9; i++)
    grid[i] = ' ';
}