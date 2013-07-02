#version 120

uniform sampler2DRect position;
uniform sampler2DRect velocity;

uniform float t;
uniform float dt;

vec4 getPosition() {
	return texture2DRect(position, gl_TexCoord[0].xy);
}

vec4 getVelocity() {
	return texture2DRect(velocity, gl_TexCoord[0].xy);
}

bool isnan(float number) {
	return number != number;
}
//this function is a force and returns velocity
void main() {
	
	vec3 p = getPosition().xyz;
	vec3 v = getVelocity().xyz;
	//if (position.y > 0)

//	vec3 centerline = p;
//	centerline.xz *= 0.0;
//	
//	vec3 inplane = p;
//	inplane.y *= 0.0;
//
//	centerline = normalize(centerline);
//	inplane = normalize(centerline);
//	vec3 tangent = cross(centerline, inplane);

//	if (p.y > 0.0)
		v += vec3(0,-80.0,0);
//	else
//		v = vec3(0.8*v.x,0.8 * abs(v.y),0.8*v.z)+ tangent * length(inplane);

	 //v+= tangent * length(inplane);

	if (!isnan(length(v)))
		gl_FragColor.xyz = v;
	 else
	 	gl_FragColor.xyz = vec3(0,0,0);

	 gl_FragColor.w = 1.0;
}