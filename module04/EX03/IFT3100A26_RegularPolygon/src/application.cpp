// IFT3100A26_RegularPolygon/application.cpp
// Classe principale de l'application.

#include "application.h"

void Application::setup()
{
  ofLog() << "<app::setup>";

  renderer.setup();

  update_window_title();
}

void Application::update()
{
  renderer.update();
}

void Application::draw()
{
  renderer.draw();
}

void Application::keyReleased(int key)
{
  switch (key)
  {
    case '1': renderer.set_side_count(3);  break; // triangle
    case '2': renderer.set_side_count(4);  break; // carré
    case '3': renderer.set_side_count(5);  break; // pentagone
    case '4': renderer.set_side_count(6);  break; // hexagone
    case '5': renderer.set_side_count(7);  break; // heptagone
    case '6': renderer.set_side_count(8);  break; // octogone
    case '7': renderer.set_side_count(9);  break; // ennéagone
    case '8': renderer.set_side_count(10); break; // décagone
    case '9': renderer.set_side_count(11); break; // hendécagone
    case '0': renderer.set_side_count(12); break; // dodécagone

    case OF_KEY_UP:   // touche ↑ : ajouter un côté
      renderer.set_side_count(renderer.side_count + 1);
      break;

    case OF_KEY_DOWN: // touche ↓ : retirer un côté
      renderer.set_side_count(renderer.side_count - 1);
      break;

    default:
      break;
  }

  update_window_title();
}

void Application::update_window_title()
{
  ofSetWindowTitle("polygone régulier : " + renderer.polygon_name + " (1-0 ↑ ↓)");
}

void Application::exit()
{
  ofLog() << "<app::exit>";
}
