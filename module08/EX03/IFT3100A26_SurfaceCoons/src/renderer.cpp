// IFT3100A26_SurfaceCoons/renderer.cpp
// Classe responsable du rendu de l'application.

#include "renderer.h"

// une surface de Coons est construite à partir de 4 courbes paramétriques qui délimitent ses bords :
//
//   C1(u) et C2(u) sont les bords opposés en v = 0 et en v = 1,
//   C3(v) et C4(v) sont les bords opposés en u = 0 et en u = 1,
//
// et qui se rencontrent aux quatre coins de la surface :
//
//   C1(0) = C3(0), C1(1) = C4(0), C2(0) = C3(1), C2(1) = C4(1)
//
// l'équation de la surface est la somme de deux interpolations linéaires entre des courbes opposées,
// moins l'interpolation bilinéaire des quatre coins, qui sont autrement comptés deux fois :
//
//   lerp u(u, v) = (1 - v) C1(u) + v C2(u)
//   lerp v(u, v) = (1 - u) C3(v) + u C4(v)
//   blerp(u, v)  = (1 - u)(1 - v) C1(0) + u (1 - v) C1(1) + (1 - u) v C2(0) + u v C2(1)
//
//   surface coons(u, v) = lerp u(u, v) + lerp v(u, v) - blerp(u, v)
//
// dans cet exemple, chaque courbe de bord est une courbe de Bézier cubique.

// indices des 4 points de contrôle de chacune des courbes de bord dans le tableau des points de contrôle,
// où les indices 0 à 3 sont les coins partagés par deux courbes et les indices 4 à 11 sont les points intérieurs
const int boundary_index[boundary_count][4] = {
  {0, 4,  5,  1},  // C1 : de C1(0) = C3(0) à C1(1) = C4(0)
  {2, 6,  7,  3},  // C2 : de C2(0) = C3(1) à C2(1) = C4(1)
  {0, 8,  9,  2},  // C3 : de C3(0) = C1(0) à C3(1) = C2(0)
  {1, 10, 11, 3}}; // C4 : de C4(0) = C1(1) à C4(1) = C2(1)

// fonction d'évaluation d'une courbe de Bézier cubique avec un maximum de valeurs précalculées
// (les puissances de t et de (1 - t) ainsi que les quatre polynômes de Bernstein ne dépendent pas de la dimension,
// ils sont donc calculés une seule fois puis réutilisés pour x, y et z)
void bezier_cubic_precomputed(
  float t,
  float p1x, float p1y, float p1z,
  float p2x, float p2y, float p2z,
  float p3x, float p3y, float p3z,
  float p4x, float p4y, float p4z,
  float&  x,  float& y, float&  z)
{
  // puissances du paramètre t et de son complément u = 1 - t
  float u = 1 - t;
  float uu = u * u;
  float uuu = uu * u;
  float tt = t * t;
  float ttt = tt * t;

  // polynômes de Bernstein de degré 3, communs aux trois dimensions
  float b1 = uuu;
  float b2 = 3 * uu * t;
  float b3 = 3 * u * tt;
  float b4 = ttt;

  x = b1 * p1x + b2 * p2x + b3 * p3x + b4 * p4x;
  y = b1 * p1y + b2 * p2y + b3 * p3y + b4 * p4y;
  z = b1 * p1z + b2 * p2z + b3 * p3z + b4 * p4z;
}

// fonction qui calcule une interpolation linéaire entre deux valeurs numériques
float interpolation_linear(float value1, float value2, float t)
{
  if (t < 0.0f)
    return value1;

  if (t > 1.0f)
    return value2;

  return (1.0f - t) * value1 + t * value2;
}

// fonction qui calcule une interpolation entre deux valeurs numériques avec la fonction 'smoothstep'
float interpolation_smoothstep(float value1, float value2, float t)
{
  if (t < 0.0f)
    t = 0.0f;

  if (t > 1.0f)
    t = 1.0f;

  t = t * t * (3.0f - 2.0f * t);

  return (1.0 - t) * value1 + t * value2;
}

