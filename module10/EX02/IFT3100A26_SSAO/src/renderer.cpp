// IFT3100A26_SSAO/renderer.cpp
// Classe responsable du rendu de l'application.

#include "renderer.h"

void Renderer::setup()
{
  ofSetFrameRate(60);
  ofSetSphereResolution(32);

  // utiliser des textures GL_TEXTURE_2D avec des coordonnées de texture normalisées (entre 0 et 1)
  ofDisableArbTex();

  // mode de rendu au lancement de l'application
  render_mode = render_mode_lighting_ssao;

  // couleurs de l'arrière-plan et du matériau (un blanc cassé mat, comme un modèle en argile)
  color_background = ofColor(31, 31, 31);
  color_albedo = ofColor(230, 224, 214);

  // paramètres du modèle d'illumination
  ambient = 0.5f;
  diffuse = 0.6f;

  // paramètres du SSAO
  sample_count = 32;
  use_hemisphere = true;
  radius_factor = 0.5f;

  // mouvement de la lumière (en degrés par seconde)
  light_orbit_angle = 0.0f;
  light_orbit_speed = 20.0f;
  use_light_motion = true;

  // oscillation de la scène autour de l'axe vertical (amplitude en degrés et période en secondes)
  rotation_time = 0.0f;
  rotation_amplitude = 30.0f;
  rotation_period = 21.0f;
  rotation_angle = 0.0f;
  use_rotation = true;

  // inclinaison de la scène vers la caméra pour voir le plancher de haut
  tilt_angle = -25.0f;

  // chargement du modèle
  // (ofxAssimpModelLoader ajuste la dimension du modèle à celle de la fenêtre au chargement, d'où une petite échelle)
  teapot.load("teapot.obj");
  teapot.disableMaterials();
  teapot_scale = 0.5f;

  // chargement des shaders des quatre passes
  shader_gbuffer.load("geometry_330_vs.glsl", "gbuffer_330_fs.glsl");
  shader_ssao.load("screen_330_vs.glsl", "ssao_330_fs.glsl");
  shader_blur.load("screen_330_vs.glsl", "blur_330_fs.glsl");
  shader_lighting.load("screen_330_vs.glsl", "lighting_330_fs.glsl");

  // chargement du shader qui affiche une couche du g-buffer
  shader_gbuffer_view.load("screen_330_vs.glsl", "gbuffer_view_330_fs.glsl");

  // noyau d'échantillons et texture de bruit
  generate_kernel();
  generate_noise();

  // allocation des framebuffers
  allocate_framebuffers(ofGetWidth(), ofGetHeight());
}

// fonction qui alloue les framebuffers à la dimension de la fenêtre d'affichage
void Renderer::allocate_framebuffers(int width, int height)
{
  framebuffer_width = width;
  framebuffer_height = height;

  ofFboSettings settings;

  settings.width = width;
  settings.height = height;

  // pas d'interpolation entre les texels : chaque pixel lit exactement les valeurs de sa surface
  settings.textureTarget = GL_TEXTURE_2D;
  settings.minFilter = GL_NEAREST;
  settings.maxFilter = GL_NEAREST;

  // g-buffer : deux buffers de rendu couleur en virgule flottante (la position et la normale peuvent être négatives ou plus grandes que 1),
  // et un buffer de profondeur pour le test de profondeur de la passe géométrie
  settings.numColorbuffers = 2;
  settings.internalformat = GL_RGBA32F;
  settings.useDepth = true;

  gbuffer.allocate(settings);

  // framebuffers du facteur d'occlusion ambiante, avant et après le flou (un seul buffer de rendu couleur, sans profondeur)
  settings.numColorbuffers = 1;
  settings.useDepth = false;

  fbo_ssao.allocate(settings);
  fbo_blur.allocate(settings);

  ofLog() << "<allocate framebuffers: " << width << "x" << height << ">";
}

