// IFT3100A26_RayIntersection/renderer.cpp
// Classe responsable du rendu de l'application.

#include "renderer.h"

// un rayon est défini par une position P et une direction normalisée D : rayon(t) = P + t D, avec t >= 0

// fonction d'intersection entre un rayon et un plan Q . N = d : t = (d - P . N) / (D . N)
bool intersect_ray_plane(
  const glm::vec3& origin, const glm::vec3& direction,
  const glm::vec3& normal, float distance,
  float& t, float& dot_product)
{
  // produit scalaire entre la direction du rayon et la normale du plan
  dot_product = glm::dot(direction, normal);

  // le rayon est parallèle au plan ou va dans la même direction que sa normale
  if (dot_product >= 0.0f)
    return false;

  // moment de l'impact
  t = (distance - glm::dot(origin, normal)) / dot_product;

  // l'impact doit être devant l'origine du rayon
  return t >= 0.0f;
}

// fonction d'intersection entre un rayon et une boîte alignée sur les axes, définie par son coin minimum et ses dimensions (rx, ry, rz)
// (pour chaque axe, seule la face visible est testée : t = (fx - Sx) / Dx, puis 0 <= [P(t)]y <= ry et 0 <= [P(t)]z <= rz)
bool intersect_ray_box(
  const glm::vec3& origin, const glm::vec3& direction,
  const glm::vec3& corner, const glm::vec3& size,
  float& t, int& face)
{
  // position de départ du rayon dans le repère de la boîte (elle doit être à l'extérieur de la boîte)
  glm::vec3 start = origin - corner;

  bool is_hit = false;

  for (int axis = 0; axis < 3; ++axis)
  {
    // le rayon est parallèle aux deux faces de cet axe
    if (direction[axis] == 0.0f)
      continue;

    // face visible : celle en 0 si le rayon va dans la direction positive de l'axe, sinon celle en r
    float face_position = direction[axis] > 0.0f ? 0.0f : size[axis];

    // moment de l'impact avec le plan de la face
    float t_face = (face_position - start[axis]) / direction[axis];

    // l'impact doit être devant l'origine du rayon
    if (t_face < 0.0f)
      continue;

    // validation du point d'impact dans la portée de la face, selon les deux autres axes
    int axis1 = (axis + 1) % 3;
    int axis2 = (axis + 2) % 3;

    float position1 = start[axis1] + t_face * direction[axis1];
    float position2 = start[axis2] + t_face * direction[axis2];

    if (position1 >= 0.0f && position1 <= size[axis1] && position2 >= 0.0f && position2 <= size[axis2])
    {
      if (!is_hit || t_face < t)
      {
        t = t_face;
        face = axis * 2 + (direction[axis] > 0.0f ? 0 : 1);
        is_hit = true;
      }
    }
  }

  return is_hit;
}

// fonction d'intersection entre un rayon et une sphère de centre C et de rayon r
// (a t² + b t + c = 0, avec a = D . D, b = 2 (P . D), c = P . P - r² et P exprimé par rapport à C)
bool intersect_ray_sphere(
  const glm::vec3& origin, const glm::vec3& direction,
  const glm::vec3& center, float radius,
  float& t, float& discriminant)
{
  // position de départ du rayon dans le repère de la sphère
  glm::vec3 p = origin - center;

  // coefficients de l'équation quadratique
  float a = glm::dot(direction, direction);
  float b = 2.0f * glm::dot(p, direction);
  float c = glm::dot(p, p) - radius * radius;

  discriminant = b * b - 4.0f * a * c;

  // le rayon passe à côté de la sphère
  if (discriminant < 0.0f)
    return false;

  // les deux moments d'impact, avec t1 <= t2
  float root = sqrt(discriminant);
  float t1 = (-b - root) / (2.0f * a);
  float t2 = (-b + root) / (2.0f * a);

  if (t1 >= 0.0f)
  {
    t = t1;
    return true;
  }

  if (t2 >= 0.0f)
  {
    t = t2;
    return true;
  }

  // la sphère est derrière l'origine du rayon
  return false;
}

