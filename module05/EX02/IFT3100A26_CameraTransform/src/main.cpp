// IFT3100A26_CameraTransform/main.cpp
// Exemple d'une scène de cubes colorés (7×7×7) observée par six caméras placées autour (front, back, left, right, top et bottom) et orientées vers le centre de la scène.
// Les touches 1 à 6 sélectionnent la caméra active et les touches O et P sélectionnent la projection orthogonale ou en perspective. Les caméras inactives peuvent être affichées avec leur gizmo, leur frustum de vision et leur nom.
// Pour les translations, les touches W/S et ↑/↓ font un dolly, A/D et ←/→ un truck et Q/E un boom. Pour les rotations, les touches U/J font un tangage (tilt), H/K un lacet (pan dans openFrameworks) et Y/I un roulis (roll). Les touches Z/X diminuent ou augmentent le champ de vision (entre 1 et 179 degrés).
// Les touches G, V, B, C et F activent ou désactivent la grille, les axes, les cubes, le damier et le remplissage, la touche T affiche ou masque les gizmos, frustums et noms des caméras inactives, et la touche R ou la barre d'espace réinitialise les caméras.

#include "ofMain.h"
#include "application.h"

int main()
{
  ofSetupOpenGL(512, 512, OF_WINDOW);
  ofRunApp(new Application());
}
