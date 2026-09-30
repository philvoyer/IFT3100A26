// IFT3100A26_TeaParty/application.cpp
// Classe principale de l'application.

#include "application.h"

void Application::setup()
{
  ofLog() << "<app::setup>";

  ofSetWindowTitle("teapot (↑ ↓ ← → 1 2 3 w e r f)");

  is_key_press_up = false;
  is_key_press_down = false;
  is_key_press_left = false;
  is_key_press_right = false;

  // initialiser le chronomètre pour que le premier delta de temps soit valide
  time_current = ofGetElapsedTimef();
  time_last = time_current;
  time_elapsed = 0.0f;

  renderer.setup();
}

void Application::update()
{
  time_current = ofGetElapsedTimef();
  time_elapsed = time_current - time_last;
  time_last = time_current;

  if (is_key_press_up)
    renderer.offset_z += renderer.delta_z * time_elapsed;
  if (is_key_press_down)
    renderer.offset_z -= renderer.delta_z * time_elapsed;
  // les flèches ← et → déplacent la scène vers la gauche et vers la droite de la fenêtre
  if (is_key_press_left)
    renderer.offset_x -= renderer.delta_x * time_elapsed;
  if (is_key_press_right)
    renderer.offset_x += renderer.delta_x * time_elapsed;

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
  switch (key)
  {
    case OF_KEY_LEFT: // touche ←
      is_key_press_left = true;
      break;

    case OF_KEY_UP: // touche ↑
      is_key_press_up = true;
      break;

    case OF_KEY_RIGHT: // touche →
      is_key_press_right = true;
      break;

    case OF_KEY_DOWN: // touche ↓
      is_key_press_down = true;
      break;

    default:
      break;
  }
}

void Application::keyReleased(int key)
{
  switch (key)
  {
    case '1':
      renderer.mesh_render_mode = MeshRenderMode::fill;
      ofLog() << "<mesh render mode: fill>";
      break;

    case '2':
      renderer.mesh_render_mode = MeshRenderMode::wireframe;
      ofLog() << "<mesh render mode: wireframe>";
      break;

    case '3':
      renderer.mesh_render_mode = MeshRenderMode::vertex;
      ofLog() << "<mesh render mode: vertex>";
      break;

    case OF_KEY_LEFT: // touche ←
      is_key_press_left = false;
      break;

    case OF_KEY_UP: // touche ↑
      is_key_press_up = false;
      break;

    case OF_KEY_RIGHT: // touche →
      is_key_press_right = false;
      break;

    case OF_KEY_DOWN: // touche ↓
      is_key_press_down = false;
      break;

    // les touches w, e et r activent ou désactivent la translation, la rotation et la proportion
    // (mêmes touches que les outils de manipulation W, E et R de Unity)
    case 'e':
      renderer.is_active_rotation = !renderer.is_active_rotation;
      ofLog() << "<rotation is active: " << renderer.is_active_rotation << ">";
      break;

    case 'f':
      renderer.is_flip_axis_y = !renderer.is_flip_axis_y;
      ofLog() << "<axis Y is flipped: " << renderer.is_flip_axis_y << ">";
      break;

    case 'r':
      renderer.is_active_proportion = !renderer.is_active_proportion;
      ofLog() << "<proportion is active: " << renderer.is_active_proportion << ">";
      break;

    case 'w':
      renderer.is_active_translation = !renderer.is_active_translation;
      ofLog() << "<translation is active: " << renderer.is_active_translation << ">";
      break;
  }
}

void Application::exit()
{
  ofLog() << "<app::exit>";
}