void Renderer::setup()
{
  ofSetFrameRate(60);
  ofSetBackgroundColor(0);

  // paramètres
  line_resolution = 50;
  line_width_outline = 2.0f;
  line_width_isoline = 2.0f;
  line_width_curve = 8.0f;

  radius = 24.0f;
  scale = 10.0f;
  offset = 64.0f;

  motion_speed = 250.0f;

  // rotation lente de la caméra autour de l'axe y (en degrés par seconde)
  camera_angle = 0.0f;
  camera_rotation_speed = 10.0f;

  // deux oscillateurs désynchronisés, un pour chaque paramètre de la surface :
  // leurs périodes différentes font en sorte que le repère de la tête de lecture couvre toute la surface au fil du temps
  oscillation_u = 0.5f;
  oscillation_v = 0.5f;
  oscillation_period_u = 7.5f;
  oscillation_period_v = 10.5f;
  oscillation_amplitude = 0.5f;

  // temps d'animation, qui avance seulement quand l'animation est active
  animation_time = 0.0f;

  isAnimationActive = true;

  // initialisation des sommets des lignes
  for (index = 0; index <= line_resolution; ++index)
  {
    for (int curve = 0; curve < boundary_count; ++curve)
      boundary_renderer[curve].addVertex(glm::vec3(0.0f));

    for (int i = 0; i <= isoline_count; ++i)
    {
      isoline_u_renderer[i].addVertex(glm::vec3(0.0f));
      isoline_v_renderer[i].addVertex(glm::vec3(0.0f));
    }

    playhead_u_renderer.addVertex(glm::vec3(0.0f));
    playhead_v_renderer.addVertex(glm::vec3(0.0f));
  }

  // nom de la surface
  surface_name = "de Coons";

  // couleur de chacune des courbes de bord
  curve_color[0] = ofColor(255, 0, 0);   // C1 : rouge
  curve_color[1] = ofColor(0, 255, 0);   // C2 : vert
  curve_color[2] = ofColor(0, 0, 255);   // C3 : bleu
  curve_color[3] = ofColor(128, 128, 0); // C4 : olive

  // couleur de chaque point de contrôle, qui est la somme des couleurs des courbes auxquelles il appartient
  // (un coin appartient à deux courbes, par exemple C1 rouge + C3 bleu = violet)
  for (index = 0; index < ctrl_point_count; ++index)
  {
    int r = 0;
    int g = 0;
    int b = 0;

    for (int curve = 0; curve < boundary_count; ++curve)
    {
      for (int k = 0; k < 4; ++k)
      {
        if (boundary_index[curve][k] == index)
        {
          r += curve_color[curve].r;
          g += curve_color[curve].g;
          b += curve_color[curve].b;
        }
      }
    }

    ctrl_point_color[index] = ofColor(std::min(r, 255), std::min(g, 255), std::min(b, 255));
  }

  // type d'interpolation au lancement de l'application
  interpolation_type = InterpolationType::lerp;

  // initialisation de la scène
  reset();
}

void Renderer::reset()
{
  // initialisation des variables
  framebuffer_width = ofGetWidth();
  framebuffer_height = ofGetHeight();

  // ratios de positionnement dans la fenêtre
  float x0 = framebuffer_width / 4.0f;
  float x1 = framebuffer_width * 3.0f / 4.0f;
  float y0 = framebuffer_height / 5.0f;
  float y1 = framebuffer_height * 4.0f / 5.0f;

  // espacement entre les points de contrôle le long d'un bord
  float dx = (x1 - x0) / 3.0f;
  float dy = (y1 - y0) / 3.0f;

  // décalage des points intérieurs pour que les courbes de bord soient ondulées
  float bump_x = framebuffer_width / 16.0f;
  float bump_y = framebuffer_height / 10.0f;

  // profondeur des points intérieurs, pour que la surface ait une forme en 3D (une selle) :
  // les bords C1 et C2 sont bombés vers l'avant et les bords C3 et C4 vers l'arrière
  float depth = framebuffer_height / 6.0f;

  // recul de la caméra, pour que la surface reste dans la fenêtre pendant la rotation
  camera_distance = framebuffer_height * 0.4f;

  // coins de la surface
  initial_position[0] = {x0, y0, 0}; // C1(0) = C3(0)
  initial_position[1] = {x1, y0, 0}; // C1(1) = C4(0)
  initial_position[2] = {x0, y1, 0}; // C2(0) = C3(1)
  initial_position[3] = {x1, y1, 0}; // C2(1) = C4(1)

  // points intérieurs de la courbe C1 (bord du haut)
  initial_position[4]  = {x0 + dx,     y0 - bump_y,  depth};
  initial_position[5]  = {x0 + dx * 2, y0 + bump_y,  depth};

  // points intérieurs de la courbe C2 (bord du bas)
  initial_position[6]  = {x0 + dx,     y1 + bump_y,  depth};
  initial_position[7]  = {x0 + dx * 2, y1 - bump_y,  depth};

  // points intérieurs de la courbe C3 (bord de gauche)
  initial_position[8]  = {x0 - bump_x, y0 + dy,     -depth};
  initial_position[9]  = {x0 + bump_x, y0 + dy * 2, -depth};

  // points intérieurs de la courbe C4 (bord de droite)
  initial_position[10] = {x1 + bump_x, y0 + dy,     -depth};
  initial_position[11] = {x1 - bump_x, y0 + dy * 2, -depth};

  for (index = 0; index < ctrl_point_count; ++index)
    ctrl_point[index] = initial_position[index];

  // sélection du premier point intérieur de la courbe C1
  selected_index = 4;
  selected_ctrl_point = &ctrl_point[selected_index];

  delta_x = motion_speed;
  delta_y = motion_speed;
  delta_z = motion_speed;

  ofLog() << "<reset>";
}

