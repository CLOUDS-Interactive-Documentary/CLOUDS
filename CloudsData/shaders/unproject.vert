#version 110
#extension GL_ARB_texture_rectangle : enable

uniform vec2 dim;
uniform vec2 textureScale;

// CORRECTION
uniform vec2 shift;
uniform vec2 scale;

// DEPTH
uniform sampler2DRect depthTex;
uniform vec2 principalPoint;
uniform vec2 fov;
uniform float farClip;
uniform float nearClip;
uniform float edgeClip;
uniform vec2 simplify;

//COLOR INTRINSICS
uniform mat4 extrinsics;
uniform mat3 colorRotate;
uniform vec3 colorTranslate;
uniform vec2 colorFOV;
uniform vec2 colorPP;
uniform vec3 dK;
uniform vec2 dP;

//SIN DISTORTION
uniform vec2 sinAmp;
uniform vec2 sinFreq;
uniform vec2 sinPos;

//PERLIN DISTORTION
uniform float noiseAmp;
uniform float noiseDensity;
uniform float noisePosition;
uniform vec3 noiseShape;

//POINT AFFECT
uniform float pointMin;
uniform float pointMax;

uniform int useTexture;

varying float VZPositionValid0;

const float epsilon = 1e-6;

//START NOISE ---------------------------
float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
}

//
// Description : Array and textureless GLSL 2D/3D/4D simplex
//               noise functions.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
//

vec4 mod289(vec4 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0; }

float mod289(float x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0; }

vec4 permute(vec4 x) {
	return mod289(((x*34.0)+1.0)*x);
}

float permute(float x) {
	return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
	return 1.79284291400159 - 0.85373472095314 * r;
}

float taylorInvSqrt(float r)
{
	return 1.79284291400159 - 0.85373472095314 * r;
}

vec4 grad4(float j, vec4 ip)
{
	const vec4 ones = vec4(1.0, 1.0, 1.0, -1.0);
	vec4 p,s;
	
	p.xyz = floor( fract (vec3(j) * ip.xyz) * 7.0) * ip.z - 1.0;
	p.w = 1.5 - dot(abs(p.xyz), ones.xyz);
	s = vec4(lessThan(p, vec4(0.0)));
	p.xyz = p.xyz + (s.xyz*2.0 - 1.0) * s.www;
	
	return p;
}

// (sqrt(5) - 1)/4 = F4, used once below
#define F4 0.309016994374947451

float snoise(vec4 v)
{
	const vec4  C = vec4( 0.138196601125011,  // (5 - sqrt(5))/20  G4
						 0.276393202250021,  // 2 * G4
						 0.414589803375032,  // 3 * G4
						 -0.447213595499958); // -1 + 4 * G4
	
	// First corner
	vec4 i  = floor(v + dot(v, vec4(F4)) );
	vec4 x0 = v -   i + dot(i, C.xxxx);
	
	// Other corners
	
	// Rank sorting originally contributed by Bill Licea-Kane, AMD (formerly ATI)
	vec4 i0;
	vec3 isX = step( x0.yzw, x0.xxx );
	vec3 isYZ = step( x0.zww, x0.yyz );
	//  i0.x = dot( isX, vec3( 1.0 ) );
	i0.x = isX.x + isX.y + isX.z;
	i0.yzw = 1.0 - isX;
	//  i0.y += dot( isYZ.xy, vec2( 1.0 ) );
	i0.y += isYZ.x + isYZ.y;
	i0.zw += 1.0 - isYZ.xy;
	i0.z += isYZ.z;
	i0.w += 1.0 - isYZ.z;
	
	// i0 now contains the unique values 0,1,2,3 in each channel
	vec4 i3 = clamp( i0, 0.0, 1.0 );
	vec4 i2 = clamp( i0-1.0, 0.0, 1.0 );
	vec4 i1 = clamp( i0-2.0, 0.0, 1.0 );
	
	//  x0 = x0 - 0.0 + 0.0 * C.xxxx
	//  x1 = x0 - i1  + 1.0 * C.xxxx
	//  x2 = x0 - i2  + 2.0 * C.xxxx
	//  x3 = x0 - i3  + 3.0 * C.xxxx
	//  x4 = x0 - 1.0 + 4.0 * C.xxxx
	vec4 x1 = x0 - i1 + C.xxxx;
	vec4 x2 = x0 - i2 + C.yyyy;
	vec4 x3 = x0 - i3 + C.zzzz;
	vec4 x4 = x0 + C.wwww;
	
	// Permutations
	i = mod289(i);
	float j0 = permute( permute( permute( permute(i.w) + i.z) + i.y) + i.x);
	vec4 j1 = permute( permute( permute( permute (
												  i.w + vec4(i1.w, i2.w, i3.w, 1.0 ))
										+ i.z + vec4(i1.z, i2.z, i3.z, 1.0 ))
							   + i.y + vec4(i1.y, i2.y, i3.y, 1.0 ))
					  + i.x + vec4(i1.x, i2.x, i3.x, 1.0 ));
	
	// Gradients: 7x7x6 points over a cube, mapped onto a 4-cross polytope
	// 7*7*6 = 294, which is close to the ring size 17*17 = 289.
	vec4 ip = vec4(1.0/294.0, 1.0/49.0, 1.0/7.0, 0.0) ;
	
	vec4 p0 = grad4(j0,   ip);
	vec4 p1 = grad4(j1.x, ip);
	vec4 p2 = grad4(j1.y, ip);
	vec4 p3 = grad4(j1.z, ip);
	vec4 p4 = grad4(j1.w, ip);
	
	// Normalise gradients
	vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;
	p4 *= taylorInvSqrt(dot(p4,p4));
	
	// Mix contributions from the five corners
	vec3 m0 = max(0.6 - vec3(dot(x0,x0), dot(x1,x1), dot(x2,x2)), 0.0);
	vec2 m1 = max(0.6 - vec2(dot(x3,x3), dot(x4,x4)            ), 0.0);
	m0 = m0 * m0;
	m1 = m1 * m1;
	return 49.0 * ( dot(m0*m0, vec3( dot( p0, x0 ), dot( p1, x1 ), dot( p2, x2 )))
				   + dot(m1*m1, vec2( dot( p3, x3 ), dot( p4, x4 ) ) ) ) ;
	
}

