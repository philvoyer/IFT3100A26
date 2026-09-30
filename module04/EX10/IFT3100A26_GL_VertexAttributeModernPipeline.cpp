// IFT3100A26_GL_VertexAttributeModernPipeline.cpp
// Exemple d'une section de code pour assigner les pointeurs vers chaque attribut de la structure de sommet d'un buffer de géométrie (pipeline moderne, avec un programme de shader).

// entrée
// • Identifiant unique d'un buffer de géométrie déjà initialisé.
// • Identifiant unique d'un programme de shader dont les shaders sont déjà compilés et attachés.
// sortie
// • Un buffer de géométrie prêt à l'utilisation où chaque attribut est adéquatement configuré.

GLuint vbo = ...;
GLuint shader_program = ...;

// définir l'ordre de localisation des attributs de sommet
const GLuint location_vertex_attribute_position = 0;
const GLuint location_vertex_attribute_normal   = 1;
const GLuint location_vertex_attribute_texcoord = 2;
const GLuint location_vertex_attribute_color    = 3;

// configurer la localisation des attributs du programme et leurs noms en GLSL
// (doit être fait avant l'édition de liens du programme de shader)
// note : en GLSL 330, la localisation peut aussi être fixée directement dans le shader de sommets,
// par exemple avec « layout(location = 0) in vec3 position; », sans appel à glBindAttribLocation
glBindAttribLocation(shader_program, location_vertex_attribute_position, "position");
glBindAttribLocation(shader_program, location_vertex_attribute_normal,   "normal");
glBindAttribLocation(shader_program, location_vertex_attribute_texcoord, "texcoord");
glBindAttribLocation(shader_program, location_vertex_attribute_color,    "color");

// faire l'édition de liens du programme de shader
glLinkProgram(shader_program);

// sélectionner le shader
// (nécessaire pour la commande de rendu; la configuration des pointeurs d'attributs plus bas n'en dépend pas)
glUseProgram(shader_program);

// en profil core (OpenGL 3.3 et plus), un vertex array object (vao) est obligatoire
// il mémorise la configuration des attributs qui suit

// déclarer un identifiant pour référencer un nouveau vao
GLuint vao;

// création d'un nouveau vao dont l'identifiant unique sera retourné par OpenGL
glGenVertexArrays(1, &vao);

// sélectionner le nouveau vao (doit être fait avant de configurer les attributs)
glBindVertexArray(vao);

// sélectionner le buffer de géométrie
// (le vao mémorise le buffer associé à chaque attribut lors de l'appel à glVertexAttribPointer)
glBindBuffer(GL_ARRAY_BUFFER, vbo);

// assigner les pointeurs vers chaque attribut de la structure de sommet du buffer de géométrie
glVertexAttribPointer(location_vertex_attribute_position, 3, GL_FLOAT,         GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
glVertexAttribPointer(location_vertex_attribute_normal,   3, GL_FLOAT,         GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
glVertexAttribPointer(location_vertex_attribute_texcoord, 2, GL_FLOAT,         GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, texcoord));
glVertexAttribPointer(location_vertex_attribute_color,    4, GL_UNSIGNED_BYTE, GL_TRUE,  sizeof(Vertex), (void*) offsetof(Vertex, color));

// activer les pointeurs d'attributs
glEnableVertexAttribArray(location_vertex_attribute_position);
glEnableVertexAttribArray(location_vertex_attribute_normal);
glEnableVertexAttribArray(location_vertex_attribute_texcoord);
glEnableVertexAttribArray(location_vertex_attribute_color);

// pour dessiner : sélectionner le vao (glBindVertexArray(vao)), puis émettre une commande de rendu
// (l'activation des attributs est mémorisée dans le vao, il n'est pas nécessaire de les désactiver après le rendu)
