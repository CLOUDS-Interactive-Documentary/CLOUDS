#version 120

varying vec4 vColor;

void main(){
    
    gl_TexCoord[0] = gl_MultiTexCoord0;
    
    vColor = gl_Color;
    
    vec4 pos = gl_Vertex;
    
    vec3 viewDir = normalize( -( gl_ModelViewMatrix * gl_Vertex ).xyz );
//    vec3 friendViewDir = normalize( -( gl_ModelViewMatrix * vec4(gl_Normal,1.0) ).xyz );
    vec3 rotationToEye = cross( gl_Normal, viewDir );
    pos.xyz += rotationToEye * clamp(gl_TexCoord[0].s,.0,.5);

    gl_Position = gl_ModelViewProjectionMatrix * pos;
    
}