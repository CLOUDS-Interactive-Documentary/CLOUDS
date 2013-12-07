#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect tex;
uniform float fade;

void main(void) 
{
      vec2 st = gl_TexCoord[0].st;
      int sum = 0;

      if (texture2DRect(tex, st + vec2(-1.0, -1.0)).a == 1.0) ++sum;
      if (texture2DRect(tex, st + vec2( 0.0, -1.0)).a == 1.0) ++sum;
      if (texture2DRect(tex, st + vec2( 1.0, -1.0)).a == 1.0) ++sum;
 
      if (texture2DRect(tex, st + vec2(-1.0,  0.0)).a == 1.0) ++sum;
      if (texture2DRect(tex, st + vec2( 1.0,  0.0)).a == 1.0) ++sum;
 
      if (texture2DRect(tex, st + vec2(-1.0,  1.0)).a == 1.0) ++sum;
      if (texture2DRect(tex, st + vec2( 0.0,  1.0)).a == 1.0) ++sum;
      if (texture2DRect(tex, st + vec2( 1.0,  1.0)).a == 1.0) ++sum;

      vec4 liveColor = vec4(1.0, 1.0, 1.0, 1.0);
      vec4 deadColor = vec4(1.0, 1.0, 1.0, fade);

      if (sum < 2) gl_FragColor = deadColor;
      else if (sum > 3) gl_FragColor = deadColor;
      else if (sum == 3) gl_FragColor = liveColor;
      else gl_FragColor = texture2DRect(tex, st);
 }