// fonction d'intersection entre un rayon et un triangle, en deux étapes :
// intersection avec le plan du triangle, puis coordonnées barycentriques (w0, w1, w2) du point d'impact
bool intersect_ray_triangle(
  const glm::vec3& origin, const glm::vec3& direction,
  const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2,
  float& t, float& w0, float& w1, float& w2)
{
  // étape 1 : intersection entre le rayon et le plan du triangle

  glm::vec3 q1 = p1 - p0;
  glm::vec3 q2 = p2 - p0;

  glm::vec3 normal = glm::normalize(glm::cross(q1, q2));

  float dot_product = glm::dot(direction, normal);

  // le rayon est parallèle au plan du triangle (le triangle peut être frappé des deux côtés)
  if (dot_product == 0.0f)
    return false;

  // moment de l'impact avec le plan Q . N = d, où d = P0 . N
  t = (glm::dot(p0, normal) - glm::dot(origin, normal)) / dot_product;

  // l'impact doit être devant l'origine du rayon
  if (t < 0.0f)
    return false;

  glm::vec3 position = origin + direction * t;

  // étape 2 : coordonnées barycentriques du point d'impact par rapport aux 3 sommets du triangle

  glm::vec3 r = position - p0;

  float q1q1 = glm::dot(q1, q1);
  float q2q2 = glm::dot(q2, q2);
  float q1q2 = glm::dot(q1, q2);
  float rq1 = glm::dot(r, q1);
  float rq2 = glm::dot(r, q2);

  float determinant = q1q1 * q2q2 - q1q2 * q1q2;

  w1 = (q2q2 * rq1 - q1q2 * rq2) / determinant;
  w2 = (q1q1 * rq2 - q1q2 * rq1) / determinant;
  w0 = 1.0f - w1 - w2;

  return w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f;
}

// fonction qui calcule la direction d'un rayon de réflexion : reflection(I, N) = I - 2 (N . I) N
glm::vec3 reflection(const glm::vec3& incident, const glm::vec3& normal)
{
  return incident - normal * (2.0f * glm::dot(normal, incident));
}

void Renderer::setup()
{
  ofSetFrameRate(60);
  ofSetBackgroundColor(0);

  // résolution des sphères en fil de fer
  ofSetSphereResolution(16);

  // paramètres
  line_width_ray = 4.0f;
  line_width_outline = 2.0f;

  radius = 16.0f;

  motion_speed = 250.0f;

  // rotation lente de la caméra autour de l'axe y (en degrés par seconde)
  camera_angle = 0.0f;
  camera_rotation_speed = 10.0f;

  // deux oscillateurs désynchronisés pour que le rayon balaie toute la scène
  oscillation_x = 0.0f;
  oscillation_y = 0.0f;
  oscillation_period_x = 7.5f;
  oscillation_period_y = 10.5f;
  oscillation_amplitude = 0.5f;

  // temps d'animation, qui avance seulement quand l'animation est active
  animation_time = 0.0f;

  // nombre d'intervalles de la grille qui représente le plan
  plane_grid_count = 10;

  // nombre d'échantillons lancés à chaque frame (1 : le rayon seul) et ouverture du cône des échantillons (environ 3 degrés)
  sample_count = 1;
  sample_spread = 0.05f;

  isAnimationActive = true;

  // nom de chaque surface
  surface_name[surface_plane] = "plane";
  surface_name[surface_box] = "box";
  surface_name[surface_sphere] = "sphere";
  surface_name[surface_triangle] = "triangle";

  // couleur de chaque surface
  surface_color[surface_plane] = ofColor(0, 127, 255);    // plan : bleu
  surface_color[surface_box] = ofColor(255, 127, 0);      // boîte : orange
  surface_color[surface_sphere] = ofColor(0, 255, 127);   // sphère : vert
  surface_color[surface_triangle] = ofColor(255, 0, 255); // triangle : magenta

  // initialisation de la scène
  reset();
}

