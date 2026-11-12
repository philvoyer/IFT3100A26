// IFT3100A26_CurveHermite/renderer.cpp
// Classe responsable du rendu de l'application.

#include "renderer.h"

// deux implémentations équivalentes de l'évaluation d'une courbe de Hermite cubique :
//
//   hermite(t) = (2t³ - 3t² + 1) P1 + (t³ - 2t² + t) V1 + (t³ - t²) V2 + (-2t³ + 3t²) P2
//
// les paramètres p1 et p4 sont les points de départ et d'arrivée (P1 et P2),
// les paramètres p2 et p3 sont les tangentes au départ et à l'arrivée (V1 et V2).
//
// les deux fonctions retournent la même position, mais pas avec le même coût :
// - hermite_direct : 63 multiplications par appel (21 par dimension);
// - hermite_precomputed : 17 multiplications par appel (5 pour les puissances de t et les fonctions de base, 12 pour x, y et z).
// l'exemple utilise hermite_precomputed, car la fonction est appelée plus de 100 fois par frame,
// soit environ 6000 fois par seconde à 60 fps.
// le nombre d'opérations mathématiques (multiplications, additions et soustractions) est donc d'environ :
// - hermite_direct : 90 par appel, soit environ 540 000 par seconde;
// - hermite_precomputed : 32 par appel, soit environ 192 000 par seconde.

// fonction d'évaluation d'une courbe de Hermite sans aucune valeur précalculée
// (traduction directe de l'équation : les puissances de t sont recalculées dans chaque terme et pour chaque dimension)
void hermite_direct(
  float t,
  float p1x, float p1y, float p1z,
  float p2x, float p2y, float p2z,
  float p3x, float p3y, float p3z,
  float p4x, float p4y, float p4z,
  float&  x,  float& y, float&  z)
{
  x = (2 * t * t * t - 3 * t * t + 1) * p1x + (t * t * t - 2 * t * t + t) * p2x + (t * t * t - t * t) * p3x + (-2 * t * t * t + 3 * t * t) * p4x;
  y = (2 * t * t * t - 3 * t * t + 1) * p1y + (t * t * t - 2 * t * t + t) * p2y + (t * t * t - t * t) * p3y + (-2 * t * t * t + 3 * t * t) * p4y;
  z = (2 * t * t * t - 3 * t * t + 1) * p1z + (t * t * t - 2 * t * t + t) * p2z + (t * t * t - t * t) * p3z + (-2 * t * t * t + 3 * t * t) * p4z;
}

// fonction d'évaluation d'une courbe de Hermite avec un maximum de valeurs précalculées
// (les puissances de t et les quatre fonctions de base ne dépendent pas de la dimension,
// elles sont donc calculées une seule fois puis réutilisées pour x, y et z)
void hermite_precomputed(
  float t,
  float p1x, float p1y, float p1z,
  float p2x, float p2y, float p2z,
  float p3x, float p3y, float p3z,
  float p4x, float p4y, float p4z,
  float&  x,  float& y, float&  z)
{
  // puissances du paramètre t
  float tt = t * t;
  float ttt = tt * t;

  // fonctions de base de Hermite, communes aux trois dimensions
  float h1 = 2 * ttt - 3 * tt + 1;
  float h2 = ttt - 2 * tt + t;
  float h3 = ttt - tt;

  // les fonctions de base des deux points ont une somme de 1 (h1 + h4 = 1), donc h4 ne coûte aucune multiplication
  float h4 = 1 - h1;

  x = h1 * p1x + h2 * p2x + h3 * p3x + h4 * p4x;
  y = h1 * p1y + h2 * p2y + h3 * p3y + h4 * p4y;
  z = h1 * p1z + h2 * p2z + h3 * p3z + h4 * p4z;
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
  curve_type = CurveType::hermite;

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
    case CurveType::hermite:

      curve_name = "Hermite cubique";

      ctrl_point1 = initial_position1;
      ctrl_point2 = initial_position2;
      ctrl_point3 = initial_position4;
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
  // approximation de la courbe avec une séquence de lignes
  for (index = 0; index <= line_resolution; ++index)
  {
    // paramètres selon le type de courbe
    switch (curve_type)
    {
      case CurveType::hermite:

        tangent1 = ctrl_point2 - ctrl_point1;
        tangent2 = ctrl_point3 - ctrl_point4;

        hermite_precomputed(
          index / (float) line_resolution,
          ctrl_point1.x, ctrl_point1.y, ctrl_point1.z,
          tangent1.x, tangent1.y, tangent1.z,
          tangent2.x, tangent2.y, tangent2.z,
          ctrl_point4.x, ctrl_point4.y, ctrl_point4.z,
          position.x, position.y, position.z);

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
    case CurveType::hermite:

      hermite_precomputed(
        playhead,
        ctrl_point1.x, ctrl_point1.y, ctrl_point1.z,
        tangent1.x, tangent1.y, tangent1.z,
        tangent2.x, tangent2.y, tangent2.z,
        ctrl_point4.x, ctrl_point4.y, ctrl_point4.z,
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
  ofDrawEllipse(initial_position1.x, initial_position1.y, radius / 2.0f, radius / 2.0f);
  ofDrawEllipse(initial_position2.x, initial_position2.y, radius / 2.0f, radius / 2.0f);
  ofDrawEllipse(initial_position3.x, initial_position3.y, radius / 2.0f, radius / 2.0f);
  ofDrawEllipse(initial_position4.x, initial_position4.y, radius / 2.0f, radius / 2.0f);
  ofDrawEllipse(initial_position5.x, initial_position5.y, radius / 2.0f, radius / 2.0f);

  // dessiner la ligne de contour
  ofSetColor(0, 0, 255);
  ofSetLineWidth(line_width_outline);
  ofDrawLine(ctrl_point1.x, ctrl_point1.y, ctrl_point2.x, ctrl_point2.y);
  ofDrawLine(ctrl_point3.x, ctrl_point3.y, ctrl_point4.x, ctrl_point4.y);

  if (curve_type == CurveType::bezier_cubic)
  {
    ofDrawLine(ctrl_point2.x, ctrl_point2.y, ctrl_point3.x, ctrl_point3.y);
    ofDrawLine(ctrl_point4.x, ctrl_point4.y, ctrl_point1.x, ctrl_point1.y);
  }

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
