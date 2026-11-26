// IFT3100A26_RenderArchitecture/renderer.cpp
// Classe responsable du rendu de l'application.

#include "renderer.h"

// fonction qui retourne le nom d'un format interne de texture
const char* gl_format_name(GLint format)
{
  switch (format)
  {
    case GL_RGBA8:              return "GL_RGBA8";
    case GL_RGBA16F:            return "GL_RGBA16F";
    case GL_RGBA32F:            return "GL_RGBA32F";
    case GL_DEPTH_COMPONENT16:  return "GL_DEPTH_COMPONENT16";
    case GL_DEPTH_COMPONENT24:  return "GL_DEPTH_COMPONENT24";
    case GL_DEPTH_COMPONENT32F: return "GL_DEPTH_COMPONENT32F";
    default:                    return "unknown format";
  }
}

// fonction qui retourne le nombre d'octets par pixel d'un format interne de texture
// (une profondeur de 24 bits est généralement stockée sur 32 bits)
int gl_format_bytes(GLint format)
{
  switch (format)
  {
    case GL_RGBA8:              return 4;
    case GL_RGBA16F:            return 8;
    case GL_RGBA32F:            return 16;
    case GL_DEPTH_COMPONENT16:  return 2;
    case GL_DEPTH_COMPONENT24:  return 4;
    case GL_DEPTH_COMPONENT32F: return 4;
    default:                    return 0;
  }
}

void Renderer::setup()
{
  ofSetFrameRate(60);
  ofSetSphereResolution(32);

  // utiliser des textures GL_TEXTURE_2D avec des coordonnées de texture normalisées (entre 0 et 1)
  ofDisableArbTex();

  // mode de rendu au lancement de l'application
  render_mode = RenderMode::forward;

  // couleurs des objets (les mêmes que celles des surfaces de l'exemple 9.1)
  color_triangle = ofColor(255, 0, 255);
  color_cube = ofColor(255, 127, 0);
  color_sphere = ofColor(0, 255, 127);
  color_teapot = ofColor(0, 127, 255);

  color_background = ofColor(31, 31, 31);

  // paramètres du modèle d'illumination
  ambient = 0.1f;
  brightness = 32.0f;
  specular = 0.5f;

  // couleur des quatre lumières
  light_color[0] = glm::vec3(0.6f, 0.6f, 0.6f);
  light_color[1] = glm::vec3(0.6f, 0.3f, 0.3f);
  light_color[2] = glm::vec3(0.3f, 0.6f, 0.3f);
  light_color[3] = glm::vec3(0.3f, 0.3f, 0.6f);

  // mouvement des lumières (en degrés par seconde)
  light_orbit_angle = 0.0f;
  light_orbit_speed = 20.0f;
  use_light_motion = true;

  // rotation des objets (en degrés par seconde)
  rotation_angle = 0.0f;
  rotation_speed = 20.0f;
  use_rotation = true;

  // temps des oscillations
  oscillation_time = 0.0f;

  // paramètres des oscillations, avec des périodes dans un rapport de 5 à 7 (en secondes)
  triangle_oscillation_period_x = 5.0f;
  triangle_oscillation_period_y = 7.0f;
  triangle_oscillation_amplitude = 35.0f;

  cube_oscillation_period_x = 4.0f;
  cube_oscillation_period_z = 5.6f;
  cube_oscillation_amplitude_x = 0.3f;
  cube_oscillation_amplitude_z = 0.35f;

  teapot_oscillation_period_tilt = 6.0f;
  teapot_oscillation_period_y = 8.4f;
  teapot_oscillation_amplitude_tilt = 20.0f;
  teapot_oscillation_amplitude_y = 0.15f;

  sphere_oscillation_period_x = 3.0f;
  sphere_oscillation_period_y = 4.2f;
  sphere_oscillation_amplitude = 0.25f;

  // triangle équilatéral sur un cercle de rayon 1, avec une normale vers la caméra
  triangle.setMode(OF_PRIMITIVE_TRIANGLES);
  triangle.addVertex(glm::vec3(0.0f, -1.0f, 0.0f));
  triangle.addVertex(glm::vec3(cosf(glm::radians(30.0f)), 0.5f, 0.0f));
  triangle.addVertex(glm::vec3(-cosf(glm::radians(30.0f)), 0.5f, 0.0f));
  triangle.addNormal(glm::vec3(0.0f, 0.0f, 1.0f));
  triangle.addNormal(glm::vec3(0.0f, 0.0f, 1.0f));
  triangle.addNormal(glm::vec3(0.0f, 0.0f, 1.0f));

  // chargement du modèle (ofxAssimpModelLoader ajuste sa dimension à celle de la fenêtre)
  teapot.load("teapot.obj");
  teapot.disableMaterials();
  teapot_scale = 0.725f;

  // chargement des polices de caractères
  font.load("Consolas Bold.ttf", 14, true, true);
  font_info.load("Consolas Bold.ttf", 10, true, true);

  // affichage des informations sur le g-buffer
  show_info = true;

  // chargement des shaders
  shader_forward.load("geometry_330_vs.glsl", "forward_330_fs.glsl");
  shader_gbuffer.load("geometry_330_vs.glsl", "gbuffer_330_fs.glsl");
  shader_deferred.load("screen_330_vs.glsl", "deferred_330_fs.glsl");
  shader_gbuffer_view.load("screen_330_vs.glsl", "gbuffer_view_330_fs.glsl");

  // allocation du g-buffer
  allocate_gbuffer(ofGetWidth(), ofGetHeight());
}

