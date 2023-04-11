//Importamos cada una de las librerias

//Con respecto a nuestro encabezado

#include<stdio.h>
#include<conio.h>

//Para cada una de nuestras figuras
#include<graphics.h>
#include<dos.h>

//Inicializamos
void main()
{
	int gd = DETECT,gm;
	int x,y,a;
	initgraph(&gd, &gm, "C:\\Turboc3\\BGI");
	x=getmaxx()/2;
	y=getmaxy()/2;
	for(a=30;a<200;a++)
	
	{
		delay(100);
		setcolor(a/10);
		arc(x,y,0,180,a-10);
	}
	
	getch();
	
}