//END NOISE ---------------------------


///MAIN ---------------------------
void main(void)
{
    //align to texture
    vec2 halfvec = vec2(.5,.5);
    float depth = texture2DRect(depthTex, floor(gl_Vertex.xy) + halfvec).r * 65535.;
    float right = texture2DRect(depthTex, floor(gl_Vertex.xy + vec2(simplify.x,0.0)) + halfvec ).r * 65535.;
    float down  = texture2DRect(depthTex, floor(gl_Vertex.xy + vec2(0.0,simplify.y)) + halfvec ).r * 65535.;
    float left  = texture2DRect(depthTex, floor(gl_Vertex.xy + vec2(-simplify.x,0.0)) + halfvec ).r * 65535.;
    float up    = texture2DRect(depthTex, floor(gl_Vertex.xy + vec2(0.0,-simplify.y)) + halfvec ).r * 65535.;
    float bl    = texture2DRect(depthTex, vec2(floor(gl_Vertex.x - simplify.x),floor( gl_Vertex.y + simplify.y)) + halfvec ).r * 65535.;
    float ur    = texture2DRect(depthTex, vec2(floor(gl_Vertex.x  + simplify.x),floor(gl_Vertex.y - simplify.y)) + halfvec ).r * 65535.;

    //cull invalid verts
    VZPositionValid0 = (depth < farClip &&
                        right < farClip &&
                        down < farClip &&
                        left < farClip &&
                        up < farClip &&
                        bl < farClip &&
                        ur < farClip &&
                        
                        depth > nearClip &&
                        right > nearClip &&
                        down > nearClip &&
                        left > nearClip &&
                        up > nearClip &&
                        bl > nearClip &&
                        ur > nearClip &&
                        
                        abs(down - depth) < edgeClip &&
                        abs(right - depth) < edgeClip &&
                        abs(up - depth) < edgeClip &&
                        abs(left - depth) < edgeClip &&
                        abs(ur - depth) < edgeClip &&
                        abs(bl - depth) < edgeClip
						) ? 1.0 : 0.0;


	vec4 pos = vec4((gl_Vertex.x - principalPoint.x) * depth / fov.x,
                    (gl_Vertex.y - principalPoint.y) * depth / fov.y, depth, 1.0);

	
    //projective texture on the geometry
    if(useTexture == 1){
        vec4 texCd;
		//http://opencv.willowgarage.com/documentation/camera_calibration_and_3d_reconstruction.html
		//vec3 projection = colorRotate * pos.xyz + colorTranslate + vec3(shift*dim / textureScale,0);
		vec4 projection = extrinsics * pos + vec4(shift*dim / textureScale,0,0);
		
		if(projection.z != 0.0) {

			vec2 xyp = projection.xy / projection.z;
			float r2 = pow(xyp.x, 2.0) + pow(xyp.y, 2.0);
			float r4 = r2*r2;
			float r6 = r4*r2;
			vec2 xypp = xyp;
			xypp.x = xyp.x * (1.0 + dK.x*r2 + dK.y*r4 + dK.z*r6) + 2.0*dP.x * xyp.x * xyp.y + dP.y*(r2 + 2.0 * pow(xyp.x,2.0) );
			xypp.y = xyp.y * (1.0 + dK.x*r2 + dK.y*r4 + dK.z*r6) + dP.x * (r2 + 2.0*pow(xyp.y, 2.0) ) + 2.0*dP.y*xyp.x*xyp.y;
			vec2 uv = (colorFOV * xypp + colorPP) * textureScale;
			texCd.xy = ((uv-dim/2.0) * scale) + dim/2.0;
		}

        gl_TexCoord[0] = texCd;
    }
    
	//EFFECTS --
	if(sinAmp.x > 0.0 || sinAmp.y > 0.0){
		vec2 sinOffset = sin(gl_Vertex.xy * sinFreq + sinPos.xy) * sinAmp;
		pos.z += sinOffset.x + sinOffset.y;
	}

//	float noiseDistort = snoise(vec4(noiseShape * pos.xyz / noiseDensity, noisePosition));
//	pos.z -= noiseDistort * noiseAmp;
//	gl_PointSize = pow(map(noiseDistort, -1.0, 1.0, pointMin, pointMax), 2.0);

//	if(noiseAmp > 0.0){
//		vec3 noiseDistort = vec3(snoise(vec4(pos.xyz / noiseDensity, noisePosition)),
//								 snoise(vec4(pos.yzx / noiseDensity, noisePosition)),
//								 snoise(vec4(pos.zxy / noiseDensity, noisePosition))) * noiseAmp;
//	}

    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;
	//gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;

    gl_FrontColor = gl_Color;
}
