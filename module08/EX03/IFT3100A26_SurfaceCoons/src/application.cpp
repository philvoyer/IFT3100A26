// IFT3100A26_SurfaceCoons/application.cpp
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
  ofSetWindowTitle("surface " + renderer.surface_name + " (↑↓←→ wasd qe tab 5-6 r spacebar)");

  time_current = ofGetElapsedTimef();
  time_elapsed = time_current - time_last;
  time_last = time_current;

  if (is_key_press_up)
    renderer.selected_ctrl_point->y -= renderer.delta_y * time_elapsed;
  if (is_key_press_down)
    renderer.selected_ctrl_point->y += renderer.delta_y * time_elapsed;
  if (is_key_press_left)
    renderer.selected_ctrl_point->x -= renderer.delta_x * time_elapsed;
  if (is_key_press_right)
    renderer.selected_ctrl_point->x += renderer.delta_x * time_elapsed;
  if (is_key_press_forward)
    renderer.selected_ctrl_point->z += renderer.delta_z * time_elapsed;
  if (is_key_press_backward)
    renderer.selected_ctrl_point->z -= renderer.delta_z * time_elapsed;

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

    case 101: // touche e (avancer le point vers l'avant, en z positif)
      is_key_press_forward = true;
      break;

    case 113: // touche q (reculer le point vers l'arrière, en z négatif)
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
    case OF_KEY_TAB: // touche tab
      renderer.select_next_ctrl_point();
      break;

    case 53: // touche 5
      ofLog() << "<select interpolation type : lerp>";
      renderer.interpolation_type = InterpolationType::lerp;
      break;

    case 54: // touche 6
      ofLog() << "<select interpolation type : smoothstep>";
      renderer.interpolation_type = InterpolationType::smoothstep;
      break;

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

    case 101: // touche e (avancer le point vers l'avant, en z positif)
      is_key_press_forward = false;
      break;

    case 113: // touche q (reculer le point vers l'arrière, en z négatif)
      is_key_press_backward = false;
      break;

    case ' ' : // barre d'espacement
      renderer.isAnimationActive = !renderer.isAnimationActive;
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
