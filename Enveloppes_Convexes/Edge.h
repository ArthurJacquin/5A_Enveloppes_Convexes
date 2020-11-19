#pragma once
#include <vector>
#include "Vertex.h"
#include "Vec3.h"
#include "OpenGLcore.h"

class Edge
{
	std::vector<Vertex*> edgePoints;
	bool exterior;
	Vec3 normale;
	std::vector<Vertex> bufferPts;

	uint32_t VBO;

public:
	Edge();
	Edge(Vertex* v1, Vertex* v2);

	std::vector<Vertex*>getEdgePoints() const { return edgePoints; }
	bool getSide() const { return exterior; }
	Vec3 getNormale() const { return normale; }

	void setInterior() { exterior = false; }
	void reCalculateNormale() { normale = Vec3(edgePoints[1]->x - edgePoints[0]->x, -(edgePoints[1]->y - edgePoints[0]->y), 0); }
	
	void updateBuffers();
	uint32_t GetVBO()const { return VBO; }
	void updateBufferPoints();

};