// fonction qui alloue le g-buffer à la dimension de la fenêtre d'affichage
void Renderer::allocate_gbuffer(int width, int height)
{
  ofFboSettings settings;

  settings.width = width;
  settings.height = height;

  // trois buffers de rendu couleur en virgule flottante (position, normale et couleur)
  settings.numColorbuffers = 3;
  settings.internalformat = GL_RGBA32F;

  // buffer de profondeur accessible comme une texture (quatrième couche)
  settings.useDepth = true;
  settings.depthStencilAsTexture = true;

  // pas d'interpolation entre les texels
  settings.textureTarget = GL_TEXTURE_2D;
  settings.minFilter = GL_NEAREST;
  settings.maxFilter = GL_NEAREST;

  gbuffer.allocate(settings);

  ofLog() << "<allocate g-buffer: " << width << "x" << height << ">";
}

void Renderer::update()
{
  // nom du mode de rendu actif
  switch (render_mode)
  {
    case RenderMode::forward:
      render_mode_name = "forward rendering";
      break;

    case RenderMode::deferred:
      render_mode_name = "deferred shading";
      break;

    case RenderMode::layer_position:
      render_mode_name = "g-buffer layer: position";
      break;

    case RenderMode::layer_normal:
      render_mode_name = "g-buffer layer: normal";
      break;

    case RenderMode::layer_color:
      render_mode_name = "g-buffer layer: color";
      break;

    case RenderMode::layer_depth:
      render_mode_name = "g-buffer layer: depth";
      break;

    case RenderMode::layer_all:
      render_mode_name = "g-buffer layers: position, normal, color, depth";
      break;

    default:
      break;
  }

  float w = ofGetWidth();
  float h = ofGetHeight();

  // dimension des objets, relative à la fenêtre d'affichage
  object_size = std::min(w, h) / 3.5f;
  teapot_offset_y = object_size * 0.3625f;

  // centre de chaque case de la grille de 2x2 objets
  position_triangle = glm::vec3(w * 0.25f, h * 0.25f, 0.0f);
  position_cube = glm::vec3(w * 0.75f, h * 0.25f, 0.0f);
  position_sphere = glm::vec3(w * 0.25f, h * 0.75f, 0.0f);
  position_teapot = glm::vec3(w * 0.75f, h * 0.75f, 0.0f);

  // plans d'occlusion de la caméra par défaut d'openFrameworks (champ de vision vertical de 60 degrés)
  camera_distance = (h / 2.0f) / tanf(glm::radians(30.0f));
  clip_near = camera_distance / 10.0f;
  clip_far = camera_distance * 10.0f;

  // rotation des objets
  if (use_rotation)
    rotation_angle = fmodf(rotation_angle + rotation_speed * ofGetLastFrameTime(), 360.0f);

  // temps des oscillations
  if (use_rotation)
    oscillation_time += ofGetLastFrameTime();

  // inclinaison du triangle autour des axes x et y (courbe de Lissajous)
  triangle_tilt_x = oscillate(oscillation_time, triangle_oscillation_amplitude, triangle_oscillation_period_x);
  triangle_tilt_y = oscillate(oscillation_time, triangle_oscillation_amplitude, triangle_oscillation_period_y);

  // déplacement du cube dans le plan horizontal
  cube_offset = glm::vec3(
    oscillate(oscillation_time, cube_oscillation_amplitude_x * object_size, cube_oscillation_period_x),
    0.0f,
    oscillate(oscillation_time, cube_oscillation_amplitude_z * object_size, cube_oscillation_period_z));

  // tangage et mouvement vertical du teapot
  teapot_tilt = oscillate(oscillation_time, teapot_oscillation_amplitude_tilt, teapot_oscillation_period_tilt);
  teapot_bob = oscillate(oscillation_time, teapot_oscillation_amplitude_y * object_size, teapot_oscillation_period_y);

  // étirement et écrasement de la sphère (stretch & squash), avec un volume constant (x * y * z = 1)
  sphere_scale_x = 1.0f + oscillate(oscillation_time, sphere_oscillation_amplitude, sphere_oscillation_period_x);
  sphere_scale_y = 1.0f + oscillate(oscillation_time, sphere_oscillation_amplitude, sphere_oscillation_period_y);
  sphere_scale_z = 1.0f / (sphere_scale_x * sphere_scale_y);

  // rotation des lumières autour du centre de la scène
  if (use_light_motion)
    light_orbit_angle = fmodf(light_orbit_angle + light_orbit_speed * ofGetLastFrameTime(), 360.0f);

  light_orbit_radius = std::min(w, h) * 0.45f;
  light_distance_z = object_size * 2.0f;

  for (int i = 0; i < light_count; ++i)
  {
    float angle = glm::radians(light_orbit_angle + i * 360.0f / light_count);

    light_position_world[i] = glm::vec3(
      w / 2.0f + light_orbit_radius * cosf(angle),
      h / 2.0f + light_orbit_radius * sinf(angle),
      light_distance_z);
  }
}

