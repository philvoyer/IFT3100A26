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
    case '1':
      renderer.side_count = 3;
      renderer.polygon_name = "triangle";
      break;

    case '2':
      renderer.side_count = 4;
      renderer.polygon_name = "carré";
      break;

    case '3':
      renderer.side_count = 5;
      renderer.polygon_name = "pentagone";
      break;

    case '4':
      renderer.side_count = 6;
      renderer.polygon_name = "hexagone";
      break;

    case '5':
      renderer.side_count = 7;
      renderer.polygon_name = "heptagone";
      break;

    case '6':
      renderer.side_count = 8;
      renderer.polygon_name = "octogone";
      break;

    case '7':
      renderer.side_count = 9;
      renderer.polygon_name = "ennéagone";
      break;

    case '8':
      renderer.side_count = 10;
      renderer.polygon_name = "décagone";
      break;

    case '9':
      renderer.side_count = 11;
      renderer.polygon_name = "hendécagone";
      break;

    case '0':
      renderer.side_count = 12;
      renderer.polygon_name = "dodécagone";
      break;

    default:
      break;
  }

  update_window_title();
}

void Application::update_window_title()
{
  ofSetWindowTitle("polygone régulier : " + renderer.polygon_name + " (1-0)");
}

void Application::exit()
{
  ofLog() << "<app::exit>";
}
