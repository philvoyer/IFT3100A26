// IFT3100A26_CameraFrustum/renderer.cpp
// Classe responsable du rendu de l'application.

#include "renderer.h"

void Renderer::setup()
{
  ofSetFrameRate(60);
  ofSetBackgroundColor(31);

  reset();
}

void Renderer::reset()
{
  // paramètres de dessin
  circle_radius = 5.0f;
  stroke_width = 3.0f;
  color_invalid_stroke = ofColor(255, 0, 0);
  color_invalid_fill = ofColor(255, 0, 0, 127);

  // attributs de la caméra
  camera_position.z = 0.0f;
  camera_resolution_w = 1920.0f;
  camera_resolution_h = 1080.0f;
  camera_clip_n = 10.0f;
  camera_clip_f = 100.0f;
  camera_fov_v = 60.0f;
  camera_zoom = compute_zoom_from_fov(camera_fov_v);
}

void Renderer::update()
{
  // facteur de proportion du frustum par rapport à la fenêtre d'affichage
  scale_factor = ofGetHeight() * 0.005f;

  // calculer le ratio d'aspect à partir de la résolution de l'image de la caméra
  camera_aspect_ratio = camera_resolution_w / camera_resolution_h;

  // conversion de l'angle du champ de vision vertical en angle du champ de vision horizontal
  // l'angle du champ de vision de cet exemple est vertical, comme celui attendu par la fonction glm::perspective (fovy)
  // et comme celui qu'on voit dans la vue de côté du frustum,
  // alors que l'angle du champ de vision horizontal est seulement calculé pour être affiché dans le panneau,
  // les deux angles étant reliés par le ratio d'aspect : tan(fov_h / 2) = tan(fov_v / 2) * ratio d'aspect
  camera_fov_h = glm::degrees(2.0f * atanf(tanf(glm::radians(camera_fov_v) / 2.0f) * camera_aspect_ratio));

  // position de la caméra
  camera_position.x = ofGetWidth() / 2.0f;
  camera_position.y = ofGetHeight() / 8.0f;

  // calculer la position de l'intersection entre la ligne de vue et les plans d'occlusion avant et arrière
  position_clip_n_c.x = camera_position.x;
  position_clip_f_c.x = camera_position.x;
  position_clip_n_c.y = camera_position.y + camera_clip_n * scale_factor;
  position_clip_f_c.y = camera_position.y + camera_clip_f * scale_factor;

  // position vers laquelle la caméra est orientée
  camera_target.x = camera_position.x;
  camera_target.y = position_clip_f_c.y;
  camera_target.z = camera_position.z;

  // calculer la hauteur des plans d'occlusion avant et arrière
  clip_half_height_n = camera_clip_n * tanf(glm::radians(camera_fov_v / 2.0f));
  clip_half_height_f = camera_clip_f * tanf(glm::radians(camera_fov_v / 2.0f));
  clip_height_n = clip_half_height_n * 2.0f;
  clip_height_f = clip_half_height_f * 2.0f;

  // calculer la position de l'intersection entre le champ de vision et les plans d'occlusion avant et arrière
  position_clip_n_b.x = position_clip_n_c.x - clip_half_height_n * scale_factor;
  position_clip_n_b.y = position_clip_n_c.y;
  position_clip_n_t.x = position_clip_n_c.x + clip_half_height_n * scale_factor;
  position_clip_n_t.y = position_clip_n_c.y;
  position_clip_f_b.x = position_clip_f_c.x - clip_half_height_f * scale_factor;
  position_clip_f_b.y = position_clip_f_c.y;
  position_clip_f_t.x = position_clip_f_c.x + clip_half_height_f * scale_factor;
  position_clip_f_t.y = position_clip_f_c.y;

  // ligne de contour du frustum
  camera_frustum_outline.clear();
  camera_frustum_outline.addVertex(position_clip_n_b.x, position_clip_n_b.y);
  camera_frustum_outline.addVertex(position_clip_n_t.x, position_clip_n_t.y);
  camera_frustum_outline.addVertex(position_clip_f_t.x, position_clip_f_t.y);
  camera_frustum_outline.addVertex(position_clip_f_b.x, position_clip_f_b.y);
  camera_frustum_outline.close();

  // valider les attributs de la caméra (le frustum est invalide si le champ de vision vaut 0 ou 180 degrés,
  // si le facteur de zoom est nul ou infini ou si le plan d'occlusion arrière n'est pas plus loin que le plan avant)
  is_frustum_valid = camera_fov_v > 0.0f && camera_fov_v < 180.0f && camera_zoom > 0.0f && std::isfinite(camera_zoom) && camera_clip_f > camera_clip_n;

  // générer des matrices à partir de l'état courant des attributs de caméra
  generate_matrix();
}

