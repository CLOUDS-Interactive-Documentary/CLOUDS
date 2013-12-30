#version 120

uniform sampler2DRect particles0;
uniform sampler2DRect particles1;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(texture2DRect(particles0, gl_TexCoord[0].st).xyz, 1.0);
}
