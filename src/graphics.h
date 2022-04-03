#ifndef GRAPHICS_H
#define GRAPHICS_H


#define SCREEN_W 640 //tamanho da janela que sera criada
#define SCREEN_H 640


extern int clear_graph_on_overflow;


void new_graph();

void set_recreate_graph();

void update_graph_data(double time, double level, double input, double output);

void draw_graph();

int window_closed();


#endif