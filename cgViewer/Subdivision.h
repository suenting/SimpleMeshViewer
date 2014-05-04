#ifndef SUBDIVISION_H
#define SUBDIVISION_H

#include "pch.h"
#include "Model.h"

class Subdivision
{
public:
	static void LoopSubdivision(Model& model, bool enableMask);
	//static	ModelFactory* Instance();
private:

	static inline void LoopGenerateNewFaces(Model& model);
	static inline void LoopRepositionVertices(Model& model);

	static inline size_t AddVertexToModel(Model& model, Math::Vector3f vertex);

	// generates list of edges for a given model, assumes model has been triangulated
	static void	GenerateEdgeList(Model& model, std::vector<Edge>& edgeList);

	Subdivision(void);
	~Subdivision(void);
};

#endif
