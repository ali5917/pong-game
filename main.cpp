#include <iostream>
#include "C:\raylib\raylib\src\raylib.h"
#include <fstream>
using namespace std;

// Global Variables and Constants
#define GAME_START 0
#define GAME_REGISTER 1
#define GAME_MENU 2
#define GAME_PLAYING 3
#define GAME_WIN 4
#define GAME_LOSE 5
#define GAME_WALLET 6 
#define GAME_PROFILE 7

int gameState = GAME_START;

int aiScore = 0, playerScore = 0;
int gameLevel = -1; // 1 = Karachi, 2 = New York, 3 = Rome
int walletBalance = 1000;
float ballSpeed;
float aiSpeed;
float playerSpeed;

const int screenWidth = 1280;
const int screenHeight = 800;

// Colours
Color ballColor;
Color paddleColor;
Color cBrown = {109, 65, 42, 255};
Color cGreen = {81, 116, 95, 255};
Color karachiBall = {60, 24, 24, 255};
Color newYorkBall = {106, 101, 0, 255};
Color romeBall = WHITE;
Color karachiPaddle = {255, 174, 11, 255};
Color newYorkPaddle = {81, 116, 95, 255};
Color romePaddle = {199, 60, 0, 255};

// User info
int isBoy = -1;
int isMature = -1;
int isPak = -1;

// void saveUserData () {
//     ofstream thisFile ("userDetails.txt");
    
//     if (thisFile.is_open()) {
//         thisFile << isBoy << endl;
//         thisFile << isMature << endl;
//         thisFile << isPak << endl;
        
//         thisFile.close();
//     }

// }

// void readUserData(bool &isBoy, bool &isMature, bool &isPak) {
//     ifstream thisFile("userDetails.txt");

//     if (thisFile.is_open()) {
//         thisFile >> isBoy;
//         thisFile >> isMature;
//         thisFile >> isPak;
        
//         thisFile.close();
//     }
// }


bool isEligible (int level, int &balance) {
    int arr[3] = {50, 100, 250};
    if (balance >= arr[level - 1]) {
        return true;
    }
    return false; 
}

class Ball {
    private:
        float x;
        float y;
        float radius;
        float speedX;
        float speedY;

    public:
        Ball (float x, float y, float radius, float speedX, float speedY) :
            x(x),
            y(y),
            radius(radius),
            speedX(speedX),
            speedY(speedY) {}
        
        void draw () {
            if (gameLevel == 1) {
                ballColor = karachiBall;
            } else if (gameLevel == 2) {
                ballColor = newYorkBall;
            } else if (gameLevel == 3) {
                ballColor = romeBall;
            }
            DrawCircle(x, y, radius, ballColor);
        }

        void update () {
            x += speedX;
            y += speedY;

            if ((y + radius) >= GetScreenHeight() || (y - radius) <= 0) {
                speedY *= -1;
            }

            if ((x + radius) >= GetScreenWidth()) {
                aiScore ++;
                resetBall();
            }
            
            if ((x - radius) <= 0) {
                playerScore ++;
                resetBall();
            }
        }

        void resetBall () {
            x = GetScreenWidth() / 2;
            y = GetScreenHeight() / 2;

            int speeds[2] = {1, -1};
            speedX *= speeds[GetRandomValue(0, 1)];
            speedY *= speeds[GetRandomValue(0, 1)];
        }

        float getY () {
            return y;
        }

        float getX () {
            return x;
        }

        float getRadius () {
            return radius;
        }

        void InvertSpeedX () {
            speedX *= -1;
        }

        void setSpeedX (float s) {
            speedX = s;
        }

        void setSpeedY (float s) {
            speedY = s;
        }
};

class Paddle {
    protected: 
        float x;
        float y;
        float width;
        float height;
        float speed;
    
    public:
        Paddle (float x, float y, float w, float h, float s) :
            x(x),
            y(y),
            width(w),
            height(h),
            speed(s) {}

