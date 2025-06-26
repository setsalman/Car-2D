#include <iostream>  // input-output stream
#include <conio.h>  // keyboard input handling
#include <windows.h>  // Windows-specific functions 
#include <ctime>  // time-based random generation
using namespace std; // will not have to write std 

// Constants for game dimensions
const int SCREEN_WIDTH = 90;
const int SCREEN_HEIGHT = 26;
const int WIN_WIDTH = 70;

const int CAR_WIDTH = 4;
const int CAR_HEIGHT = 4;

const int CAR_MIN_X = 18;
const int CAR_MAX_X = 50;
const int ENEMY_TRIGGER_Y = 10;

// Handle to the console output device
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;

// Game state struct
struct GameState {
    int enemyY[3];
    int enemyX[3];
    int enemyFlag[3];
    int CarPos;
    int score;
};

// Car design using a 2D array of characters
char car[CAR_HEIGHT][CAR_WIDTH] = {
    {' ', '*', '*', ' '},
    {'*', '*', '*', '*'},
    {' ', '*', '*', ' '},
    {'*', '*', '*', '*'}
};

// Move the cursor to the specified (x, y) position on the console
void gotoxy(int x, int y) {
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(console, CursorPosition);
}

// Set visibility and size of the console cursor
void setcursor(bool visible, DWORD size) {
    if (size == 0) size = 20;
    CONSOLE_CURSOR_INFO cursor;
    cursor.bVisible = visible;
    cursor.dwSize = size;
    SetConsoleCursorInfo(console, &cursor);
}

// Draw the border for the game window
void drawBorder() {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < 17; j++) {
            gotoxy(j, i); cout << "|";
            gotoxy(WIN_WIDTH - j, i); cout << "|";
        }
    }
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        gotoxy(SCREEN_WIDTH, i);
        cout << "|";
    }
}

// Generate a new random horizontal position for enemy `ind`
void genEnemy(GameState &state, int ind) {
    state.enemyX[ind] = 17 + rand() % 33;
}

// Draw enemy on screen at its current position
void drawEnemy(const GameState &state, int ind) {
    if (state.enemyFlag[ind]) {
        gotoxy(state.enemyX[ind], state.enemyY[ind]);     cout << "****";
        gotoxy(state.enemyX[ind], state.enemyY[ind] + 1); cout << " ** ";
        gotoxy(state.enemyX[ind], state.enemyY[ind] + 2); cout << "****";
        gotoxy(state.enemyX[ind], state.enemyY[ind] + 3); cout << " ** ";
    }
}

// Erase enemy from screen
void eraseEnemy(const GameState &state, int ind) {
    if (state.enemyFlag[ind]) {
        for (int i = 0; i < 4; i++) {
            gotoxy(state.enemyX[ind], state.enemyY[ind] + i);
            cout << "    ";
        }
    }
}

// Reset enemy position
void resetEnemy(GameState &state, int ind) {
    eraseEnemy(state, ind);
    state.enemyY[ind] = 1;
    genEnemy(state, ind);
}

// Draw the car at current car position
void drawCar(const GameState &state) {
    for (int i = 0; i < CAR_HEIGHT; i++) {
        for (int j = 0; j < CAR_WIDTH; j++) {
            gotoxy(j + state.CarPos, i + 22);
            cout << car[i][j];
        }
    }
}

// Erase the car from screen
void eraseCar(const GameState &state) {
    for (int i = 0; i < CAR_HEIGHT; i++) {
        for (int j = 0; j < CAR_WIDTH; j++) {
            gotoxy(j + state.CarPos, i + 22);
            cout << " ";
        }
    }
}

// Bounding box collision detection
bool checkCollision(const GameState &state, int enemyIndex) {
    int ex = state.enemyX[enemyIndex];
    int ey = state.enemyY[enemyIndex];

    int cx = state.CarPos;
    int cy = 22;

    // Simple AABB (axis-aligned bounding box)
    if (ey + 3 >= cy && ey <= cy + 3 &&
        ex + 3 >= cx && ex <= cx + 3) {
        return true;
    }
    return false;
}

