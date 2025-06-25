// Including input-output stream for console operations
#include <iostream> 

// For keyboard input handling (like getch, kbhit)
#include <conio.h>

// For Windows-specific functions like cursor manipulation
#include <windows.h>

// For using time-based random generation
#include <ctime>

using namespace std;

// Define width of the entire screen
#define SCREEN_WIDTH 90

// Define height of the screen
#define SCREEN_HEIGHT 26

// Define width of the game window area
#define WIN_WIDTH 70

// Handle to the console output device
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

// Coordinate position in the console
COORD CursorPosition;

// Y-positions of enemies
int enemyY[3];

// X-positions of enemies
int enemyX[3];

// Flags to indicate active enemies
int enemyFlag[3];

// Car design using a 2D array of characters
char car[4][4] = {
    {' ', '*', '*', ' '},
    {'*', '*', '*', '*'},
    {' ', '*', '*', ' '},
    {'*', '*', '*', '*'}
};

// Position of the car on the screen (X-axis)
int CarPos = WIN_WIDTH / 2;

// Game score
int score = 0;

// Move the cursor to the specified (x, y) position on the console
void gotoxy(int x, int y) {
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(console, CursorPosition);
}

// Set visibility and size of the console cursor
void setcursor(bool visible, DWORD size) {
    if (size == 0) size = 20;

    CONSOLE_CURSOR_INFO lpcursor;
    lpcursor.bVisible = visible;
    lpcursor.dwSize = size;
    SetConsoleCursorInfo(console, &lpcursor);
}

// Draw the border for the game window
void drawBorder() {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < 17; j++) {
            gotoxy(0 + j, i);
            cout << "|";
            gotoxy(WIN_WIDTH - j, i);
            cout << "|";
        }
    }
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        gotoxy(SCREEN_WIDTH, i);
        cout << "|";
    }
}

// Generate a new random horizontal position for enemy `ind`
void genEnemy(int ind) {
    enemyX[ind] = 17 + rand() % 33;
}

// Draw enemy on screen at its current position
void drawEnemy(int ind) {
    if (enemyFlag[ind] == true) {
        gotoxy(enemyX[ind], enemyY[ind]);
        cout << "****";
        gotoxy(enemyX[ind], enemyY[ind] + 1);
        cout << " ** ";
        gotoxy(enemyX[ind], enemyY[ind] + 2);
        cout << "****";
        gotoxy(enemyX[ind], enemyY[ind] + 3);
        cout << " ** ";
    }
}

// Erase enemy from screen by printing spaces over it
void eraseEnemy(int ind) {
    if (enemyFlag[ind] == true) {
        gotoxy(enemyX[ind], enemyY[ind]);
        cout << "    ";
        gotoxy(enemyX[ind], enemyY[ind] + 1);
        cout << "    ";
        gotoxy(enemyX[ind], enemyY[ind] + 2);
        cout << "    ";
        gotoxy(enemyX[ind], enemyY[ind] + 3);
        cout << "    ";
    }
}

// Reset enemy position after it goes out of screen
void resetEnemy(int ind) {
    eraseEnemy(ind);
    enemyY[ind] = 1;
    genEnemy(ind);
}

// Draw the car at current car position
void drawCar() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            gotoxy(j + CarPos, i + 22);
            cout << car[i][j];
        }
    }
}

// Erase the car by printing spaces over it
void eraseCar() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            gotoxy(j + CarPos, i + 22);
            cout << " ";
        }
    }
}

// Check if car collides with the first enemy
int collision() {
    if (enemyY[0] + 4 >= 23) {
        if (enemyX[0] + 4 - CarPos >= 0 && enemyX[0] + 4 - CarPos < 9) {
            return 1;
        }
    }
    return 0;
}

// Display game over screen and wait for a keypress
void gameover() {
    system("cls");
    cout << endl;
    cout << "\t\t ------------------- " << endl;
    cout << "\t\t -----GAME OVER----- " << endl << endl;
    cout << "Press any key to go back to menu.... \n";
    getch();
}

// Update and display the score on screen
void updateScore() {
    gotoxy(WIN_WIDTH + 7, 5);
    cout << "Score: " << score << endl;
}

