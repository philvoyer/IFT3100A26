// IFT3100A25 ~ draw_depth_330_vs.glsl

#version 330

// attributs de sommet
in vec3 position;
in vec3 normal;

// attributs en sortie
out vec3 surface_position;
out vec3 surface_normal;

out float surface_depth;

// attributs uniformes
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

uniform float depth_range;

void main()
{
  // calculer la matrice normale
  mat3 normal_matrix = transpose(inverse(mat3(modelViewMatrix)));

  // transformation de la normale du sommet dans l'espace de vue
  surface_normal = normalize(normal_matrix * normal);

  // transformation de la position du sommet dans l'espace de vue
  surface_position = vec3(modelViewMatrix * vec4(position, 1.0));

  // transformation de la position du sommet par les matrices de modèle, vue et projection
  gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);

  // calculer la profondeur relativement l'intervalle de profondeur
  surface_depth = gl_Position.z / depth_range;
}
