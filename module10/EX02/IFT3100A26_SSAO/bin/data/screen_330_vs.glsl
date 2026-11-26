// IFT3100A26 ~ screen_330_vs.glsl
// Shader de sommets d'un quadrilatère en pleine fenêtre, utilisé par les passes en espace écran :
// le SSAO (ssao_330_fs.glsl), le flou (blur_330_fs.glsl), l'éclairage (lighting_330_fs.glsl) et l'affichage des couches du g-buffer (gbuffer_view_330_fs.glsl).

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
