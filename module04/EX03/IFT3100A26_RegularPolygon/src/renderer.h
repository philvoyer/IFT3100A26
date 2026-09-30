// IFT3100A26_RegularPolygon/renderer.h
// Classe responsable du rendu de l'application.

#pragma once

#include "ofMain.h"

class Renderer
{
public:

  std::string polygon_name;

  float polygon_center_x;
  float polygon_center_y;

  float angle;
  float offset;
  float radius;

  float point_diameter;

  int side_count;

  void setup();
  void update();
  void draw();

  void set_side_count(int count);
};
