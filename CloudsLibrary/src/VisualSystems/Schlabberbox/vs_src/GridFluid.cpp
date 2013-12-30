
#include "GridFluid.h"

inline float trilinearInterpolate(float x, float y, float z,
                                  float p000, float p100, float p010, float p001, float p110, float p101, float p011, float p111) {
	return p000 * (1 - x) * (1 - y) * (1 - z) + p100 * x * (1 - y) * (1 - z) +
	       p010 * (1 - x) * y * (1 - z) + p001 * (1 - x) * (1 - y) * z +
	       p101 * x * (1 - y) * z + p011 * (1 - x) * y * z + p110 * x * y * (1 - z) +
	       p111 * x * y * z ;
}

inline int FluidBox::getArrayIndex(int x, int y, int z) {
	return ((x) + (y) * size + (z) * size * size);
}

FluidBox::FluidBox() {
	width = height = depth = 500;
}

FluidBox::~FluidBox() {

	delete[] velX;
	delete[] velY;
	delete[] velZ;

	delete[] velXOld;
	delete[] velYOld;
	delete[] velZOld;

	particles.clear();
}

void FluidBox::setup(int s, float v) {
	int N = s;

	size = N;
	viscosity = v;

	//allocate the arrays
	velX = new float[N*N*N];
	velY = new float[N*N*N];
	velZ = new float[N*N*N];

	//for calculations
	velXOld = new float[N*N*N];
	velYOld = new float[N*N*N];
	velZOld = new float[N*N*N];


	for(int i=0; i<N*N*N; i++) {
		velX[i] = velY[i] = velZ[i] = velXOld[i] = velYOld[i] = velZOld[i] = 0;
		velocity.push_back(ofVec3f());
	}

	//build the meshes
	meshVelocity.setMode(OF_PRIMITIVE_LINES);
	float scaleMulti = width / (float)N;
	int i = 0;

	for(int x=0; x<N; x++) {
		for(int y=0; y<N; y++) {
			for(int z=0; z<N; z++) {
				meshVelocity.addColor(ofFloatColor(1.f, 0, 0, 0.f));
				meshVelocity.addVertex(ofVec3f(x * scaleMulti, y * scaleMulti, z * scaleMulti));
				meshVelocity.addColor(ofFloatColor(0.f, 1.f, 0, 0.f));
				meshVelocity.addVertex(ofVec3f(x * scaleMulti, y * scaleMulti, z * scaleMulti));
				i++;
			}
		}
	}

	//box = ofMesh::box(1, 1, 1);
	box.load("box.ply");
	cornerSize = 3;

	particleSpeed = 80;

	gravity.set(0, -1, 0);
}

ofVec3f FluidBox::getVelocityAt(int x, int y, int z) {
	int index = getArrayIndex(x, y, z);
	return ofVec3f(velX[index], velY[index], velZ[index]);
}

void FluidBox::update() {

	//vecToFloats();

	advanceParticles();

	long time = ofGetElapsedTimeMillis();
	dt =  (time - timeOld) / 1000.f;
	timeOld = time;

	//solve diffusion equation in each dimension
	diffuse(1, velXOld, velX, viscosity, dt, 4, size);
	diffuse(2, velYOld, velY, viscosity, dt, 4, size);
	diffuse(3, velZOld, velZ, viscosity, dt, 4, size);

	convect(velXOld, velYOld, velZOld, velX, velY, 4, size);

	//advect (advance velocity field) in each dimension)
	advect(1, velX, velXOld, velXOld, velYOld, velZOld, dt, size);
	advect(2, velY, velYOld, velXOld, velYOld, velZOld, dt, size);
	advect(3, velZ, velZOld, velXOld, velYOld, velZOld, dt, size);

	convect(velX, velY, velZ, velXOld, velYOld, 4, size);

	floatsToVec();

	for(vector<ofVec3f>::iterator it = velocity.begin(); it<velocity.end(); it++) {
		//*it += (gravity - *it)*.001;
	}
}

