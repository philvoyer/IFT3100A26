// IFT3100A25 ~ draw_depth_330_fs.glsl

#version 330

// attributs interpolés à partir des valeurs en sortie du shader de sommets
in vec3 surface_position;
in vec3 surface_normal;
in float surface_depth;

// attribut en sortie
out vec4 fragment_color;

void main()
{
  float ndcDepth = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);

  float clipDepth = ndcDepth / gl_FragCoord.w;

  float depth_color = 1.0 - surface_depth;

  fragment_color = vec4(depth_color, depth_color, depth_color, 1.0);
}
