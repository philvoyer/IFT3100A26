// IFT3100A26_LambertTeapot/renderer.cpp
// Classe responsable du rendu de l'application.

#include "renderer.h"

void Renderer::setup()
{
  ofSetFrameRate(60);

  // paramètres
  scale_teapot = 1.5f;

  // largeur de la fenêtre au moment du chargement du modèle
  window_width_reference = ofGetWidth();

  // vitesse de rotation du teapot en degrés par seconde
  rotation_speed = 18.0f;
  rotation_angle = 0.0f;
  use_rotation = true;

  // chargement du modèle
  teapot.load("teapot.obj");

  // désactiver le matériau par défaut du modèle
  teapot.disableMaterials();

  // chargement du shader
  shader_lambert.load("lambert_330_vs.glsl", "lambert_330_fs.glsl");
  shader_normal.load("draw_normal_330_vs.glsl", "draw_normal_330_fs.glsl");

  // sélectionner le shader courant
  shader = shader_lambert;
}

void Renderer::update()
{
  // position au centre de la fenêtre d'affichage
  center_x = ofGetWidth() / 2.0f;
  center_y = ofGetHeight() / 2.0f;

  // proportion de la fenêtre courante par rapport à la fenêtre de référence
  window_proportion = std::min(ofGetWidth(), ofGetHeight()) / window_width_reference;

  // transformation du teapot, proportionnelle à la taille de la fenêtre
  float scale = scale_teapot * window_proportion;
  teapot.setScale(scale, scale, scale);
  teapot.setPosition(center_x, center_y + 90.0f * window_proportion, 0);

  // accumuler l'angle de rotation (il reste en place lorsque la rotation est désactivée, puis reprend d'où il était)
  if (use_rotation)
    rotation_angle += rotation_speed * ofGetLastFrameTime();

  teapot.setRotation(0, rotation_angle, 0.0f, 1.0f, 0.0f);

  // positionner la lumière dans l'espace de vue (relative à la caméra) plutôt que dans l'espace du monde,
  // car le shader compare cette position à celle des fragments, aussi exprimée dans l'espace de vue
  // (l'objet ofLight sert ici seulement à mémoriser la position transmise au shader, il n'est pas activé)
  light.setGlobalPosition(center_x, center_y, 255.0f * window_proportion);
}

void Renderer::draw()
{
  // couleur de l'arrière-plan
  ofSetBackgroundColor(color_background.r, color_background.g, color_background.b);

  // activer l'occlusion en profondeur
  ofEnableDepthTest();

  // activer le shader
  shader.begin();

  // passer les attributs uniformes du shader (la position de la lumière est dans l'espace de vue)
  shader.setUniform3f("color_ambient",  color_ambient.r / 255.0f, color_ambient.g / 255.0f, color_ambient.b / 255.0f);
  shader.setUniform3f("color_diffuse",  color_diffuse.r / 255.0f, color_diffuse.g / 255.0f, color_diffuse.b / 255.0f);
  shader.setUniform3f("light_position", light.getGlobalPosition());

  // dessiner le teapot
  teapot.draw(OF_MESH_FILL);

  // désactiver le shader
  shader.end();

  // désactiver l'occlusion en profondeur
  ofDisableDepthTest();
}
