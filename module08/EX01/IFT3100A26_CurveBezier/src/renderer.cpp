// IFT3100A26_CurveBezier/renderer.cpp
// Classe responsable du rendu de l'application.

#include "renderer.h"

// voici deux implémentations équivalentes de l'évaluation d'une courbe de Bézier quadratique :
//
//   bézier quadratique(t) = (1 - t)² P1 + 2 (1 - t) t P2 + t² P3
//
// les deux fonctions retournent la même position, mais pas avec le même coût :
// - bezier_quadratic_direct : 21 multiplications par appel (7 par dimension);
// - bezier_quadratic_precomputed : 13 multiplications par appel (4 pour les fonctions de base, 9 pour x, y et z).
// l'exemple utilise bezier_quadratic_precomputed, car la fonction est appelée plus de 100 fois par frame,
// soit environ 6000 fois par seconde à 60 fps.
// le nombre d'opérations mathématiques (multiplications, additions et soustractions) est donc d'environ :
// - bezier_quadratic_direct : 36 par appel, soit environ 216 000 par seconde;
// - bezier_quadratic_precomputed : 20 par appel, soit environ 120 000 par seconde.

// fonction d'évaluation d'une courbe de Bézier quadratique sans aucune valeur précalculée
// (traduction directe de l'équation : (1 - t) et les produits sont recalculés dans chaque terme et pour chaque dimension)
void bezier_quadratic_direct(
  float t,
  float p1x, float p1y, float p1z,
  float p2x, float p2y, float p2z,
  float p3x, float p3y, float p3z,
  float&  x,  float& y, float&  z)
{
  x = (1 - t) * (1 - t) * p1x + 2 * (1 - t) * t * p2x + t * t * p3x;
  y = (1 - t) * (1 - t) * p1y + 2 * (1 - t) * t * p2y + t * t * p3y;
  z = (1 - t) * (1 - t) * p1z + 2 * (1 - t) * t * p2z + t * t * p3z;
}

// fonction d'évaluation d'une courbe de Bézier quadratique avec un maximum de valeurs précalculées
// (les trois polynômes de Bernstein ne dépendent pas de la dimension,
// ils sont donc calculés une seule fois puis réutilisés pour x, y et z)
void bezier_quadratic_precomputed(
  float t,
  float p1x, float p1y, float p1z,
  float p2x, float p2y, float p2z,
  float p3x, float p3y, float p3z,
  float&  x,  float& y, float&  z)
{
  // complément du paramètre t
  float u = 1 - t;

  // polynômes de Bernstein de degré 2, communs aux trois dimensions
  float b1 = u * u;
  float b2 = 2 * u * t;
  float b3 = t * t;

  x = b1 * p1x + b2 * p2x + b3 * p3x;
  y = b1 * p1y + b2 * p2y + b3 * p3y;
  z = b1 * p1z + b2 * p2z + b3 * p3z;
}

// voici deux implémentations équivalentes de l'évaluation d'une courbe de Bézier cubique :
//
//   bézier cubique(t) = (1 - t)³ P1 + 3 t (1 - t)² P2 + 3 t² (1 - t) P3 + t³ P4
//
// les deux fonctions retournent la même position, mais pas avec le même coût :
// - bezier_cubic_direct : 42 multiplications par appel (14 par dimension);
// - bezier_cubic_precomputed : 20 multiplications par appel (8 pour les puissances et les fonctions de base, 12 pour x, y et z).
// l'exemple utilise bezier_cubic_precomputed, car la fonction est appelée plus de 100 fois par frame,
// soit environ 6000 fois par seconde à 60 fps.
// le nombre d'opérations mathématiques (multiplications, additions et soustractions) est donc d'environ :
// - bezier_cubic_direct : 69 par appel, soit environ 414 000 par seconde;
// - bezier_cubic_precomputed : 30 par appel, soit environ 180 000 par seconde.

