/* HBlurVertexShader.glsl */

varying vec2 v_texcoord;

void main()
{
	v_texcoord = gl_MultiTexCoord0.st;
	gl_Position = ftransform();
	gl_FrontColor = gl_Color;
}
