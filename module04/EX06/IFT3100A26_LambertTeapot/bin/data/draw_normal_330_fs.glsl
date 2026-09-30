// IFT3100A26 ~ draw_normal_330_fs.glsl

#version 330

// attributs interpolés à partir des valeurs en sortie du shader de sommets
in vec3 surface_normal;

// attribut en sortie
out vec4 fragment_color;

void main()
{
  // re-normaliser la normale après interpolation (N)
  vec3 normal = normalize(surface_normal);

  // convertir les composantes XYZ de la normale de l'intervalle [-1, 1] vers l'intervalle [0, 1] pour obtenir une couleur RGB
  fragment_color = vec4(normal * 0.5 + 0.5, 1.0);
}
