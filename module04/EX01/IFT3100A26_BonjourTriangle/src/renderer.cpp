// IFT3100A26_BonjourTriangle/renderer.cpp
// Classe responsable du rendu de l'application.

#include "renderer.h"

void Renderer::setup()
{
  ofSetFrameRate(60);

  point_diameter = 8.0f;
}

void Renderer::update()
{
  framebuffer_width = ofGetWidth();
  framebuffer_height = ofGetHeight();

  // position aléatoire pour chaque sommet
  vertex1_x = ofRandom(0, framebuffer_width);
  vertex1_y = ofRandom(0, framebuffer_height);
  vertex2_x = ofRandom(0, framebuffer_width);
  vertex2_y = ofRandom(0, framebuffer_height);
  vertex3_x = ofRandom(0, framebuffer_width);
  vertex3_y = ofRandom(0, framebuffer_height);

  // couleur aléatoire pour la zone de remplissage
  color_r = ofRandom(0, 256);
  color_g = ofRandom(0, 256);
  color_b = ofRandom(0, 256);

  ofLog() << std::setprecision(4) << "<triangle: v1:("
          << vertex1_x << ", " << vertex1_y << ") v2:("
          << vertex2_x << ", " << vertex2_y << ") v3:("
          << vertex3_x << ", " << vertex3_y << ") color:("
          << (int) color_r << ", " << (int) color_g << ", " << (int) color_b << ")>";
}

void Renderer::draw()
{
  ofBackgroundGradient(ofColor(191), ofColor(63));

  // remplir les formes dessinées
  ofFill();

  // dessiner le triangle
  ofSetColor(color_r, color_g, color_b);
  ofDrawTriangle(
    vertex1_x, vertex1_y,
    vertex2_x, vertex2_y,
    vertex3_x, vertex3_y);

  // dessiner les sommets
  ofSetColor(31);
  ofDrawEllipse(vertex1_x, vertex1_y, point_diameter, point_diameter);
  ofDrawEllipse(vertex2_x, vertex2_y, point_diameter, point_diameter);
  ofDrawEllipse(vertex3_x, vertex3_y, point_diameter, point_diameter);
}