void Renderer::draw()
{
  // position des lumières dans l'espace de vue
  glm::mat4 matrix_view = ofGetCurrentViewMatrix();

  for (int i = 0; i < light_count; ++i)
    light_position_view[i] = glm::vec3(matrix_view * glm::vec4(light_position_world[i], 1.0f));

  if (render_mode == RenderMode::forward)
  {
    // rendu immédiat : éclairer chaque objet au moment où il est dessiné
    ofSetBackgroundColor(color_background);

    ofEnableDepthTest();

    shader_forward.begin();
    shader_forward.setUniform1f("ambient", ambient);
    shader_forward.setUniform1f("brightness", brightness);
    shader_forward.setUniform1f("specular", specular);
    set_light_uniforms(shader_forward);

    draw_scene(shader_forward);

    shader_forward.end();

    ofDisableDepthTest();
  }
  else
  {
    // shading en différé, passe géométrie : rendre les attributs des surfaces visibles dans le g-buffer
    ofDisableAlphaBlending();

    gbuffer.begin();

    // écrire dans les trois buffers de rendu couleur en même temps (MRT)
    gbuffer.activateAllDrawBuffers();

    // vider les couches du g-buffer
    ofClear(0, 0, 0, 0);

    ofEnableDepthTest();

    shader_gbuffer.begin();
    draw_scene(shader_gbuffer);
    shader_gbuffer.end();

    ofDisableDepthTest();

    gbuffer.end();

    float w = ofGetWidth();
    float h = ofGetHeight();

    if (render_mode == RenderMode::deferred)
    {
      // shading en différé, passe éclairage : évaluer le modèle d'illumination une fois par pixel
      shader_deferred.begin();
      shader_deferred.setUniformTexture("gbuffer_position", gbuffer.getTexture(0), 1);
      shader_deferred.setUniformTexture("gbuffer_normal", gbuffer.getTexture(1), 2);
      shader_deferred.setUniformTexture("gbuffer_color", gbuffer.getTexture(2), 3);
      shader_deferred.setUniform3f("color_background", color_background.r / 255.0f, color_background.g / 255.0f, color_background.b / 255.0f);
      shader_deferred.setUniform1f("ambient", ambient);
      shader_deferred.setUniform1f("brightness", brightness);
      shader_deferred.setUniform1f("specular", specular);
      set_light_uniforms(shader_deferred);

      // dessiner un quadrilatère en pleine fenêtre
      gbuffer.getTexture(0).draw(0.0f, 0.0f, w, h);

      shader_deferred.end();
    }
    else if (render_mode == RenderMode::layer_all)
    {
      // dessiner les quatre couches du g-buffer dans une grille de 2x2
      draw_gbuffer_layer(0, 0.0f, 0.0f, w / 2.0f, h / 2.0f);
      draw_gbuffer_layer(1, w / 2.0f, 0.0f, w / 2.0f, h / 2.0f);
      draw_gbuffer_layer(2, 0.0f, h / 2.0f, w / 2.0f, h / 2.0f);
      draw_gbuffer_layer(3, w / 2.0f, h / 2.0f, w / 2.0f, h / 2.0f);
    }
    else
    {
      // dessiner une seule couche du g-buffer en pleine fenêtre
      draw_gbuffer_layer((int) render_mode - (int) RenderMode::layer_position, 0.0f, 0.0f, w, h);
    }

    ofEnableAlphaBlending();
  }

  // dessiner l'interface par-dessus la scène
  draw_interface();
}

