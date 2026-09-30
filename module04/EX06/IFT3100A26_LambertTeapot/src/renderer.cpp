// IFT3100A26_LambertTeapot/renderer.cpp
// Classe responsable du rendu de l'application.

#include "renderer.h"

void Renderer::setup()
{
  ofSetFrameRate(60);

  // paramètres
  // (multiplicateur de l'échelle normalisée du modèle : le chargeur ramène sa plus grande dimension
  // à la moitié de la largeur de la fenêtre au moment du chargement, ce n'est pas une taille en pixels)
  scale_teapot = 1.5f;

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

  // transformation du teapot (le décalage vertical de 90 pixels centre visuellement le modèle)
  teapot.setScale(scale_teapot, scale_teapot, scale_teapot);
  teapot.setPosition(center_x, center_y + 90, 0);

  // accumuler l'angle de rotation (il reste en place lorsque la rotation est désactivée, puis reprend d'où il était)
  if (use_rotation)
    rotation_angle += rotation_speed * ofGetLastFrameTime();

  teapot.setRotation(0, rotation_angle, 0.0f, 1.0f, 0.0f);

  // positionner la lumière dans l'espace de vue (relative à la caméra) plutôt que dans l'espace du monde,
  // car le shader compare cette position à celle des fragments, aussi exprimée dans l'espace de vue
  // (l'objet ofLight sert ici seulement à mémoriser la position transmise au shader, il n'est pas activé)
  light.setGlobalPosition(center_x, center_y, 255.0f);
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
