#include <SDL/SDL.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>

#include "graphics.h"


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


inline void c_pixeldraw(Tcanvas *canvas, int x, int y, PixelType color)
{
  *( ((PixelType*)canvas->canvas->pixels) + ((-y+canvas->Yoffset) * canvas->canvas->w + x+ canvas->Xoffset)) = color;
}

inline void c_hlinedraw(Tcanvas *canvas, int xstep, int y, PixelType color)
{
  int offset =  (-y+canvas->Yoffset) * canvas->canvas->w;
  int x;

  for (x = 0; x< canvas->Width+canvas->Xoffset ; x+=xstep) {
        *( ((PixelType*)canvas->canvas->pixels) + (offset + x)) = color;
  }
}

inline void c_vlinedraw(Tcanvas *canvas, int x, int ystep, PixelType color)
{
  int offset = x+canvas->Xoffset;
  int y;
  int Ystep = ystep*canvas->canvas->w;

  for (y = 0; y< canvas->Height+canvas->Yext ; y+=ystep) {
    *( ((PixelType*)canvas->canvas->pixels) + (offset + y*canvas->canvas->w)) = color;
  }
}


inline void c_linedraw(Tcanvas *canvas, double x0, double y0, double x1, double y1, PixelType color) {
  double x;

  for (x=x0; x<=x1; x+=canvas->Xstep) {
    c_pixeldraw(canvas, (int)(x*canvas->Width/canvas->Xmax+0.5), (int)((double)canvas->Height/canvas->Ymax*(y1*(x1-x)+y1*(x-x0))/(x1-x0)+0.5),color);
  }
}


Tcanvas *c_open(int Width, int Height, double Xmax, double Ymax)
{
  int x,y;
  Tcanvas *canvas;
  canvas = malloc(sizeof(Tcanvas));

  canvas->Xoffset = 10;
  canvas->Yoffset = Height;

  canvas->Xext = 10;
  canvas->Yext = 10;



  canvas->Height = Height;
  canvas->Width  = Width; 
  canvas->Xmax   = Xmax;
  canvas->Ymax   = Ymax;

  canvas->Xstep  = Xmax/(double)Width/2;

  //  canvas->zpixel = (PixelType *)canvas->canvas->pixels +(Height-1)*canvas->canvas->w;

  SDL_Init(SDL_INIT_VIDEO); //SDL init
  canvas->canvas = SDL_SetVideoMode(canvas->Width+canvas->Xext, canvas->Height+canvas->Yext, BPP, SDL_SWSURFACE); 

  c_hlinedraw(canvas, 1, 0, (PixelType) SDL_MapRGB(canvas->canvas->format,  255, 255,  255));
  for (y=10;y<Ymax;y+=10) {
    c_hlinedraw(canvas, 3, y*Height/Ymax , (PixelType) SDL_MapRGB(canvas->canvas->format,  220, 220,  220));
  }
  c_vlinedraw(canvas, 0, 1, (PixelType) SDL_MapRGB(canvas->canvas->format,  255, 255,  255));
  for (x=10;x<Xmax;x+=10) {
    c_vlinedraw(canvas, x*Width/Xmax, 3, (PixelType) SDL_MapRGB(canvas->canvas->format,  220, 220,  220));
  }

  return canvas;
}



Tdataholder *datainit(int Width, int Height, double Xmax, double Ymax, double Lcurrent, double INcurrent, double OUTcurrent) {
  Tdataholder *data = malloc(sizeof(Tdataholder));


  data->canvas=c_open(Width, Height, Xmax, Ymax);
  data->Tcurrent=0;
  data->Lcurrent=Lcurrent;
  data->Lcolor= (PixelType) SDL_MapRGB(data->canvas->canvas->format,  255, 180,  0);
  data->INcurrent=INcurrent;
  data->INcolor=(PixelType) SDL_MapRGB(data->canvas->canvas->format,  180, 255,  0);
  data->OUTcurrent=OUTcurrent;
  data->OUTcolor=(PixelType) SDL_MapRGB(data->canvas->canvas->format,  0, 180,  255);


  return data;
}

void setdatacolors(Tdataholder *data, PixelType Lcolor, PixelType INcolor, PixelType OUTcolor) {
  data->Lcolor=Lcolor;
  data->INcolor=INcolor;
  data->OUTcolor=OUTcolor;
}




void datadraw(Tdataholder *data, double time, double level, double inangle, double outangle) {
  c_linedraw(data->canvas,data->Tcurrent,data->Lcurrent,time,level,data->Lcolor);
  c_linedraw(data->canvas,data->Tcurrent,data->INcurrent,time,inangle,data->INcolor);
  c_linedraw(data->canvas,data->Tcurrent,data->OUTcurrent,time,outangle,data->OUTcolor);
  data->Tcurrent = time;
  data->Lcurrent = level;
  data->INcurrent = inangle;
  data->OUTcurrent = outangle;

  //SDL_Flip(data->canvas->canvas);
}

int quit_event() {
  SDL_Event event;

  while(SDL_PollEvent(&event)) { 
    if(event.type == SDL_QUIT) { 
      // close files, etc...

      SDL_Quit();
      return 1;
    }
  }

  return 0;
}

