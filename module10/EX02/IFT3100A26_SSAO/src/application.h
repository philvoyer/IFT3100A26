// IFT3100A26_SSAO/application.h
// Classe principale de l'application.

#pragma once

#include "ofMain.h"

#include "renderer.h"

class Application : public ofBaseApp
{
public:

  Renderer renderer;

  ofTrueTypeFont font;

  // limite du nombre d'images par seconde (60 images par seconde et synchronisation verticale), désactivée avec la touche v
  bool use_frame_limit;

  void setup();
  void update();
  void draw();
  void exit();

  void keyReleased(int key);
  void windowResized(int w, int h);

  void update_window_title();
  void apply_frame_limit();
  void draw_label(const std::string& text, float x, float y);
};