void FluidBox::draw() {
	float scaleMulti = width / (float)size;
	int i = 0;
	int j = 0;
	for(unsigned int ix=0; ix<size; ix++) {
		for(unsigned int iy=0; iy<size; iy++) {
			for(unsigned int iz=0; iz<size; iz++) {
				meshVelocity.setColor(j, ofFloatColor(1, 1, 1, 1));
				meshVelocity.setVertex(j, ofVec3f(ix * scaleMulti, iy * scaleMulti, iz * scaleMulti));

				ofVec3f vel = velocity[getArrayIndex(ix, iy, iz)];

				meshVelocity.setColor(j, ofFloatColor(1, 1, 1, 1));
				meshVelocity.setColor(j+1, ofFloatColor(0, 0, 0, 0));

				float len = vel.lengthSquared();

				if(len > .01)
					meshVelocity.setVertex(j+1, (ofVec3f(ix, iy, iz) + vel * 6) * scaleMulti);
				else
					meshVelocity.setVertex(j+1, ofVec3f(ix * scaleMulti, iy * scaleMulti, iz * scaleMulti));

				/*
				if(len > .01) {
					float c = len * 100;
					ofPushMatrix();
					ofSetColor(c, c, c);
					ofTranslate(ix * scaleMulti, iy * scaleMulti, iz * scaleMulti);
					//ofTranslate(vel);
					ofScale(scaleMulti, scaleMulti, scaleMulti);
					box.drawWireframe();
					ofPopMatrix();
				}
				*/

				i++;
				j+=2;
			}
		}
	}
	if(drawForceField)
		meshVelocity.draw();

	ofSetColor(255);

	//TODO: optimize this for performance
	for(std::vector<ofVec3f>::iterator it = particles.begin(); it<particles.end(); it++) {
		if(drawLines) {
			for(std::vector<ofVec3f>::iterator jt = it; jt<particles.end(); jt++) {
				if((*it).squareDistance(*jt) < 5) {
					ofLine(*it * scaleMulti, *jt * scaleMulti);
				}
			}
		}

		if(drawCorners) {
			ofPushMatrix();
			ofTranslate(*it * scaleMulti);
			ofScale(cornerSize, cornerSize, cornerSize);
			box.draw();
			ofPopMatrix();
		}
	}
}

void FluidBox::addVelocity(int x, int y, int z, float amountX, float amountY, float amountZ) {
	int index = getArrayIndex(x, y, z);

	velX[index] += amountX;
	velY[index] += amountY;
	velZ[index] += amountZ;
}


void FluidBox::addForceLeft(ofVec3f force) {
	for(unsigned int ix=1; ix<size*.1; ix++) {
		for(unsigned int iy= size * .1; iy<size * .5; iy++) {
			for(unsigned int iz= size * .1; iz<size * .5; iz++) {
				if(ofRandom(1) > .6) {
					addVelocity(ix, iy, iz, force.x, force.y, force.z);
				}
			}
		}
	}
}

void FluidBox::addForceRight(ofVec3f force) {
	for(unsigned int ix=size*.9; ix<size-1; ix++) {
		for(unsigned int iy=size * .25; iy<size * .75; iy++) {
			for(unsigned int iz= size *.25; iz<size * .75; iz++) {
				if(ofRandom(1) > .6) {
					addVelocity(ix, iy, iz, force.x, force.y, force.z);
				}
			}
		}
	}
}


void FluidBox::addParticles(int numParticlesX, int numParticlesY, int numParticlesZ,
                            float x, float y, float z, float width, float height, float depth) {
	if (numParticlesX == 0 || numParticlesY == 0 || numParticlesZ == 0)
		return;

	for (float xi = (width / (numParticlesX)) / 2; xi <= width; xi += width / (numParticlesX)) {
		for (float yi = (height / (numParticlesY)) / 2; yi <= height; yi += height / (numParticlesY)) {
			for (float zi = (depth / (numParticlesZ)) / 2; zi <= depth; zi += depth / (numParticlesZ)) {
				particles.push_back(ofVec3f(xi, yi, zi));
			}
		}
	}
}


void FluidBox::addParticle(float x, float y, float z) {
	particles.push_back(ofVec3f(x, y, z));
}

