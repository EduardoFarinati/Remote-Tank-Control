#ifndef GRAPHICS_H
#define GRAPHICS_H


#include <SDL/SDL.h>


#define SCREEN_W 640 //tamanho da janela que sera criada
#define SCREEN_H 480

//#define BPP 8
//typedef Uint8 PixelType;
//#define BPP 16
//typedef Uint16 PixelType;
#define BPP 32
typedef Uint32 PixelType;

// Graphics struct definition
typedef struct canvas {
  SDL_Surface *canvas;
  int Height; // canvas height
  int Width;  // canvas width
  int Xoffset; // X off set, in canvas pixels
  int Yoffset; // Y off set, in canvas pixels
  int Xext; // X extra width
  int Yext; // Y extra height
  double Xmax;
  double Ymax;
  double Xstep; // half a distance between X pixels in 'Xmax' scale

  PixelType *zpixel;

} Tcanvas;

typedef struct dataholder {
  Tcanvas *canvas;
  double   Tcurrent;
  double   Lcurrent;
  PixelType Lcolor;
  double   INcurrent;
  PixelType INcolor;
  double   OUTcurrent;
  PixelType OUTcolor;

} Tdataholder;


Tdataholder *datainit(
    int Width,
    int Height,
    double Xmax,
    double Ymax,
    double Lcurrent,
    double INcurrent,
    double OUTcurrent
);

void datadraw(
    Tdataholder *data,
    double time,
    double level,
    double inangle,
    double outangle
);

int quit_event();

void quit();


#endif