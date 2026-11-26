// IFT3100A26_RenderArchitecture/renderer.h
// Classe responsable du rendu de l'application.

#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

// nombre de lumières de la scène (le même que dans les shaders)
const int light_count = 4;

// énumération des modes de rendu
enum class RenderMode {forward, deferred, layer_position, layer_normal, layer_color, layer_depth, layer_all};

class Renderer
{
public:

  // shaders des deux architectures de rendu
  ofShader shader_forward;
  ofShader shader_gbuffer;
  ofShader shader_deferred;
  ofShader shader_gbuffer_view;

  // g-buffer : framebuffer avec trois buffers de rendu couleur et un buffer de profondeur
  ofFbo gbuffer;

  // objets de la scène
  ofMesh triangle;
  ofxAssimpModelLoader teapot;

  // couleur de chaque objet (albédo)
  ofColor color_triangle;
  ofColor color_cube;
  ofColor color_sphere;
  ofColor color_teapot;

  ofColor color_background;

  // paramètres du modèle d'illumination (Blinn-Phong)
  float ambient;
  float brightness;
  float specular;

  // lumières ponctuelles
  glm::vec3 light_position_world[light_count];
  glm::vec3 light_position_view[light_count];
  glm::vec3 light_color[light_count];

  float light_orbit_radius;
  float light_orbit_angle;
  float light_orbit_speed;
  float light_distance_z;

  bool use_light_motion;

  // rotation des objets
  float rotation_angle;
  float rotation_speed;

  // temps des oscillations
  float oscillation_time;

  // oscillations du triangle
  float triangle_oscillation_period_x;
  float triangle_oscillation_period_y;
  float triangle_oscillation_amplitude;

  // inclinaison du triangle (en degrés)
  float triangle_tilt_x;
  float triangle_tilt_y;

  // oscillations du cube
  float cube_oscillation_period_x;
  float cube_oscillation_period_z;
  float cube_oscillation_amplitude_x;
  float cube_oscillation_amplitude_z;

  // déplacement du cube
  glm::vec3 cube_offset;

  // oscillations du teapot
  float teapot_oscillation_period_tilt;
  float teapot_oscillation_period_y;
  float teapot_oscillation_amplitude_tilt;
  float teapot_oscillation_amplitude_y;

  // inclinaison (en degrés) et déplacement vertical du teapot
  float teapot_tilt;
  float teapot_bob;

  // oscillations de la sphère
  float sphere_oscillation_period_x;
  float sphere_oscillation_period_y;
  float sphere_oscillation_amplitude;

  // facteurs d'échelle de la sphère
  float sphere_scale_x;
  float sphere_scale_y;
  float sphere_scale_z;

  bool use_rotation;

  // affichage des informations sur le g-buffer (touche i)
  bool show_info;

  // polices de caractères de l'interface
  ofTrueTypeFont font;
  ofTrueTypeFont font_info;

  // dimension des objets et du teapot
  float object_size;
  float teapot_scale;
  float teapot_offset_y;

  // centre de chaque case de la grille de 2x2 objets
  glm::vec3 position_triangle;
  glm::vec3 position_cube;
  glm::vec3 position_sphere;
  glm::vec3 position_teapot;

  // distance et plans d'occlusion de la caméra
  float camera_distance;
  float clip_near;
  float clip_far;

  RenderMode render_mode;

  string render_mode_name;

  void setup();
  void update();
  void draw();

  void allocate_gbuffer(int width, int height);
  void draw_scene(ofShader& shader);
  void draw_gbuffer_layer(int layer, float x, float y, float width, float height);
  void set_light_uniforms(ofShader& shader);
  void draw_interface();
  void draw_label(const std::string& text, float x, float y);
  void draw_gbuffer_info(float x, float y);

  float oscillate(float time, float amplitude, float period);
};
