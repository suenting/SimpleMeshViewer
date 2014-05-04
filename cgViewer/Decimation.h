#ifndef DECIMATION_H
#define DECIMATION_H
#include "pch.h"
#include "Model.h"
class Decimation
{
public:
	static void QuadraticMultipleChoiceEdgeDecimation(Model &model, size_t numCandidates, size_t numIterations);
private:
	static void CollapseEdge(Model &model, Edge* edge);

	static void RemoveFaceFromList(Model& model, std::vector<Face*> &removeFaceList);
	static void RemoveEdgeFromList(Model& model, std::vector<Edge*> &removeEdgeList);
	static void RemoveVertexFromList(Model& model, std::vector<Vertex*> &removeVertexList);

	static void MergeVerticiesFromList(Model& model, Vertex* newVertex, std::vector<Vertex*> &mergeVertexList);
	static void MergeEdges(Model& model, std::vector<Edge*> &mergeEdgeList);

	Decimation(void);
	~Decimation(void);
};

#endif
