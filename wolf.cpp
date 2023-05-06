#include<iostream>
#include<SDL2/SDL.h>
#include<cmath>
using namespace std;

const int WIDTH = 800, HEIGHT = 600;

struct Wall
{
    SDL_Color lightColor;
    SDL_Color darkColor;
};


int main(int argc, char *argv[])
{
    //Game Logic
    int map[10][10] = {
        {1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,2,2,0,0,2,2,0,1},
        {1,0,2,2,0,0,2,2,0,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,3,3,3,3,3,3,0,1},
        {1,0,0,0,1,1,0,0,0,1},
        {1,0,0,0,1,1,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1}
    }; 
    Wall allWalls[3] = {
        {{255, 163, 117}, {255, 163, 117}},
        {{159, 164, 196},{158, 118, 143}},
        {{183, 182, 194},{131, 117, 105}}
    };
    float speed = 5;
    float rotationSpeed = 3;
    double xPos= 5, yPos = 5;
    double xDir = -1, yDir = 0;
    double xPlane = 0, yPlane = 0.7;

    double time = 0;
    double oldTime = 0;

    int screen_fps = 60;
    int ticks_per_frame = 1000/60;

    //Renderer Logic
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *gWindow = SDL_CreateWindow("Old School FPS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Renderer* gRenderer = NULL;
    
    if( NULL == gWindow){ 
        std::cout << "ERROR: Window not created: " << SDL_GetError() << endl;
        return 1;    
    }

    gRenderer = SDL_CreateRenderer(gWindow,-1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == NULL)
    {
        std::cout << "ERROR: Window not created: " << SDL_GetError() << endl;
        return 1;    
    }
    
    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

    SDL_Event windowEvent;
    while (true)
    {
        

        SDL_SetRenderDrawColor( gRenderer,255, 209, 173, 0xFF );
        SDL_RenderClear(gRenderer);

        SDL_Rect floor;
        floor.x = 0;
        floor.y = HEIGHT/2;
        floor.w = WIDTH;
        floor.h = HEIGHT/2;

        SDL_SetRenderDrawColor(gRenderer, 59, 49, 46, 0xFF);
        SDL_RenderFillRect(gRenderer, &floor);
        for (int x = 0; x < WIDTH; x++)
        {
            double xCam = 2 * x / double(WIDTH) - 1;
            double xRayDir = xDir + xPlane * xCam;
            double yRayDir = yDir + yPlane * xCam;

            int xMap = int(xPos);
            int yMap = int(yPos);

            double xSideDist;
            double ySideDist;

            double xDeltaDist = (xRayDir == 0) ? 1e30 : abs(1/xRayDir);
            double yDeltaDist = (yRayDir == 0) ? 1e30 : abs(1/yRayDir);
            double perpWallDist;

            int xStep;
            int yStep;

            int hit = 0;
            int side;

            if(xRayDir < 0){
                xStep = -1;
                xSideDist = (xPos - xMap) * xDeltaDist;
            }
            else
            {
                xStep = 1;
                xSideDist = (xMap + 1.0 - xPos) * xDeltaDist;
            }

            if(yRayDir < 0){
                yStep = -1;
                ySideDist = (yPos - yMap) * yDeltaDist;
            }
            else{
                yStep = 1;
                ySideDist = (yMap + 1.0 - yPos) * yDeltaDist;
            }

            while (hit == 0){
                if (xSideDist < ySideDist)
                {
                    xSideDist += xDeltaDist;
                    xMap += xStep;
                    side = 0;
                }
                else
                {
                    ySideDist += yDeltaDist;
                    yMap += yStep;
                    side = 1;
                }

                if (map[xMap][yMap] > 0) hit = 1;
            }

            if(side == 0) perpWallDist = (xSideDist - xDeltaDist);
            else perpWallDist = (ySideDist - yDeltaDist);

            int lineHeight = (int)( HEIGHT / perpWallDist);

            int drawStart = -lineHeight /2 + HEIGHT / 2;
            if(drawStart < 0) drawStart = 0;
            int drawEnd = lineHeight / 2 + HEIGHT / 2;
            if (drawEnd > HEIGHT) drawEnd = HEIGHT; 
            
            Wall curWall = allWalls[map[xMap][yMap]];

            if (side == 0)
            {
                SDL_SetRenderDrawColor(gRenderer,curWall.lightColor.r, curWall.lightColor.g, curWall.lightColor.b, 0xFF);
            }
            else{
                SDL_SetRenderDrawColor(gRenderer,curWall.darkColor.r, curWall.darkColor.g, curWall.darkColor.b, 0xff);
            }
            
            SDL_RenderDrawLine(gRenderer, x, drawStart, x, drawEnd);
        }
        
        SDL_RenderPresent(gRenderer);
        oldTime = time;
        time = SDL_GetTicks();
        float frameTime = (time - oldTime)/ 1000;
        // cout << 1.0/frameTime << endl;
        float lastDirx = xDir;
        float lastXPlane = xPlane;
        float deltaTSpeed = speed * frameTime;
        float deltaTRotationSpeed = rotationSpeed * frameTime;
        if( SDL_PollEvent(&windowEvent)){
            if( SDL_QUIT == windowEvent.type){break;}
            if(windowEvent.type == SDL_KEYDOWN)
            {
                switch (windowEvent.key.keysym.sym)
                {
                case SDLK_w:
                    if(map[int(xPos + xDir * deltaTSpeed)][int(yPos)] == 0) xPos += xDir * deltaTSpeed;
                    if(map[int(xPos)][int(yPos + yDir * deltaTSpeed)] == 0) yPos += yDir * deltaTSpeed;
                    break;
                case SDLK_s:
                    if(map[int(xPos - xDir * deltaTSpeed)][int(yPos)] == 0) xPos -= xDir * deltaTSpeed;
                    if(map[int(xPos)][int(yPos - yDir * deltaTSpeed)] == 0) yPos -= yDir * deltaTSpeed;
                    break;
                case SDLK_d:
                    xDir = xDir * cos(-deltaTRotationSpeed) - yDir * sin(-deltaTRotationSpeed);
                    yDir = lastDirx * sin(-deltaTRotationSpeed) + yDir * cos(-deltaTRotationSpeed);

                    xPlane = xPlane * cos(-deltaTRotationSpeed) - yPlane * sin(-deltaTRotationSpeed);
                    yPlane = lastXPlane * sin(-deltaTRotationSpeed) + yPlane * cos(-deltaTRotationSpeed);
                    break;
                case SDLK_a:
                    xDir = xDir * cos(deltaTRotationSpeed) - yDir * sin(deltaTRotationSpeed);
                    yDir = lastDirx * sin(deltaTRotationSpeed) + yDir * cos(deltaTRotationSpeed);

                    xPlane = xPlane * cos(deltaTRotationSpeed) - yPlane * sin(deltaTRotationSpeed);
                    yPlane = lastXPlane * sin(deltaTRotationSpeed) + yPlane * cos(deltaTRotationSpeed);
                    break;
                default:
                    break;
                }
            }
        }
        SDL_Delay(ticks_per_frame - frameTime);
    }

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);

    SDL_Quit();
    return EXIT_SUCCESS;
}