void FluidBox::advanceParticles() {
	float xVel = 0;
	float yVel = 0;
	float zVel = 0;
	ofVec3f point;

	//advance all particles by velocity at current cell times dt
	for (unsigned int i = 0; i < particles.size(); i++) {
		point = particles[i];
		xVel = trilinearInterpArray(velX, point.x, point.y, point.z);
		yVel = trilinearInterpArray(velY, point.x, point.y, point.z);
		zVel = trilinearInterpArray(velZ, point.x, point.y, point.z);

		xVel *= particleSpeed;
		yVel *= particleSpeed;
		zVel *= particleSpeed;

		//add velocity and keep in bounds
		particles[i].x = MIN(MAX(particles[i].x + xVel * dt, 0), size-1);
		particles[i].y = MIN(MAX(particles[i].y + yVel * dt, 0), size-1);
		particles[i].z = MIN(MAX(particles[i].z + zVel * dt, 0), size-1);
	}
}


inline float FluidBox::trilinearInterpArray(float* anArray, float x, float y, float z) {
	return trilinearInterpolate(x - (int)x, y - (int)y, z - (int)z,
	                            anArray[getArrayIndex(floor(x), floor(y), floor(z))],
	                            anArray[getArrayIndex(ceil(x), floor(y), floor(z))],
	                            anArray[getArrayIndex(floor(x), ceil(y), floor(z))],
	                            anArray[getArrayIndex(floor(x), floor(y), ceil(z))],
	                            anArray[getArrayIndex(ceil(x), ceil(y), floor(z))],
	                            anArray[getArrayIndex(ceil(x), floor(y), ceil(z))],
	                            anArray[getArrayIndex(floor(x), ceil(y), ceil(z))],
	                            anArray[getArrayIndex(ceil(x), ceil(y), ceil(z))]);
}

void FluidBox::swap(float* a, float* b) {
	for(int i=0; i<size*size*size; i++) {
		float temp = a[i];
		a[i] = b[i];
		b[i] = temp;
	}
}


void FluidBox::keepInBounds(int b, float *x, int N) {
	//use the value of b to determine which dimension to keep in bounds

	//mirror all values in x array at boundaries to keep them from escaping
	for(int j = 1; j < N - 1; j++) {
		for(int i = 1; i < N - 1; i++) {
			x[getArrayIndex(i, j, 0  )] = b == 3 ? -x[getArrayIndex(i, j, 1  )] : x[getArrayIndex(i, j, 1  )];
			x[getArrayIndex(i, j, N-1)] = b == 3 ? -x[getArrayIndex(i, j, N-2)] : x[getArrayIndex(i, j, N-2)];
		}
	}
	for(int k = 1; k < N - 1; k++) {
		for(int i = 1; i < N - 1; i++) {
			x[getArrayIndex(i, 0  , k)] = b == 2 ? -x[getArrayIndex(i, 1  , k)] : x[getArrayIndex(i, 1  , k)];
			x[getArrayIndex(i, N-1, k)] = b == 2 ? -x[getArrayIndex(i, N-2, k)] : x[getArrayIndex(i, N-2, k)];
		}
	}
	for(int k = 1; k < N - 1; k++) {
		for(int j = 1; j < N - 1; j++) {
			x[getArrayIndex(0  , j, k)] = b == 1 ? -x[getArrayIndex(1  , j, k)] : x[getArrayIndex(1  , j, k)];
			x[getArrayIndex(N-1, j, k)] = b == 1 ? -x[getArrayIndex(N-2, j, k)] : x[getArrayIndex(N-2, j, k)];
		}
	}

	x[getArrayIndex(0, 0, 0)]       = 0.33f * (x[getArrayIndex(1, 0, 0)]
	                                  + x[getArrayIndex(0, 1, 0)]
	                                  + x[getArrayIndex(0, 0, 1)]);
	x[getArrayIndex(0, N-1, 0)]     = 0.33f * (x[getArrayIndex(1, N-1, 0)]
	                                  + x[getArrayIndex(0, N-2, 0)]
	                                  + x[getArrayIndex(0, N-1, 1)]);
	x[getArrayIndex(0, 0, N-1)]     = 0.33f * (x[getArrayIndex(1, 0, N-1)]
	                                  + x[getArrayIndex(0, 1, N-1)]
	                                  + x[getArrayIndex(0, 0, N)]);
	x[getArrayIndex(0, N-1, N-1)]   = 0.33f * (x[getArrayIndex(1, N-1, N-1)]
	                                  + x[getArrayIndex(0, N-2, N-1)]
	                                  + x[getArrayIndex(0, N-1, N-2)]);
	x[getArrayIndex(N-1, 0, 0)]     = 0.33f * (x[getArrayIndex(N-2, 0, 0)]
	                                  + x[getArrayIndex(N-1, 1, 0)]
	                                  + x[getArrayIndex(N-1, 0, 1)]);
	x[getArrayIndex(N-1, N-1, 0)]   = 0.33f * (x[getArrayIndex(N-2, N-1, 0)]
	                                  + x[getArrayIndex(N-1, N-2, 0)]
	                                  + x[getArrayIndex(N-1, N-1, 1)]);
	x[getArrayIndex(N-1, 0, N-1)]   = 0.33f * (x[getArrayIndex(N-2, 0, N-1)]
	                                  + x[getArrayIndex(N-1, 1, N-1)]
	                                  + x[getArrayIndex(N-1, 0, N-2)]);
	x[getArrayIndex(N-1, N-1, N-1)] = 0.33f * (x[getArrayIndex(N-2, N-1, N-1)]
	                                  + x[getArrayIndex(N-1, N-2, N-1)]
	                                  + x[getArrayIndex(N-1, N-1, N-2)]);
}

