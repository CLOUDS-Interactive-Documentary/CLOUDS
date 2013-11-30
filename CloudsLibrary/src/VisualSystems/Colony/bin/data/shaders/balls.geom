#version 120
#extension GL_EXT_geometry_shader4 : enable
#define RESOLUTION 16
#define PI 3.14159265359
#define SPREAD_RADIUS 100.

uniform vec2 screenResolution;


float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    //IN
    vec3 p_in = gl_PositionIn[0].xyz;
    vec2 data = gl_PositionIn[1].xy;     //size = data.x, angle = data.y;
    
    //TORUS PLACEMENT
    vec3 theta = p_in/vec3(screenResolution/(2. *PI),1.);
    vec3 p= vec3(cos(theta.x), 0, sin(theta.x))*SPREAD_RADIUS;
    p += vec3(0,gl_PositionIn[0].y,0);
    
    //CAMERA FACING
//    vec3 forward = normalize(gl_ModelViewMatrix[2].xyz); //TODO: Needs normalization?
    vec3 forward = normalize(vec3(gl_ModelViewMatrix[0].z,gl_ModelViewMatrix[1].z,gl_ModelViewMatrix[2].z)); //TODO: Needs normalization?
    vec3 up = vec3(cos(data.y), sin(data.y), 0.); //normalized
    vec3 right = cross(forward, up);
    
    //POINT GENERATION
    float res = float(RESOLUTION);
    for (int i = 0 ; i <= RESOLUTION ; i++) {
        //FIXME: The mod3 is a total hack, probably because TRIANGLE_FAN isn't working the way I expected
        if (mod(i, 3.) == 0.) {
            gl_Position = gl_ModelViewProjectionMatrix * vec4(p, 1.0);
            gl_FrontColor = gl_FrontColorIn[0];
            EmitVertex();
        }
        
        //Point Shift
        float f = (float(i) / res ) * 2. * PI;
//        vec3 p1 = data.x * vec3(cos(data.y + f), sin(data.y + f), 0);
        
        vec3 p1 = cos(data.y + f) * right + sin(data.y + f) * up;
        p1 *= data.x;
        
        gl_Position = gl_ModelViewProjectionMatrix * vec4(p + p1, 1.0);
        gl_FrontColor = vec4(1. + cos(data.y - f), 1. + sin(data.y - f), 1. ,1.) * 0.5 ;
        EmitVertex();
    }
    
    EndPrimitive();
}