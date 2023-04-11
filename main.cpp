//PARALELA - PROYECTO
// ABRIL 2023

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <omp.h>
using namespace std;

#define numberOfTriesAllow 1000
#define maxRadiusMCU 20


struct Square
{

    int x; // coordenada x de la esquina superior izquierda
    int y; // coordenada y de la esquina superior izquierda
    int initialX; // Posición inicial X
    int initialY; // Posición inicial Y
    int width; // anchura del cuadrado
    int height; // altura del cuadrado
    float angle; // Ángulo
    int radius; // Radio
    float angularSpeed; // Velocidad angular
    SDL_Color color; // Color

};

Square generateRandomSquare(SDL_Window *window)
{
    Square square;
    int size = (rand() % 45) + 5;
    int winWidth, winHeight;
    SDL_GetWindowSize(window, &winWidth, &winHeight);
    square.x = (rand() % (winWidth - size * 2)) + size;  // módulo de ventana menos el tamaño multiplicado por 2 más el tamaño para garantizar que el cuadrado quepa dentro de la ventana
    square.y = (rand() % (winHeight - size * 2)) + size; 
    square.initialX = square.x;
    square.initialY = square.y;
    square.angle = rand() % 360;

    int x = rand() % 2;
    if (x == 0)
    {
        square.angularSpeed = -2;
    }
    else
    {
        square.angularSpeed = 2;
    }
    square.radius = rand() % maxRadiusMCU + 5;
    square.width = size;
    square.height = square.width;
    square.color = {static_cast<unsigned char>(rand() % 256),
                    static_cast<unsigned char>(rand() % 256),
                    static_cast<unsigned char>(rand() % 256),
                    255}; // Generar un color aleatorio con alfa completo
    return square;
}


bool checkCollision(Square square1, Square square2)
{
    // Calcular las coordenadas x e y de las cajas delimitadoras
    int square1X1 = square1.x;
    int square1X2 = square1.x + square1.width;
    int square1Y1 = square1.y;
    int square1Y2 = square1.y + square1.height;
    int square2X1 = square2.x;
    int square2X2 = square2.x + square2.width;
    int square2Y1 = square2.y;
    int square2Y2 = square2.y + square2.height;

    // Comprueba si las cajas delimitadoras se solapan en las coordenadas x e y
    if (square1X1 < square2X2 && square1X2 > square2X1 && square1Y1 < square2Y2 && square1Y2 > square2Y1)
    {
        return true; // Se solapan
    }
    return false; // No se solapan
}

