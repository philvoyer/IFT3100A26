// IFT3100A26_SSAO/renderer.h
// Classe responsable du rendu de l'application.

#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

// nombre maximum d'échantillons du noyau (le même que dans le shader ssao_330_fs.glsl)
// (au-delà de 64 échantillons, la qualité s'améliore à peine alors que le coût de la passe SSAO double :
// 128 échantillons servent à montrer ce compromis, avec un noyau qui reste sous la limite de composantes uniformes garantie par OpenGL 3.3)
const int sample_count_max = 128;

// dimension de la texture de bruit (4x4 vecteurs aléatoires, la même que le bloc du flou dans blur_330_fs.glsl)
const int noise_size = 4;

// modes de rendu
const int render_mode_lighting_ssao = 0;    // éclairage avec occlusion ambiante
const int render_mode_lighting = 1;         // éclairage sans occlusion ambiante
const int render_mode_ambient_ssao = 2;     // réflexion ambiante seulement, avec occlusion ambiante
const int render_mode_ssao_raw = 3;         // facteur d'occlusion ambiante, avant le flou
const int render_mode_ssao_blur = 4;        // facteur d'occlusion ambiante, après le flou
const int render_mode_layers = 5;           // couches du SSAO : position, normale, facteur avant et après le flou
const int render_mode_count = 6;

// nom de chaque mode de rendu (affiché à l'écran)
const char* const render_mode_name[render_mode_count] =
{
  "lighting with ssao",
  "lighting without ssao",
  "ambient with ssao",
  "ssao (raw)",
  "ssao (blur)",
  "ssao layers: position, normal, raw, blur"
};

class Renderer
{
public:

  // shaders des quatre passes de rendu
  ofShader shader_gbuffer;
  ofShader shader_ssao;
  ofShader shader_blur;
  ofShader shader_lighting;

  // shader qui affiche une couche du g-buffer
  ofShader shader_gbuffer_view;

  // g-buffer : un framebuffer avec deux buffers de rendu couleur (position et normale dans l'espace de vue)
  ofFbo gbuffer;

  // framebuffers du facteur d'occlusion ambiante, avant et après le flou
  ofFbo fbo_ssao;
  ofFbo fbo_blur;

  // texture de 4x4 vecteurs aléatoires dans le plan xy, répétée sur toute la fenêtre
  ofTexture texture_noise;
  float noise_data[noise_size * noise_size * 3];

  // noyau d'échantillons (positions aléatoires dans un hémisphère ou une sphère de rayon 1)
  glm::vec3 kernel[sample_count_max];
  int sample_count;
  bool use_hemisphere;

  // paramètres du SSAO : le rayon de la zone d'échantillonnage est relatif à la dimension des objets (radius_factor)
  float radius_factor;
  float radius;
  float bias;

  // matrice de projection de la passe géométrie (pour projeter les échantillons dans le g-buffer)
  glm::mat4 matrix_projection;

  // objets de la scène
  ofxAssimpModelLoader teapot;

  // couleurs de l'arrière-plan et du matériau de la scène (argile)
  ofColor color_background;
  ofColor color_albedo;

  // paramètres du modèle d'illumination
  float ambient;
  float diffuse;

  // lumière ponctuelle qui tourne au-dessus de la scène
  glm::vec3 light_position_scene;
  glm::vec3 light_position_view;
  float light_orbit_angle;
  float light_orbit_speed;
  bool use_light_motion;

  // oscillation de la scène autour de l'axe vertical, et inclinaison pour voir le plancher de haut
  float rotation_time;
  float rotation_amplitude;
  float rotation_period;
  float rotation_angle;
  float tilt_angle;
  bool use_rotation;

  // dimension de référence des objets, relative à la fenêtre d'affichage
  float unit;
  float teapot_scale;

  int render_mode;

  int framebuffer_width;
  int framebuffer_height;

  void setup();
  void update();
  void draw();

  void allocate_framebuffers(int width, int height);
  void generate_kernel();
  void generate_noise();
  void transform_scene();
  void draw_scene();
  void draw_gbuffer_layer(int layer, float x, float y, float width, float height);
};
