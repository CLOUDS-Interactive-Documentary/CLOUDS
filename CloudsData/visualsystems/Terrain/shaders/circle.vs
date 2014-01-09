/* HBlurVertexShader.glsl */

varying vec2 pos;
void main()
{
	pos = gl_MultiTexCoord0.st;
	gl_Position = ftransform();
}