        void draw () {
            if (gameLevel == 1) {
                paddleColor = karachiPaddle;
            } else if (gameLevel == 2) {
                paddleColor = newYorkPaddle;
            } else if (gameLevel == 3) {
                paddleColor = romePaddle;
            }
            
            float borderThickness = 3.0f;
            DrawRectangleRounded(Rectangle{x - borderThickness, y - borderThickness, width + 2 * borderThickness, height + 2 * borderThickness}, 0.8, 0, WHITE);
            DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, paddleColor);
        }

        void update () {
            if (IsKeyDown(KEY_UP)) {
                y -= speed;
            }

            if (IsKeyDown(KEY_DOWN)) {
                y += speed;
            }

            // Limiting Vertical Movement
            if (y <= 0) y = 0;
            if ((y + height) >= GetScreenHeight()) y = GetScreenHeight() - height;
        }
        
        
        float getY () {
            return y;
        }

        float getX () {
            return x;
        }

        float getWidth () {
            return width;
        }

        float getHeight () {
            return height;
        }

        void setSpeed (float s) {
            speed = s;
        }
};

class AiPaddle : public Paddle {
    public:
        AiPaddle (float x, float y, float w, float h, float s) : Paddle (x, y, w, h, s) {}

        void update (int ballY) {
            if ((y + height / 2) > ballY) y -= speed;
            if ((y + height / 2) <= ballY) y += speed;

            // Limiting Vertical Movement
            if (y <= 0) y = 0;
            if ((y + height) >= GetScreenHeight()) y = GetScreenHeight() - height;
        }
};

class Game {
    public:
        Ball *ball;
        Paddle *player;
        AiPaddle *ai;
        Font lovelo;
        Texture2D karachi;
        Texture2D newYork;
        Texture2D rome;
        Texture2D paused;
        Texture2D start;
        Texture2D reg1;
        Texture2D reg2;
        Texture2D reg3;
        Texture2D menu;
        Texture2D win;
        Texture2D lose;
        Texture2D wallet;

        // Profiles
        Texture2D pakboy;
        Texture2D pakman;
        Texture2D pakgirl;
        Texture2D pakwoman;
        Texture2D foreignboy;
        Texture2D foreignman;
        Texture2D foreigngirl;
        Texture2D foreignwoman;


        Game () {
            ball = new Ball (GetScreenWidth() / 2, GetScreenHeight() / 2, 20.0, 10.0, 10.0);
            ai = new AiPaddle (10, GetScreenHeight() / 2 - 60, 25, 120, 8.5);
            player = new Paddle (GetScreenWidth() - 25 - 10, GetScreenHeight() / 2 - 60, 25, 120, 8.0); 
            
            lovelo = LoadFontEx("./assets/fonts/Lovelo Black.otf", 300, NULL, 0);
            karachi = LoadTexture("./assets/screens/karachi.png");
            newYork = LoadTexture("./assets/screens/newyork.png");
            rome = LoadTexture("./assets/screens/rome.png");
            start = LoadTexture("./assets/screens/start.png");
            paused = LoadTexture("./assets/screens/paused.png");
            reg1 = LoadTexture("./assets/screens/reg1.png");
            reg2 = LoadTexture("./assets/screens/reg2.png");
            reg3 = LoadTexture("./assets/screens/reg3.png");
            menu = LoadTexture("./assets/screens/menu.png");
            win = LoadTexture("./assets/screens/win.png");
            lose = LoadTexture("./assets/screens/lose.png");
            wallet = LoadTexture("./assets/screens/wallet.png");
            
            pakboy = LoadTexture("./assets/screens/profiles/pakboy.png");
            pakman = LoadTexture("./assets/screens/profiles/pakman.png");
            pakgirl = LoadTexture("./assets/screens/profiles/pakgirl.png");
            pakwoman = LoadTexture("./assets/screens/profiles/pakwoman.png");
            foreignboy = LoadTexture("./assets/screens/profiles/foreignboy.png");
            foreignman = LoadTexture("./assets/screens/profiles/foreignman.png");
            foreigngirl = LoadTexture("./assets/screens/profiles/foreigngirl.png");
            foreignwoman = LoadTexture("./assets/screens/profiles/foreignwoman.png");
        }

        ~Game () {
            unLoadTextures();
            UnloadFont(lovelo);
            delete ball; 
            delete ai; 
            delete player;
        }

        void unLoadTextures() {
            UnloadTexture(karachi);
            UnloadTexture(newYork);
            UnloadTexture(rome);
            UnloadTexture(paused);
            UnloadTexture(start);
            UnloadTexture(reg1);
            UnloadTexture(reg2);
            UnloadTexture(reg3);
            UnloadTexture(menu);
            UnloadTexture(win);
            UnloadTexture(lose);
            UnloadTexture(wallet);
            UnloadTexture(pakboy);
            UnloadTexture(pakman);
            UnloadTexture(pakgirl);
            UnloadTexture(pakwoman);
            UnloadTexture(foreignboy);
            UnloadTexture(foreignman);
            UnloadTexture(foreigngirl);
            UnloadTexture(foreignwoman);
        }