// Show game instructions
void instructions() {
    system("cls");
    cout << "Instructions: \n";
    cout << "\n ----------------";
    cout << "\n Avoid cars by moving left or right....";
    cout << "\n Press 'A' to move left, 'D' to move right..";
    cout << "\n Press ESC key to exit during the game.";
    cout << "\n\n Press any key to go back to main menu..";
    getch();
}

// The main game logic function
void play() {
    // Reset car position
    CarPos = -1 + WIN_WIDTH / 2;

    // Reset score
    score = 0;

    // Activate first enemy
    enemyFlag[0] = 1;

    // Deactivate second enemy
    enemyFlag[1] = 0;

    // Set Y position for first enemy and X for second
    enemyY[0] = enemyX[1] = 1;

    // Clear the screen
    system("cls");

    // Draw game boundary
    drawBorder();

    // Show initial score
    updateScore();

    // Generate positions for two enemies
    genEnemy(0);
    genEnemy(1);

    // Print title
    gotoxy(WIN_WIDTH + 7, 2);
    cout << "Car Game";

    gotoxy(WIN_WIDTH + 6, 4);
    cout << "----------";

    gotoxy(WIN_WIDTH + 7, 12);
    cout << "Controls:";

    gotoxy(WIN_WIDTH + 2, 14);
    cout << "A - Left";

    gotoxy(WIN_WIDTH + 2, 15);
    cout << "D - Right";

    // Prompt for game start
    gotoxy(18, 5);
    cout << "Press any key to start: ";
    getch();

    // Clear prompt line
    gotoxy(18, 5);
    cout << "                         ";

    // Start game loop
    while (1) {
        // If a key is pressed
        if (kbhit()) {
            char c = getch();

            // Move car left if 'A' or 'a' is pressed
            if (c == 'a' || c == 'A') {
                if (CarPos > 18) CarPos -= 4;
            }

            // Move car right if 'D' or 'd' is pressed
            if (c == 'd' || c == 'D') {
                if (CarPos < 50) CarPos += 4;
            }

            // Exit game if ESC key is pressed
            if (c == 27) break;
        }

        // Draw car and enemies
        drawCar();
        drawEnemy(0);
        drawEnemy(1);

        // Check for collision
        if (collision()) {
            gameover();
            return;
        }

        // Delay to control game speed
        Sleep(50);

        // Erase car and enemies to update next frame
        eraseCar();
        eraseEnemy(0);
        eraseEnemy(1);

        // Activate second enemy if first reaches Y = 10
        if (enemyY[0] == 10 && enemyFlag[1] == 0) enemyFlag[1] = 1;

        // Move enemies down if active
        if (enemyFlag[0] == 1) enemyY[0]++;
        if (enemyFlag[1] == 1) enemyY[1]++;

        // Reset enemy and update score if it crosses the screen
        if (enemyY[0] > SCREEN_HEIGHT - 4) {
            resetEnemy(0);
            score++;
            updateScore();
        }
        if (enemyY[1] > SCREEN_HEIGHT - 4) {
            resetEnemy(1);
            score++;
            updateScore();
        }
    }
}

// Main function
int main() {
    // Hide cursor and set size
    setcursor(0, 0);

    // Seed random number generator
    srand((unsigned)time(NULL));

    // Main menu loop
    while (1) {
        // Clear screen
        system("cls");

        gotoxy(10, 5);
        cout << "--------------------";

        gotoxy(10, 6);
        cout << "-- CAR GAME --";

        gotoxy(10, 7);
        cout << "--------------------";

        gotoxy(10, 9);
        cout << "1. Start the Game";

        gotoxy(10, 10);
        cout << "2. Instructions";

        gotoxy(10, 11);
        cout << "3. Quit";

        gotoxy(10, 13);
        cout << "Select Option: ";

        // Get user input
        char op = getche();

        // Start game if option 1
        if (op == '1') play();

        // Show instructions if option 2
        else if (op == '2') instructions();

        // Exit game if option 3
        else if (op == '3') exit(0);
    }
}