void FluidBox::solveLinearSystem(int b, float *x, float *x0,
                                 float a, float c, int iter, int N) {
	//iteratively solves linear system for *x, using initial vector x0,
	//size N, iterations iter

	float cInverted = 1.0 / c; //to speed up, since * is faster
	for (int k = 0; k < iter; k++) {
		for (int m = 1; m < N - 1; m++) {
			for (int j = 1; j < N - 1; j++) {
				for (int i = 1; i < N - 1; i++) {
					x[getArrayIndex(i, j, m)] =
					    (x0[getArrayIndex(i, j, m)]
					     + a*(    x[getArrayIndex(i+1, j  , m  )]
					              +x[getArrayIndex(i-1, j  , m  )]
					              +x[getArrayIndex(i  , j+1, m  )]
					              +x[getArrayIndex(i  , j-1, m  )]
					              +x[getArrayIndex(i  , j  , m+1)]
					              +x[getArrayIndex(i  , j  , m-1)]
					         )) * cInverted;
				}
			}
		}
		keepInBounds(b, x, N);
	}
}

void FluidBox::diffuse(int b, float *x, float *x0, float diffuseRate,
                       float dt, int iter, int N) {
	//use poisson equation to diffuse + solve for next state
	float a = dt * diffuseRate * (N - 2) * (N - 2);
	solveLinearSystem(b, x, x0, a, 1 + 6 * a, iter, N);
}