        void drawStart () {
            DrawTexture(start, 0, 0, WHITE);
        }
        
        void drawReg() {
            if (isBoy == -1) {
                DrawTexture(reg1, 0, 0, WHITE);
                if (IsKeyPressed(KEY_B)) {
                    isBoy = 1;
                };
                if (IsKeyPressed(KEY_G)) {
                    isBoy = 0;
                }; 
            } else if (isMature == -1) {
                DrawTexture(reg2, 0, 0, WHITE);
                if (IsKeyPressed(KEY_Y)) {
                    isMature = 1;
                }
                if (IsKeyPressed(KEY_N)) {
                    isMature = 0;
                }
            } else if (isPak == -1) {
                DrawTexture(reg3, 0, 0, WHITE);
                if (IsKeyPressed(KEY_Y)) {
                    isPak = 1;
                }
                if (IsKeyPressed(KEY_X)) {
                    isPak = 0;
                }
            }
        }

        void drawMenu () {
            DrawTexture(menu, 0, 0, WHITE);
            if (IsKeyPressed(KEY_K) && isEligible(1, walletBalance)) {
                walletBalance -= 50;
                gameLevel = 1;
                gameState = GAME_PLAYING;
            }
            if (IsKeyPressed(KEY_N) && isEligible(2, walletBalance)) {
                walletBalance -= 100;
                gameLevel = 2;
                gameState = GAME_PLAYING;
            }
            if (IsKeyPressed(KEY_R) && isEligible(3, walletBalance)) {
                walletBalance -= 250;
                gameLevel = 3;
                gameState = GAME_PLAYING;
            }
            if (IsKeyPressed(KEY_W)) gameState = GAME_WALLET;
            if (IsKeyPressed(KEY_P)) gameState = GAME_PROFILE;
        }

        void drawWin () {
            DrawTexture (win, 0, 0, WHITE);
        }

        void drawLose () {
            DrawTexture (lose, 0, 0, WHITE);
        }

        void drawWallet () {
            DrawTexture(wallet, 0, 0, WHITE);
            DrawText(TextFormat("%i", walletBalance), screenWidth / 2 - 20, screenHeight / 2 - 50, 200, WHITE);
            if (IsKeyPressed(KEY_M)) {
                gameState = GAME_MENU;
            }
        }

        void drawProfile () {
            if (isPak) {
                if (isMature) {
                    if (isBoy) {
                        DrawTexture(pakman, 0, 0, WHITE);
                    } else DrawTexture(pakwoman, 0, 0, WHITE);
                } else if (isBoy) {
                    DrawTexture(pakboy, 0, 0, WHITE);
                } else DrawTexture(pakgirl, 0, 0, WHITE);
            }  else if (isMature) {
                if (isBoy) {
                    DrawTexture(foreignman, 0, 0, WHITE);
                } else DrawTexture(foreignwoman, 0, 0, WHITE);
            } else if (isBoy) {
                DrawTexture(foreignboy, 0, 0, WHITE);
            } else DrawTexture(foreigngirl, 0, 0, WHITE);

            if (IsKeyPressed(KEY_M)) gameState = GAME_MENU;

        }

