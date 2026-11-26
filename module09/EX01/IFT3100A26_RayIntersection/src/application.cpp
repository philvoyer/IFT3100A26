// IFT3100A26_RayIntersection/application.cpp
// Classe principale de l'application.

#include "application.h"

void Application::setup()
{
  ofLog() << "<app::setup>";

  is_key_press_up = false;
  is_key_press_down = false;
  is_key_press_left = false;
  is_key_press_right = false;
  is_key_press_forward = false;
  is_key_press_backward = false;

  renderer.setup();

  // initialiser le temps du dernier frame pour le calcul du temps écoulé
  time_last = ofGetElapsedTimef();
}

void Application::update()
{
  ofSetWindowTitle("intersections rayon-surface (↑↓←→ wasd qe r +- cx spacebar) samples " + ofToString(renderer.sample_count) + " cone " + ofToString(renderer.sample_cone_angle, 1) + "°");

  time_current = ofGetElapsedTimef();
  time_elapsed = time_current - time_last;
  time_last = time_current;

  if (is_key_press_up)
    renderer.ray_origin.y -= renderer.delta_y * time_elapsed;
  if (is_key_press_down)
    renderer.ray_origin.y += renderer.delta_y * time_elapsed;
  if (is_key_press_left)
    renderer.ray_origin.x -= renderer.delta_x * time_elapsed;
  if (is_key_press_right)
    renderer.ray_origin.x += renderer.delta_x * time_elapsed;
  if (is_key_press_forward)
    renderer.ray_origin.z += renderer.delta_z * time_elapsed;
  if (is_key_press_backward)
    renderer.ray_origin.z -= renderer.delta_z * time_elapsed;

  renderer.update();
}

void Application::draw()
{
  renderer.draw();
}

void Application::mouseReleased(int x, int y, int button)
{
  renderer.reset();
}

void Application::keyPressed(int key)
{
  switch(key)
  {
    case OF_KEY_LEFT: // touche ←
    case 97:          // touche a
      is_key_press_left = true;
      break;

    case OF_KEY_UP: // touche ↑
    case 119:       // touche w
      is_key_press_up = true;
      break;

    case OF_KEY_RIGHT: // touche →
    case 100:          // touche d
      is_key_press_right = true;
      break;

    case OF_KEY_DOWN: // touche ↓
    case 115:         // touche s
      is_key_press_down = true;
      break;

    case 101: // touche e (avancer l'origine du rayon vers l'avant, en z positif)
      is_key_press_forward = true;
      break;

    case 113: // touche q (reculer l'origine du rayon vers l'arrière, en z négatif)
      is_key_press_backward = true;
      break;

    default:
      break;
  }
}

void Application::keyReleased(int key)
{
  switch(key)
  {
    case 114: // touche r
      renderer.reset();
      break;

    case OF_KEY_LEFT: // touche ←
    case 97:          // touche a
      is_key_press_left = false;
      break;

    case OF_KEY_UP: // touche ↑
    case 119:       // touche w
      is_key_press_up = false;
      break;

    case OF_KEY_RIGHT: // touche →
    case 100:          // touche d
      is_key_press_right = false;
      break;

    case OF_KEY_DOWN: // touche ↓
    case 115:         // touche s
      is_key_press_down = false;
      break;

    case 101: // touche e (avancer l'origine du rayon vers l'avant, en z positif)
      is_key_press_forward = false;
      break;

    case 113: // touche q (reculer l'origine du rayon vers l'arrière, en z négatif)
      is_key_press_backward = false;
      break;

    case ' ' : // barre d'espacement
      renderer.isAnimationActive = !renderer.isAnimationActive;
      break;

    case 43: // touche + (doubler le nombre d'échantillons)
    case 61: // touche = (la même touche que +, sans majuscule)
      if (renderer.sample_count * 2 <= sample_count_max)
        renderer.sample_count *= 2;
      ofLog() << "<sample count: " << renderer.sample_count << ">";
      break;

    case 45: // touche - (diviser le nombre d'échantillons par deux)
      if (renderer.sample_count > 1)
        renderer.sample_count /= 2;
      ofLog() << "<sample count: " << renderer.sample_count << ">";
      break;

    case 99: // touche c (concentrer le cône des échantillons, jusqu'à son seuil minimal)
      renderer.sample_cone_angle /= renderer.sample_cone_angle_step;
      if (renderer.sample_cone_angle < renderer.sample_cone_angle_min)
        renderer.sample_cone_angle = renderer.sample_cone_angle_min;
      ofLog() << "<sample cone angle: " << renderer.sample_cone_angle << ">";
      break;

    case 120: // touche x (dilater le cône des échantillons, jusqu'à son seuil maximal)
      renderer.sample_cone_angle *= renderer.sample_cone_angle_step;
      if (renderer.sample_cone_angle > renderer.sample_cone_angle_max)
        renderer.sample_cone_angle = renderer.sample_cone_angle_max;
      ofLog() << "<sample cone angle: " << renderer.sample_cone_angle << ">";
      break;

    default:
      break;
  }
}

void Application::windowResized(int w, int h)
{
  renderer.reset();

  ofLog() << "<app::windowResized: (" << w << ", " << h << ")>";
}

void Application::exit()
{
  ofLog() << "<app::exit>";
}
