// IFT3100A26_SSAO/application.cpp
// Classe principale de l'application.

#include "application.h"

void Application::setup()
{
  ofLog() << "<app::setup>";

  renderer.setup();

  font.load("Consolas Bold.ttf", 14, true, true);

  // limite de 60 images par seconde (ofSetFrameRate dans Renderer::setup)
  use_frame_limit = true;

  update_window_title();
}

void Application::update()
{
  renderer.update();
}

void Application::draw()
{
  renderer.draw();

  float w = ofGetWidth();
  float h = ofGetHeight();
  float margin = 24.0f;
  float line = 22.0f;

  if (renderer.render_mode == render_mode_layers)
  {
    // nom de chaque couche du SSAO
    draw_label("position", w * 0.25f, h / 2.0f - margin / 2.0f);
    draw_label("normal", w * 0.75f, h / 2.0f - margin / 2.0f);
    draw_label("ssao raw", w * 0.25f, h - margin / 2.0f);
    draw_label("ssao blur", w * 0.75f, h - margin / 2.0f);
  }

  // mode de rendu actif et paramètres du SSAO
  std::string text_mode = std::to_string(renderer.render_mode + 1) + "  " + render_mode_name[renderer.render_mode];
  std::string text_kernel = std::string("kernel   ") + (renderer.use_hemisphere ? "hemisphere (normal)" : "sphere (crytek)");
  std::string text_samples = "samples  " + std::to_string(renderer.sample_count);
  std::string text_radius = "radius   " + ofToString(renderer.radius_factor, 2);

  // nombre d'images par seconde, pour voir le coût du nombre d'échantillons
  // (avec la limite, il reste à 60 tant que le rendu prend moins de 16 ms ; sans la limite, il montre le vrai coût)
  std::string text_fps = "fps      " + ofToString(ofGetFrameRate(), 0) + (use_frame_limit ? " (limited, v to unlock)" : " (unlimited)");

  // fond semi-transparent pour que le texte blanc reste lisible, entre autres sur le facteur d'occlusion ambiante
  // des modes 4, 5 et 6, qui est blanc là où il n'y a aucune occlusion (et partout où aucune surface ne couvre la fenêtre)
  float width_max = std::max(std::max(font.stringWidth(text_mode), font.stringWidth(text_kernel)),
                             std::max(font.stringWidth(text_samples), font.stringWidth(text_radius)));
  width_max = std::max(width_max, font.stringWidth(text_fps));
  float padding = 8.0f;

  ofSetColor(0, 0, 0, 160);
  ofDrawRectangle(margin - padding, margin - line * 0.8f - padding, width_max + padding * 2.0f, line * 5.3f + padding * 2.0f);

  ofSetColor(255);

  font.drawString(text_mode, margin, margin);
  font.drawString(text_kernel, margin, margin + line * 1.5f);
  font.drawString(text_samples, margin, margin + line * 2.5f);
  font.drawString(text_radius, margin, margin + line * 3.5f);
  font.drawString(text_fps, margin, margin + line * 4.5f);
}

void Application::keyReleased(int key)
{
  switch (key)
  {
    case 49: // touche 1 (éclairage avec occlusion ambiante)
    case 50: // touche 2 (éclairage sans occlusion ambiante)
    case 51: // touche 3 (réflexion ambiante seulement, avec occlusion ambiante)
    case 52: // touche 4 (facteur d'occlusion ambiante, avant le flou)
    case 53: // touche 5 (facteur d'occlusion ambiante, après le flou)
    case 54: // touche 6 (couches du SSAO : position, normale, facteur avant et après le flou)
      renderer.render_mode = key - 49;
      update_window_title();
      ofLog() << "<render mode: " << render_mode_name[renderer.render_mode] << ">";
      break;

    case 104: // touche h (alterner entre un noyau en hémisphère et un noyau en sphère)
      renderer.use_hemisphere = !renderer.use_hemisphere;
      renderer.generate_kernel();
      break;

    case OF_KEY_UP: // touche ↑ (agrandir le rayon de la zone d'échantillonnage)
      renderer.radius_factor = std::min(renderer.radius_factor * 1.25f, 2.0f);
      ofLog() << "<radius: " << renderer.radius_factor << ">";
      break;

    case OF_KEY_DOWN: // touche ↓ (réduire le rayon de la zone d'échantillonnage)
      renderer.radius_factor = std::max(renderer.radius_factor / 1.25f, 0.05f);
      ofLog() << "<radius: " << renderer.radius_factor << ">";
      break;

    case OF_KEY_RIGHT: // touche → (doubler le nombre d'échantillons, jusqu'à 128)
      renderer.sample_count = std::min(renderer.sample_count * 2, sample_count_max);
      renderer.generate_kernel();
      break;

    case OF_KEY_LEFT: // touche ← (diviser le nombre d'échantillons par deux, jusqu'à 4)
      renderer.sample_count = std::max(renderer.sample_count / 2, 4);
      renderer.generate_kernel();
      break;

    case 108: // touche l (activer ou désactiver le mouvement de la lumière)
      renderer.use_light_motion = !renderer.use_light_motion;
      ofLog() << "<light motion: " << (renderer.use_light_motion ? "on" : "off") << ">";
      break;

    case 118: // touche v (activer ou désactiver la limite du nombre d'images par seconde)
      use_frame_limit = !use_frame_limit;
      apply_frame_limit();
      break;

    case 114: // touche r (activer ou désactiver l'oscillation de la scène)
      renderer.use_rotation = !renderer.use_rotation;
      ofLog() << "<rotation: " << (renderer.use_rotation ? "on" : "off") << ">";
      break;

    default:
      break;
  }
}

void Application::windowResized(int w, int h)
{
  // les framebuffers doivent avoir la même dimension que la fenêtre d'affichage
  renderer.allocate_framebuffers(w, h);

  ofLog() << "<app::windowResized: (" << w << ", " << h << ")>";
}

// fonction qui active ou désactive la limite du nombre d'images par seconde
// (sans la limite, l'application dessine aussi vite que possible : le nombre d'images par seconde montre alors le vrai coût du rendu,
// mais le processeur graphique travaille à plein régime et des déchirures d'image peuvent apparaître sans la synchronisation verticale)
void Application::apply_frame_limit()
{
  if (use_frame_limit)
  {
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
  }
  else
  {
    ofSetFrameRate(0);
    ofSetVerticalSync(false);
  }

  ofLog() << "<frame limit: " << (use_frame_limit ? "on" : "off") << ">";
}

void Application::update_window_title()
{
  ofSetWindowTitle("ssao (123456 h ↑↓←→ l r v) " + std::string(render_mode_name[renderer.render_mode]));
}

void Application::exit()
{
  ofLog() << "<app::exit>";
}

// fonction qui affiche une étiquette centrée horizontalement, sur un fond semi-transparent
void Application::draw_label(const std::string& text, float x, float y)
{
  float x_left = roundf(x - font.stringWidth(text) / 2.0f);
  float padding = 4.0f;

  ofRectangle box = font.getStringBoundingBox(text, x_left, y);

  ofSetColor(0, 0, 0, 160);
  ofDrawRectangle(box.x - padding, box.y - padding, box.width + padding * 2.0f, box.height + padding * 2.0f);

  ofSetColor(255);
  font.drawString(text, x_left, y);
}
