// IFT3100A26_BonjourTriangle/renderer.h
// Classe responsable du rendu de l'application.

#pragma once

#include "ofMain.h"

class Renderer
{
public:

  float vertex1_x;
  float vertex1_y;
  float vertex2_x;
  float vertex2_y;
  float vertex3_x;
  float vertex3_y;

  unsigned char color_r;
  unsigned char color_g;
  unsigned char color_b;

  float point_diameter;

  int framebuffer_width;
  int framebuffer_height;

  void setup();
  void update();
  void draw();
};