// fonction d'évaluation d'une courbe de Bézier cubique sans aucune valeur précalculée
// (traduction directe de l'équation : (1 - t) et les puissances sont recalculés dans chaque terme et pour chaque dimension)
void bezier_cubic_direct(
  float t,
  float p1x, float p1y, float p1z,
  float p2x, float p2y, float p2z,
  float p3x, float p3y, float p3z,
  float p4x, float p4y, float p4z,
  float&  x,  float& y, float&  z)
{
  x = (1 - t) * (1 - t) * (1 - t) * p1x + 3 * t * (1 - t) * (1 - t) * p2x + 3 * t * t * (1 - t) * p3x + t * t * t * p4x;
  y = (1 - t) * (1 - t) * (1 - t) * p1y + 3 * t * (1 - t) * (1 - t) * p2y + 3 * t * t * (1 - t) * p3y + t * t * t * p4y;
  z = (1 - t) * (1 - t) * (1 - t) * p1z + 3 * t * (1 - t) * (1 - t) * p2z + 3 * t * t * (1 - t) * p3z + t * t * t * p4z;
}

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
  line_resolution = 100;
  line_width_outline = 4.0f;
  line_width_curve = 8.0f;

  radius = 32.0f;
  scale = 10.0f;
  offset = 64.0f;

  motion_speed = 250.0f;

  oscillation = 0.5f;
  oscillation_period = 5.0f;
  oscillation_amplitude = 0.5f;

  isAnimationActive = true;

  // initialisation des sommets de la ligne
  for (index = 0; index <= line_resolution; ++index)
    line_renderer.addVertex(ofPoint());

  // courbe au lancement de l'application
  curve_type = CurveType::bezier_quadratic;

  // type d'interpolation au lancement de l'application
  interpolation_type = InterpolationType::lerp;

  // initialisation de la scène
  reset();
}

void Renderer::reset()
{
  // initialisation des variables
  framebuffer_width  = ofGetWidth();
  framebuffer_height = ofGetHeight();

  // ratios de positionnement dans la fenêtre
  float w_1_8 = framebuffer_width / 8.0f;
  float w_1_4 = framebuffer_width / 4.0f;
  float w_1_2 = framebuffer_width / 2.0f;
  float w_3_4 = framebuffer_width * 3.0f / 4.0f;
  float w_7_8 = framebuffer_width * 7.0f / 8.0f;
  float h_1_5 = framebuffer_height / 5.0f;
  float h_1_3 = framebuffer_height / 3.0f;
  float h_4_5 = framebuffer_height * 4.0f / 5.0f;

  initial_position1 = {w_1_8, h_4_5, 0};
  initial_position2 = {w_1_4, h_1_3, 0};
  initial_position3 = {w_1_2, h_1_5, 0};
  initial_position4 = {w_3_4, h_1_3, 0};
  initial_position5 = {w_7_8, h_4_5, 0};

  // paramètres selon le type de courbe
  switch (curve_type)
  {
    case CurveType::bezier_quadratic:

      curve_name = "Bézier quadratique";

      ctrl_point1 = initial_position1;
      ctrl_point2 = initial_position3;
      ctrl_point3 = initial_position5;
      ctrl_point4 = initial_position5;

      selected_ctrl_point = &ctrl_point2;

      break;

    case CurveType::bezier_cubic:

      curve_name = "Bézier cubique";

      ctrl_point1 = initial_position1;
      ctrl_point2 = initial_position2;
      ctrl_point3 = initial_position4;
      ctrl_point4 = initial_position5;

      selected_ctrl_point = &ctrl_point2;

      break;

    default:
      break;
  }

  delta_x = motion_speed;
  delta_y = motion_speed;

  ofLog() << "<reset>";
}

