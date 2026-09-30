// IFT3100A26_GL_VertexBufferDraw.cpp
// Exemple de différentes approches pour dessiner un buffer de géométrie (glDrawArrays et glDrawElements avec divers types de primitives topologiques).

// entrée
// • Identifiant unique d'un buffer de géométrie déjà initialisé, dont les attributs de sommet sont configurés (voir les exemples 4.9 et 4.10).
// sortie
// • Rendu graphique du contenu du buffer de géométrie dans un framebuffer.

// identifiant d'un buffer de géométrie
GLuint vbo = ...;

// index du premier sommet à rendre
int start = ...;

// nombre de sommets à rendre
int count = ...;

// tableau d'indices de sommet
GLushort* index_array = ...;

// sélectionner le buffer de géométrie
// (avec un vao, on sélectionne plutôt le vao qui mémorise cette configuration : glBindVertexArray(vao), voir l'exemple 4.10)
glBindBuffer(GL_ARRAY_BUFFER, vbo);

// exemples de commande de rendu avec différents types de primitives topologiques :

// 1. dessiner un ensemble de points
glDrawArrays(GL_POINTS, start, count);

// 2. dessiner un circuit de lignes
glDrawArrays(GL_LINE_LOOP, start, count);

// 3. dessiner une série de triangles indépendants
// (count doit être un multiple de 3 : trois sommets par triangle)
glDrawArrays(GL_TRIANGLES, start, count);

// 4. dessiner un quadrilatère à partir de deux triangles connexes
// (l'ordre des 4 sommets doit suivre un zigzag, par exemple : haut-gauche, bas-gauche, haut-droite, bas-droite)
glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

// 5. dessiner une série de triangles connexes
glDrawArrays(GL_TRIANGLE_STRIP, start, count);

// 6. dessiner une suite de triangles connexes à partir d'un tableau d'indices
// note : avec glDrawElements, count est le nombre d'indices à utiliser (et non le nombre de sommets).
// note : ici, le tableau d'indices est en mémoire RAM (profil de compatibilité).
// En profil core, les indices doivent être dans un buffer lié à GL_ELEMENT_ARRAY_BUFFER
// et le dernier argument devient un décalage en octets dans ce buffer (ex. (void*) 0).
glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_SHORT, index_array);
