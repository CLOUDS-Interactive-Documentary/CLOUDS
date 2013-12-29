#ifndef GRIDFLUID_H
#define GRIDFLUID_H

#include "ofMain.h"
//#include "Fluid.h"
//#include "haganlib.h"
//#include "Vector3.h"

#include <cmath>
#include <cstdlib>
#include <vector>


/**
 * A fluid simulator that simulates a cube (NxNxN) over time.
 * Works by solving a Poisson equation for the density,
 * and advection of the velocity field to determine how velocity changes.
 */
class FluidBox {
public:

	/** Size n for each dimension of the cube. */
	unsigned int size;

	/** Location of "marker particles" that represent the fluid. */
	std::vector<ofVec3f> particles;

	FluidBox();

	~FluidBox();

	void setup(int nSize, float viscosity = 0.001);
	void update();

	void draw();

	void addVelocity(int x, int y, int z, float amountX, float amountY, float amountZ);
	ofVec3f getVelocityAt(int x, int y, int z);

	void addParticles(int numParticlesX, int numParticlesY, int numParticlesZ,
		float x, float y, float z, float width, float height, float depth);
	void addParticle(float x, float y, float z);

	void addForceLeft(ofVec3f force);
	void addForceRight(ofVec3f force);
	void addForceAt(ofVec3f pos, ofVec3f force);
	void addForceAt(int ix, int iy, int iz, ofVec3f force);

	float width, height, depth;

	bool drawLines;
	bool drawForceField;
	bool drawCorners;

	float viscosity;
	float particleSpeed;

private:
	void floatsToVec();
	void vecToFloats();

	int getArrayIndex(int x, int y, int z);

	void swap(float* a, float * b);

	/** Advances particles based on location in velocity field and time step. */
	void advanceParticles();

	/**
	 * Keeps the fluid in its bounds by setting velocity at bounds equal and
	 * opposite to its neighbor.
	 */
	void keepInBounds(int b, float *x, int N);

	/**
	 * Solves a linear system of equations
	 * @param x		The pointer to the vector returned
	 * @param x0	The initial vector
	 * @param a		The matrix?
	 */
	void solveLinearSystem(int b, float *x, float *x0, float a, float c, int iter, int N);

	/**
	 * Diffuses, or spreads out the fluid by solving a Poisson equation.
	 */
	void diffuse(int b, float *x, float *x0, float diffuseRate, float dt, int iter, int N);

	/**
	 * Advances the velocity field of the fluid.
	 * (moves the velocity at each point to update the velocities on the grid)
	 */
	void advect(int b, float *d, float *d0,  float *velocX, float *velocY, float *velocZ, float dt, int N);

	/**
	 * Applies convection with a backwards particle trace.
	 * Refer to FluidFlowForTheRestOfUs:
	 * (http://poseidon.cs.byu.edu/~cline/fluidFlowForTheRestOfUs.pdf)
		The velocity field is convected using the backwards particle trace
		method described in [Stam 1999]. A backwards particle trace starts
		at the location for which we want to update the velocity field, x =
		(x, y, z). A particle is traced from x backwards through u for
		−Dt time, arriving at point y. The velocity at x is then replaced
		by the current velocity at point y. Note that this step cannot be
		done in place, so we use the temporary vectors in the cells to store
		intermediate values, and then copy the temporary values to u after
		all of the particles have been traced.
	 */
	void convect(float *velocX, float *velocY, float *velocZ, float *p, float *div,
	             int iter, int N);

	/**
	 * Returns the value of a point by trilinear interpolation
	 * for an index in an array.
	 * x,y,z, must be between [0,1]
	 */
	inline float trilinearInterpArray(float* anArray, float x, float y, float z);

	/** Time step for the simulation. */
	float dt;


	float *velX;
	float *velY;
	float *velZ;

	float *velXOld;
	float *velYOld;
	float *velZOld;

	long timeOld;

	std::vector<ofVec3f> velocity;

	ofMesh meshVelocity;
	ofMesh box;

	float cornerSize;
	ofVec3f gravity;
};





#endif //FLUID_H
