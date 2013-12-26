varying float zdist;
uniform mat4 geoTransform;
void main() {

	vec4 pos = geoTransform * gl_Vertex;
	zdist = pos.y;
	gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
	gl_FrontColor = gl_Color;
}