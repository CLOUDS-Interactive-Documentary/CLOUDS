#version 120
#extension GL_ARB_texture_rectangle : enable

uniform vec3 lightColor;
uniform vec3 lightPos;

uniform sampler2DRect displacment;
uniform sampler2DRect overlayMap;
uniform vec2 overlayDim;
uniform int bUseOverlay = 0;

uniform vec2 displacmentDim;
uniform vec2 texSize = vec2(500., 500.);

uniform float maxCubeScale = 20.;
uniform float blocksMinDist = .5;
uniform float blockSize = 2.;
uniform float blockResolution = 43.;
uniform float blocksMinSize = 1.;

uniform float blocksAlpha = 1.;

uniform float maxHeight = 10.;
uniform float minHeight = .1;

uniform float drawEdges;
uniform float edgeOffset = 0.;
uniform float lineWidth = 10.;
uniform float inverseSize = .01;

varying vec4 col;
varying vec3 norm;
varying vec3 vertex;
varying vec3 vPos;
varying vec3 ePos;
varying vec4 lPos;
varying vec4 ecPosition;
varying vec2 uv;
varying vec2 facadeUV;

varying vec4 position;

uniform mat4 invProjection;

varying vec2 projImgUV;

vec4 getDisplacment( vec2 _uv ){
	if(bUseOverlay == 0){
		return texture2DRect( displacment, _uv * displacmentDim );
	}
	
	return texture2DRect( displacment, _uv * displacmentDim ) * texture2DRect( overlayMap, _uv * overlayDim );
}

void main()
{
	lPos = vec4( gl_LightSource[0].position.xyz, 1.);
	
	uv = gl_MultiTexCoord0.xy;
	vertex = gl_Vertex.xyz;
	
	//get displacement
	vec4 displacmentSample = getDisplacment( uv );
	float disp = displacmentSample.x;
	
	col = vec4( displacmentSample.xyz, max( blocksAlpha, disp * .78 + .22));//<-- aplha taken from the original render algorthim
	norm = gl_NormalMatrix * gl_Normal;
	
	//emulating particio's scaling algorithm in the x & z axis
	vPos = gl_Vertex.xyz;

	vec2 cubeCenter = uv * vec2(blockResolution) - vec2(blockResolution)*.5;
	vec2 localPos = vPos.xz - cubeCenter;

	//scale it using patricio's algorithm
	localPos *= (1.0-blocksMinDist) - disp * blocksMinSize;//scale it
		
	//reposition our vertex in the x & z planes
	vPos.xz = localPos + cubeCenter;//back to world space
	

	//extrude our building vertically
	if(vPos.y > .1){
		vPos.y += disp * maxHeight;
	}
	else{
		vPos.y = 0.;
	}

	ecPosition = gl_ModelViewMatrix * vec4(vPos, 1.);
	
	if(int(drawEdges) == 1)
	{
		ecPosition.xyz += norm * lineWidth * .5;
		ecPosition.xyz += normalize(norm) * edgeOffset;
	}
	
	ePos = normalize(ecPosition.xyz/ecPosition.w);
	position = gl_ProjectionMatrix * ecPosition;
	gl_Position = position;
	
	gl_FrontColor = gl_Color;
	
	projImgUV = normalize( vec3(invProjection * vec4(vPos, 1.))).xy * .5 + .5;
}