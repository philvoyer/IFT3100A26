// IFT3100A26_CameraProjection/main.cpp
// Exemple de rendu d'un teapot en rotation vu par une caméra en perspective, avec l'affichage à l'écran de la matrice de modèle-vue et de la matrice de projection.
// Les touches 1, 2 et 3 sélectionnent le shader (Lambert, normales ou profondeur) et la touche 4 alterne entre une instance d'ofCamera et la caméra par défaut d'openFrameworks.
// Les flèches déplacent le teapot sur les axes X et Z, la touche R active ou désactive sa rotation et la barre d'espace écrit l'état de la caméra et ses matrices dans la console.
// Une interface permet d'ajuster les couleurs (arrière-plan, ambiante et diffuse), les plans d'occlusion avant et arrière (near et far), l'angle du champ de vision vertical et le facteur de zoom (recalculés l'un par rapport à l'autre).

#include "ofMain.h"
#include "application.h"

int main()
{
  // paramètres du contexte de rendu
  ofGLFWWindowSettings windowSettings;

  // résolution de la fenêtre d'affichage
  windowSettings.setSize(960, 540);

  // sélection de la version de OpenGL
  windowSettings.setGLVersion(3, 3);

  // création de la fenêtre
  ofCreateWindow(windowSettings);

  // démarrer l'exécution de l'application
  ofRunApp(new Application());
}
