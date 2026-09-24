// IFT3100A26_Oscillator/application.h
// Classe principale de l'application.

#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "renderer.h"

class Application : public ofBaseApp
{
public:

  ofxPanel gui;

  ofParameter<float> slider_amplitude_x;
  ofParameter<float> slider_amplitude_y;
  ofParameter<float> slider_period_x;
  ofParameter<float> slider_period_y;

  // affichage en lecture seule de la fréquence calculée (f = 1 / T)
  ofxLabel label_frequency_x;
  ofxLabel label_frequency_y;

  Renderer renderer;

  void setup();
  void update();
  void draw();
};
