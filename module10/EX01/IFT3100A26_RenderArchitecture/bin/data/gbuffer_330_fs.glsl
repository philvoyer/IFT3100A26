// IFT3100A26 ~ gbuffer_330_fs.glsl

#version 330

// attributs interpolés à partir des valeurs en sortie du shader de sommets
in vec3 surface_position;
in vec3 surface_normal;

// attributs en sortie, un par couche du g-buffer
layout(location = 0) out vec4 gbuffer_position;
layout(location = 1) out vec4 gbuffer_normal;
layout(location = 2) out vec4 gbuffer_color;

// couleur de l'objet (albédo)
uniform vec3 color_albedo;

void main()
{
  // écrire la position de la surface dans la couche 1
  gbuffer_position = vec4(surface_position, 1.0);

  // écrire la normale de la surface dans la couche 2, re-normalisée après interpolation
  gbuffer_normal = vec4(normalize(surface_normal), 1.0);

  // écrire la couleur de la surface dans la couche 3 (alpha à 1 : une surface couvre ce fragment)
  gbuffer_color = vec4(color_albedo, 1.0);
}
