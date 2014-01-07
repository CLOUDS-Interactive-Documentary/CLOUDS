//varying vars get passed from the vertex shader to the fragment shader
varying float normPos;
varying float filled;
void main() {
	
	normPos = gl_Normal.x;
	filled = gl_Normal.y;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
	gl_FrontColor = gl_Color;
	
}
