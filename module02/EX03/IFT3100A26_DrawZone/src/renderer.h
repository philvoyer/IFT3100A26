// IFT3100A25_DrawZone/renderer.h
// Classe responsable du rendu de l'application.

#pragma once

#include "ofMain.h"

class Renderer
{
public:

  int mouse_press_x;
  int mouse_press_y;

  int mouse_current_x;
  int mouse_current_y;

  float radius;

  bool is_mouse_button_pressed;

  void setup();
  void draw();

  void draw_zone(float x1, float y1, float x2, float y2) const;
  void draw_cursor(float x, float y) const;
};