// fonction qui dessine les quatre objets de la scène avec un shader
void Renderer::draw_scene(ofShader& shader)
{
  ofPushMatrix();

    // positionner le triangle
    ofTranslate(position_triangle);

    // inclinaison autour des axes x et y
    ofRotateDeg(triangle_tilt_x, 1.0f, 0.0f, 0.0f);
    ofRotateDeg(triangle_tilt_y, 0.0f, 1.0f, 0.0f);

    // rotation dans son plan
    ofRotateDeg(rotation_angle, 0.0f, 0.0f, 1.0f);

    // dimension du triangle
    ofScale(object_size * 0.638f, object_size * 0.638f, object_size * 0.638f);

    // couleur du triangle
    shader.setUniform3f("color_albedo", color_triangle.r / 255.0f, color_triangle.g / 255.0f, color_triangle.b / 255.0f);

    // dessiner un triangle
    triangle.draw();

  ofPopMatrix();

  ofPushMatrix();

    // positionner le cube
    ofTranslate(position_cube + cube_offset);

    // rotation autour de l'axe vertical
    ofRotateDeg(rotation_angle, 0.0f, 1.0f, 0.0f);

    // rotation locale
    ofRotateDeg(45.0f, 1.0f, 0.0f, 0.0f);
    ofRotateDeg(45.0f, 0.0f, 1.0f, 0.0f);

    // couleur du cube
    shader.setUniform3f("color_albedo", color_cube.r / 255.0f, color_cube.g / 255.0f, color_cube.b / 255.0f);

    // dessiner un cube
    ofDrawBox(0.0f, 0.0f, 0.0f, object_size * 0.7f);

  ofPopMatrix();

  ofPushMatrix();

    // positionner la sphère
    ofTranslate(position_sphere);

    // couleur de la sphère
    shader.setUniform3f("color_albedo", color_sphere.r / 255.0f, color_sphere.g / 255.0f, color_sphere.b / 255.0f);

    // étirement et écrasement de la sphère
    ofScale(sphere_scale_x, sphere_scale_y, sphere_scale_z);

    // dessiner une sphère
    ofDrawSphere(0.0f, 0.0f, 0.0f, object_size * 0.494f);

  ofPopMatrix();

  ofPushMatrix();

    // positionner le teapot
    ofTranslate(position_teapot + glm::vec3(0.0f, teapot_bob, 0.0f));

    // tangage dans le plan de la fenêtre
    ofRotateDeg(teapot_tilt, 0.0f, 0.0f, 1.0f);

    // couleur du teapot
    shader.setUniform3f("color_albedo", color_teapot.r / 255.0f, color_teapot.g / 255.0f, color_teapot.b / 255.0f);

    // dimension du teapot
    teapot.setScale(teapot_scale, teapot_scale, teapot_scale);

    // centrer le teapot dans sa case (l'origine du modèle est à sa base)
    teapot.setPosition(0.0f, teapot_offset_y, 0.0f);

    // rotation autour de l'axe vertical, dans le sens inverse du cube
    teapot.setRotation(0, -rotation_angle, 0.0f, 1.0f, 0.0f);

    // dessiner un teapot
    teapot.draw(OF_MESH_FILL);

  ofPopMatrix();
}