void Renderer::reset()
{
  // initialisation des variables
  framebuffer_width = ofGetWidth();
  framebuffer_height = ofGetHeight();

  float w = framebuffer_width;
  float h = framebuffer_height;

  // recul de la caméra, pour que la scène reste dans la fenêtre pendant la rotation
  camera_distance = h * 0.4f;

  // longueur du rayon quand il ne frappe aucune surface, et longueur des vecteurs affichés au point d'impact
  ray_length = w * 1.5f;
  normal_length = h / 8.0f;

  // origine du rayon, en haut et vers l'avant de la scène
  ray_origin_initial = {w * 0.5f, h * 0.05f, h * 0.3f};
  ray_origin = ray_origin_initial;

  // plan horizontal au bas de la scène, dont la normale pointe vers le haut de la fenêtre (l'axe y pointe vers le bas)
  plane_center = {w * 0.5f, h * 0.8f, 0.0f};
  plane_normal = {0.0f, -1.0f, 0.0f};
  plane_distance = glm::dot(plane_center, plane_normal);
  plane_half_width = w * 0.42f;
  plane_half_depth = h * 0.35f;

  // boîte posée sur le plan, à gauche
  box_corner = {w * 0.1f, h * 0.55f, -h * 0.08f};
  box_size = {w * 0.14f, h * 0.25f, h * 0.16f};

  // sphère au centre, au-dessus du plan
  sphere_center = {w * 0.45f, h * 0.58f, 0.0f};
  sphere_radius = h * 0.13f;

  // triangle à droite, dont la normale (P1 - P0) x (P2 - P0) pointe vers l'avant de la scène
  triangle_p0 = {w * 0.64f, h * 0.8f, h * 0.02f};
  triangle_p1 = {w * 0.74f, h * 0.36f, 0.0f};
  triangle_p2 = {w * 0.88f, h * 0.8f, -h * 0.04f};

  delta_x = motion_speed;
  delta_y = motion_speed;
  delta_z = motion_speed;

  ofLog() << "<reset>";
}

