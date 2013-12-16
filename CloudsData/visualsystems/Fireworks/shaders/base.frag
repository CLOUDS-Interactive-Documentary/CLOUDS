uniform float lifespan;
uniform float nearClip;
uniform float farClip;

uniform sampler2D circleMap;
uniform sampler2D squareMap;
uniform sampler2D triangleMap;
uniform sampler2D starMap;

uniform vec2 mapDim;

varying vec3 ecPosition3;
varying vec3 eye;
varying vec4 color;
varying float age;
varying float pointSize;
varying float attenuation;

varying vec4 q;
varying float tIndex;

varying float fogAmount;

vec3 rotateVectorByQuaternion( vec3 v, vec4 q ) {
	
	vec3 dest = vec3( 0.0 );
	
	float x = v.x, y  = v.y, z  = v.z;
	float qx = q.x, qy = q.y, qz = q.z, qw = q.w;
	
	// calculate quaternion * vector
	float ix =  qw * x + qy * z - qz * y,
	iy =  qw * y + qz * x - qx * z,
	iz =  qw * z + qx * y - qy * x,
	iw = -qx * x - qy * y - qz * z;
	
	// calculate result * inverse quaternion
	dest.x = ix * qw + iw * -qx + iy * -qz - iz * -qy;
	dest.y = iy * qw + iw * -qy + iz * -qx - ix * -qz;
	dest.z = iz * qw + iw * -qz + ix * -qy - iy * -qx;
	
	return dest;
}

float linearizeDepth( in float d ) {
    return (2.0 * nearClip) / (farClip + nearClip - d * (farClip - nearClip));
}


void main(){
	
	//rotation
	vec2 rotUV = rotateVectorByQuaternion( vec3(gl_TexCoord[0].xy*2.-1., 0.), q ).xy * .5 + .5;
	rotUV =	clamp( rotUV, vec2(0.), vec2(1.) );
	
	//depth
	float depthVal = 1. - pow( linearizeDepth( gl_FragCoord.z ), 2.);
	
	//texture sampling
	int textureIndex = int(floor(tIndex));
	vec4 texCol;
	
	if(pointSize > 2.){
		
		if(textureIndex == 0)		texCol = texture2D( triangleMap, rotUV );
		else if(textureIndex == 1)	texCol = texture2D( circleMap, rotUV );
		else if(textureIndex == 2)	texCol = texture2D( squareMap, rotUV );
		else						texCol = texture2D( triangleMap, rotUV );
		
		
		//discard low alphas
		if(texCol.w < .01)	discard;
	}
	
	//color
	gl_FragColor = color * depthVal;
}