int main(int argc, char *argv[])
{
    // Generador de números aleatorios
    srand((unsigned int)time(NULL));
    int numberSquares = 30;

    if (argc > 1)
    {
      
        if (atoi(argv[1]) == 0)
        {
            printf("The argument must be a number\n");
            return 1;
        }
        numberSquares = atoi(argv[1]);
    }
    // Dimensiones
    int windowWidth = 640;
    int windowHeight = 480;
 
    int frames = 0;
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    SDL_Window *window = SDL_CreateWindow("GAME COSINE", // creates a window
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          windowWidth, windowHeight, 0);

    // Renderizado
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return 1;
    }

    // array de N cuadrados
    Square squaresArray[numberSquares];
    // calcula el speedup
    double timeInit = omp_get_wtime();

    // genera random los cuadrados
    for (int i = 0; i < numberSquares; i++)
    {
        Square square;
        square = generateRandomSquare(window);

        int numberOfTries = 0;
        int j = 0;
        // Revisa si se solapan
        while (j < numberSquares && numberOfTries < numberOfTriesAllow && j < i)
        {
            Square previousSquare = squaresArray[j];

            if (checkCollision(square, previousSquare))
            {
                // Si se solapa genera otro nuevo cuadraro
                square = generateRandomSquare(window);
                numberOfTries++;
                j = -1;
            }
            j++;
        }

    
        if (numberOfTries >= numberOfTriesAllow)
        {
            square.x = -1;
            square.y = -1;
            square.width = 0;
            square.height = 0;
        }

        
        squaresArray[i] = square;
    }

    
    Uint32 prevTicks = SDL_GetTicks();
    Uint32 start = SDL_GetTicks();
    bool firstTime = true;
    while (true)
    {
        // Eventos
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                return 0;
            }
        }

        // Limpia pantalla
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        
        Uint32 currentTicks = SDL_GetTicks();
        float deltaTime = (currentTicks - prevTicks) / 1000.0f;
        prevTicks = currentTicks;

        for (int i = 0; i < numberSquares; i++)
        {
            // Colision en las paredes
            if (squaresArray[i].x < 0 || squaresArray[i].x > windowWidth - squaresArray[i].width)
            {
                squaresArray[i].angularSpeed *= -1;
            }
            if (squaresArray[i].y < 0 || squaresArray[i].y > windowHeight - squaresArray[i].height)
            {
                squaresArray[i].angularSpeed *= -1;
            }
            for (int j = 0; j < numberSquares; j++)
            {
                
                if (i != j)
                {
                    
                    float angleI = squaresArray[i].angle + (squaresArray[i].angularSpeed * deltaTime);

                    
                    int posIx = squaresArray[i].initialX + squaresArray[i].radius * cos(angleI);
                    int posIy = squaresArray[i].initialY + squaresArray[i].radius * sin(angleI);

                    
                    float angleJ = squaresArray[j].angle + (squaresArray[j].angularSpeed * deltaTime);

                   
                    int posJx = squaresArray[j].initialX + squaresArray[j].radius * cos(angleJ);
                    int posJy = squaresArray[j].initialY + squaresArray[j].radius * sin(angleJ);

                    SDL_Rect rectangleI = {posIx, posIy, squaresArray[i].width, squaresArray[i].height};
                    SDL_Rect rectangleJ = {posJx, posJy, squaresArray[j].width, squaresArray[j].height};
                    SDL_bool HasIntersectionNext = SDL_HasIntersection(&rectangleI, &rectangleJ);

                  
                    float otherAngleI = squaresArray[i].angle + (squaresArray[i].angularSpeed * deltaTime * (-1));

                   
                    int otherPosIx = squaresArray[i].initialX + squaresArray[i].radius * cos(otherAngleI);
                    int otherPosIy = squaresArray[i].initialY + squaresArray[i].radius * sin(otherAngleI);

                    
                    float otherAngleJ = squaresArray[j].angle + (squaresArray[j].angularSpeed * deltaTime * (-1));

                   
                    int otherPosJx = squaresArray[j].initialX + squaresArray[j].radius * cos(otherAngleJ);
                    int otherPosJy = squaresArray[j].initialY + squaresArray[j].radius * sin(otherAngleJ);

                    SDL_Rect otherRectangleI = {otherPosIx, otherPosIy, squaresArray[i].width, squaresArray[i].height};
                    SDL_Rect otherRectangleJ = {otherPosJx, otherPosJy, squaresArray[j].width, squaresArray[j].height};
                    SDL_bool otherHasIntersectionNext = SDL_HasIntersection(&otherRectangleI, &otherRectangleJ);

                    if (HasIntersectionNext && otherHasIntersectionNext)
                    {
                        squaresArray[i].width = squaresArray[i].width / 2;
                        squaresArray[i].height = squaresArray[i].height / 2;
                    }
                    else if (HasIntersectionNext)
                    {
                        squaresArray[i].angularSpeed *= -1;
                    }
                }
            }
        }

        // Update de los valores
        for (int i = 0; i < numberSquares; i++)
        {
            
            squaresArray[i].angle = squaresArray[i].angle + (squaresArray[i].angularSpeed * deltaTime);

           
            squaresArray[i].x = squaresArray[i].initialX + squaresArray[i].radius * cos(squaresArray[i].angle);
            squaresArray[i].y = squaresArray[i].initialY + squaresArray[i].radius * sin(squaresArray[i].angle);

            // Burbujas
            SDL_SetRenderDrawColor(renderer, squaresArray[i].color.r, squaresArray[i].color.g, squaresArray[i].color.b, squaresArray[i].color.a);
            SDL_Rect rectangle = {squaresArray[i].x, squaresArray[i].y, squaresArray[i].width, squaresArray[i].height};
            SDL_RenderFillRect(renderer, &rectangle);
        }

        
        SDL_RenderPresent(renderer);

       
        frames++;
        Uint32 end = SDL_GetTicks();
        Uint32 elapsed = end - start;

        if (elapsed >= 1000)
        {
            double fps = frames / (elapsed / 1000.0);
            printf("FPS: %f\n", fps);
            frames = 0;
            start = end;
        }
        if (firstTime)
        {
            double timeFin = omp_get_wtime();
            double delta = timeFin - timeInit;
            cout << "Sequential program took " << delta << " seconds" << endl;
        }
        firstTime = false;
    }
    // Limpieza
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}