// fonction qui dessine une couche du g-buffer dans un rectangle
void Renderer::draw_gbuffer_layer(int layer, float x, float y, float width, float height)
{
  shader_gbuffer_view.begin();
  shader_gbuffer_view.setUniformTexture("gbuffer_position", gbuffer.getTexture(0), 1);
  shader_gbuffer_view.setUniformTexture("gbuffer_normal", gbuffer.getTexture(1), 2);
  shader_gbuffer_view.setUniformTexture("gbuffer_color", gbuffer.getTexture(2), 3);
  shader_gbuffer_view.setUniformTexture("gbuffer_depth", gbuffer.getDepthTexture(), 4);
  shader_gbuffer_view.setUniform1i("layer", layer);
  shader_gbuffer_view.setUniform1f("position_scale", std::max(ofGetWidth(), ofGetHeight()));
  shader_gbuffer_view.setUniform1f("clip_near", clip_near);
  shader_gbuffer_view.setUniform1f("clip_far", clip_far);
  shader_gbuffer_view.setUniform1f("depth_min", camera_distance - object_size);
  shader_gbuffer_view.setUniform1f("depth_max", camera_distance + object_size);

  gbuffer.getTexture(0).draw(x, y, width, height);

  shader_gbuffer_view.end();
}

// fonction qui dessine l'interface : mode de rendu, étiquettes et informations sur le g-buffer
void Renderer::draw_interface()
{
  float w = ofGetWidth();
  float h = ofGetHeight();
  float margin = 24.0f;

  ofSetColor(255);

  // mode de rendu actif
  font.drawString(std::to_string((int) render_mode + 1) + "  " + render_mode_name, margin, margin);

  if (render_mode == RenderMode::layer_all)
  {
    // nom de chaque couche du g-buffer
    draw_label("position", w * 0.25f, h / 2.0f - margin / 2.0f);
    draw_label("normal", w * 0.75f, h / 2.0f - margin / 2.0f);
    draw_label("color", w * 0.25f, h - margin / 2.0f);
    draw_label("depth", w * 0.75f, h - margin / 2.0f);
  }
  else if (render_mode == RenderMode::forward || render_mode == RenderMode::deferred)
  {
    // numéro et nom de chaque objet
    draw_label("1.1 triangle", w * 0.25f, h / 2.0f - margin / 2.0f);
    draw_label("1.2 cube", w * 0.75f, h / 2.0f - margin / 2.0f);
    draw_label("2.1 sphere", w * 0.25f, h - margin / 2.0f);
    draw_label("2.2 teapot", w * 0.75f, h - margin / 2.0f);
  }

  // informations sur le g-buffer
  if (show_info)
    draw_gbuffer_info(margin, margin * 1.75f);
}

