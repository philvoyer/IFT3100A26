// IFT3100A26_RenderArchitecture/application.cpp
// Classe principale de l'application.

#include "application.h"

void Application::setup()
{
  ofLog() << "<app::setup>";

  renderer.setup();
}

void Application::update()
{
  renderer.update();

  ofSetWindowTitle("render architecture (1234567 i l r) " + renderer.render_mode_name);
}

void Application::draw()
{
  renderer.draw();
}

void Application::keyReleased(int key)
{
  switch (key)
  {
    case 49: // touche 1
      renderer.render_mode = RenderMode::forward;
      ofLog() << "<render mode: forward rendering>";
      break;

    case 50: // touche 2
      renderer.render_mode = RenderMode::deferred;
      ofLog() << "<render mode: deferred shading>";
      break;

    case 51: // touche 3
      renderer.render_mode = RenderMode::layer_position;
      ofLog() << "<render mode: g-buffer layer: position>";
      break;

    case 52: // touche 4
      renderer.render_mode = RenderMode::layer_normal;
      ofLog() << "<render mode: g-buffer layer: normal>";
      break;

    case 53: // touche 5
      renderer.render_mode = RenderMode::layer_color;
      ofLog() << "<render mode: g-buffer layer: color>";
      break;

    case 54: // touche 6
      renderer.render_mode = RenderMode::layer_depth;
      ofLog() << "<render mode: g-buffer layer: depth>";
      break;

    case 55: // touche 7
      renderer.render_mode = RenderMode::layer_all;
      ofLog() << "<render mode: g-buffer layers: position, normal, color, depth>";
      break;

    case 108: // touche l
      renderer.use_light_motion = !renderer.use_light_motion;
      ofLog() << "<light motion: " << (renderer.use_light_motion ? "on" : "off") << ">";
      break;

    case 105: // touche i
      renderer.show_info = !renderer.show_info;
      ofLog() << "<g-buffer info: " << (renderer.show_info ? "on" : "off") << ">";
      break;

    case 114: // touche r
      renderer.use_rotation = !renderer.use_rotation;
      ofLog() << "<rotation: " << (renderer.use_rotation ? "on" : "off") << ">";
      break;

    default:
      break;
  }
}

void Application::windowResized(int w, int h)
{
  // ajuster la dimension du g-buffer à celle de la fenêtre
  renderer.allocate_gbuffer(w, h);

  ofLog() << "<app::windowResized: (" << w << ", " << h << ")>";
}

void Application::exit()
{
  ofLog() << "<app::exit>";
}
