// IFT3100A26_ImageFilter/renderer.cpp
// Classe responsable du rendu de l'application.

#include "renderer.h"

void Renderer::setup()
{
  ofSetFrameRate(60);
  ofSetBackgroundColor(31);
  ofSetLogLevel(OF_LOG_VERBOSE);
  ofDisableArbTex();

  tint_color.set(255, 255, 255);
  factor_tint_mix = 0.618f;
  factor_opacity = 1.0f;
  offset_vertical = 32;
  offset_horizontal = 32;

  image.load("teapot.jpg");

  resize_window();

  shader.load("image_filter_330_vs.glsl", "image_filter_330_fs.glsl");
}

void Renderer::draw()
{
  // dessiner l'image sur la première moitié de la surface de la fenêtre
  image.draw(offset_horizontal, offset_vertical, image.getWidth(), image.getHeight());

  // activer le filtre
  shader.begin();

  // passer les attributs uniformes au shader
  shader.setUniformTexture("image", image.getTexture(), 1);
  shader.setUniform3f("tint_color", tint_color.r / 255.0f, tint_color.g / 255.0f, tint_color.b / 255.0f);
  shader.setUniform1f("factor_tint_mix", factor_tint_mix);
  shader.setUniform1f("factor_opacity", factor_opacity);

  // dessiner l'image sur la seconde moitié de la surface de la fenêtre
  image.draw(image.getWidth() + offset_horizontal * 2, offset_vertical, image.getWidth(), image.getHeight());

  // désactiver le filtre
  shader.end();
}

// fonction qui ajuste la taille de la fenêtre en fonction de la taille de l'image
void Renderer::resize_window()
{
  ofSetWindowShape(
    image.getWidth() * 2 + offset_horizontal * 3,
    image.getHeight() * 1 + offset_vertical * 2);
}