void Renderer::update()
{
  // rotation lente de la caméra autour de l'axe y et mise à jour des deux oscillateurs
  if (isAnimationActive)
  {
    camera_angle = fmod(camera_angle + camera_rotation_speed * ofGetLastFrameTime(), 360.0f);

    animation_time += ofGetLastFrameTime();

    oscillation_x = oscillate(animation_time, oscillation_amplitude, oscillation_period_x);
    oscillation_y = oscillate(animation_time, oscillation_amplitude, oscillation_period_y);
  }

  // cible du rayon, qui balaie la scène de gauche à droite et de haut en bas
  ray_target = {
    framebuffer_width * (0.5f + oscillation_x * 0.8f),
    framebuffer_height * (0.7f + oscillation_y * 0.4f),
    -framebuffer_height * 0.1f};

  // direction normalisée du rayon, de son origine vers sa cible
  ray_direction = glm::normalize(ray_target - ray_origin);

  // valeurs par défaut des valeurs intermédiaires qui ne sont pas toujours calculées
  box_face = -1;
  triangle_w0 = 0.0f;
  triangle_w1 = 0.0f;
  triangle_w2 = 0.0f;

  // test d'intersection avec le plan
  is_hit[surface_plane] = intersect_ray_plane(
    ray_origin, ray_direction,
    plane_normal, plane_distance,
    hit_t[surface_plane], plane_dot);

  hit_normal[surface_plane] = plane_normal;

  // un plan est théoriquement infini, mais celui de la scène est borné par une largeur et une profondeur
  if (is_hit[surface_plane])
  {
    glm::vec3 position = ray_origin + ray_direction * hit_t[surface_plane];

    if (fabs(position.x - plane_center.x) > plane_half_width || fabs(position.z - plane_center.z) > plane_half_depth)
      is_hit[surface_plane] = false;
  }

  // test d'intersection avec la boîte
  is_hit[surface_box] = intersect_ray_box(
    ray_origin, ray_direction,
    box_corner, box_size,
    hit_t[surface_box], box_face);

  // normale de la face de la boîte frappée par le rayon (vers l'extérieur de la boîte)
  if (is_hit[surface_box])
  {
    hit_normal[surface_box] = {0.0f, 0.0f, 0.0f};
    hit_normal[surface_box][box_face / 2] = box_face % 2 == 0 ? -1.0f : 1.0f;
  }

  // test d'intersection avec la sphère
  is_hit[surface_sphere] = intersect_ray_sphere(
    ray_origin, ray_direction,
    sphere_center, sphere_radius,
    hit_t[surface_sphere], sphere_discriminant);

  // test d'intersection avec le triangle
  is_hit[surface_triangle] = intersect_ray_triangle(
    ray_origin, ray_direction,
    triangle_p0, triangle_p1, triangle_p2,
    hit_t[surface_triangle], triangle_w0, triangle_w1, triangle_w2);

  // normale du triangle, orientée vers l'origine du rayon puisque le triangle peut être frappé des deux côtés
  hit_normal[surface_triangle] = glm::normalize(glm::cross(triangle_p1 - triangle_p0, triangle_p2 - triangle_p0));

  if (glm::dot(ray_direction, hit_normal[surface_triangle]) > 0.0f)
    hit_normal[surface_triangle] = -hit_normal[surface_triangle];

  // position de chaque point d'impact, obtenue en ramenant la valeur de t dans l'équation du rayon
  for (index = 0; index < surface_count; ++index)
  {
    if (is_hit[index])
      hit_position[index] = ray_origin + ray_direction * hit_t[index];
  }

  // normale de la sphère au point d'impact
  if (is_hit[surface_sphere])
    hit_normal[surface_sphere] = (hit_position[surface_sphere] - sphere_center) / sphere_radius;

  // surface la plus rapprochée, celle avec la plus petite valeur de t
  nearest_index = -1;

  for (index = 0; index < surface_count; ++index)
  {
    if (is_hit[index] && (nearest_index == -1 || hit_t[index] < hit_t[nearest_index]))
      nearest_index = index;
  }

  // direction du rayon de réflexion au point d'impact le plus rapproché
  if (nearest_index != -1)
    reflection_direction = reflection(ray_direction, hit_normal[nearest_index]);

  // échantillons de Monte Carlo : le premier est le rayon lui-même, les autres ont une direction choisie au hasard
  // dans un petit cône autour de celle du rayon, comme les rayons qui couvrent la surface d'un pixel dans un raytracer
  if (sample_count < 1)
    sample_count = 1;
  if (sample_count > sample_count_max)
    sample_count = sample_count_max;

  for (index = 0; index < surface_count; ++index)
    sample_hit_count[index] = 0;

  sample_miss_count = 0;

  // repère orthonormé (u, v) perpendiculaire à la direction du rayon, pour décaler la direction des échantillons
  // (à partir de l'axe y si la direction a une composante en x, sinon à partir de l'axe x, pour éviter deux vecteurs presque parallèles)
  glm::vec3 axis = fabs(ray_direction.x) > 0.1f ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec3 u = glm::normalize(glm::cross(axis, ray_direction));
  glm::vec3 v = glm::cross(ray_direction, u);

  for (int sample = 0; sample < sample_count; ++sample)
  {
    if (sample == 0)
    {
      // le premier échantillon est le rayon lui-même
      sample_direction[sample] = ray_direction;
      sample_nearest_index[sample] = nearest_index;
      sample_t[sample] = nearest_index != -1 ? hit_t[nearest_index] : 0.0f;
    }
    else
    {
      // point aléatoire dans un disque de rayon sample_spread, perpendiculaire à la direction du rayon
      // (la racine carrée répartit les points uniformément sur le disque, sinon ils seraient plus denses près du centre)
      float angle = ofRandom(0.0f, TWO_PI);
      float distance = sample_spread * sqrt(ofRandom(0.0f, 1.0f));

      // direction de l'échantillon : direction du rayon décalée vers ce point aléatoire
      sample_direction[sample] = glm::normalize(ray_direction + u * (distance * cos(angle)) + v * (distance * sin(angle)));

      // surface la plus rapprochée frappée par l'échantillon
      sample_nearest_index[sample] = raycast(ray_origin, sample_direction[sample], sample_t[sample]);
    }

    if (sample_nearest_index[sample] != -1)
      ++sample_hit_count[sample_nearest_index[sample]];
    else
      ++sample_miss_count;
  }
}

