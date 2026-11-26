// IFT3100A26 ~ screen_330_vs.glsl

#version 330

// attributs de sommet
in vec4 position;
in vec2 texcoord;

// attribut en sortie
out vec2 surface_texcoord;

// attribut uniforme
uniform mat4 modelViewProjectionMatrix;

void main()
{
  // passer les coordonnées de texture au shader de fragments
  surface_texcoord = texcoord;

  // transformation de la position du sommet
  gl_Position = modelViewProjectionMatrix * position;
}
