// IFT3100A26 ~ deferred_330_fs.glsl

#version 330

// nombre de lumières de la scène
const int light_count = 4;

// coordonnées de texture interpolées
in vec2 surface_texcoord;

// attribut en sortie
out vec4 fragment_color;

// couches du g-buffer
uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normal;
uniform sampler2D gbuffer_color;

// couleur de l'arrière-plan
uniform vec3 color_background;

// paramètres du modèle d'illumination (Blinn-Phong)
uniform float ambient;
uniform float brightness;
uniform float specular;

// position (dans l'espace de vue) et couleur des sources de lumière
uniform vec3 light_position[light_count];
uniform vec3 light_color[light_count];

// fonction qui calcule le modèle d'illumination de Blinn-Phong pour toutes les lumières
vec3 blinn_phong(vec3 p, vec3 n, vec3 albedo)
{
  // calculer la direction de la surface vers la caméra (v)
  vec3 v = normalize(-p);

  // calculer la réflexion ambiante
  vec3 color = albedo * ambient;

  for (int i = 0; i < light_count; ++i)
  {
    // calculer la direction de la surface vers la lumière (l)
    vec3 l = normalize(light_position[i] - p);

    // calculer le niveau de réflexion diffuse (n • l)
    float reflection_diffuse = max(dot(n, l), 0.0);

    // réflexion spéculaire par défaut
    float reflection_specular = 0.0;

    // calculer la réflexion spéculaire seulement s'il y a réflexion diffuse
    if (reflection_diffuse > 0.0)
    {
      // calculer la direction du demi-vecteur de réflexion (h) en fonction du vecteur de vue (v) et de lumière (l)
      vec3 h = normalize(v + l);

      // calculer le niveau de réflexion spéculaire (n • h)
      reflection_specular = pow(max(dot(n, h), 0.0), brightness);
    }

    // ajouter la contribution de cette lumière
    color += (albedo * reflection_diffuse + specular * reflection_specular) * light_color[i];
  }

  return color;
}

void main()
{
  // lire les attributs de la surface dans les couches du g-buffer
  vec4 color = texture(gbuffer_color, surface_texcoord);
  vec3 p = texture(gbuffer_position, surface_texcoord).xyz;
  vec3 n = normalize(texture(gbuffer_normal, surface_texcoord).xyz);

  // utiliser la couleur de l'arrière-plan si aucune surface ne couvre ce pixel
  if (color.a == 0.0)
  {
    fragment_color = vec4(color_background, 1.0);
    return;
  }

  // calculer la couleur du pixel
  fragment_color = vec4(blinn_phong(p, n, color.rgb), 1.0);
}
