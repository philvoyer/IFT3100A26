// IFT3100A26_Oscillator/application.cpp
// Classe principale de l'application.

#include "application.h"

void Application::setup()
{
  ofSetWindowTitle("oscillateur");

  ofLog() << "<app::setup>";

  renderer.setup();

  slider_amplitude_x.set("amplitude x", renderer.amplitude_x, 0.0f, 256.0f);
  slider_amplitude_y.set("amplitude y", renderer.amplitude_y, 0.0f, 256.0f);
  slider_period_x.set("period x", renderer.period_x, 0.1f, 10.0f);
  slider_period_y.set("period y", renderer.period_y, 0.1f, 10.0f);

  label_frequency_x.setup("frequency x", "");
  label_frequency_y.setup("frequency y", "");

  gui.setup("attributs");

  gui.add(slider_amplitude_x);
  gui.add(slider_amplitude_y);
  gui.add(slider_period_x);
  gui.add(slider_period_y);
  gui.add(&label_frequency_x);
  gui.add(&label_frequency_y);
}

void Application::update()
{
  renderer.amplitude_x = slider_amplitude_x;
  renderer.amplitude_y = slider_amplitude_y;
  renderer.period_x = slider_period_x;
  renderer.period_y = slider_period_y;

  renderer.update();

  // rafraîchir l'affichage de la fréquence calculée à partir de la période courante
  label_frequency_x.setup("frequency x", ofToString(renderer.frequency_x, 3) + " Hz");
  label_frequency_y.setup("frequency y", ofToString(renderer.frequency_y, 3) + " Hz");
}

void Application::draw()
{
  renderer.draw();

  gui.draw();
}
