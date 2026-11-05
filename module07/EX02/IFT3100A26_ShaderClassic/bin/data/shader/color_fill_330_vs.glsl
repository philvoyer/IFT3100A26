// IFT3100A26 ~ color_fill_330_vs.glsl

#version 330

// attribut de sommet
in vec4 position;

// attributs uniformes
uniform mat4x4 modelViewMatrix;
uniform mat4x4 projectionMatrix;

void main()
{
  // transformation de la position du sommet par les matrices de modèle, vue et projection
  gl_Position = projectionMatrix * modelViewMatrix * position;
}
