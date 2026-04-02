// Lily particle h!!!!!!

#include "noise1234.h"

#define PARTICLE_COUNT 1000

float * particlePos;
float * particleVel;

void initParticle() {
	particlePos = malloc(3 * PARTICLE_COUNT * 4);
	particleVel = malloc(3 * PARTICLE_COUNT * 4);
}

void randomizeParticlePos() {
	for(int i = 0; i < PARTICLE_COUNT; i++) {
		int i3 = i * 3;
		particlePos[i3] = (((float)rand()/(float)(RAND_MAX)) * 5.0f) - 2.5f;
		particlePos[i3+1] = (((float)rand()/(float)(RAND_MAX)) * 5.0f) - 2.5f;
		particlePos[i3+2] = 0.0f;

		particleVel[i3] = ((((float)rand()/(float)(RAND_MAX)) * 1.0f) - 0.50f) * 0.01f;
		particleVel[i3+1] = ((((float)rand()/(float)(RAND_MAX)) * 1.0f) - 0.50f) * 0.01f;
		particleVel[i3+2] = 0.0f;
	}
} 

void particleTick(float delta, float turbseed) {
	float deltaFactor = delta/(1001.0f/60.0f);
	float turbstrength = 0.001f * deltaFactor;
	float dd = 1.1f; //dimensional detail

	for(int i = 0; i < PARTICLE_COUNT; i++) {
		int i3 = i*3;
		int i31 = i3+1;
		int i32 = i3+2;
		float px = particlePos[i3]; float py = particlePos[i31]; float pz = particlePos[i32];

		//Turbulence
		float diffperpart = (float)i * 0.00001f;
		particleVel[i3  ] += turbstrength * noise4(dd*px, dd*py, dd*pz, turbseed + diffperpart); 
		particleVel[i31] += turbstrength * noise4(dd*px, dd*py, dd*pz, turbseed + 1.0f + diffperpart); 
		particleVel[i32] += turbstrength * noise4(dd*px, dd*py, dd*pz, turbseed + 2.0f + diffperpart); 
		particleVel[i32] += 0.0001f;

		//Drag
		float dragConst = 0.9f; //frame dependent
		particleVel[i3] *= dragConst; 
		particleVel[i31] *= dragConst; 
		particleVel[i32] *= dragConst; 

		//Apply vel
		particlePos[i3 ] += particleVel[i3 ];
		particlePos[i31] += particleVel[i31];
		particlePos[i32] += particleVel[i32];
		
	}	
}