void Renderer::update()
{
  for (index = 0; index <= line_resolution; ++index)
  {
    // paramètres selon le type de courbe
    switch (curve_type)
    {
      case CurveType::bezier_quadratic:

        bezier_quadratic_precomputed(
          index / (float) line_resolution,
          ctrl_point1.x, ctrl_point1.y, ctrl_point1.z,
          ctrl_point2.x, ctrl_point2.y, ctrl_point2.z,
          ctrl_point3.x, ctrl_point3.y, ctrl_point3.z,
          position.x, position.y, position.z);

        // pour masquer le 4e point de contrôle
        ctrl_point4 = ctrl_point3;

        break;

      case CurveType::bezier_cubic:

        bezier_cubic_precomputed(
          index / (float) line_resolution,
          ctrl_point1.x, ctrl_point1.y, ctrl_point1.z,
          ctrl_point2.x, ctrl_point2.y, ctrl_point2.z,
          ctrl_point3.x, ctrl_point3.y, ctrl_point3.z,
          ctrl_point4.x, ctrl_point4.y, ctrl_point4.z,
          position.x, position.y, position.z);

        break;

      default:
        break;
    }
    // affecter la position du point sur la courbe
    line_renderer[index] = position;
  }

  // mise à jour de l'oscillateur, décalé de son amplitude pour que sa valeur reste entre 0 et 1
  if (isAnimationActive)
    oscillation = oscillate(ofGetElapsedTimef(), oscillation_amplitude, oscillation_period) + oscillation_amplitude;

  // animation de la tête de lecture en fonction de la valeur courante de l'oscillateur
  switch (interpolation_type)
  {
    case InterpolationType::lerp:
      playhead = interpolation_linear(0.0f, 1.0f, oscillation);
      break;

    case InterpolationType::smoothstep:
      playhead = interpolation_smoothstep(0.0f, 1.0f, oscillation);
      break;

    default:
      break;
  }

  // calculer la position courante de l'animation sur la courbe
  switch (curve_type)
  {
    case CurveType::bezier_quadratic:

      bezier_quadratic_precomputed(
        playhead,
        ctrl_point1.x, ctrl_point1.y, ctrl_point1.z,
        ctrl_point2.x, ctrl_point2.y, ctrl_point2.z,
        ctrl_point3.x, ctrl_point3.y, ctrl_point3.z,
        playhead_position.x, playhead_position.y, playhead_position.z);

      break;

    case CurveType::bezier_cubic:

      bezier_cubic_precomputed(
        playhead,
        ctrl_point1.x, ctrl_point1.y, ctrl_point1.z,
        ctrl_point2.x, ctrl_point2.y, ctrl_point2.z,
        ctrl_point3.x, ctrl_point3.y, ctrl_point3.z,
        ctrl_point4.x, ctrl_point4.y, ctrl_point4.z,
        playhead_position.x, playhead_position.y, playhead_position.z);

      break;

    default:
      break;
  }
}

void Renderer::draw()
{
  // dessiner les positions initiales
  ofSetColor(63, 63, 63);
  ofDrawEllipse(initial_position1.x, initial_position1.y, radius/2, radius/2);
  ofDrawEllipse(initial_position2.x, initial_position2.y, radius/2, radius/2);
  ofDrawEllipse(initial_position3.x, initial_position3.y, radius/2, radius/2);
  ofDrawEllipse(initial_position4.x, initial_position4.y, radius/2, radius/2);
  ofDrawEllipse(initial_position5.x, initial_position5.y, radius/2, radius/2);

  // dessiner la ligne de contour
  ofSetColor(0, 0, 255);
  ofSetLineWidth(line_width_outline);
  ofDrawLine(ctrl_point1.x, ctrl_point1.y, ctrl_point2.x, ctrl_point2.y);
  ofDrawLine(ctrl_point2.x, ctrl_point2.y, ctrl_point3.x, ctrl_point3.y);
  ofDrawLine(ctrl_point3.x, ctrl_point3.y, ctrl_point4.x, ctrl_point4.y);
  ofDrawLine(ctrl_point4.x, ctrl_point4.y, ctrl_point1.x, ctrl_point1.y);

  // dessiner la courbe
  ofSetColor(0, 255, 0);
  ofSetLineWidth(line_width_curve);
  line_renderer.draw();

  // dessiner les points de contrôle
  ofSetColor(255, 0, 0);
  ofDrawEllipse(ctrl_point1.x, ctrl_point1.y, radius, radius);
  ofDrawEllipse(ctrl_point2.x, ctrl_point2.y, radius, radius);
  ofDrawEllipse(ctrl_point3.x, ctrl_point3.y, radius, radius);
  ofDrawEllipse(ctrl_point4.x, ctrl_point4.y, radius, radius);

  // dessiner un repère à la position courante de l'animation sur la courbe
  ofSetColor(255);
  ofDrawEllipse(playhead_position.x, playhead_position.y, radius / 1.0f, radius / 1.0f);
}

// fonction d'oscillation x(t) = amplitude * sin(2 * PI * t / période)
float Renderer::oscillate(float time, float amplitude, float period)
{
  return amplitude * sin(2.0 * PI * time / period);
}
