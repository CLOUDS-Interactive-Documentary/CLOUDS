#version 120

varying float freshTraversalPosition;
varying float finalizedTraversalBit;
varying float totalTraversalPosition;
void main() {

	freshTraversalPosition = gl_Normal.x;
	finalizedTraversalBit = gl_Normal.y;
	totalTraversalPosition = gl_Normal.z;
	
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;

}
