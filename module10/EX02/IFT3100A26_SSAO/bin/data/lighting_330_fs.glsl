// IFT3100A26 ~ lighting_330_fs.glsl
// Passe éclairage : la réflexion ambiante est modulée par le facteur d'occlusion ambiante du SSAO
// (notes de cours, section « Réflexion ambiante »), puis une lumière ponctuelle ajoute une réflexion diffuse de Lambert.
// Toute la scène a le même matériau mat et pâle, comme un modèle en argile, pour que l'occlusion soit facile à voir.

#version 330

// coordonnées de texture interpolées (position du pixel dans la fenêtre, entre 0 et 1)
in vec2 surface_texcoord;

// attribut en sortie
out vec4 fragment_color;

// couches du g-buffer (position et normale dans l'espace de vue)
uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normal;

// facteur d'occlusion ambiante, après le flou
uniform sampler2D texture_ssao;

// couleur de l'arrière-plan et du matériau (albédo)
uniform vec3 color_background;
uniform vec3 color_albedo;

// intensité de la réflexion ambiante et de la réflexion diffuse
uniform float ambient;
uniform float diffuse;

// position de la lumière dans l'espace de vue (le même espace que la position des surfaces dans le g-buffer)
uniform vec3 light_position;

// utiliser ou non le facteur d'occlusion ambiante
uniform bool use_ssao;

// afficher seulement la réflexion ambiante (sans la lumière ponctuelle)
uniform bool use_ambient_only;

void main()
{
  vec4 position = texture(gbuffer_position, surface_texcoord);

  // aucune surface ne couvre ce pixel : couleur de l'arrière-plan
  if (position.w == 0.0)
  {
    fragment_color = vec4(color_background, 1.0);
    return;
  }

  vec3 p = position.xyz;
  vec3 n = normalize(texture(gbuffer_normal, surface_texcoord).xyz);

  // facteur d'occlusion ambiante (1 : aucune occlusion)
  float ambient_occlusion = use_ssao ? texture(texture_ssao, surface_texcoord).r : 1.0;

  // réflexion ambiante modulée par l'occlusion ambiante
  vec3 color = color_albedo * ambient * ambient_occlusion;

  if (!use_ambient_only)
  {
    // réflexion diffuse de Lambert (n • l)
    vec3 l = normalize(light_position - p);
    color += color_albedo * diffuse * max(dot(n, l), 0.0);
  }

  fragment_color = vec4(color, 1.0);
}
