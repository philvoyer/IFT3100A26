// IFT3100A26_CameraFrustum/application.h
// Classe principale de l'application.

#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include "renderer.h"

class Application : public ofBaseApp
{
public:

  ofxPanel gui;

  ofParameter<float> ui_camera_fov_v;
  ofParameter<float> ui_camera_zoom_v;
  ofParameter<float> ui_camera_clip_n;
  ofParameter<float> ui_camera_clip_f;
  ofParameter<float> ui_camera_resolution_w;
  ofParameter<float> ui_camera_resolution_h;

  ofxLabel ui_label_clip_height_n;
  ofxLabel ui_label_clip_height_f;
  ofxLabel ui_label_aspect_ratio;
  ofxLabel ui_label_fov_h;

  ofParameter<ofColor> ui_color_stroke;
  ofParameter<ofColor> ui_color_fill;

  Renderer renderer;

  void setup();
  void update();
  void draw();
  void exit();

  void keyReleased(int key);
};
