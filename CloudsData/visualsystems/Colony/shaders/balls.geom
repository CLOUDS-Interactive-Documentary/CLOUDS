#version 120
#extension all : warn
#define RESOLUTION 4
#define PI 3.14159265359
#define SPREAD_RADIUS 100.


uniform vec2 screenResolution;

void main() {
    //IN
    vec3 p_in = gl_PositionIn[0].xyz;
    vec2 data = gl_PositionIn[1].xy;     //size = data.x, angle = data.y;
    
    //TORUS PLACEMENT
    vec3 theta = p_in / vec3(screenResolution/(2. *PI),1.);
    vec3 p= vec3(cos(theta.x), 0, sin(theta.x)) * SPREAD_RADIUS;
    p += vec3(0, p_in.y, 0);
    
    //CAMERA FACING
    //    vec3 forward = normalize(transpose(gl_ModelViewMatrix)[2].xyz);
    vec3 forward = vec3(0., 0., 1.);
//    vec3 up = vec3(cos(data.y), sin(data.y), 0.);
    //    vec3 up = vec3(0.,1.,0.);
//    vec3 right = cross(forward, up);
    vec3 up = vec3(0.,1.,0.);
    vec3 right = vec3(1.,0.,0.);
    
    //POINT GENERATION
    float res = float(RESOLUTION);
    
    for (int i = 0; i < RESOLUTION * 8 ; i++) {
        float sgn = (i < RESOLUTION * 4 ) ? 1. : -1.;
//        float sgn = 1.;
        float shift = floor(mod(i, 2.)); //0 or 1
        /* 
         Draw a point either at a or b
         c
         |\
         | \
         a--b
         */
        float f = (sgn * float(i/2 + shift) / res ) * 2. * PI;
        vec3 p1 = cos(data.y + f) * right + sin(data.y + f) * forward;
        p1 *= data.x;
        
        gl_Position = gl_ModelViewProjectionMatrix * vec4(p + p1, 1.0);
        float colorParam = mod(data.y - abs(f), 2. * PI);
        gl_FrontColor = vec4(1. + cos(colorParam), 1. + sin(colorParam), 1. ,1.) * 0.5 ; //abs might be unneccesary
        EmitVertex();
        
        if (shift == 1.){
            //Draw c
            gl_Position = gl_ModelViewProjectionMatrix * vec4(p + sgn * up * data.x * 2., 1.); //why *2?
            gl_FrontColor = gl_FrontColorIn[0];
            EmitVertex();
        }
    }
    EndPrimitive();
}