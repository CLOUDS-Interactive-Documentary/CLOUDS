#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect posTexture;
uniform sampler2DRect velTexture;
uniform float bound;
uniform float dimX;
uniform float dimY;
varying vec2 uv;

float hash( float n ) { return fract(sin(n)*43758.5453123); }
float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
	
    float n = p.x + p.y*157.0 + 113.0*p.z;
    return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                   mix( hash(n+157.0), hash(n+158.0),f.x),f.y),
               mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                   mix( hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
}

float noise( in float x, in float y, in float z)
{
	return noise( vec3(x,y,z) );
}

void main()
{
	//TODO: make these uniforms
	float noiseScl = .4, offset = .1, noiseSampleScale = .01, velAtten = .98, radius = 30., accScl = .2, gravity = .02;
	
	vec3 pos = texture2DRect( posTexture, uv).xyz;
	vec3 vel = texture2DRect( velTexture, uv).xyz;
	
	vec3 p = pos * noiseSampleScale;
	vec3 acc;
	
	//noise influence
	acc.x = noise(p.x+offset, p.y, p.z) - noise(p.x-offset, p.y, p.z);
	acc.y = noise(p.x, p.y+offset, p.z) - noise(p.x, p.y-offset, p.z);
	acc.z = noise(p.x, p.y, p.z+offset) - noise(p.x, p.y, p.z-offset);
	acc *= noiseScl;
	
	vel *= velAtten;
	
	//collision testing
	vec3 delta;
	float dist;
	int collisionCount = 0;
	for (int i=0; i<int(dimX); i++)
	{
		for (int j=0; j<int(dimY); j++)
		{
			if(collisionCount<4)
			{
				delta = texture2DRect( posTexture, vec2(float(i), float(j))).xyz - pos;
				dist = abs(delta.x) + abs(delta.y) + abs(delta.z);
				if(dist < radius)
				{
					acc -= normalize(delta) * (1. - dist / radius);
					collisionCount++;
				}
			}
		}
	}
	
	//attract them to the center axis
	acc.xz -= normalize(pos.xz) * .01;
	
	//acceleration & gravity
	vel += acc * accScl;
	vel.y += gravity;
	
	//draw it
   	gl_FragColor = vec4(vel, 1.0);
}