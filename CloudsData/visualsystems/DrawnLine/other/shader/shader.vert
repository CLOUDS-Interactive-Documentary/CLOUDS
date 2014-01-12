#version 120

varying vec4 globalColor;
uniform mat4 matrix;

void main()
{
    
    vec4 modifiedPosition = matrix * gl_Vertex;
    vec4 position = gl_ModelViewProjectionMatrix * modifiedPosition;
    
////    // the sine wave travels along the x-axis (across the screen),
////    // so we use the x coordinate of each vertex for the calculation,
////    // but we displace all the vertex along the y axis (up the screen)/
////    float displacementHeight = 100.0;
////    float displacementY = sin(time + (position.x / 100.0)) * displacementHeight;
////	
////    vec4 modifiedPosition = position;
////	modifiedPosition.y += displacementY;
//    
//    
//    position.xy -= offset;
//    float dist = distance(position.xy);
//    float angle = atan2(position.y, position.x);
//    position.x = sin(angle) * dist;
//    
    
    
	gl_Position = position;
    
    // in OpenGL 2.0 we must get the global color using the gl_Color command,
    // and store it in a globalColor (varying) that is passed to our frag shader.
    // please note that the frag shader also has a globalColor (varying),
    // and this is the standard way of passing data from the vertex shader to the frag shader.
    globalColor = gl_Color;
}