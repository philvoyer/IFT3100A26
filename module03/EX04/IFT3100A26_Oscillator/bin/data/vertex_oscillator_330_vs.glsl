// IFT3100A26 ~ vertex_oscillator_330_vs.glsl

#version 330

// attributs uniformes
uniform mat4 modelViewProjectionMatrix;

uniform float amplitude_x;
uniform float amplitude_y;

uniform float period_x;
uniform float period_y;

uniform float time;

// attribut de sommet
in vec4 position;

// constantes
const float PI = 3.1415926535897932384626433832795;

// fonction d'oscillation x(t) = amplitude * sin(2 * PI * t / période)
float oscillate(float time, float amplitude, float period)
{
  return amplitude * sin(2.0 * PI * time / period);
}

void main()
{
  // copier la position d'origine du sommet
  vec4 oscillated_position = position;

  // calculer l'oscillation sur l'axe X
  oscillated_position.x += oscillate(time, amplitude_x, period_x);

  // calculer l'oscillation sur l'axe Y
  oscillated_position.y += oscillate(time, amplitude_y, period_y);

  // transformation de la position du sommet par les matrices de modèle, vue et projection
  gl_Position = modelViewProjectionMatrix * oscillated_position;
}