//
//
//
//
//
//
//

int graphics_main( int argc, const char* argv[] ) {
  Tdataholder *data;
  double t=0;

  data = datainit(640,480,55,110,45,0,0);

  for (t=0;t<50;t+=0.1) {
    datadraw(data,t,(double)(50+20*cos(t/5)),(double)(70+10*sin(t/10)),(double)(20+5*cos(t/2.5)));
  }

  while(1) {
    quit_event();
  }
}

//
//
//
//
//
//
//

#include <pthread.h>


// Canvas to draw state variables
Tdataholder* graph_data;
pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;

// Defines data queue
typedef struct graph_data_raw_t {
  double time;
  double level;
  double input;
  double output;
} graph_data_raw;

#define MAX_QUEUE 20
typedef struct data_queue_t {
  size_t head;
  size_t tail;
  graph_data_raw data[MAX_QUEUE];
} data_queue;
data_queue queue = {
  .head = 0,
  .tail = 0
};
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;


int queue_read(graph_data_raw* data) {
  pthread_mutex_lock(&queue_mutex);
 
  // Empty queue
  if(queue.tail == queue.head) {
    pthread_mutex_unlock(&queue_mutex);
    return 0;
  }
  else {
    *data = queue.data[queue.tail];
    queue.tail = (queue.tail + 1) % MAX_QUEUE;

    pthread_mutex_unlock(&queue_mutex);
    return 1;
  }
}

void queue_write(graph_data_raw data) {
  pthread_mutex_lock(&queue_mutex);
  queue.data[queue.head] = data;
  queue.head = (queue.head + 1) % MAX_QUEUE;

  if(queue.head == queue.tail) {
    queue.tail = (queue.tail + 1) % MAX_QUEUE;
  }

  pthread_mutex_unlock(&queue_mutex);
}

void clear_queue() {
  int i;

  pthread_mutex_lock(&queue_mutex);
  queue.head = 0;
  queue.tail = 0;
  for(i = 0; i < MAX_QUEUE; i++) {
    queue.data[i] = (graph_data_raw) {
      .time = 0,
      .level = 0,
      .input = 0,
      .output = 0
    };
  } 
  pthread_mutex_unlock(&queue_mutex);
}

int _new_graph_flag = 0;
pthread_mutex_t new_graph_mutex = PTHREAD_MUTEX_INITIALIZER;

void set_recreate_graph() {
  pthread_mutex_lock(&new_graph_mutex);
  _new_graph_flag = 1;
  pthread_mutex_unlock(&new_graph_mutex);
}

void graph_recreated() {
  pthread_mutex_lock(&new_graph_mutex);
  _new_graph_flag = 0;
  pthread_mutex_unlock(&new_graph_mutex);
}

int should_recreate_graph() {
  int new_graph_flag;

  pthread_mutex_lock(&new_graph_mutex);
  new_graph_flag = _new_graph_flag;
  pthread_mutex_unlock(&new_graph_mutex);

  return new_graph_flag;
}

#define WIDTH 55
#define HEIGHT 110
double level_current = 45;
double input_current = 50;
double output_current = 0;
void new_graph() {
  graph_data = datainit(
    SCREEN_W,
    SCREEN_H,
    WIDTH,
    HEIGHT,
    level_current,
    input_current,
    output_current
  );
}

void recreate_graph() {
  // Clears memory and recreates graph
  pthread_mutex_lock(&data_mutex);
  free(graph_data->canvas);
  free(graph_data);
  new_graph();
  pthread_mutex_unlock(&data_mutex);

  clear_queue();
  graph_recreated();
}

int clear_graph_on_overflow = 1;
int current_mod = 0;
void modular_datadraw(Tdataholder *data, double time, double level, double inangle, double outangle) {
  if(ceil(time) > (current_mod + 1) * WIDTH) {
    current_mod += 1;
    if(clear_graph_on_overflow) {
      level_current = level;
      input_current = inangle;
      output_current = outangle;
      set_recreate_graph();
    }
  }
  else {
    double mod_time = time - current_mod * WIDTH;
    datadraw(data, mod_time, level, inangle, outangle);
  }
}

void update_graph_data(double time, double level, double input, double output) {
  graph_data_raw data;

  if(pthread_mutex_trylock(&data_mutex) == 0) {
    while(queue_read(&data) != 0) {
      modular_datadraw(graph_data, data.time, data.level, data.input, data.output);
    }
    modular_datadraw(graph_data, time, level, input, output);
    pthread_mutex_unlock(&data_mutex);
  }
  else {
    data = (graph_data_raw) {
      .time = time,
      .level = level,
      .input = input,
      .output = output
    };

    queue_write(data);
  }
}

void draw_graph() {
  if(!should_recreate_graph()) {
    pthread_mutex_lock(&data_mutex);
    SDL_Flip(graph_data->canvas->canvas);
    pthread_mutex_unlock(&data_mutex);
  }
  else {
    recreate_graph();
  }
}

int window_closed() {
  return quit_event();
}
