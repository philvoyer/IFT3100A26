// IFT3100A26_CurveHermite/renderer.h
// Classe responsable du rendu de l'application.

#pragma once

#include "ofMain.h"

// énumération de types de courbe
enum class CurveType {bezier_cubic, hermite};

// énumération de types d'interpolation
enum class InterpolationType {lerp, smoothstep};

class Renderer
{
public:

  CurveType curve_type;

  InterpolationType interpolation_type;

  string  curve_name;

  ofPolyline line_renderer;

  glm::vec3* selected_ctrl_point;

  glm::vec3 ctrl_point1;
  glm::vec3 ctrl_point2;
  glm::vec3 ctrl_point3;
  glm::vec3 ctrl_point4;

  glm::vec3 initial_position1;
  glm::vec3 initial_position2;
  glm::vec3 initial_position3;
  glm::vec3 initial_position4;
  glm::vec3 initial_position5;

  glm::vec3 position;

  glm::vec3 tangent1;
  glm::vec3 tangent2;

  glm::vec3 playhead_position;

  float playhead;

  float line_width_outline;
  float line_width_curve;

  float radius;
  float scale;
  float offset;

  float delta_x;
  float delta_y;

  float motion_speed;

  float oscillation;
  float oscillation_period;
  float oscillation_amplitude;

  float animation_time;

  int framebuffer_width;
  int framebuffer_height;

  int line_resolution;

  int index;

  bool isAnimationActive;

  void setup();
  void update();
  void draw();
  void reset();

  float oscillate(float time, float amplitude, float period);
};