// fonction qui génère le noyau d'échantillons du SSAO, dans un hémisphère (z positif) ou dans une sphère de rayon 1
void Renderer::generate_kernel()
{
  // même suite de nombres aléatoires à chaque génération, pour comparer les deux types de noyau
  ofSeedRandom(3100);

  for (int i = 0; i < sample_count; ++i)
  {
    // direction aléatoire, puis distance aléatoire à partir du centre
    glm::vec3 sample = glm::vec3(
      ofRandom(-1.0f, 1.0f),
      ofRandom(-1.0f, 1.0f),
      use_hemisphere ? ofRandom(0.0f, 1.0f) : ofRandom(-1.0f, 1.0f));

    sample = glm::normalize(sample) * ofRandom(0.0f, 1.0f);

    // rapprocher la plupart des échantillons du centre, où l'occlusion des surfaces voisines est la plus importante
    float scale = (float) i / (float) sample_count;
    scale = 0.1f + 0.9f * scale * scale;

    kernel[i] = sample * scale;
  }

  ofLog() << "<generate kernel: " << sample_count << " samples in a " << (use_hemisphere ? "hemisphere" : "sphere") << ">";
}

// fonction qui génère la texture de bruit : 4x4 vecteurs aléatoires dans le plan xy, qui font tourner le noyau d'un pixel à l'autre
void Renderer::generate_noise()
{
  ofSeedRandom(1003);

  for (int i = 0; i < noise_size * noise_size; ++i)
  {
    noise_data[i * 3 + 0] = ofRandom(-1.0f, 1.0f);
    noise_data[i * 3 + 1] = ofRandom(-1.0f, 1.0f);
    noise_data[i * 3 + 2] = 0.0f;
  }

  texture_noise.allocate(noise_size, noise_size, GL_RGB32F);
  texture_noise.loadData(noise_data, noise_size, noise_size, GL_RGB);

  // la texture est répétée sur toute la fenêtre, sans interpolation entre ses texels
  texture_noise.setTextureWrap(GL_REPEAT, GL_REPEAT);
  texture_noise.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
}

void Renderer::update()
{
  float w = ofGetWidth();
  float h = ofGetHeight();
  float dt = ofGetLastFrameTime();

  // dimension de référence des objets, relative à la fenêtre d'affichage
  unit = std::min(w, h) / 6.0f;

  // rayon de la zone d'échantillonnage et décalage de profondeur, relatifs à la dimension des objets
  radius = unit * radius_factor;
  bias = unit * 0.02f;

  // oscillation de la scène autour de l'axe vertical
  if (use_rotation)
    rotation_time += dt;

  rotation_angle = rotation_amplitude * sinf(glm::two_pi<float>() * rotation_time / rotation_period);

  // la lumière tourne au-dessus de la scène (l'axe y d'openFrameworks pointe vers le bas, alors le haut est en y négatif)
  if (use_light_motion)
    light_orbit_angle = fmodf(light_orbit_angle + light_orbit_speed * dt, 360.0f);

  light_position_scene = glm::vec3(
    unit * 2.5f * cosf(glm::radians(light_orbit_angle)),
    -unit * 3.0f,
    unit * 2.5f * sinf(glm::radians(light_orbit_angle)));
}

