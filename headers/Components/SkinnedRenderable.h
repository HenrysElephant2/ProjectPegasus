#ifndef SKINNED_RENDERABLE_H
#define SKINNED_RENDERABLE_H

#include "Component.h"

struct SkinnedRenderable:Component {
	GLuint VBO;
	GLuint IBO;
	int numVertices;
	int program; // index into shadermanager
	Material material;
	BoneHierarchy bones;
	SkinnedRenderable(GLuint VBO_in, GLuint IBO_in, int vertex_Count, int program_in, Material & material_in, int ownerID, BoneHierarchy &bones_in):Component(ownerID)
	{
		VBO = VBO_in;
		IBO = IBO_in;
		numVertices = vertex_Count;
		program = program_in;
		material = material_in;
		bones = bones_in;
	}
	SkinnedRenderable(){}
	SkinnedRenderable( const SkinnedRenderable &obj)
	{
		VBO = obj.VBO;
		IBO = obj.IBO;
		numVertices = obj.numVertices;
		program = obj.program;
		material = obj.material;
		bones = obj.bones;
	}

	void readFromXML(XmlElement * element);
};

#endif