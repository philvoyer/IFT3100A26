// IFT3100A26_GL_VertexBufferSwap.cpp
// Exemple d'une section de code qui alterne les rôles de deux buffers de géométrie (mise à jour et rendu) afin d'éviter de bloquer le pipeline de rendu.

// entrée
// • Deux buffers de géométrie indépendants avec une structure identique mais des données distinctes, dont les attributs de sommet sont configurés (voir les exemples 4.9 et 4.10).
// sortie
// • Le buffer de géométrie utilisé pour le rendu n'est pas celui dans lequel il y a mise à jour durant le même frame.

// deux buffers de géométrie déjà initialisés
GLuint vbo_alpha = ...;
GLuint vbo_beta = ...;

// un vao pour chaque buffer de géométrie, configuré avec les attributs de son buffer (voir l'exemple 4.10)
// note : les fonctions qui configurent les attributs (ex. glVertexAttribPointer) mémorisent le buffer sélectionné au moment de leur appel.
// Sélectionner un autre buffer par la suite ne change donc pas la source des attributs : pour que l'inversion ait un effet,
// les vao sont inversés en même temps que les vbo.
// (sans vao, il faut plutôt refaire la configuration des attributs après avoir sélectionné le buffer de rendu, voir l'exemple 4.9)
GLuint vao_alpha = ...;
GLuint vao_beta = ...;

// buffer de géométrie utilisé pour la mise à jour (écriture)
GLuint vbo_update = vbo_alpha;
GLuint vao_update = vao_alpha;

// buffer de géométrie utilisé pour le rendu (lecture)
GLuint vbo_draw = vbo_beta;
GLuint vao_draw = vao_beta;

// booléen qui indique si on est prêt pour une inversion des buffers de géométrie
bool is_ready_to_swap = false;

if (is_ready_to_swap)
{
  // variable temporaire pour l'échange
  GLuint temp;

  // inverser les buffers de géométrie
  temp = vbo_update;
  vbo_update = vbo_draw;
  vbo_draw = temp;

  // inverser les vao en même temps
  temp = vao_update;
  vao_update = vao_draw;
  vao_draw = temp;

  is_ready_to_swap = false;
}

if (/* mise à jour du modèle */)
{
  // sélectionner le vbo de mise à jour
  glBindBuffer(GL_ARRAY_BUFFER, vbo_update);

  // mise à jour du buffer de géométrie
  // ...
  // attention : après une inversion, ce buffer contient les données d'avant la dernière mise à jour.
  // Une mise à jour partielle (glBufferSubData) doit donc aussi appliquer les modifications précédentes,
  // sinon elles sont perdues. Une mise à jour complète du buffer (glBufferData) n'a pas ce problème.

  // désélectionner le buffer
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // inverser les buffers de géométrie lors du prochain frame
  is_ready_to_swap = true;
}

// sélectionner le vao de rendu (il est configuré avec le vbo de rendu)
glBindVertexArray(vao_draw);

// rendre le buffer de géométrie avec une commande de rendu
// ex. glDrawArrays ou glDrawElements

// note : d'autres techniques permettent aussi d'éviter de bloquer le pipeline lors de la mise à jour d'un buffer :
// réallouer le buffer avec glBufferData(GL_ARRAY_BUFFER, taille, NULL, GL_DYNAMIC_DRAW) avant de le remplir,
// ou utiliser glMapBufferRange avec l'option GL_MAP_INVALIDATE_BUFFER_BIT
