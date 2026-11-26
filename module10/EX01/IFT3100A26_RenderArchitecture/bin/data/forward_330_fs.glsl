// IFT3100A26 ~ forward_330_fs.glsl

#version 330

// nombre de lumières de la scène
const int light_count = 4;

// attributs interpolés à partir des valeurs en sortie du shader de sommets
in vec3 surface_position;
in vec3 surface_normal;

// attribut en sortie
out vec4 fragment_color;

// couleur de l'objet (albédo)
uniform vec3 color_albedo;

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
  // re-normaliser la normale après interpolation
  vec3 n = normalize(surface_normal);

  // calculer la couleur du fragment
  fragment_color = vec4(blinn_phong(surface_position, n, color_albedo), 1.0);
}
