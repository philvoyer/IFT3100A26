// IFT3100A26_ImageFilter/renderer.h
// Classe responsable du rendu de l'application.

#pragma once

#include "ofMain.h"

class Renderer
{
public:

  ofImage image;

  ofShader shader;

  float factor_tint_mix;
  float factor_opacity;

  ofColor tint_color;

  int offset_vertical;
  int offset_horizontal;

  void setup();
  void draw();

  void resize_window();
};
