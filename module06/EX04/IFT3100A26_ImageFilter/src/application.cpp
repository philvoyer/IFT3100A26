// IFT3100A26_ImageFilter/application.cpp
// Classe principale de l'application.

#include "application.h"

void Application::setup()
{
  ofSetWindowTitle("filtre d'image (teinte et opacité)");

  ofLog() << "<app::setup>";

  renderer.setup();

  gui.setup();
  gui.add(color_picker.set("teinte", renderer.tint_color, ofColor(0, 0), ofColor(255, 255)));
  gui.add(slider_tint_mix.set("mix de couleur", renderer.factor_tint_mix, 0.0f, 1.0f));
  gui.add(slider_opacity.set("opacity", renderer.factor_opacity, 0.0f, 1.0f));
}

void Application::update()
{
  renderer.tint_color = color_picker;
  renderer.factor_tint_mix = slider_tint_mix;
  renderer.factor_opacity = slider_opacity;
}

void Application::draw()
{
  renderer.draw();

  gui.draw();
}

void Application::dragEvent(ofDragInfo dragInfo)
{
  ofLog() << "<app::ofDragInfo file[0]: " << dragInfo.files.at(0)
          << " at : " << dragInfo.position << ">";

  renderer.image.load(dragInfo.files.at(0));

  if (renderer.image.getWidth() > 0 && renderer.image.getHeight() > 0)
    renderer.resize_window();
}

void Application::exit()
{
  ofLog() << "<app::exit>";
}
