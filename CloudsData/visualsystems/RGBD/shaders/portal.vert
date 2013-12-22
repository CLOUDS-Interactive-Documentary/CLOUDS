uniform float rotate;
uniform mat4 rot;

const float epsilon = 1e-6;

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
	vec4 pos = gl_Vertex*rotationMatrix(vec3(0.,0.,1.), rotate * gl_Normal.x);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
	gl_FrontColor = gl_Color;

}
