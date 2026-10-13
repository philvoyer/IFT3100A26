// IFT3100A26_CameraFrustum/main.cpp
// Exemple de visualisation en vue de côté du frustum de vision d'une caméra en perspective.
// La position de la caméra, son point de visée, les plans d'occlusion avant et arrière et leurs intersections avec la ligne de vue et le champ de vision vertical sont marqués d'un point.
// Un panneau permet d'ajuster les plans d'occlusion avant et arrière (near et far), l'angle du champ de vision vertical et le facteur de zoom (recalculés l'un par rapport à l'autre), la résolution de l'image de la caméra ainsi que les couleurs de contour et de remplissage.
// Le panneau affiche aussi des valeurs calculées (hauteur des plans d'occlusion, ratio d'aspect et angle du champ de vision horizontal) et le frustum est dessiné en rouge lorsque ses attributs sont invalides. La barre d'espace écrit dans la console les matrices de vue, de caméra, de projection, de modèle-vue et de modèle-vue-projection.

#include "ofMain.h"
#include "application.h"

int main()
{
  ofSetupOpenGL(800, 600, OF_WINDOW);
  ofRunApp(new Application());
}
