
//projection uniforms
uniform vec2 principalPoint;
uniform vec2 fov;
uniform float farClip;

//focal uniforms
uniform float focalDistance;
uniform float focalRange;
uniform float fogNear;
uniform float fogRange;
uniform int project;

varying float VInFocus0;
varying float VZPositionValid0;
void main(void)
{
	
	if(project == 1){
		vec4 pos = vec4((gl_Vertex.x - principalPoint.x) * gl_Vertex.z / fov.x,
						(gl_Vertex.y - principalPoint.y) * gl_Vertex.z / fov.y, gl_Vertex.z, 1.0);
	    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
		VZPositionValid0 = (gl_Vertex.z < farClip && gl_Vertex.z > 200.) ? 1.0 : 0.0;
	}
    else{
		gl_Position = ftransform();
		VZPositionValid0 = 1.0;
	}
    
    VInFocus0 = min(abs(gl_Position.z - focalDistance) / focalRange, 1.0);
}
