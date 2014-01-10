uniform float rotate;
uniform float hoverPercent;
uniform mat4 rot;

const float epsilon = 1e-6;

varying float zDist; //for attenuation

varying float timerGeoFlag; //0 if it is a ring, 1 if it is a timer
varying float ringPosition;
varying float segmentPosition;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
}

mat4 rotationMatrix(vec3 axis, float angle)
{
//    vec3 axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main(void)
{
	
	//metadata
	ringPosition    = gl_Normal.z; //which ring index are we
	segmentPosition = gl_Normal.y; //where around the ring are we
	timerGeoFlag    = gl_MultiTexCoord0.s; //are we part of the timer mesh
	
	vec4 pos = gl_Vertex*rotationMatrix(vec3(0.,0.,1.), rotate * gl_Normal.x);
	pos.z -= pow(ringPosition, 1.2) * 20.0;
	vec4 eyePos = gl_ModelViewMatrix * pos;
	zDist = -eyePos.z;
	
	gl_Position = gl_ProjectionMatrix * eyePos;
	gl_FrontColor = gl_Color;
}