void Renderer::draw()
{
  float w = ofGetWidth();
  float h = ofGetHeight();

  // le mélange de couleurs est désactivé pour que chaque framebuffer contienne exactement les valeurs écrites par les shaders
  ofDisableAlphaBlending();

  // passe 1, géométrie : la position et la normale des surfaces visibles sont rendues dans les couches du g-buffer
  gbuffer.begin();

  // écrire dans les deux buffers de rendu couleur en même temps (MRT)
  gbuffer.activateAllDrawBuffers();

  // vider les couches du g-buffer (la composante w de la position à 0 indique qu'aucune surface ne couvre un fragment)
  ofClear(0, 0, 0, 0);

  // matrice de projection utilisée pour rendre le g-buffer, la même que le SSAO doit utiliser pour y projeter ses échantillons
  matrix_projection = ofGetCurrentMatrix(OF_MATRIX_PROJECTION);

  ofEnableDepthTest();

  shader_gbuffer.begin();

  ofPushMatrix();

  transform_scene();

  // position de la lumière dans l'espace de vue, transformée par la même matrice de modèle-vue que la scène
  light_position_view = glm::vec3(ofGetCurrentMatrix(OF_MATRIX_MODELVIEW) * glm::vec4(light_position_scene, 1.0f));

  draw_scene();

  ofPopMatrix();

  shader_gbuffer.end();

  ofDisableDepthTest();

  gbuffer.end();

  // passe 2, SSAO : un quadrilatère en pleine fenêtre calcule le facteur d'occlusion ambiante de chaque pixel à partir du g-buffer
  fbo_ssao.begin();
  ofClear(255, 255, 255, 255);

  shader_ssao.begin();
  shader_ssao.setUniformTexture("gbuffer_position", gbuffer.getTexture(0), 1);
  shader_ssao.setUniformTexture("gbuffer_normal", gbuffer.getTexture(1), 2);
  shader_ssao.setUniformTexture("texture_noise", texture_noise, 3);
  shader_ssao.setUniform2f("noise_scale", w / noise_size, h / noise_size);
  shader_ssao.setUniform3fv("kernel", &kernel[0].x, sample_count);
  shader_ssao.setUniform1i("sample_count", sample_count);
  shader_ssao.setUniform1f("radius", radius);
  shader_ssao.setUniform1f("bias", bias);
  shader_ssao.setUniform1i("use_hemisphere", use_hemisphere);
  shader_ssao.setUniformMatrix4f("projection", matrix_projection);

  // dessiner le quadrilatère en pleine fenêtre (les coordonnées de texture de cette texture servent à lire le g-buffer)
  gbuffer.getTexture(0).draw(0.0f, 0.0f, w, h);

  shader_ssao.end();
  fbo_ssao.end();

  // passe 3, flou : la moyenne de 4x4 pixels efface le motif de la texture de bruit
  fbo_blur.begin();
  ofClear(255, 255, 255, 255);

  shader_blur.begin();
  shader_blur.setUniformTexture("texture_ssao", fbo_ssao.getTexture(), 1);
  shader_blur.setUniform2f("texel_size", 1.0f / w, 1.0f / h);

  fbo_ssao.getTexture().draw(0.0f, 0.0f, w, h);

  shader_blur.end();
  fbo_blur.end();

  ofSetColor(255);

  if (render_mode == render_mode_ssao_raw)
  {
    // facteur d'occlusion ambiante avant le flou, en niveaux de gris
    fbo_ssao.draw(0.0f, 0.0f);
  }
  else if (render_mode == render_mode_ssao_blur)
  {
    // facteur d'occlusion ambiante après le flou, en niveaux de gris
    fbo_blur.draw(0.0f, 0.0f);
  }
  else if (render_mode == render_mode_layers)
  {
    // dessiner les couches du SSAO dans une grille de 2x2 : la position et la normale du g-buffer,
    // puis le facteur d'occlusion ambiante avant et après le flou
    draw_gbuffer_layer(0, 0.0f, 0.0f, w / 2.0f, h / 2.0f);
    draw_gbuffer_layer(1, w / 2.0f, 0.0f, w / 2.0f, h / 2.0f);
    fbo_ssao.draw(0.0f, h / 2.0f, w / 2.0f, h / 2.0f);
    fbo_blur.draw(w / 2.0f, h / 2.0f, w / 2.0f, h / 2.0f);
  }
  else
  {
    // passe 4, éclairage : la réflexion ambiante est modulée par le facteur d'occlusion ambiante
    shader_lighting.begin();
    shader_lighting.setUniformTexture("gbuffer_position", gbuffer.getTexture(0), 1);
    shader_lighting.setUniformTexture("gbuffer_normal", gbuffer.getTexture(1), 2);
    shader_lighting.setUniformTexture("texture_ssao", fbo_blur.getTexture(), 3);
    shader_lighting.setUniform3f("color_background", color_background.r / 255.0f, color_background.g / 255.0f, color_background.b / 255.0f);
    shader_lighting.setUniform3f("color_albedo", color_albedo.r / 255.0f, color_albedo.g / 255.0f, color_albedo.b / 255.0f);
    shader_lighting.setUniform1f("ambient", ambient);
    shader_lighting.setUniform1f("diffuse", diffuse);
    shader_lighting.setUniform3f("light_position", light_position_view);
    shader_lighting.setUniform1i("use_ssao", render_mode != render_mode_lighting);
    shader_lighting.setUniform1i("use_ambient_only", render_mode == render_mode_ambient_ssao);

    gbuffer.getTexture(0).draw(0.0f, 0.0f, w, h);

    shader_lighting.end();
  }

  ofEnableAlphaBlending();
}