void Renderer::draw()
{
  // transformation
  ofPushMatrix();
  ofTranslate(0.0f, ofGetHeight(), 0.0f);
  ofScale(1.0f, -1.0f, 1.0f);

  // dessiner la zone de remplissage du frustum (en rouge si le frustum est invalide)
  ofSetColor(is_frustum_valid ? color_fill : color_invalid_fill);
  ofBeginShape();
  ofVertex(position_clip_n_b.x, position_clip_n_b.y);
  ofVertex(position_clip_n_t.x, position_clip_n_t.y);
  ofVertex(position_clip_f_t.x, position_clip_f_t.y);
  ofVertex(position_clip_f_b.x, position_clip_f_b.y);
  ofEndShape();

  // dessiner la ligne de contour du frustum (en rouge si le frustum est invalide)
  ofSetColor(is_frustum_valid ? color_stroke : color_invalid_stroke);
  ofSetLineWidth(stroke_width);
  camera_frustum_outline.draw();

  // dessiner les lignes du frustum
  ofSetLineWidth(1.0f);
  ofDrawLine(camera_position.x, camera_position.y, camera_position.x, position_clip_f_c.y + 32);
  ofDrawLine(camera_position.x, camera_position.y, position_clip_f_b.x, position_clip_f_c.y);
  ofDrawLine(camera_position.x, camera_position.y, position_clip_f_t.x, position_clip_f_t.y);
  ofDrawLine(position_clip_n_b.x, position_clip_n_b.y, position_clip_n_t.x, position_clip_n_t.y);
  ofDrawLine(position_clip_f_b.x, position_clip_f_b.y, position_clip_f_t.x, position_clip_f_t.y);

  // dessiner la position de la caméra
  ofDrawCircle(camera_position.x, camera_position.y, circle_radius);

  // dessiner la position observée par la caméra
  ofDrawCircle(camera_target.x, camera_target.y, circle_radius);

  // dessiner la position de l'intersection entre la ligne de vue et les plans d'occlusion avant et arrière
  ofDrawCircle(position_clip_n_c.x, position_clip_n_c.y, circle_radius * 0.618f);
  ofDrawCircle(position_clip_f_c.x, position_clip_f_c.y, circle_radius * 0.618f);

  // dessiner la position de l'intersection entre le champ de vision et les plans d'occlusion avant et arrière
  ofDrawCircle(position_clip_n_b.x, position_clip_n_c.y, circle_radius);
  ofDrawCircle(position_clip_n_t.x, position_clip_n_c.y, circle_radius);
  ofDrawCircle(position_clip_f_b.x, position_clip_f_c.y, circle_radius);
  ofDrawCircle(position_clip_f_t.x, position_clip_f_c.y, circle_radius);

  ofPopMatrix();
}

// fonction qui construit différentes matrices à partir de l'état courant des attributs de caméra
void Renderer::generate_matrix()
{
  // construire une matrice de modèle
  matrix_model = glm::mat4(1.0f);

  // vecteur qui indique le haut de la caméra
  // dans la vue de côté, la caméra regarde vers l'axe +Y de la fenêtre d'affichage, donc le haut de la caméra est dans le plan de la fenêtre (ici l'axe +X)
  glm::vec3 camera_up = glm::vec3(1.0f, 0.0f, 0.0f);

  // construire une matrice de vue à partir de la position de la caméra, de la position vers laquelle elle est orientée et de son vecteur haut
  matrix_view = glm::lookAt(camera_position, camera_target, camera_up);

  // la matrice de transformation de la caméra est l'inverse de la matrice de vue
  matrix_camera = glm::inverse(matrix_view);

  // construire une matrice de projection (la fonction glm::perspective attend un angle de champ de vision vertical)
  matrix_projection = glm::perspective(glm::radians(camera_fov_v), camera_aspect_ratio,  camera_clip_n, camera_clip_f);

  // construire une matrice de modèle-vue
  matrix_model_view = matrix_view * matrix_model;

  // construire une matrice de modèle-vue-projection
  matrix_model_view_projection = matrix_projection * matrix_view * matrix_model;

  // dans un rendu 3D, ces matrices seraient la source des attributs uniformes à passer à un shader de sommets
  // (p. ex. avec ofShader::setUniformMatrix4f) pour transformer les sommets de la scène
  // cet exemple n'en fait cependant aucun usage, car son rendu en 2D (vue de côté du frustum) n'a pas de lien avec ces matrices,
  // mais elles peuvent être écrites dans la console avec la barre d'espace pour observer l'effet des attributs de la caméra
}

// fonction qui écrit dans la console les matrices construites à partir de l'état courant des attributs de caméra
void Renderer::log_matrix()
{
  ofLog() << "matrix view:\n" << glm::to_string(matrix_view);
  ofLog() << "matrix camera:\n" << glm::to_string(matrix_camera);
  ofLog() << "matrix projection:\n" << glm::to_string(matrix_projection);
  ofLog() << "matrix model view:\n" << glm::to_string(matrix_model_view);
  ofLog() << "matrix model view projection:\n" << glm::to_string(matrix_model_view_projection);
}

// fonction qui calcule un angle de champ de vision à partir d'un facteur de zoom
float Renderer::compute_fov_from_zoom(float zoom)
{
  return glm::degrees(2.0f * atanf(1.0f / zoom));
}

// fonction qui calcule un facteur de zoom à partir d'un angle de champ de vision
float Renderer::compute_zoom_from_fov(float fov)
{
  return 1.0f / tanf(glm::radians(fov) / 2.0f);
}