void FluidBox::advect(int b, float *d, float *d0,  float *velocX,
                      float *velocY, float *velocZ, float dt, int N) {
	//move the velocity field by 1 time step
	float i0, i1, j0, j1, k0, k1;

	float dtx = dt * (N - 2);
	float dty = dt * (N - 2);
	float dtz = dt * (N - 2);

	float s0, s1, t0, t1, u0, u1;
	float tmp1, tmp2, tmp3, x, y, z;

	float Nfloat = N;
	float ifloat, jfloat, kfloat;
	int i, j, k;

	//apply velocity to each cell
	for(k = 1, kfloat = 1; k < N - 1; k++, kfloat++) {
		for(j = 1, jfloat = 1; j < N - 1; j++, jfloat++) {
			for(i = 1, ifloat = 1; i < N - 1; i++, ifloat++) {
				tmp1 = dtx * velocX[getArrayIndex(i, j, k)];
				tmp2 = dty * velocY[getArrayIndex(i, j, k)];
				tmp3 = dtz * velocZ[getArrayIndex(i, j, k)];
				x    = ifloat - tmp1;
				y    = jfloat - tmp2;
				z    = kfloat - tmp3;

				if(x < 0.5f) x = 0.5f;
				if(x > Nfloat + 0.5f) x = Nfloat + 0.5f;
				i0 = floorf(x);
				i1 = i0 + 1.0f;
				if(y < 0.5f) y = 0.5f;
				if(y > Nfloat + 0.5f) y = Nfloat + 0.5f;
				j0 = floorf(y);
				j1 = j0 + 1.0f;
				if(z < 0.5f) z = 0.5f;
				if(z > Nfloat + 0.5f) z = Nfloat + 0.5f;
				k0 = floorf(z);
				k1 = k0 + 1.0f;

				s1 = x - i0;
				s0 = 1.0f - s1;
				t1 = y - j0;
				t0 = 1.0f - t1;
				u1 = z - k0;
				u0 = 1.0f - u1;

				int i0i = (int)i0;
				int i1i = (int)i1;
				int j0i = (int)j0;
				int j1i = (int)j1;
				int k0i = (int)k0;
				int k1i = (int)k1;

				d[getArrayIndex(i, j, k)] =

				    s0 * ( t0 * (u0 * d0[getArrayIndex(i0i, j0i, k0i)]
				                 +u1 * d0[getArrayIndex(i0i, j0i, k1i)])
				           +( t1 * (u0 * d0[getArrayIndex(i0i, j1i, k0i)]
				                    +u1 * d0[getArrayIndex(i0i, j1i, k1i)])))
				    +s1 * ( t0 * (u0 * d0[getArrayIndex(i1i, j0i, k0i)]
				                  +u1 * d0[getArrayIndex(i1i, j0i, k1i)])
				            +( t1 * (u0 * d0[getArrayIndex(i1i, j1i, k0i)]
				                     +u1 * d0[getArrayIndex(i1i, j1i, k1i)])));
			}
		}
	}
	//reflect at boundaries to keep in
	keepInBounds(b, d, N);
}

void FluidBox::convect(float *velocX, float *velocY, float *velocZ, float *p, float *div, int iter, int N) {
	for (int k = 1; k < N - 1; k++) {
		for (int j = 1; j < N - 1; j++) {
			for (int i = 1; i < N - 1; i++) {
				div[getArrayIndex(i, j, k)] = -0.5f*(
				                                  velocX[getArrayIndex(i+1, j  , k  )]
				                                  -velocX[getArrayIndex(i-1, j  , k  )]
				                                  +velocY[getArrayIndex(i  , j+1, k  )]
				                                  -velocY[getArrayIndex(i  , j-1, k  )]
				                                  +velocZ[getArrayIndex(i  , j  , k+1)]
				                                  -velocZ[getArrayIndex(i  , j  , k-1)]
				                              )/N;
				p[getArrayIndex(i, j, k)] = 0;
			}
		}
	}
	keepInBounds(0, div, N);
	keepInBounds(0, p, N);
	solveLinearSystem(0, p, div, 1, 6, iter, N);

	for (int k = 1; k < N - 1; k++) {
		for (int j = 1; j < N - 1; j++) {
			for (int i = 1; i < N - 1; i++) {
				velocX[getArrayIndex(i, j, k)] -= 0.5f * (  p[getArrayIndex(i+1, j, k)]
				                                  -p[getArrayIndex(i-1, j, k)]) * N;
				velocY[getArrayIndex(i, j, k)] -= 0.5f * (  p[getArrayIndex(i, j+1, k)]
				                                  -p[getArrayIndex(i, j-1, k)]) * N;
				velocZ[getArrayIndex(i, j, k)] -= 0.5f * (  p[getArrayIndex(i, j, k+1)]
				                                  -p[getArrayIndex(i, j, k-1)]) * N;
			}
		}
	}
	keepInBounds(1, velocX, N);
	keepInBounds(2, velocY, N);
	keepInBounds(3, velocZ, N);
}

void FluidBox::floatsToVec() {
	for (unsigned int i = 0; i < size*size*size; i++) {
		velocity[i].set(velX[i], velY[i], velZ[i]);
	}
}

void FluidBox::vecToFloats() {
	for (unsigned int i = 0; i < size*size*size; i++) {
		velX[i] = velocity[i].x;
		velY[i] = velocity[i].y;
		velZ[i] = velocity[i].z;
	}
}
