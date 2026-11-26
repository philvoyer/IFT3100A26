// IFT3100A26 ~ ssao_330_fs.glsl
// Occlusion ambiante en espace écran (SSAO : screen space ambient occlusion).
// Pour chaque pixel, des échantillons sont placés autour de la position de la surface, dans l'espace de vue,
// puis projetés à l'écran pour comparer leur profondeur avec celle de la surface visible à cet endroit dans le g-buffer.
// Un échantillon caché par la géométrie de la scène contribue à l'occlusion du pixel.
// Le résultat est un facteur entre 0 (complètement occulté) et 1 (aucune occlusion), écrit en niveaux de gris.

#version 330

// nombre maximum d'échantillons (le même que dans renderer.h)
const int sample_count_max = 128;

// coordonnées de texture interpolées (position du pixel dans la fenêtre, entre 0 et 1)
in vec2 surface_texcoord;

// attribut en sortie
out vec4 fragment_color;

// couches du g-buffer (position et normale dans l'espace de vue)
uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normal;

// petite texture de vecteurs aléatoires, répétée sur toute la fenêtre, pour faire varier l'orientation des échantillons d'un pixel à l'autre
uniform sampler2D texture_noise;

// facteur d'échelle des coordonnées de texture pour répéter la texture de bruit (dimension de la fenêtre divisée par celle de la texture)
uniform vec2 noise_scale;

// noyau d'échantillons : des positions aléatoires dans une sphère ou un hémisphère de rayon 1
uniform vec3 kernel[sample_count_max];
uniform int sample_count;

// rayon de la zone d'échantillonnage autour de la surface (dans les unités de la scène)
uniform float radius;

// petit décalage de profondeur pour éviter qu'une surface plane s'occulte elle-même
uniform float bias;

// type de noyau : un hémisphère orienté selon la normale (l'amélioration courante de la technique)
// ou une sphère qui n'utilise pas la normale (la technique originale de Crytek pour le jeu Crysis, 2007)
uniform bool use_hemisphere;

// matrice de projection de la passe géométrie, pour projeter les échantillons dans le g-buffer
uniform mat4 projection;

void main()
{
  // lire la position de la surface visible dans le g-buffer
  vec4 position = texture(gbuffer_position, surface_texcoord);

  // aucune surface ne couvre ce pixel : aucune occlusion
  if (position.w == 0.0)
  {
    fragment_color = vec4(1.0);
    return;
  }

  vec3 p = position.xyz;
  vec3 n = normalize(texture(gbuffer_normal, surface_texcoord).xyz);

  // vecteur aléatoire de la texture de bruit pour ce pixel (dans le plan xy)
  vec3 random = normalize(texture(texture_noise, surface_texcoord * noise_scale).xyz);

  // base orthonormée de l'espace tangent orientée selon la normale (méthode de Gram-Schmidt),
  // avec une tangente aléatoire pour faire tourner le noyau autour de la normale d'un pixel à l'autre
  vec3 t = normalize(random - n * dot(random, n));
  vec3 b = cross(n, t);
  mat3 tbn = mat3(t, b, n);

  float occlusion = 0.0;

  for (int i = 0; i < sample_count; ++i)
  {
    // position de l'échantillon dans l'espace de vue
    // hémisphère : le noyau est orienté selon la normale, alors tous les échantillons sont au-dessus de la surface
    // sphère : le noyau est réfléchi par le vecteur aléatoire, et environ la moitié des échantillons sont sous la surface
    vec3 offset = use_hemisphere ? tbn * kernel[i] : reflect(kernel[i], random);
    vec3 s = p + offset * radius;

    // projeter l'échantillon dans l'espace écran pour trouver où le lire dans le g-buffer
    vec4 clip = projection * vec4(s, 1.0);
    vec2 uv = (clip.xy / clip.w) * 0.5 + 0.5;

    // surface visible à l'endroit de l'échantillon
    vec4 occluder = texture(gbuffer_position, uv);

    // aucune surface à cet endroit : l'échantillon n'est pas caché
    if (occluder.w == 0.0)
      continue;

    // ignorer progressivement les surfaces beaucoup plus proches de la caméra que le rayon,
    // pour qu'un objet en avant-plan n'assombrisse pas le contour des objets derrière lui
    float range = smoothstep(0.0, 1.0, radius / abs(p.z - occluder.z));

    // l'échantillon est caché si la surface visible est plus proche de la caméra que lui
    // (dans l'espace de vue, la caméra regarde vers -z, alors une surface plus proche a un z plus grand)
    occlusion += (occluder.z >= s.z + bias ? 1.0 : 0.0) * range;
  }

  // facteur d'occlusion ambiante : la proportion des échantillons qui ne sont pas cachés
  float ambient_occlusion = 1.0 - occlusion / float(sample_count);

  fragment_color = vec4(vec3(ambient_occlusion), 1.0);
}
