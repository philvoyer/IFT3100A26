// IFT3100A26_CameraFrustum/renderer.h
// Classe responsable du rendu de l'application.

#pragma once

#include "ofMain.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

class Renderer
{
public:

  ofPolyline camera_frustum_outline;

  ofColor color_stroke;
  ofColor color_fill;
  ofColor color_invalid_stroke;
  ofColor color_invalid_fill;

  bool is_frustum_valid;

  float camera_clip_n;
  float camera_clip_f;

  float camera_fov_h;
  float camera_fov_v;

  float camera_zoom;

  float clip_half_height_n;
  float clip_half_height_f;

  float clip_height_n;
  float clip_height_f;

  float camera_resolution_w;
  float camera_resolution_h;

  float camera_aspect_ratio;

  float circle_radius;
  float stroke_width;
  float scale_factor;

  glm::vec3 camera_position;
  glm::vec3 camera_target;

  glm::vec3 position_clip_n_b;
  glm::vec3 position_clip_n_c;
  glm::vec3 position_clip_n_t;
  glm::vec3 position_clip_f_b;
  glm::vec3 position_clip_f_c;
  glm::vec3 position_clip_f_t;

  glm::mat4 matrix_model;
  glm::mat4 matrix_view;
  glm::mat4 matrix_camera;
  glm::mat4 matrix_projection;
  glm::mat4 matrix_model_view;
  glm::mat4 matrix_model_view_projection;

  void setup();
  void reset();
  void update();
  void draw();

  void generate_matrix();
  void log_matrix();

  float compute_fov_from_zoom(float zoom);
  float compute_zoom_from_fov(float fov);
};