// fonction qui place la scène au centre de la fenêtre, l'incline vers la caméra et la fait osciller autour de l'axe vertical
void Renderer::transform_scene()
{
  ofTranslate(ofGetWidth() / 2.0f, ofGetHeight() / 2.0f, 0.0f);
  ofRotateDeg(tilt_angle, 1.0f, 0.0f, 0.0f);
  ofRotateDeg(rotation_angle, 0.0f, 1.0f, 0.0f);
}

// tas de billes de dimensions variables
const int marble_count = 52;

const float marble[marble_count][4] =
{
  {-2.430f, 0.070f, 0.945f, 0.070f},
  {-1.502f, 0.070f, 1.629f, 0.070f},
  {-1.471f, 0.071f, 1.326f, 0.071f},
  {-2.174f, 0.071f, 0.587f, 0.071f},
  {-1.474f, 0.073f, 0.513f, 0.073f},
  {-1.329f, 0.079f, 1.561f, 0.079f},
  {-1.788f, 0.081f, 1.059f, 0.081f},
  {-2.256f, 0.082f, 1.620f, 0.082f},
  {-1.412f, 0.099f, 0.739f, 0.099f},
  {-2.401f, 0.099f, 1.458f, 0.099f},
  {-1.984f, 0.101f, 1.588f, 0.101f},
  {-1.823f, 0.111f, 0.266f, 0.111f},
  {-1.275f, 0.134f, 0.487f, 0.134f},
  {-0.921f, 0.140f, 1.580f, 0.140f},
  {-1.160f, 0.141f, 1.429f, 0.141f},
  {-1.591f, 0.144f, 0.151f, 0.144f},
  {-2.337f, 0.161f, 0.753f, 0.161f},
  {-1.467f, 0.176f, 1.012f, 0.176f},
  {-1.444f, 0.191f, 0.594f, 0.073f},
  {-0.713f, 0.202f, 1.313f, 0.202f},
  {-1.723f, 0.209f, 1.455f, 0.209f},
  {-2.252f, 0.215f, 0.350f, 0.215f},
  {-1.401f, 0.222f, 1.450f, 0.114f},
  {-1.722f, 0.249f, 1.133f, 0.115f},
  {-1.254f, 0.287f, 0.722f, 0.147f},
  {-2.144f, 0.290f, 1.165f, 0.290f},
  {-1.810f, 0.300f, 0.678f, 0.300f},
  {-2.164f, 0.311f, 0.801f, 0.073f},
  {-1.457f, 0.324f, 0.808f, 0.076f},
  {-1.203f, 0.326f, 1.181f, 0.170f},
  {-1.442f, 0.337f, 0.615f, 0.075f},
  {-1.760f, 0.351f, 0.246f, 0.137f},
  {-1.452f, 0.369f, 0.352f, 0.187f},
  {-1.205f, 0.425f, 0.925f, 0.103f},
  {-1.798f, 0.437f, 1.055f, 0.102f},
  {-2.113f, 0.441f, 0.837f, 0.071f},
  {-2.336f, 0.484f, 0.779f, 0.164f},
  {-2.011f, 0.516f, 0.910f, 0.075f},
  {-2.346f, 0.552f, 0.468f, 0.154f},
  {-2.019f, 0.552f, 0.306f, 0.195f},
  {-1.513f, 0.565f, 1.004f, 0.215f},
  {-1.797f, 0.572f, 0.937f, 0.076f},
  {-2.093f, 0.578f, 0.739f, 0.100f},
  {-1.928f, 0.579f, 1.041f, 0.091f},
  {-2.159f, 0.582f, 0.927f, 0.087f},
  {-1.790f, 0.612f, 1.334f, 0.217f},
  {-1.944f, 0.617f, 0.823f, 0.074f},
  {-2.081f, 0.631f, 1.059f, 0.072f},
  {-2.133f, 0.653f, 0.559f, 0.099f},
  {-1.779f, 0.663f, 1.051f, 0.070f},
  {-2.329f, 0.705f, 1.078f, 0.171f},
  {-2.070f, 0.753f, 0.866f, 0.116f}
};

