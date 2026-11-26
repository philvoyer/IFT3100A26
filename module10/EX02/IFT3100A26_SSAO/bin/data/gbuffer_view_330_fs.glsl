// IFT3100A26 ~ gbuffer_view_330_fs.glsl

#version 330

// coordonnées de texture interpolées
in vec2 surface_texcoord;

// attribut en sortie
out vec4 fragment_color;

// couches du g-buffer
uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normal;

// couche à afficher (0 : position, 1 : normale)
uniform int layer;

// échelle des positions
uniform float position_scale;

void main()
{
  vec3 color = vec3(0.0);

  vec4 position = texture(gbuffer_position, surface_texcoord);

  // vérifier si une surface couvre ce pixel
  bool is_covered = position.w > 0.0;

  if (layer == 0 && is_covered)
  {
    // convertir la position en couleur (x en rouge, y en vert et z en bleu)
    color = 0.5 + 0.5 * position.xyz / position_scale;
  }
  else if (layer == 1 && is_covered)
  {
    // convertir la normale en couleur (x en rouge, y en vert et z en bleu)
    color = 0.5 + 0.5 * texture(gbuffer_normal, surface_texcoord).xyz;
  }

  fragment_color = vec4(color, 1.0);
}
