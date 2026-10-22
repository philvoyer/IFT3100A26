// IFT3100A26_GLSL_TextureSampling.glsl
// Exemple de sections de code pour échantillonner une texture, compressée ou non, en GLSL.

// exemple de déclaration des coordonnées de mapping en provenance d'une structure de sommets
in vec2 surface_texcoord;

// exemple d'un échantillonneur de texture
uniform sampler2D texture_diffuse;

// exemples d'échantillonneurs de textures compressées
uniform sampler2D texture_height;
uniform sampler2D texture_normal;

// variables pour recevoir les échantillons de la texture
vec4 sample4;
vec3 sample3;
vec2 sample2;
float sample1;
vec3 normal;

// échantillonnage de la texture

// échantillonner une couleur RGBA
sample4 = texture(texture_diffuse, surface_texcoord);

// échantillonner une couleur RGB
sample3 = texture(texture_diffuse, surface_texcoord).rgb;

// échantillonner les composantes rouge et verte de la couleur
sample2 = texture(texture_diffuse, surface_texcoord).rg;

// échantillonner la composante rouge de la couleur
sample1 = texture(texture_diffuse, surface_texcoord).r;

// échantillonner une couleur RGB avec un swizzle des composantes
sample3 = texture(texture_diffuse, surface_texcoord).brg;

// échantillonner une couleur RGBA avec d'autres étiquettes de composante
sample4 = texture(texture_diffuse, surface_texcoord).rgba;
sample4 = texture(texture_diffuse, surface_texcoord).xyzw;
sample4 = texture(texture_diffuse, surface_texcoord).stpq;

// échantillonnage d'une texture compressée
// (le GPU décode les blocs compressés de manière transparente, l'appel à texture() est le même)

// BC1 (DXT1) : RGB, alpha de 1 bit
sample4 = texture(texture_diffuse, surface_texcoord);

// BC4 (RGTC1) : une seule composante, dans le canal rouge (ex. height map ou texture de rugosité)
sample1 = texture(texture_height, surface_texcoord).r;

// BC5 (RGTC2) : deux composantes, dans les canaux rouge et vert (ex. normal map)
sample2 = texture(texture_normal, surface_texcoord).rg;

// reconstruire la composante z de la normale à partir de x et y (vecteur unitaire)
normal.xy = sample2 * 2.0 - 1.0;
normal.z = sqrt(max(0.0, 1.0 - dot(normal.xy, normal.xy)));

// la reconstruction de la normale suppose que les composantes sont stockées dans l'intervalle [0, 1]
// et ramenées dans l'intervalle [-1, 1] par * 2.0 - 1.0, ce qui est la convention habituelle des normal maps