// fonction qui retourne la surface la plus rapprochée frappée par un rayon (-1 si aucune), avec la distance de l'impact
int Renderer::raycast(const glm::vec3& origin, const glm::vec3& direction, float& t_nearest)
{
  int nearest = -1;

  // distance de l'impact avec une surface
  float t;

  // valeurs intermédiaires des tests d'intersection, qui ne servent pas ici
  float dot_product;
  int face;
  float discriminant;
  float w0, w1, w2;

  // plan, borné par une largeur et une profondeur
  if (intersect_ray_plane(origin, direction, plane_normal, plane_distance, t, dot_product))
  {
    glm::vec3 position = origin + direction * t;

    if (fabs(position.x - plane_center.x) <= plane_half_width && fabs(position.z - plane_center.z) <= plane_half_depth)
    {
      nearest = surface_plane;
      t_nearest = t;
    }
  }

  // boîte
  if (intersect_ray_box(origin, direction, box_corner, box_size, t, face) && (nearest == -1 || t < t_nearest))
  {
    nearest = surface_box;
    t_nearest = t;
  }

  // sphère
  if (intersect_ray_sphere(origin, direction, sphere_center, sphere_radius, t, discriminant) && (nearest == -1 || t < t_nearest))
  {
    nearest = surface_sphere;
    t_nearest = t;
  }

  // triangle
  if (intersect_ray_triangle(origin, direction, triangle_p0, triangle_p1, triangle_p2, t, w0, w1, w2) && (nearest == -1 || t < t_nearest))
  {
    nearest = surface_triangle;
    t_nearest = t;
  }

  return nearest;
}

