// IFT3100A26 ~ blur_330_fs.glsl
// Flou de la texture d'occlusion ambiante : la moyenne d'un bloc de 4x4 pixels,
// la même dimension que la texture de bruit, ce qui efface le motif répété des vecteurs aléatoires.

#version 330

// coordonnées de texture interpolées (position du pixel dans la fenêtre, entre 0 et 1)
in vec2 surface_texcoord;

// attribut en sortie
out vec4 fragment_color;

// texture d'occlusion ambiante produite par la passe SSAO
uniform sampler2D texture_ssao;

// dimension d'un pixel dans l'espace des coordonnées de texture (1 / largeur, 1 / hauteur)
uniform vec2 texel_size;

void main()
{
  float sum = 0.0;

  for (int x = -2; x < 2; ++x)
  {
    for (int y = -2; y < 2; ++y)
    {
      vec2 offset = vec2(float(x), float(y)) * texel_size;
      sum += texture(texture_ssao, surface_texcoord + offset).r;
    }
  }

  fragment_color = vec4(vec3(sum / 16.0), 1.0);
}