        void draw () {   
            if (gameState == GAME_START) {
                drawStart();
                if (IsKeyPressed(KEY_R)) gameState = 1;
            } 
            
            if (gameState == GAME_REGISTER) {
                drawReg();
                if (isBoy != -1 && isMature != -1 && isPak != -1) {
                    // saveUserData();
                    gameState = GAME_MENU;
                }
            }

            if (gameState == GAME_MENU) {
                drawMenu();
            }

            if (gameState == GAME_PLAYING) {
                if (gameLevel == 1) {
                    DrawTexture(karachi, 0, 0, WHITE); 
                    // DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, DARKGRAY);
                    DrawTextEx(lovelo, "KARACHI", {screenWidth / 2 - 160, 20}, 80, 0, BLACK);
                    DrawTextEx(lovelo, TextFormat("%i", aiScore), {screenWidth / 5 - 20, 20}, 80, 0, BLACK);
                    DrawTextEx(lovelo, TextFormat("%i", playerScore), {4 * screenWidth / 5 - 20, 20}, 80, 0, BLACK);

                } else if (gameLevel == 2) {
                    DrawTexture(newYork, 0, 0, WHITE); 
                    DrawTextEx(lovelo, "NEW YORK", {screenWidth / 2 - 200, 20}, 80, 0, BLACK);
                    DrawTextEx(lovelo, TextFormat("%i", aiScore), {screenWidth / 5 - 20, 20}, 80, 0, BLACK);
                    DrawTextEx(lovelo, TextFormat("%i", playerScore), {4 * screenWidth / 5 - 20, 20}, 80, 0, BLACK);
                    
                    // increasing speed of objects
                    static bool speedSet = false;
                    if (!speedSet) {
                        ball->setSpeedX(14.0);
                        ball->setSpeedY(14.0);
                        ai->setSpeed(12);
                        player->setSpeed(10);
                        speedSet = true;
                    }

                } else if (gameLevel == 3) {
                    DrawTexture(rome, 0, 0, WHITE); 
                    DrawTextEx(lovelo, "ROME", {screenWidth / 2 - 120, 20}, 80, 0, BLACK);
                    DrawTextEx(lovelo, TextFormat("%i", aiScore), {screenWidth / 5 - 20, 20}, 80, 0, BLACK);
                    DrawTextEx(lovelo, TextFormat("%i", playerScore), {4 * screenWidth / 5 - 20, 20}, 80, 0, BLACK);
                    
                    // increasing speed of objects
                    static bool speedSet = false;
                    if (!speedSet) {
                        ball->setSpeedX(16.0);
                        ball->setSpeedY(16.0);
                        ai->setSpeed(14);
                        player->setSpeed(12);
                        speedSet = true;
                    }
                }                
                ball->draw();
                player->draw();
                ai->draw();
            }

            if (gameState == GAME_WIN) {
                drawWin();
                playerScore = 0;
                aiScore = 0;
                if (IsKeyPressed(KEY_M)) gameState = GAME_MENU;
                if (IsKeyPressed(KEY_R)) gameState = GAME_PLAYING;
            }

            if (gameState == GAME_LOSE) {
                drawLose();
                playerScore = 0;
                aiScore = 0;
                if (IsKeyPressed(KEY_M)) gameState = GAME_MENU;
                if (IsKeyPressed(KEY_R)) gameState = GAME_PLAYING;
            }

            if (gameState == GAME_WALLET) {
                drawWallet();
            }

            if (gameState == GAME_PROFILE) {
                drawProfile();
            }
        }

        void update () {
            if (gameState != GAME_PLAYING) return;

            ball->update();
            player->update();
            ai->update(ball->getY());

            // Checking For Paddle - Ball Hits
            if (CheckCollisionCircleRec({ball->getX(), ball->getY()}, ball->getRadius(), {player->getX(), player->getY(), player->getWidth(), player->getHeight()})) {
                ball->InvertSpeedX();
            }

            if (CheckCollisionCircleRec({ball->getX(), ball->getY()}, ball->getRadius(), {ai->getX(), ai->getY(), ai->getWidth(), ai->getHeight()})) {
                ball->InvertSpeedX();
            }

            // Result Logic
            if (gameLevel == 1) {
                if (playerScore == 3) gameState = GAME_WIN;
                if (aiScore == 3) gameState = GAME_LOSE;

            } else if (gameLevel == 2) {
                if (playerScore == 5) gameState = GAME_WIN;
                if (aiScore == 5) gameState = GAME_LOSE;

            } else if (gameLevel == 3) {
                if (playerScore == 7) gameState = GAME_WIN;
                if (aiScore == 7) gameState = GAME_LOSE;
            }
        }

        void drawPaused () {
            DrawTexture(paused, 0, 0, WHITE);
        }
};

int main () {

    InitWindow(screenWidth, screenHeight, "PONG MAX");
    
    Game game;
    bool isPaused = false;

    SetTargetFPS(60);
    while (WindowShouldClose() == false) {
        
        // Updating if not paused
        if (IsKeyPressed(KEY_P) && gameState == GAME_PLAYING){
            isPaused = !isPaused;
        }

        if (IsKeyPressed(KEY_R) && isPaused) {
            playerScore = 0;
            aiScore = 0;
            isPaused = false;
        }
        
        if (IsKeyPressed(KEY_M) && isPaused) {
            gameState = GAME_MENU;
            isPaused = false;
        }

        if (!isPaused) {
            game.update();
        }
        

        // Drawing
        BeginDrawing();

        ClearBackground(BLACK);
        game.draw();
        if (isPaused) game.drawPaused();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}