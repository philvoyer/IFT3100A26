// IFT3100A26_CameraFrustum/application.cpp
// Classe principale de l'application.

#include "application.h"

void Application::setup()
{
  ofSetWindowTitle("frustum de vision d'une caméra");

  renderer.setup();

  // initialisation du ui
  // note : les bornes permettent volontairement des valeurs invalides pour illustrer des cas limites
  // (fov de 0 ou 180 degrés, zoom nul ou infini, plan d'occlusion arrière plus proche que le plan avant),
  // le frustum est dessiné en rouge si des valeurs sont invalides
  gui.setup();
  gui.add(ui_camera_clip_n.set("camera near", renderer.camera_clip_n, 0.01f, ofGetHeight() / 10.0f));
  gui.add(ui_camera_clip_f.set("camera far", renderer.camera_clip_f, 0.0f, ofGetHeight() / 3.0f));
  gui.add(ui_camera_fov_v.set("camera fov v", renderer.camera_fov_v, 0.0f, 180.0f));
  gui.add(ui_camera_zoom_v.set("camera zoom", renderer.camera_zoom, 0.0f, 10.0f));
  gui.add(ui_camera_resolution_w.set("resolution w", renderer.camera_resolution_w, 1.0f, 3840.0f));
  gui.add(ui_camera_resolution_h.set("resolution h", renderer.camera_resolution_h, 1.0f, 2160.0f));

  // valeurs calculées à partir des attributs de la caméra (lecture seule)
  gui.add(ui_label_clip_height_n.setup("near height", ""));
  gui.add(ui_label_clip_height_f.setup("far height", ""));
  gui.add(ui_label_aspect_ratio.setup("aspect ratio", ""));
  gui.add(ui_label_fov_h.setup("fov h", ""));

  gui.add(ui_color_stroke.set("color stroke", ofColor(255), ofColor(0, 0), ofColor(255, 255)));
  gui.add(ui_color_fill.set("color fill", ofColor(255, 255, 0, 127), ofColor(0, 0), ofColor(255, 255)));

  ofLog() << "<app::setup>";
}

void Application::update()
{
  // ajustement manuel du plan d'occlusion avant
  if (renderer.camera_clip_n != ui_camera_clip_n)
    renderer.camera_clip_n = ui_camera_clip_n;

  // ajustement manuel du plan d'occlusion arrière
  if (renderer.camera_clip_f != ui_camera_clip_f)
    renderer.camera_clip_f = ui_camera_clip_f;

  // ajustement manuel du champ de vision
  if (renderer.camera_fov_v != ui_camera_fov_v)
  {
    renderer.camera_fov_v = ui_camera_fov_v;

    // recalculer le zoom
    renderer.camera_zoom = renderer.compute_zoom_from_fov(renderer.camera_fov_v);
    ui_camera_zoom_v = renderer.camera_zoom;
  }

  // ajustement manuel du zoom
  if (renderer.camera_zoom != ui_camera_zoom_v)
  {
    renderer.camera_zoom = ui_camera_zoom_v;

    // recalculer le champ de vision
    renderer.camera_fov_v = renderer.compute_fov_from_zoom(renderer.camera_zoom);
    ui_camera_fov_v = renderer.camera_fov_v;
  }

  // ajustement manuel de la résolution de l'image de la caméra
  renderer.camera_resolution_w = ui_camera_resolution_w;
  renderer.camera_resolution_h = ui_camera_resolution_h;

  // ajustement manuel des couleurs de dessin
  renderer.color_fill = ui_color_fill;
  renderer.color_stroke = ui_color_stroke;

  renderer.update();

  // afficher les valeurs calculées à partir des attributs de la caméra
  ui_label_clip_height_n = ofToString(renderer.clip_height_n, 2);
  ui_label_clip_height_f = ofToString(renderer.clip_height_f, 2);
  ui_label_aspect_ratio = ofToString(renderer.camera_aspect_ratio, 3);
  ui_label_fov_h = ofToString(renderer.camera_fov_h, 2);
}

void Application::draw()
{
  renderer.draw();
  gui.draw();
}

void Application::keyReleased(int key)
{
  switch (key)
  {
    case ' ': // touche espace
      renderer.log_matrix();
      break;

    default:
      break;
  }
}

void Application::exit()
{
  ofLog() << "<app::exit>";
}
