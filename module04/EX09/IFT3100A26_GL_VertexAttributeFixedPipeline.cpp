// IFT3100A26_GL_VertexAttributeFixedPipeline.cpp
// Exemple d'une section de code pour assigner les pointeurs vers chaque attribut de la structure de sommet d'un buffer de géométrie (pipeline fixe).

// entrée
// • Identifiant unique d'un buffer de géométrie déjà initialisé.
// sortie
// • Un buffer de géométrie prêt à l'utilisation où chaque attribut est adéquatement configuré.

// note : ces fonctions appartiennent au pipeline fixe (profil de compatibilité), elles sont retirées du profil core
// (voir l'exemple 4.10 pour le pipeline moderne)

// identifiant du buffer de géométrie déjà initialisé
GLuint vbo = ...;

// sélectionner le buffer de géométrie
// (il doit rester sélectionné jusqu'à la commande de rendu, qui utilise le buffer actif au moment de son exécution)
glBindBuffer(GL_ARRAY_BUFFER, vbo);

// assigner les pointeurs vers chaque attribut de la structure de sommet du buffer de géométrie
glVertexPointer  (3, GL_FLOAT,         sizeof(Vertex), (void*) offsetof(Vertex, position));
glNormalPointer  (   GL_FLOAT,         sizeof(Vertex), (void*) offsetof(Vertex, normal));
glTexCoordPointer(2, GL_FLOAT,         sizeof(Vertex), (void*) offsetof(Vertex, texcoord));
glColorPointer   (4, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*) offsetof(Vertex, color));

// activer les pointeurs d'attributs
glEnableClientState(GL_VERTEX_ARRAY);
glEnableClientState(GL_NORMAL_ARRAY);
glEnableClientState(GL_TEXTURE_COORD_ARRAY);
glEnableClientState(GL_COLOR_ARRAY);

// rendre le modèle avec une commande de rendu
// ...

// puisque ces pointeurs restent activés, il faut les désactiver après la commande de rendu
// si un autre buffer de géométrie ne fournit pas les mêmes attributs
glDisableClientState(GL_VERTEX_ARRAY);
glDisableClientState(GL_NORMAL_ARRAY);
glDisableClientState(GL_TEXTURE_COORD_ARRAY);
glDisableClientState(GL_COLOR_ARRAY);
