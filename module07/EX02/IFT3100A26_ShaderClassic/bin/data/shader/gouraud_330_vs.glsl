// IFT3100A26 ~ gouraud_330_vs.glsl

#version 330

// attributs de sommet
in vec3 position;
in vec3 normal;

// attributs en sortie
out vec3 surface_color;

// attributs uniformes
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

// couleurs de réflexion du matériau
uniform vec3 color_ambient;
uniform vec3 color_diffuse;
uniform vec3 color_specular;

// facteur de brillance spéculaire du matériau
uniform float brightness;

// position d'une source de lumière dans l'espace de vue (même espace que surface_position)
uniform vec3 light_position;

void main()
{
  // calculer la matrice normale
  mat3 normal_matrix = transpose(inverse(mat3(modelViewMatrix)));

  // transformation de la normale du sommet dans l'espace de vue
  vec3 surface_normal = normal_matrix * normal;

  // transformation de la position du sommet dans l'espace de vue
  vec3 surface_position = vec3(modelViewMatrix * vec4(position, 1.0));

  // re-normaliser la normale
  vec3 n = normalize(surface_normal);

  // calculer la direction de la surface vers la lumière (l)
  vec3 l = normalize(light_position - surface_position);

  // calculer le niveau de réflexion diffuse (n • l)
  float reflection_diffuse = max(dot(n, l), 0.0);

  // réflexion spéculaire par défaut
  float reflection_specular = 0.0;

  // calculer la réflexion spéculaire seulement s'il y a réflexion diffuse
  if (reflection_diffuse > 0.0)
  {
    // calculer la direction de la surface vers la caméra (v)
    vec3 v = normalize(-surface_position);

    // calculer la direction de la réflexion (r) du rayon incident (-l) en fonction de la normale (n)
    vec3 r = reflect(-l, n);

    // calculer le niveau de réflexion spéculaire (r • v)
    reflection_specular = pow(max(dot(v, r), 0.0), brightness);
  }

  // calculer la couleur du fragment
  surface_color = vec3(
    color_ambient +
    color_diffuse * reflection_diffuse +
    color_specular * reflection_specular);

  // transformation de la position du sommet par les matrices de modèle, vue et projection
  gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
}
