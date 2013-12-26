varying float zdist;
void main() {
	zdist = gl_Vertex.y;
	gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
	gl_FrontColor = gl_Color;
}