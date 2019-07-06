#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "Component.h"

struct ParticleSystem:Component {
	GLuint VBO;
	int nParticles;
	int program;
	ParticleSystem( GLuint VBO_in, int numParts, int program_in ) {
		VBO = VBO_in;
		nParticles = numParts;
		program = program_in;
	}
	ParticleSystem(){}
	void readFromXML(XmlElement * element);
};

#endif