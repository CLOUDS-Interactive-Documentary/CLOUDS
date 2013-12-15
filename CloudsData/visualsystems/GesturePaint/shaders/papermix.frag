
/* VBlurVertexShader.glsl */

uniform vec2 dimensions;
varying vec2 v_texCoord;
uniform sampler2DRect s_texture;
uniform sampler2DRect s_paper;

void main()
{
	vec3 paintColor = texture2DRect(s_texture, v_texCoord);
	gl_FragColor = texture2DRect(s_texture, v_texCoord);
}
