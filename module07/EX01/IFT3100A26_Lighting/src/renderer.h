// IFT3100A26_Lighting/renderer.h
// Classe responsable du rendu de l'application.

#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

class Renderer
{
public:

  ofColor light_ambient;

  ofLight light_directional;
  ofLight light_point;
  ofLight light_spot;

  ofMaterial material_cube;
  ofMaterial material_sphere;
  ofMaterial material_teapot;

  ofxAssimpModelLoader teapot;

  glm::quat orientation_directional;
  glm::quat orientation_spot;

  glm::vec3 position_cube;
  glm::vec3 position_sphere;
  glm::vec3 position_teapot;

  float camera_offset;

  float oscillation;

  float scale_cube;
  float scale_sphere;
  float scale_teapot;

  float center_x;
  float center_y;

  float speed_motion;

  float oscillation_period;
  float oscillation_amplitude;

  float offset_x;
  float offset_z;

  float delta_x;
  float delta_z;

  float initial_x;
  float initial_z;

  bool is_active_light_ambient;
  bool is_active_light_directional;
  bool is_active_light_point;
  bool is_active_light_spot;

  bool use_smooth_lighting;

  Renderer();

  void setup();
  void update();
  void draw();

  void reset();

  void lighting_on();
  void lighting_off();

  float oscillate(float time, float amplitude, float period);
};