// Show game over screen
void gameover() {
    system("cls");
    cout << "\n\t\t ------------------- \n";
    cout << "\t\t -----GAME OVER----- \n\n";
    cout << "Press any key to go back to menu.... \n";
    getch();
}

// Display the current score
void updateScore(const GameState &state) {
    gotoxy(WIN_WIDTH + 7, 5);
    cout << "Score: " << state.score << " ";
}

// Show instructions screen
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

// Handle player input
bool handleInput(GameState &state) {
    if (_kbhit()) {
        char ch = _getch();
        switch (ch) {
            case 'a':
            case 'A':
                if (state.CarPos > CAR_MIN_X) state.CarPos -= 4;
                break;
            case 'd':
            case 'D':
                if (state.CarPos < CAR_MAX_X) state.CarPos += 4;
                break;
            case 27:
                return false; // ESC to quit
        }
    }
    return true;
}

// Update enemies and handle scoring
void updateEnemies(GameState &state) {
    if (state.enemyFlag[0]) state.enemyY[0]++;
    if (state.enemyFlag[1]) state.enemyY[1]++;

    if (state.enemyY[0] > SCREEN_HEIGHT - 4) {
        resetEnemy(state, 0);
        state.score++;
        updateScore(state);
    }

    if (state.enemyY[1] > SCREEN_HEIGHT - 4) {
        resetEnemy(state, 1);
        state.score++;
        updateScore(state);
    }

    if (state.enemyY[0] == ENEMY_TRIGGER_Y && !state.enemyFlag[1]) {
        state.enemyFlag[1] = 1;
    }
}

// Render a full game frame
void renderFrame(GameState &state) {
    drawCar(state);
    drawEnemy(state, 0);
    drawEnemy(state, 1);

    Sleep(50);

    eraseCar(state);
    eraseEnemy(state, 0);
    eraseEnemy(state, 1);
}

// The main game loop
void play() {
    GameState state;

    // Initialize game state
    state.CarPos = WIN_WIDTH / 2 - 1;
    state.score = 0;
    state.enemyFlag[0] = 1;
    state.enemyFlag[1] = 0;
    state.enemyY[0] = state.enemyX[1] = 1;

    system("cls");
    drawBorder();
    updateScore(state);
    genEnemy(state, 0);
    genEnemy(state, 1);

    gotoxy(WIN_WIDTH + 7, 2);  cout << "Car Game";
    gotoxy(WIN_WIDTH + 6, 4);  cout << "----------";
    gotoxy(WIN_WIDTH + 7, 12); cout << "Controls:";
    gotoxy(WIN_WIDTH + 2, 14); cout << "A - Left";
    gotoxy(WIN_WIDTH + 2, 15); cout << "D - Right";

    gotoxy(18, 5); cout << "Press any key to start: ";
    getch();
    gotoxy(18, 5); cout << "                         ";

    while (true) {
        if (!handleInput(state)) break;
        renderFrame(state);

        if (checkCollision(state, 0) || checkCollision(state, 1)) {
            gameover();
            break;
        }

        updateEnemies(state);
    }
}

// Main function
int main() {
    // Hide cursor and set size
    setcursor(0, 0);

    // Seed random number generator
    srand((unsigned)time(NULL));

    while (true) {
        system("cls");

        gotoxy(10, 5);  cout << "--------------------";
        gotoxy(10, 6);  cout << "-- CAR GAME --";
        gotoxy(10, 7);  cout << "--------------------";

        gotoxy(10, 9);  cout << "1. Start the Game";
        gotoxy(10, 10); cout << "2. Instructions";
        gotoxy(10, 11); cout << "3. Quit";

        gotoxy(10, 13); cout << "Select Option: ";
        char op = getche();

        if (op == '1') play();
        else if (op == '2') instructions();
        else if (op == '3') break;
    }

    return 0;
}
