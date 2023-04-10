//Importamos cada una de las librerias

//Con respecto a nuestro encabezado
#include<iostream.h>
#include<stdlib.h>
#include<stdio.h>
#include<conio.h>

//Para cada una de nuestras figuras
#include<graphics.h>
#include<dos.h>

//Inicializamos el programa
void main(){
	
//Calculamos el tamaño como tal
int g=DETECT,o,col=480,row=640,,dir=2,j=8,ff=15,f=4;

//Mostramos
initgraph(&g,&o,"c:\\turboc3\\bgi");
cleardevice();
while(!kbit())
{
settextstyle(random(f),random(dir),random(j));

//Definimos los colores
setcolor(random(ff));
outtextxy(random(col),random(row),"Paralela");
delay(250);
}
getch();
closegraph();
}
