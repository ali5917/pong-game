#include <iostream>
#include <raylib.h>
using namespace std;

int cpuScore = 0, playerScore = 0;
const int screenWidth = 1280;
const int screenHeight = 800;
Color cBrown = {109, 65, 42, 255};
Color cGreen = {81, 116, 95, 255};

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
            DrawCircle(x, y, radius, WHITE);
        }

        void update () {
            x += speedX;
            y += speedY;

            if ((y + radius) >= GetScreenHeight() || (y - radius) <= 0) {
                speedY *= -1;
            }

            if ((x + radius) >= GetScreenWidth()) {
                cpuScore ++;
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
            float borderThickness = 3.0f;
            DrawRectangleRounded(Rectangle{x - borderThickness, y - borderThickness, width + 2 * borderThickness, height + 2 * borderThickness}, 0.8, 0, WHITE);
            DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, cGreen);
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
};

class CpuPaddle : public Paddle {
    public:
        CpuPaddle (float x, float y, float w, float h, float s) : Paddle (x, y, w, h, s) {}

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
        CpuPaddle *ai;
        Texture2D newYork;
        Texture2D paused;
        Font lovelo;
         
        Game () {
            ball = new Ball (GetScreenWidth() / 2, GetScreenHeight() / 2, 20.0, 7.0, 10.0);
            ai = new CpuPaddle (10, GetScreenHeight() / 2 - 60, 25, 120, 8.5);
            player = new Paddle (GetScreenWidth() - 25 - 10, GetScreenHeight() / 2 - 60, 25, 120, 8.0); 
            
            // newYork = LoadTexture("./assets/screens/new-york.png");
            newYork = LoadTexture("./assets/screens/temp1.png");

            paused = LoadTexture("./assets/screens/paused.png");
            lovelo = LoadFontEx("./assets/fonts/Lovelo Black.otf", 300, NULL, 0);

        }

        ~Game () {
            UnloadTexture(newYork);
            UnloadTexture(paused);
            UnloadFont(lovelo);
            delete ball; 
            delete ai; 
            delete player;
        }

        void draw () {      
            DrawTexture(newYork, 0, 0, WHITE);   
            // DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, DARKGRAY);

            DrawTextEx(lovelo, "NEW YORK", {screenWidth / 2 - 200, 20}, 80, 0, cBrown);
            DrawTextEx(lovelo, TextFormat("%i", cpuScore), {screenWidth / 5 - 20, 20}, 80, 0, cBrown);
            DrawTextEx(lovelo, TextFormat("%i", playerScore), {4 * screenWidth / 5 - 20, 20}, 80, 0, cBrown);
            

            ball->draw();
            player->draw();
            ai->draw();
        }

        void update () {
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
        }

        void drawPaused () {
            DrawTexture(paused, 0, 0, WHITE);
        }
};


int main () {

    InitWindow(screenWidth, screenHeight, "OOP Game");
    
    Game game;
    bool isPaused = false;

    SetTargetFPS(60);
    while (WindowShouldClose() == false) {
        
        // Updating if not paused
        if (IsKeyPressed(KEY_P)){
            isPaused = !isPaused;
        }

        if (IsKeyPressed(KEY_R) && isPaused) {
            playerScore = 0;
            cpuScore = 0;
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