// fonction qui sélectionne le prochain point de contrôle
void Renderer::select_next_ctrl_point()
{
  selected_index = (selected_index + 1) % ctrl_point_count;
  selected_ctrl_point = &ctrl_point[selected_index];

  ofLog() << "<select control point " << selected_index + 1 << ">";
}

// fonction qui évalue la position d'une des courbes de bord (une courbe de Bézier cubique) au paramètre t
glm::vec3 Renderer::evaluate_boundary(int curve, float t)
{
  const glm::vec3& p1 = ctrl_point[boundary_index[curve][0]];
  const glm::vec3& p2 = ctrl_point[boundary_index[curve][1]];
  const glm::vec3& p3 = ctrl_point[boundary_index[curve][2]];
  const glm::vec3& p4 = ctrl_point[boundary_index[curve][3]];

  glm::vec3 p;

  bezier_cubic_precomputed(
    t,
    p1.x, p1.y, p1.z,
    p2.x, p2.y, p2.z,
    p3.x, p3.y, p3.z,
    p4.x, p4.y, p4.z,
    p.x, p.y, p.z);

  return p;
}

// fonction qui évalue la position sur la surface de Coons aux paramètres (u, v)
glm::vec3 Renderer::evaluate_surface(float u, float v)
{
  // positions sur les quatre courbes de bord
  glm::vec3 c1 = evaluate_boundary(0, u);
  glm::vec3 c2 = evaluate_boundary(1, u);
  glm::vec3 c3 = evaluate_boundary(2, v);
  glm::vec3 c4 = evaluate_boundary(3, v);

  // positions des quatre coins de la surface
  const glm::vec3& c1_0 = ctrl_point[0]; // C1(0) = C3(0)
  const glm::vec3& c1_1 = ctrl_point[1]; // C1(1) = C4(0)
  const glm::vec3& c2_0 = ctrl_point[2]; // C2(0) = C3(1)
  const glm::vec3& c2_1 = ctrl_point[3]; // C2(1) = C4(1)

  // interpolation linéaire entre les courbes opposées C1 et C2
  glm::vec3 lerp_u = c1 * (1.0f - v) + c2 * v;

  // interpolation linéaire entre les courbes opposées C3 et C4
  glm::vec3 lerp_v = c3 * (1.0f - u) + c4 * u;

  // interpolation bilinéaire entre les quatre coins
  glm::vec3 blerp =
    c1_0 * ((1.0f - u) * (1.0f - v)) +
    c1_1 * (u * (1.0f - v)) +
    c2_0 * ((1.0f - u) * v) +
    c2_1 * (u * v);

  // somme des deux interpolations linéaires, moins les coins qui sont comptés deux fois
  return lerp_u + lerp_v - blerp;
}

void Renderer::update()
{
  // rotation lente de la caméra autour de l'axe y
  if (isAnimationActive)
    camera_angle = fmod(camera_angle + camera_rotation_speed * ofGetLastFrameTime(), 360.0f);

  float t;

  // approximation des courbes de bord avec une séquence de lignes
  for (index = 0; index <= line_resolution; ++index)
  {
    t = index / (float) line_resolution;

    for (int curve = 0; curve < boundary_count; ++curve)
      boundary_renderer[curve][index] = evaluate_boundary(curve, t);
  }

  // approximation de la surface avec deux familles d'isolignes
  for (int i = 0; i <= isoline_count; ++i)
  {
    float s = i / (float) isoline_count;

    for (index = 0; index <= line_resolution; ++index)
    {
      t = index / (float) line_resolution;

      // isoligne à v constant, le long de laquelle u varie
      isoline_u_renderer[i][index] = evaluate_surface(t, s);

      // isoligne à u constant, le long de laquelle v varie
      isoline_v_renderer[i][index] = evaluate_surface(s, t);
    }
  }

  // mise à jour des deux oscillateurs, décalés de leur amplitude pour que leur valeur reste entre 0 et 1
  if (isAnimationActive)
  {
    animation_time += ofGetLastFrameTime();

    oscillation_u = oscillate(animation_time, oscillation_amplitude, oscillation_period_u) + oscillation_amplitude;
    oscillation_v = oscillate(animation_time, oscillation_amplitude, oscillation_period_v) + oscillation_amplitude;
  }

  // animation de la tête de lecture sur les deux axes en fonction de la valeur courante des oscillateurs
  switch (interpolation_type)
  {
    case InterpolationType::lerp:
      playhead_u = interpolation_linear(0.0f, 1.0f, oscillation_u);
      playhead_v = interpolation_linear(0.0f, 1.0f, oscillation_v);
      break;

    case InterpolationType::smoothstep:
      playhead_u = interpolation_smoothstep(0.0f, 1.0f, oscillation_u);
      playhead_v = interpolation_smoothstep(0.0f, 1.0f, oscillation_v);
      break;

    default:
      break;
  }

  // calculer les deux isolignes de la surface sur les deux axes à la fois,
  // à la valeur courante de la tête de lecture en v (u varie) et en u (v varie)
  for (index = 0; index <= line_resolution; ++index)
  {
    t = index / (float) line_resolution;

    playhead_u_renderer[index] = evaluate_surface(t, playhead_v);
    playhead_v_renderer[index] = evaluate_surface(playhead_u, t);
  }

  // calculer la position à l'intersection des deux isolignes de la tête de lecture
  playhead_position = evaluate_surface(playhead_u, playhead_v);
}