void Renderer::draw()
{
  ofPushMatrix();

  // recul et rotation de la caméra autour du centre de la fenêtre (transformation inverse de la scène)
  ofTranslate(framebuffer_width / 2.0f, framebuffer_height / 2.0f, -camera_distance);
  ofRotateYDeg(-camera_angle);
  ofTranslate(-framebuffer_width / 2.0f, -framebuffer_height / 2.0f, 0.0f);

  ofSetLineWidth(line_width_outline);

  // dessiner le plan sous forme d'une grille
  ofSetColor(surface_color[surface_plane]);
  for (index = 0; index <= plane_grid_count; ++index)
  {
    float s = index / (float) plane_grid_count * 2.0f - 1.0f;

    ofDrawLine(
      plane_center.x + s * plane_half_width, plane_center.y, plane_center.z - plane_half_depth,
      plane_center.x + s * plane_half_width, plane_center.y, plane_center.z + plane_half_depth);

    ofDrawLine(
      plane_center.x - plane_half_width, plane_center.y, plane_center.z + s * plane_half_depth,
      plane_center.x + plane_half_width, plane_center.y, plane_center.z + s * plane_half_depth);
  }

  // dessiner la boîte, la sphère et le triangle en fil de fer
  ofNoFill();

  ofSetColor(surface_color[surface_box]);
  ofDrawBox(box_corner + box_size / 2.0f, box_size.x, box_size.y, box_size.z);

  ofSetColor(surface_color[surface_sphere]);
  ofDrawSphere(sphere_center, sphere_radius);

  ofSetColor(surface_color[surface_triangle]);
  ofDrawTriangle(triangle_p0, triangle_p1, triangle_p2);

  ofFill();

  // dessiner les échantillons additionnels en traits fins et transparents, avec un petit point de la couleur de la surface frappée
  ofSetLineWidth(1.0f);

  for (int sample = 1; sample < sample_count; ++sample)
  {
    float sample_end = sample_nearest_index[sample] != -1 ? sample_t[sample] : ray_length;

    glm::vec3 sample_end_position = ray_origin + sample_direction[sample] * sample_end;

    ofSetColor(255, 255, 255, 47);
    ofDrawLine(ray_origin, sample_end_position);

    if (sample_nearest_index[sample] != -1)
    {
      ofSetColor(surface_color[sample_nearest_index[sample]]);
      ofDrawSphere(sample_end_position, radius / 4.0f);
    }
  }

  // longueur du rayon jusqu'au point d'impact le plus rapproché, ou jusqu'à sa longueur maximale s'il ne frappe aucune surface
  float ray_end = nearest_index != -1 ? hit_t[nearest_index] : ray_length;

  // dessiner le rayon jusqu'au point d'impact le plus rapproché
  ofSetColor(255);
  ofSetLineWidth(line_width_ray);
  ofDrawLine(ray_origin, ray_origin + ray_direction * ray_end);

  // dessiner le prolongement du rayon après le point d'impact le plus rapproché, pour voir les autres impacts
  ofSetColor(95);
  ofSetLineWidth(line_width_outline);
  ofDrawLine(ray_origin + ray_direction * ray_end, ray_origin + ray_direction * ray_length);

  if (nearest_index != -1)
  {
    const glm::vec3& position = hit_position[nearest_index];

    // dessiner la normale de la surface au point d'impact le plus rapproché
    ofSetColor(0, 255, 255);
    ofSetLineWidth(line_width_ray);
    ofDrawLine(position, position + hit_normal[nearest_index] * normal_length);

    // dessiner le rayon de réflexion au point d'impact le plus rapproché
    ofSetColor(255, 255, 0);
    ofDrawLine(position, position + reflection_direction * normal_length * 2.0f);

    // dessiner un halo blanc autour du point d'impact le plus rapproché
    ofSetColor(255);
    ofDrawSphere(position, radius * 0.75f);

    ofSetColor(0, 255, 255);
    ofDrawBitmapString("N", position + hit_normal[nearest_index] * normal_length);

    ofSetColor(255, 255, 0);
    ofDrawBitmapString("R", position + reflection_direction * normal_length * 2.0f);
  }

  // dessiner le point d'impact avec chaque surface frappée par le rayon, avec la couleur de la surface
  for (index = 0; index < surface_count; ++index)
  {
    if (is_hit[index])
    {
      ofSetColor(surface_color[index]);
      ofDrawSphere(hit_position[index], radius / 2.0f);
    }
  }

  // dessiner l'origine du rayon
  ofSetColor(255);
  ofDrawSphere(ray_origin, radius / 2.0f);
  ofDrawBitmapString("P", ray_origin.x + radius, ray_origin.y - radius, ray_origin.z);

  if (nearest_index != -1)
  {
    const glm::vec3& position = hit_position[nearest_index];
    ofDrawBitmapString("I", position.x + radius, position.y + radius * 1.5f, position.z);
  }

  // dessiner le nom de chaque surface avec sa couleur
  ofSetColor(surface_color[surface_plane]);
  ofDrawBitmapString(surface_name[surface_plane], plane_center.x + plane_half_width, plane_center.y + radius * 1.5f, plane_center.z + plane_half_depth);

  ofSetColor(surface_color[surface_box]);
  ofDrawBitmapString(surface_name[surface_box], box_corner.x, box_corner.y - radius, box_corner.z);

  ofSetColor(surface_color[surface_sphere]);
  ofDrawBitmapString(surface_name[surface_sphere], sphere_center.x - radius * 2.0f, sphere_center.y - sphere_radius - radius, sphere_center.z);

  ofSetColor(surface_color[surface_triangle]);
  ofDrawBitmapString(surface_name[surface_triangle], triangle_p1.x - radius * 2.0f, triangle_p1.y - radius, triangle_p1.z);

  ofPopMatrix();

  // dessiner le résultat de chaque test d'intersection, avec les valeurs intermédiaires du calcul
  const string face_name[6] = {"x = 0", "x = rx", "y = 0", "y = ry", "z = 0", "z = rz"};

  float x = 16.0f;
  float y = 24.0f;
  float line_height = 16.0f;

  ofSetColor(255);
  ofDrawBitmapString(
    "ray      P = (" + ofToString(ray_origin.x, 0) + ", " + ofToString(ray_origin.y, 0) + ", " + ofToString(ray_origin.z, 0) + ")"
    "  D = (" + ofToString(ray_direction.x, 2) + ", " + ofToString(ray_direction.y, 2) + ", " + ofToString(ray_direction.z, 2) + ")",
    x, y);

  y += line_height;
  ofSetColor(surface_color[surface_plane]);
  ofDrawBitmapString(
    "plane    D.N = " + ofToString(plane_dot, 2) +
    (is_hit[surface_plane] ? "  t = " + ofToString(hit_t[surface_plane], 1) : "  miss"),
    x, y);

  y += line_height;
  ofSetColor(surface_color[surface_box]);
  ofDrawBitmapString(
    "box      " +
    (is_hit[surface_box] ? "face " + face_name[box_face] + "  t = " + ofToString(hit_t[surface_box], 1) : string("miss")),
    x, y);

  y += line_height;
  ofSetColor(surface_color[surface_sphere]);
  ofDrawBitmapString(
    "sphere   d = " + ofToString(sphere_discriminant, 0) +
    (is_hit[surface_sphere] ? "  t = " + ofToString(hit_t[surface_sphere], 1) : "  miss"),
    x, y);

  y += line_height;
  ofSetColor(surface_color[surface_triangle]);
  ofDrawBitmapString(
    "triangle w = (" + ofToString(triangle_w0, 2) + ", " + ofToString(triangle_w1, 2) + ", " + ofToString(triangle_w2, 2) + ")" +
    (is_hit[surface_triangle] ? "  t = " + ofToString(hit_t[surface_triangle], 1) : "  miss"),
    x, y);

  y += line_height;
  ofSetColor(255);
  ofDrawBitmapString("nearest  " + (nearest_index != -1 ? surface_name[nearest_index] : string("none")), x, y);

  // proportion des échantillons qui frappent chaque surface en premier : c'est l'estimation de Monte Carlo
  // de la part de chaque surface dans le petit cône couvert par les échantillons (plus il y a d'échantillons, plus elle est stable)
  if (sample_count > 1)
  {
    string text = "samples  " + ofToString(sample_count);

    for (index = 0; index < surface_count; ++index)
      text += "  " + surface_name[index] + " " + ofToString(100.0f * sample_hit_count[index] / sample_count, 0) + "%";

    text += "  none " + ofToString(100.0f * sample_miss_count / sample_count, 0) + "%";

    y += line_height;
    ofDrawBitmapString(text, x, y);
  }
}

// fonction d'oscillation x(t) = amplitude * sin(2 * PI * t / période)
float Renderer::oscillate(float time, float amplitude, float period)
{
  return amplitude * sin(2.0 * PI * time / period);
}
