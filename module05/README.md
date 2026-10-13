# IFT3100A26

## Module 5 : Caméra

### Exemple 5.1 (CameraFrustum)

Exemple de visualisation en vue de côté du frustum de vision d'une caméra en perspective.

La position de la caméra, son point de visée, les plans d'occlusion avant et arrière et leurs intersections avec la ligne de vue et le champ de vision vertical sont marqués d'un point.

Un panneau permet d'ajuster les plans d'occlusion avant et arrière (near et far), l'angle du champ de vision vertical et le facteur de zoom (recalculés l'un par rapport à l'autre), la résolution de l'image de la caméra ainsi que les couleurs de contour et de remplissage.

Le panneau affiche aussi des valeurs calculées (hauteur des plans d'occlusion, ratio d'aspect et angle du champ de vision horizontal) et le frustum est dessiné en rouge lorsque ses attributs sont invalides. La barre d'espace écrit dans la console les matrices de vue, de caméra, de projection, de modèle-vue et de modèle-vue-projection.

### Exemple 5.2 (CameraTransform)

Exemple d'une scène de cubes colorés (7×7×7) observée par six caméras placées autour (front, back, left, right, top et bottom) et orientées vers le centre de la scène.

Les touches 1 à 6 sélectionnent la caméra active et les touches O et P sélectionnent la projection orthogonale ou en perspective. Les caméras inactives peuvent être affichées avec leur gizmo, leur frustum de vision et leur nom.

Pour les translations, les touches W/S et ↑/↓ font un dolly, A/D et ←/→ un truck et Q/E un boom. Pour les rotations, les touches U/J font un tangage (tilt), H/K un lacet (pan dans openFrameworks) et Y/I un roulis (roll). Les touches Z/X diminuent ou augmentent le champ de vision (entre 1 et 179 degrés).

Les touches G, V, B, C et F activent ou désactivent la grille, les axes, les cubes, le damier et le remplissage, la touche T affiche ou masque les gizmos, frustums et noms des caméras inactives, et la touche R ou la barre d'espace réinitialise les caméras.

### Exemple 5.3 (CameraProjection)

Exemple de rendu d'un teapot en rotation vu par une caméra en perspective, avec l'affichage à l'écran de la matrice de modèle-vue et de la matrice de projection.

Les touches 1, 2 et 3 sélectionnent le shader (Lambert, normales ou profondeur) et la touche 4 alterne entre une instance d'ofCamera et la caméra par défaut d'openFrameworks.

Les flèches déplacent le teapot sur les axes X et Z, la touche R active ou désactive sa rotation et la barre d'espace écrit l'état de la caméra et ses matrices dans la console.

Une interface permet d'ajuster les couleurs (arrière-plan, ambiante et diffuse), les plans d'occlusion avant et arrière (near et far), l'angle du champ de vision vertical et le facteur de zoom (recalculés l'un par rapport à l'autre).
