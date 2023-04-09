
//Universidad del Valle de Guatemala
//Proyecto Paralela

//Colocamos cada una de nuestras bibliotecas

//Utilizamos las bibliotecas tanto para el encabezado como para graficar y realizar cada una de las operaciones matematicas

//Hacemos uso tambien de nuestro OPenMP

//Encabezado
#include <iostream>
#include <cstdlib>

//Tiempo
#include <ctime>

//Operaciones
#include <cmath>

//Graficas
#include <GL/glut.h>

//Interfaz
#include <omp.h>

//Realizamos nuestra figura de la escena
int flag=0,ch=0,ch1=0;
float a=0;


//Colocamos cada una de nuestras variables
int Ancho = 640;
int Altura = 480;
double Radio = 0.02;

//Cantidad de las figuras
const int N = 10;

//Posicion de las figuras
double PX[N];
double PY[N];

//Velocidad de las figuras
double VX[N];
double VY[N];

//FPS
int Contador = 0;
int fps = 0;
int Tiempo = 0;
int Antes = 0;

//Escena general
void drawScene()
{
    //Colocamos cada vez mas FPS
    #pragma omp atomic
    Contador++;

    //Tiempo en milisegundos
    Tiempo = glutGet(GLUT_ELAPSED_TIME);
    int Actual = Tiempo - Antes;

    // Actualizar el FPS
    if (Actual > 500)
    {
        fps = Contador / (Actual / 500.0);
        Contador = 0;
        Antes = Tiempo;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    //Dibujar la pez
    for (int i = 0; i < N; i++) {
   
   //Cuerpo del pez
    glPushMatrix();
    glTranslated(PX[i], PY[i], 0.0);
    glColor3f(0.0,1.0,0.0);
  glBegin(GL_POLYGON);
    glVertex2f(0.7+a,0.05);
    glVertex2f(0.75+a,0.1);
    glVertex2f(0.85+a,0.05);
    glVertex2f(0.75+a,0.0);
  glEnd();

//Aletas y cola del pez
  glBegin(GL_TRIANGLES);
  glColor3f(0.0,0.0,1.0);
    glVertex2f(0.83+a,0.05);
    glColor3f(0.0,1,0);
    glVertex2f(0.9+a,0.09);
    glVertex2f(0.9+a,0.01);
  glEnd();

  glBegin(GL_TRIANGLES);
  glColor3f(1.0,1.0,0.0);
    glVertex2f(0.75+a,0.095);
glColor3f(1.0,0.0,0.0);
glVertex2f(0.79+a,0.125);
glVertex2f(0.77+a,0.07);
  glEnd();

glBegin(GL_TRIANGLES);
glColor3f(1.0,1.0,0.0);
    glVertex2f(0.75+a,0.007);
glColor3f(1.0,0.0,0.0);
glVertex2f(0.795+a,-0.035);
glVertex2f(0.77+a,0.02);
  glEnd();


  glColor3f(0.0,0.0,0.0);
  glPointSize(4.0);
  glBegin(GL_POINTS);
  glVertex2f(0.73+a,0.065);
  glEnd();
 
  glPopMatrix();

    }

    //Mostrar el FPS
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, Ancho, 0, Altura);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2i(10, 10);
    std::string fpsStr = "FPS: " + std::to_string(fps);
    for (char c : fpsStr)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glutSwapBuffers();

    //Usar OpenMP para paralelizar
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        //Modificamos la posicion de la figura
        #pragma omp atomic
        PX[i] += VX[i];
        #pragma omp atomic
        PY[i] += VY[i];

        //Movimiento del pez
        if (PX[i] > 1.0 - Radio)
        {
            PX[i] = 1.0 - Radio;
            VX[i] = -VX[i];
        }
        else if (PX[i] < -1.0 + Radio)
        {
            PX[i] = -1.0 + Radio;
            VX[i] = -VX[i];
        }
        if (PY[i] > 1.0 - Radio)
        {
            PY[i] = 1.0 - Radio;
            VY[i] = -VY[i];
        }
        else if (PY[i] < -1.0 + Radio)
        {
            PY[i] = -1.0 + Radio;
            VY[i] = -VY[i];
        }
    }    
}

//Inicialización de OpenGL
void init()
{

    #pragma omp parallel for
    for (int i = 0; i < N; i++)
    {

        //Valores random
        PX[i] = ((double)rand() / RAND_MAX) * 3.0 - 2.0;
        PY[i] = ((double)rand() / RAND_MAX) * 3.0 - 2.0;

        //Velocidad random

        VX[i] = ((double)rand() / RAND_MAX) * 0.02 - 0.01;
        VY[i] = ((double)rand() / RAND_MAX) * 0.02 - 0.01;

    }
    //Color de la escena
    glClearColor(0, 0, 1, 0.0);

    //Angulo de la camara
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

//Main
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    //Tamaño de 640x480 como minimo
    glutInitWindowSize(Ancho, Altura);

    //Posicion de nuestro screensaver
    glutInitWindowPosition(100, 100);

    //Nombre al momento de mostrar el resultado
    glutCreateWindow("Screensaver");
    glutDisplayFunc(drawScene);

    //Threads
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <number_of_threads>" << std::endl;
        return 1;


        //Numero
    }else if (std::stoi(argv[1]) < 1) {
        std::cerr << "Numero de threads" << std::endl;
        return 1;
    }

    //Vemos la cantidad de threads
    int numThreads = std::stoi(argv[1]);

    omp_set_num_threads(numThreads);

    clock_t GG = clock();

    //Empezamos con el OpenGL
    init();

    srand(time(0));

    //Animacion en la escena
    glutIdleFunc(drawScene);
    clock_t FF = clock();

    //Tiempo total
    double SGS = double(FF - GG) / CLOCKS_PER_SEC;
    std::cout << "Tiempo: " << SGS << " segs" << std::endl;

    //Repetir
    glutMainLoop();

    return 0;
}