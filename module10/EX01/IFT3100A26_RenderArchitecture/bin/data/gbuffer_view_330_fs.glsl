// IFT3100A26 ~ gbuffer_view_330_fs.glsl

#version 330

// coordonnées de texture interpolées
in vec2 surface_texcoord;

// attribut en sortie
out vec4 fragment_color;

// couches du g-buffer
uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normal;
uniform sampler2D gbuffer_color;
uniform sampler2D gbuffer_depth;

// couche à afficher (0 : position, 1 : normale, 2 : couleur, 3 : profondeur)
uniform int layer;

// échelle des positions
uniform float position_scale;

// plans d'occlusion avant et arrière de la caméra
uniform float clip_near;
uniform float clip_far;

// intervalle de distances affiché en niveaux de gris
uniform float depth_min;
uniform float depth_max;

void main()
{
  vec3 color = vec3(0.0);

  // vérifier si une surface couvre ce pixel
  bool is_covered = texture(gbuffer_color, surface_texcoord).a > 0.0;

  if (layer == 0 && is_covered)
  {
    // convertir la position en couleur (x en rouge, y en vert et z en bleu)
    color = 0.5 + 0.5 * texture(gbuffer_position, surface_texcoord).xyz / position_scale;
  }
  else if (layer == 1 && is_covered)
  {
    // convertir la normale en couleur (x en rouge, y en vert et z en bleu)
    color = 0.5 + 0.5 * texture(gbuffer_normal, surface_texcoord).xyz;
  }
  else if (layer == 2 && is_covered)
  {
    // lire la couleur de la surface
    color = texture(gbuffer_color, surface_texcoord).rgb;
  }
  else if (layer == 3)
  {
    // convertir la profondeur, qui n'est pas linéaire, en distance à partir de la caméra (voir la section Z-buffer du module 5)
    float depth = texture(gbuffer_depth, surface_texcoord).r;
    float depth_ndc = 2.0 * depth - 1.0;
    float distance = 2.0 * clip_near * clip_far / (clip_far + clip_near - depth_ndc * (clip_far - clip_near));

    // convertir la distance en niveau de gris (plus la surface est proche, plus elle est claire)
    color = vec3(1.0 - clamp((distance - depth_min) / (depth_max - depth_min), 0.0, 1.0));
  }

  fragment_color = vec4(color, 1.0);
}
