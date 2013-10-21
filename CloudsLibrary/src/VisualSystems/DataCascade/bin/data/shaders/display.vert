
uniform sampler2DRect image;
uniform sampler2DRect shift;
uniform float height;
uniform float deviation;
uniform vec2 vertexOffset;
uniform float vertexScale;

uniform int wrappedOnly;
uniform int doRGBD;
uniform float cylinderWarpFade;
uniform float cylinderZ;
uniform float cylinderRadius;

//RGBD UNIFORMS
//TEXTURE INFORMATION
//
uniform sampler2DRect rgbdTexture;
uniform vec2 textureSize;
//COLOR
uniform vec4 colorRect;
uniform vec2 colorScale;

uniform vec2 colorFOV;
uniform vec2 colorPP;
uniform vec3 dK;
uniform vec2 dP;

uniform mat4 extrinsics;
uniform vec2 scale;

//DEPTH
uniform vec4 depthRect;
uniform vec2 depthPP;
uniform vec2 depthFOV;

//NORMAL
uniform vec4 normalRect;

//GEOMETRY
uniform vec2  simplify;
uniform float farClip;
uniform float nearClip;
uniform float edgeClip;
uniform float minDepth;
uniform float maxDepth;
//END RGBD UNIFORMS

//fog params
varying float fogFactor;
varying float wrapped;

//RGBD STUFF::
vec3 rgb2hsl( vec3 _input ){
	float h = 0.0;
	float s = 0.0;
	float l = 0.0;
	float r = _input.r;
	float g = _input.g;
	float b = _input.b;
	float cMin = min( r, min( g, b ) );
	float cMax = max( r, max( g, b ) );
	
	l = ( cMax + cMin ) / 2.0;
	if ( cMax > cMin ) {
		float cDelta = cMax - cMin;
        
		s = l < .05 ? cDelta / ( cMax + cMin ) : cDelta / ( 2.0 - ( cMax + cMin ) );
		
		// hue
		if ( r == cMax ) {
			h = ( g - b ) / cDelta;
		} else if ( g == cMax ) {
			h = 2.0 + ( b - r ) / cDelta;
		} else {
			h = 4.0 + ( r - g ) / cDelta;
		}
		
		if ( h < 0.0) {
			h += 6.0;
		}
		h = h / 6.0;
	}
	return vec3( h, s, l );
}

float depthValueFromSample( vec2 depthPos){
    vec2  halfvec = vec2(.5,.5);
	if(depthPos.x <= depthRect.x ||
	   depthPos.y <= depthRect.y ||
	   depthPos.x >= depthRect.x+depthRect.z ||
	   depthPos.y >= depthRect.y+depthRect.w)
	{
		return 0.;
	}
	depthPos.x = clamp(depthPos.x,depthRect.x+2.,depthRect.x+depthRect.z-4.);
	depthPos.y = clamp(depthPos.y,depthRect.y+2.,depthRect.y+depthRect.w-4.);
	
    float depth = rgb2hsl( texture2DRect(rgbdTexture, floor(depthPos) + halfvec ).xyz ).r;
    return depth * ( maxDepth - minDepth ) + minDepth;
}

void main(void)
{
	
	//----------
	float offset = texture2DRect(image, vec2(gl_Vertex.x, 1.0)).r;
	wrapped = 1.0;
	if(wrappedOnly == 1 && (gl_Vertex.y + offset) < height){
		wrapped = .0;
	}
	
	//this is a default vertex shader, don't modify the position
	vec4 pos = vec4(gl_Vertex.x,
					mod(gl_Vertex.y + offset, height),
					gl_Vertex.z,
					gl_Vertex.w);

	pos.x += (texture2DRect(shift, vec2(gl_Vertex.x, pos.y)).r - .5) * deviation;
	pos.xy *= vertexScale;
	pos.xy += vertexOffset;
	int warp = 1;
	if(doRGBD == 1){
		
		// RGBD STUFF
		// Here we get the position, and account for the vertex position flowing
		vec2 samplePos = vec2(pos.x, pos.y);
		vec2 depthPos = samplePos + depthRect.xy;
		float depth = depthValueFromSample( depthPos );
		
		//if the depth value is equal or less than min depth, then it didn't fall on the person.
		if(depth <= minDepth){
			depth = maxDepth;
			warp = 1;
		}
		else{
			warp = 0;
		}
		
		// Reconstruct the 3D point position
		pos = vec4((samplePos.x - depthPP.x) * depth / depthFOV.x,
				   (samplePos.y - depthPP.y) * depth / depthFOV.y,
					depth, 1.0);
	}
	
	
	//START CYLINDER
	if(warp == 1){
		float radius = cylinderRadius;
		vec2 cylinderCenter = vec2(0, cylinderZ);
		if(doRGBD == 1) {
			radius = maxDepth - cylinderZ;
		}
		
		vec2 toCenter = pos.xz - cylinderCenter;
		float distToCenter = length(toCenter);
		vec2 dirToCenter = toCenter / distToCenter;
		pos.xz = mix(pos.xz, (cylinderCenter + dirToCenter * radius), cylinderWarpFade);
	}
	
	//FOG STUFF
	vec3 vVertex = (gl_ModelViewMatrix * pos).xyz;
	const float LOG2 = 1.442695;
	gl_FogFragCoord = length(vVertex);
	fogFactor = exp2(-gl_Fog.density *
					 gl_Fog.density  *
					 gl_FogFragCoord *
					 gl_FogFragCoord *
					 LOG2);
	
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	//END FOG

	gl_Position = gl_ModelViewProjectionMatrix * pos;
	gl_FrontColor = gl_Color;
}