// fonction qui dessine une étiquette centrée horizontalement
// (le texte est dessiné deux fois avec un décalage d'un pixel pour un trait plus épais)
void Renderer::draw_label(const std::string& text, float x, float y)
{
  float x_left = roundf(x - font.stringWidth(text) / 2.0f);

  font.drawString(text, x_left, y);
  font.drawString(text, x_left + 1.0f, y);
}

// fonction qui affiche la résolution, le format et le poids en mémoire de chaque couche du g-buffer
void Renderer::draw_gbuffer_info(float x, float y)
{
  const int line_count = 9;
  std::string line[line_count];
  int n = 0;

  if (render_mode == RenderMode::forward)
  {
    line[n++] = "forward rendering: no g-buffer";
    line[n++] = "each fragment is lit directly in the window framebuffer";
  }
  else
  {
    int width = gbuffer.getWidth();
    int height = gbuffer.getHeight();
    int pixel_count = width * height;

    // format interne de chaque couche
    GLint format[4];
    for (int i = 0; i < 3; ++i)
      format[i] = gbuffer.getTexture(i).getTextureData().glInternalFormat;
    format[3] = gbuffer.getDepthTexture().getTextureData().glInternalFormat;

    // contenu et espace de chaque couche
    const char* const layer_name[4] = {"position", "normal  ", "color   ", "depth   "};
    const char* const layer_content[4] =
    {
      "view space xyz, w = 1  linear",
      "view space xyz, w = 1  linear",
      "albedo rgb, a = 1      linear",
      "window z in [0, 1]     non-linear"
    };

    int bytes_per_pixel = 0;

    line[n++] = "g-buffer  " + ofToString(width) + " x " + ofToString(height) + " = " + ofToString(pixel_count) + " pixels";
    line[n++] = "layer     format                 bytes  size      content                space";

    for (int i = 0; i < 4; ++i)
    {
      int bytes = gl_format_bytes(format[i]);
      bytes_per_pixel += bytes;

      std::string name = gl_format_name(format[i]);
      name.resize(22, ' ');

      // poids en mémoire de la couche (1 MiB = 1024 x 1024 octets)
      float layer_mebibytes = pixel_count * (float) bytes / (1024.0f * 1024.0f);

      line[n++] = std::string(layer_name[i]) + "  " + name + " " + ofToString(bytes, 2, ' ') + "     " +
        ofToString(layer_mebibytes, 1, 4, ' ') + " MiB  " + layer_content[i];
    }

    float mebibytes = pixel_count * (float) bytes_per_pixel / (1024.0f * 1024.0f);

    line[n++] = "total     " + ofToString(bytes_per_pixel) + " bytes/pixel x " + ofToString(pixel_count) + " pixels = " + ofToString(mebibytes, 1) + " MiB";
  }

  // fond semi-transparent derrière le texte
  float line_height = font_info.getLineHeight();
  float width_max = 0.0f;

  for (int i = 0; i < n; ++i)
    width_max = std::max(width_max, font_info.stringWidth(line[i]));

  float padding = 8.0f;

  ofSetColor(0, 0, 0, 160);
  ofDrawRectangle(x - padding, y - padding, width_max + padding * 2.0f, line_height * n + padding * 2.0f);

  ofSetColor(255);

  for (int i = 0; i < n; ++i)
    font_info.drawString(line[i], x, y + line_height * (i + 0.8f));
}

// fonction qui passe la position et la couleur des lumières à un shader
void Renderer::set_light_uniforms(ofShader& shader)
{
  shader.setUniform3fv("light_position", &light_position_view[0].x, light_count);
  shader.setUniform3fv("light_color", &light_color[0].x, light_count);
}

// fonction d'oscillation x(t) = amplitude * sin(2 * PI * t / période)
float Renderer::oscillate(float time, float amplitude, float period)
{
  return amplitude * sin(2.0 * PI * time / period);
}
