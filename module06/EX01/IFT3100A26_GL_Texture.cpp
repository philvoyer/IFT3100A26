// IFT3100A26_GL_Texture.cpp
// Exemple de sections de code pour configurer et manipuler des textures, compressées ou non, avec OpenGL.

// 1. ouvrir un fichier image en mode lecture et en extraire les propriétés

int image_width = ...;
int image_height = ...;
int image_component = ...;


// 2. allocation de l'espace mémoire pour les pixels de l'image

int image_size = image_width * image_height * image_component;

GLubyte* pixels = (GLubyte*) std::malloc(image_size * sizeof(GLubyte));

// lire le contenu de l'image et copier ses pixels en mémoire (RAM)
// ...


// 3. sélectionner l'unité de texture active

// index de l'unité de texture
int index = ...; // index de 0 à GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1

glActiveTexture(GL_TEXTURE0 + index);


// 4.1 création d'une texture à partir des données d'une image non-compressée (ex. png)

// identifiant de la texture
GLuint texture_id;

// générer un nouvel identifiant de texture
glGenTextures(1, &texture_id);

// lier la texture à l'unité de texture active comme texture 2D
glBindTexture(GL_TEXTURE_2D, texture_id);

// format des pixels en fonction du nombre de composantes de l'image (RGB ou RGBA)
GLenum image_format = (image_component == 4) ? GL_RGBA : GL_RGB;

// indiquer que les rangées de pixels sont contiguës en mémoire
// par défaut, OpenGL suppose des rangées alignées sur 4 octets,
// donc il s'attendrait à de 0 à 3 octets de remplissage (padding) par rangée avec du RGB
glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

// allouer la mémoire de la texture (niveau de mipmapping 0) et y copier les pixels de l'image
glTexImage2D(GL_TEXTURE_2D, 0, image_format, image_width, image_height, 0, image_format, GL_UNSIGNED_BYTE, pixels);

// les pixels sont maintenant copiés dans la mémoire du GPU (VRAM), la copie en RAM n'est plus nécessaire
std::free(pixels);


// 4.2 création d'une texture compressée à partir des données d'un fichier de type conteneur (DDS, KTX)

// les données du fichier sont déjà compressées et sont copiées telles quelles dans la mémoire du GPU (VRAM)
// le GPU les décode lors de l'échantillonnage, il n'y a donc pas de conversion à faire sur le CPU
GLubyte* compressed_data = ...;

// taille en octets des données compressées
// avec BC7, l'image est découpée en blocs de 4 x 4 pixels et chaque bloc occupe
// 16 octets le + 3 arrondit vers le haut le nombre de blocs (division entière),
// car une dimension qui n'est pas un multiple de 4 occupe un bloc partiel de plus
// en pratique, cette taille est déjà fournie dans l'en-tête du fichier DDS ou KTX
GLsizei compressed_size = ((image_width + 3) / 4) * ((image_height + 3) / 4) * 16;

// identifiant de la texture compressée
GLuint compressed_texture_id;

// générer un nouvel identifiant de texture
glGenTextures(1, &compressed_texture_id);

// lier la texture à l'unité de texture active comme texture 2D
glBindTexture(GL_TEXTURE_2D, compressed_texture_id);

// allouer la mémoire de la texture (niveau de mipmapping 0) et y copier les données compressées
// BC7 (BPTC) : RGBA, OpenGL 4.2 (sinon avec l'extension GL_ARB_texture_compression_bptc)
glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_BPTC_UNORM, image_width, image_height, 0, compressed_size, compressed_data);

// les niveaux de mipmapping d'une texture compressée ne peuvent pas être générés avec glGenerateMipmap (étape 7)
// ils doivent être précalculés et enregistrés dans le fichier, puis copiés un par un avec glCompressedTexImage2D


// 5. définir les paramètres d'enveloppement aux extrémités de la texture active

// mode d'enveloppement de texture : GL_CLAMP_TO_BORDER
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

// couleur fixe retournée à l'extérieur de la texture (par défaut (0, 0, 0, 0), soit du noir transparent)
GLfloat border_color[] = {1.0f, 0.0f, 1.0f, 1.0f}; // magenta : couleur de débogage classique, facile à repérer à l'écran
glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

// mode d'enveloppement de texture : GL_CLAMP_TO_EDGE
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

// mode d'enveloppement de texture : GL_REPEAT
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

// mode d'enveloppement de texture : GL_MIRRORED_REPEAT
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);


// 6. définir les paramètres d'interpolation des texels de la texture active

// pas de filtrage
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

// filtrage bilinéaire
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

// filtrage trilinéaire (nécessite les niveaux de mipmapping, voir l'étape 7)
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

// filtrage anisotropique
// intégré à OpenGL 4.6, sinon avec l'extension GL_EXT_texture_filter_anisotropic (ex. OpenGL 3.3 et macOS)
GLfloat largest_anisotropy;
glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_anisotropy);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_anisotropy);


// 7. mipmapping

// générer les différents niveaux de mipmapping de la texture
glGenerateMipmap(GL_TEXTURE_2D);


// 8. composition

// activer la composition
glEnable(GL_BLEND);

// configuration du mode de composition
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


// 9. correction gamma

// activer la correction gamma automatique avec l'espace de couleur sRGB
glEnable(GL_FRAMEBUFFER_SRGB);


// 10. anticrénelage

// activer le multisampling
glEnable(GL_MULTISAMPLE);

// nombre d'échantillons
GLint sample_count = 4;

// framebuffer avec anticrénelage (MSAA)
glRenderbufferStorageMultisample(GL_RENDERBUFFER, sample_count, GL_RGBA, image_width, image_height);

// texture avec anticrénelage (MSAA)
GLuint multisampled_texture_id;
glGenTextures(1, &multisampled_texture_id);
glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampled_texture_id);
glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sample_count, GL_RGBA, image_width, image_height, GL_TRUE);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampled_texture_id, 0);

// résolution de l'anticrénelage de la texture par copie dans un framebuffer
glBlitFramebuffer(0, 0, image_width, image_height, 0, 0, image_width, image_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