void Renderer::draw()
{
  ofPushMatrix();

  // recul et rotation de la caméra autour d'un axe y qui passe par le centre de la fenêtre,
  // appliqués sous forme de la transformation inverse de la scène, ce qui donne le même point de vue
  ofTranslate(framebuffer_width / 2.0f, framebuffer_height / 2.0f, -camera_distance);
  ofRotateYDeg(-camera_angle);
  ofTranslate(-framebuffer_width / 2.0f, -framebuffer_height / 2.0f, 0.0f);

  // dessiner les positions initiales
  ofSetColor(63, 63, 63);
  for (index = 0; index < ctrl_point_count; ++index)
    ofDrawSphere(initial_position[index], radius / 4.0f);

  // dessiner les isolignes de la surface
  ofSetColor(127, 127, 127);
  ofSetLineWidth(line_width_isoline);
  for (int i = 0; i <= isoline_count; ++i)
  {
    isoline_u_renderer[i].draw();
    isoline_v_renderer[i].draw();
  }

  // dessiner la ligne de contour de chaque courbe de bord
  ofSetColor(0, 0, 255);
  ofSetLineWidth(line_width_outline);
  for (int curve = 0; curve < boundary_count; ++curve)
  {
    for (int k = 0; k < 3; ++k)
    {
      const glm::vec3& p1 = ctrl_point[boundary_index[curve][k]];
      const glm::vec3& p2 = ctrl_point[boundary_index[curve][k + 1]];
      ofDrawLine(p1, p2);
    }
  }

  // dessiner les courbes de bord
  ofSetColor(0, 255, 0);
  ofSetLineWidth(line_width_curve);
  for (int curve = 0; curve < boundary_count; ++curve)
    boundary_renderer[curve].draw();

  // dessiner les isolignes animées par la tête de lecture
  ofSetColor(255);
  ofSetLineWidth(line_width_curve / 2.0f);
  playhead_u_renderer.draw();
  playhead_v_renderer.draw();

  // dessiner les points de contrôle avec la couleur de leurs courbes, le point sélectionné entouré de blanc
  for (index = 0; index < ctrl_point_count; ++index)
  {
    if (index == selected_index)
    {
      ofSetColor(255);
      ofDrawSphere(ctrl_point[index], radius * 0.75f);
    }

    ofSetColor(ctrl_point_color[index]);
    ofDrawSphere(ctrl_point[index], radius / 2.0f);
  }

  // dessiner un repère à la position courante de l'animation sur la surface
  ofSetColor(255);
  ofDrawSphere(playhead_position, radius / 2.0f);

  // dessiner l'étiquette de chaque point de contrôle
  ofSetColor(255);
  for (index = 0; index < ctrl_point_count; ++index)
    ofDrawBitmapString("P" + ofToString(index + 1), ctrl_point[index].x + radius, ctrl_point[index].y + radius, ctrl_point[index].z);

  // dessiner le nom des courbes de bord au milieu de chacune d'elles, avec la couleur de la courbe
  for (int curve = 0; curve < boundary_count; ++curve)
  {
    position = evaluate_boundary(curve, 0.5f);
    ofSetColor(curve_color[curve]);
    ofDrawBitmapString("C" + ofToString(curve + 1), position.x + radius, position.y - radius, position.z);
  }

  ofPopMatrix();
}

// fonction d'oscillation x(t) = amplitude * sin(2 * PI * t / période)
float Renderer::oscillate(float time, float amplitude, float period)
{
  return amplitude * sin(2.0 * PI * time / period);
}
