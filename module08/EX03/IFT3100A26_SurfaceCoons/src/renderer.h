// IFT3100A26_SurfaceCoons/renderer.h
// Classe responsable du rendu de l'application.

#pragma once

#include "ofMain.h"

// énumération de types d'interpolation
enum class InterpolationType {lerp, smoothstep};

// nombre de courbes de bord (C1, C2, C3 et C4)
const int boundary_count = 4;

// nombre de points de contrôle (4 coins partagés par les courbes de bord et 2 points intérieurs par courbe)
const int ctrl_point_count = 12;

// nombre d'intervalles entre les isolignes dans chaque direction de la surface
const int isoline_count = 10;

class Renderer
{
public:

  InterpolationType interpolation_type;

  string surface_name;

  // courbes de bord de la surface
  ofPolyline boundary_renderer[boundary_count];

  // isolignes de la surface à v constant (u varie) et à u constant (v varie)
  ofPolyline isoline_u_renderer[isoline_count + 1];
  ofPolyline isoline_v_renderer[isoline_count + 1];

  // isolignes animées par la tête de lecture, à v constant (u varie) et à u constant (v varie)
  ofPolyline playhead_u_renderer;
  ofPolyline playhead_v_renderer;

  glm::vec3* selected_ctrl_point;

  glm::vec3 ctrl_point[ctrl_point_count];

  glm::vec3 initial_position[ctrl_point_count];

  // couleur de chacune des courbes de bord
  ofColor curve_color[boundary_count];

  // couleur de chaque point de contrôle (la somme des couleurs des courbes auxquelles il appartient)
  ofColor ctrl_point_color[ctrl_point_count];

  glm::vec3 position;

  glm::vec3 playhead_position;

  float playhead_u;
  float playhead_v;

  float line_width_outline;
  float line_width_isoline;
  float line_width_curve;

  float radius;
  float scale;
  float offset;

  float delta_x;
  float delta_y;
  float delta_z;

  float motion_speed;

  float camera_angle;
  float camera_distance;
  float camera_rotation_speed;

  float oscillation_u;
  float oscillation_v;
  float oscillation_period_u;
  float oscillation_period_v;
  float oscillation_amplitude;

  float animation_time;

  int framebuffer_width;
  int framebuffer_height;

  int line_resolution;

  int selected_index;

  int index;

  bool isAnimationActive;

  void setup();
  void update();
  void draw();
  void reset();

  void select_next_ctrl_point();

  glm::vec3 evaluate_boundary(int curve, float t);
  glm::vec3 evaluate_surface(float u, float v);

  float oscillate(float time, float amplitude, float period);
};