// fonction qui dessine la scène : un coin de pièce (plancher et deux murs) avec des objets posés sur le plancher,
// pour créer des coins, des creux et des points de contact où l'occlusion ambiante est visible
void Renderer::draw_scene()
{
  float u = unit;

  // plancher
  ofDrawBox(0.0f, u + 0.05f * u, 0.0f, 5.0f * u, 0.1f * u, 3.5f * u);

  // mur du fond
  ofDrawBox(0.0f, -0.25f * u, -1.8f * u, 5.0f * u, 2.5f * u, 0.1f * u);

  // mur de gauche
  ofDrawBox(-2.55f * u, -0.25f * u, 0.0f, 0.1f * u, 2.5f * u, 3.5f * u);

  // teapot dans le coin du fond à gauche
  teapot.setScale(teapot_scale, teapot_scale, teapot_scale);
  teapot.setPosition(-1.34f * u, u, -0.84f * u);
  teapot.setRotation(0, 30.0f, 0.0f, 1.0f, 0.0f);
  teapot.draw(OF_MESH_FILL);

  // pile de deux cubes contre le mur du fond, à droite
  ofDrawBox(1.3f * u, 0.5f * u, -1.2f * u, u, u, u);

  ofPushMatrix();
  ofTranslate(1.3f * u, -0.3f * u, -1.2f * u);
  ofRotateDeg(25.0f, 0.0f, 1.0f, 0.0f);
  ofDrawBox(0.0f, 0.0f, 0.0f, 0.6f * u);
  ofPopMatrix();

  // pyramide de 6 cubes alignés sur 3 étages (3, 2 et 1 cubes) posée sur le plancher, à l'avant à droite
  float cube_size = 0.4f * u;
  float cube_gap = 0.4f * cube_size;
  float pyramid_x = 1.1f * u;
  float pyramid_z = 1.3f * u;

  for (int level = 0; level < 3; ++level)
  {
    int cube_count = 3 - level;
    float y = u - cube_size * (level + 0.5f);

    for (int i = 0; i < cube_count; ++i)
    {
      float x = pyramid_x + (cube_size + cube_gap) * (i - (cube_count - 1) / 2.0f);
      ofDrawBox(x, y, pyramid_z, cube_size, cube_size, cube_size);
    }
  }

  // cube seul, posé sur le plancher au milieu de l'espace vide à droite
  ofPushMatrix();
  ofTranslate(1.6f * u, u - 0.275f * u, 0.2f * u);
  ofRotateDeg(17.0f, 0.0f, 1.0f, 0.0f);
  ofDrawBox(0.0f, 0.0f, 0.0f, 0.55f * u);
  ofPopMatrix();

  // petit cube contre le mur du fond, au centre
  ofDrawBox(0.2f * u, 0.75f * u, -1.5f * u, 0.5f * u, 0.5f * u, 0.5f * u);

  // sphère posée sur le plancher, au centre
  ofDrawSphere(0.2f * u, 0.5f * u, 0.4f * u, 0.5f * u);

  // tas de billes
  for (int i = 0; i < marble_count; ++i)
    ofDrawSphere(marble[i][0] * u, u - marble[i][1] * u, marble[i][2] * u, marble[i][3] * u);

  // petite sphère posée sur le plancher, à gauche
  ofDrawSphere(-0.9f * u, 0.75f * u, 0.9f * u, 0.25f * u);
}

// fonction qui dessine une couche du g-buffer (0 : position, 1 : normale) dans un rectangle de la fenêtre
void Renderer::draw_gbuffer_layer(int layer, float x, float y, float width, float height)
{
  shader_gbuffer_view.begin();
  shader_gbuffer_view.setUniformTexture("gbuffer_position", gbuffer.getTexture(0), 1);
  shader_gbuffer_view.setUniformTexture("gbuffer_normal", gbuffer.getTexture(1), 2);
  shader_gbuffer_view.setUniform1i("layer", layer);
  shader_gbuffer_view.setUniform1f("position_scale", std::max(ofGetWidth(), ofGetHeight()));

  gbuffer.getTexture(0).draw(x, y, width, height);

  shader_gbuffer_view.end();
}
