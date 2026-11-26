// IFT3100A26 ~ gbuffer_330_fs.glsl
// Passe géométrie : aucun calcul d'illumination, seulement l'écriture de la position et de la normale
// des surfaces visibles dans les couches du g-buffer (MRT : multiple render target), dans l'espace de vue.
// Toute la scène utilise le même matériau (argile), alors le g-buffer n'a pas besoin de couche de couleur.

#version 330

// attributs interpolés à partir des valeurs en sortie du shader de sommets
in vec3 surface_position;
in vec3 surface_normal;

// attributs en sortie, un par couche du g-buffer
layout(location = 0) out vec4 gbuffer_position;
layout(location = 1) out vec4 gbuffer_normal;

void main()
{
  // couche 1 : position de la surface dans l'espace de vue
  // (la composante w à 1 indique qu'une surface couvre ce fragment, le g-buffer est vidé à 0)
  gbuffer_position = vec4(surface_position, 1.0);

  // couche 2 : normale de la surface dans l'espace de vue (normalisée à nouveau après interpolation)
  gbuffer_normal = vec4(normalize(surface_normal), 1.0);
}
