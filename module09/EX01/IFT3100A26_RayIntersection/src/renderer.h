// IFT3100A26_RayIntersection/renderer.h
// Classe responsable du rendu de l'application.

#pragma once

#include "ofMain.h"

// nombre de surfaces de la scène (plan, boîte, sphère et triangle)
const int surface_count = 4;

// index de chaque surface de la scène
const int surface_plane = 0;
const int surface_box = 1;
const int surface_sphere = 2;
const int surface_triangle = 3;

// nombre maximal d'échantillons (rayons) lancés à chaque frame
const int sample_count_max = 256;

class Renderer
{
public:

  // nom et couleur de chaque surface
  string surface_name[surface_count];
  ofColor surface_color[surface_count];

  // rayon défini par une position et une direction normalisée : rayon(t) = P + t D
  glm::vec3 ray_origin;
  glm::vec3 ray_direction;
  glm::vec3 ray_target;

  glm::vec3 ray_origin_initial;

  // plan défini par sa normale N et sa distance d à l'origine (Q . N = d), borné par une largeur et une profondeur
  glm::vec3 plane_normal;
  glm::vec3 plane_center;
  float plane_distance;
  float plane_half_width;
  float plane_half_depth;

  // boîte alignée sur les axes, définie par son coin minimum et ses dimensions
  glm::vec3 box_corner;
  glm::vec3 box_size;

  // sphère définie par son centre et son rayon
  glm::vec3 sphere_center;
  float sphere_radius;

  // triangle défini par ses 3 sommets
  glm::vec3 triangle_p0;
  glm::vec3 triangle_p1;
  glm::vec3 triangle_p2;

  // résultat du test d'intersection avec chaque surface
  bool is_hit[surface_count];
  float hit_t[surface_count];
  glm::vec3 hit_position[surface_count];
  glm::vec3 hit_normal[surface_count];

  // valeurs intermédiaires des tests d'intersection, affichées à l'écran
  float plane_dot;
  int box_face;
  float sphere_discriminant;
  float triangle_w0;
  float triangle_w1;
  float triangle_w2;

  // surface la plus rapprochée frappée par le rayon (-1 si aucune)
  int nearest_index;

  // direction du rayon de réflexion au point d'intersection le plus rapproché
  glm::vec3 reflection_direction;

  // nombre d'échantillons (rayons) lancés à chaque frame : avec 1, seul le rayon est lancé ; au-delà de 1, des rayons
  // légèrement différents sont ajoutés, dont la direction est choisie au hasard autour de celle du rayon (approche de Monte Carlo)
  int sample_count;

  // ouverture du cône dans lequel la direction des échantillons est choisie (tangente de son demi-angle)
  float sample_spread;

  // direction, distance de l'impact et surface la plus rapprochée frappée par chaque échantillon (-1 si aucune)
  glm::vec3 sample_direction[sample_count_max];
  float sample_t[sample_count_max];
  int sample_nearest_index[sample_count_max];

  // nombre d'échantillons qui frappent chaque surface en premier, et nombre d'échantillons qui ne frappent aucune surface
  int sample_hit_count[surface_count];
  int sample_miss_count;

  float line_width_ray;
  float line_width_outline;

  float radius;
  float ray_length;
  float normal_length;

  float delta_x;
  float delta_y;
  float delta_z;

  float motion_speed;

  float camera_angle;
  float camera_distance;
  float camera_rotation_speed;

  float oscillation_x;
  float oscillation_y;
  float oscillation_period_x;
  float oscillation_period_y;
  float oscillation_amplitude;

  float animation_time;

  int framebuffer_width;
  int framebuffer_height;

  int plane_grid_count;

  int index;

  bool isAnimationActive;

  void setup();
  void update();
  void draw();
  void reset();

  int raycast(const glm::vec3& origin, const glm::vec3& direction, float& t_nearest);

  float oscillate(float time, float amplitude, float period);
};
