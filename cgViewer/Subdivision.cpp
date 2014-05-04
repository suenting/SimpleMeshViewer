#include "Subdivision.h"

namespace
{
	static float beta (int n)
	{
		return (1.f/n)*((5.f/8.f)- (((3.f/8.f)+ (0.25f)*cos((2*Math::PI)/n))*((3.f/8.f)+ (0.25f)*cos((2*Math::PI)/n))));
	}
	static float alpha(int n)
	{
		return 1.f-(n*beta(n));
	}

	/*
		 3\
		/  \
	   /    \
	  5		 6
	 /		  \
	1____4_____2

		 /\
		/3 \
	   /____\
	  / \2 / \
	 /_1_\/_4_\

	*/

}

Subdivision::Subdivision(void)
{
}

Subdivision::~Subdivision(void)
{
}

// assumes that the model has already been triangulated
void Subdivision::LoopSubdivision(Model &model, bool enableMask)
{
	LoopGenerateNewFaces(model);
	if(enableMask)
	{
		LoopRepositionVertices(model);
	}

	// update the adjacent edge list withing each vertex, (may want to optimize this later...)
	for(std::vector<Vertex*>::iterator vertexIter = model.mVertexList.begin(); vertexIter != model.mVertexList.end(); ++vertexIter)
	{
		Vertex * pVertex = *vertexIter;
		pVertex->adjacentEdgeList.clear();
	}
	for(std::vector<Edge*>::iterator edgeIter = model.mEdgeList.begin(); edgeIter != model.mEdgeList.end(); ++ edgeIter)
	{
		Edge* pEdge = *edgeIter;
		pEdge->vertex1->adjacentEdgeList.push_back(pEdge);
		pEdge->vertex2->adjacentEdgeList.push_back(pEdge);
	}

	model.GenerateDisplayLists();
}

// assumes triangles
void Subdivision::LoopRepositionVertices(Model & model)
{
	std::map<Vertex*,Math::Vector3f> newVertexPositions;
	
	for(std::vector<Vertex*>::iterator it = model.mVertexList.begin(); it != model.mVertexList.end(); ++it)
	{
		Vertex *vertex = (*it);
		// get adj verticies
		std::set<Vertex*> adjVertexList;
		for(std::vector<Face>::const_iterator fIt = vertex->subDivAdjFaceList.begin(); fIt != vertex->subDivAdjFaceList.end(); ++fIt)
		{
			Face face = (*fIt);
			for(std::vector<Vertex*>::const_iterator vertIt = face.vertex.begin(); vertIt != face.vertex.end(); ++vertIt)
			{
				if((*vertIt) != vertex)
				{
					adjVertexList.insert((*vertIt));
				}
			}
		}
		float x = 0;
		float y = 0;
		float z = 0;
		size_t k = adjVertexList.size();
		Vertex currVertex = *vertex;
		currVertex = currVertex * alpha(k);
		x += currVertex.x;
		y += currVertex.y;
		z += currVertex.z;
		for(std::set<Vertex*>::const_iterator adjVertIter = adjVertexList.begin();adjVertIter != adjVertexList.end(); ++adjVertIter)
		{
			currVertex = (*(*adjVertIter));
			currVertex = currVertex * beta(k);
			x += currVertex.x;
			y += currVertex.y;
			z += currVertex.z;
		}
		Math::Vector3f nextPos(x,y,z);
		newVertexPositions[vertex] = nextPos;
		vertex->subDivAdjFaceList.clear();
	}

	for(std::map<Vertex*,Math::Vector3f>::iterator updateVertexIter = newVertexPositions.begin(); 
		updateVertexIter != newVertexPositions.end();
		++ updateVertexIter)
	{
		updateVertexIter->first->x = updateVertexIter->second.x;
		updateVertexIter->first->y = updateVertexIter->second.y;
		updateVertexIter->first->z = updateVertexIter->second.z;
	}
}

void Subdivision::LoopGenerateNewFaces(Model & model)
{
	//step 1 create a new vertex for each edge
	std::vector<Edge*> newEdgeList;
	for(std::vector<Edge*>::iterator it = model.mEdgeList.begin(); it != model.mEdgeList.end(); ++it)
	{
		Edge *pEdge = *it;
		Vertex *centerVertex = new Vertex(((*pEdge->vertex1)+(*pEdge->vertex2))/2);
		model.mVertexList.push_back((centerVertex));
		pEdge->centerVertex = centerVertex;

		// create new midpoint edges list for later
		Edge *nEdge1 = new Edge((pEdge->vertex1),centerVertex);
		Edge *nEdge2 = new Edge((pEdge->vertex2),centerVertex);
		newEdgeList.push_back(nEdge1);
		newEdgeList.push_back(nEdge2);

		pEdge->leftChild = nEdge1;
		pEdge->rightChild = nEdge2;
	}

	//step 2 subdivide all faces, assumes triangle mesh, creates new facelist
	std::vector<Face*> newFaceList;
	for(std::vector<Face*>::const_iterator i = model.mFaceList.begin(); i != model.mFaceList.end(); ++i)
	{
		Face *face = (*i);
		Vertex* v1 = face->vertex[0];
		Vertex* v2 = face->vertex[1];
		Vertex* v3 = face->vertex[2];
		Vertex* v4 ;// vertex between v1/v2
		Vertex* v5 ;// vertex between v1/v3
		Vertex* v6 ;// vertex between v2/v3

		std::vector<Edge*> tempEdgeList;
		tempEdgeList.push_back(face->edge[0]);
		tempEdgeList.push_back(face->edge[1]);
		tempEdgeList.push_back(face->edge[2]);
		for(std::vector<Edge*>::const_iterator tEdgeIter = tempEdgeList.begin();
			tEdgeIter != tempEdgeList.end();
			++tEdgeIter)
		{
			Edge * edge = *tEdgeIter;
			if(	(edge->vertex1 == v1 || edge->vertex1 == v2) &&
				(edge->vertex2 == v1 || edge->vertex2 == v2))
			{
				v4 = edge->centerVertex;
			}
			else if((edge->vertex1 == v1 || edge->vertex1 == v3) &&
					(edge->vertex2 == v1 || edge->vertex2 == v3))
			{
				v5 = edge->centerVertex;
			}
			else if((edge->vertex1 == v2 || edge->vertex1 == v3) &&
					(edge->vertex2 == v2 || edge->vertex2 == v3))
			{
				v6 = edge->centerVertex;
			}
		}
		// store the adjacent face list into the vertex
		v1->subDivAdjFaceList.push_back(*face);
		v2->subDivAdjFaceList.push_back(*face);
		v3->subDivAdjFaceList.push_back(*face);
		v4->subDivAdjFaceList.push_back(*face);
		v5->subDivAdjFaceList.push_back(*face);
		v6->subDivAdjFaceList.push_back(*face);

		// create new faces
		Face* newFace1 = new Face();
		newFace1->vertex.push_back(v1); newFace1->vertex.push_back(v4);	newFace1->vertex.push_back(v5);	
		Face* newFace2 = new Face();
		newFace2->vertex.push_back(v4); newFace2->vertex.push_back(v2);	newFace2->vertex.push_back(v6);	
		Face* newFace3 = new Face();
		newFace3->vertex.push_back(v5); newFace3->vertex.push_back(v6);	newFace3->vertex.push_back(v3);	
		Face* newFace4 = new Face();
		newFace4->vertex.push_back(v5); newFace4->vertex.push_back(v4);	newFace4->vertex.push_back(v6);

		Edge* newEdge1 = new Edge(v4,v5);//45
		Edge* newEdge2 = new Edge(v4,v6);//46
		Edge* newEdge3 = new Edge(v5,v6);//56
		Edge*  edge14;
		Edge*  edge15;
		Edge*  edge24;
		Edge*  edge26;
		Edge*  edge36;
		Edge*  edge35;
		for(std::vector<Edge*>::const_iterator tEdgeIter = tempEdgeList.begin();
			tEdgeIter != tempEdgeList.end();
			++tEdgeIter)
		{
			Edge * edge = *tEdgeIter;
			Edge * newEdge;
			for(int doTwice = 0; doTwice != 2; ++doTwice)
			{
				if(doTwice == 0)
				{
					newEdge = edge->leftChild;
				}
				else
				{
					newEdge = edge->rightChild;
				}

				if(	(newEdge->vertex1 == v1 || newEdge->vertex1 == v4) &&
					(newEdge->vertex2 == v1 || newEdge->vertex2 == v4))
				{
					edge14 = newEdge;
				}
				else if((newEdge->vertex1 == v1 || newEdge->vertex1 == v5) &&
						(newEdge->vertex2 == v1 || newEdge->vertex2 == v5))
				{
					edge15 = newEdge;
				}
				else if((newEdge->vertex1 == v2 || newEdge->vertex1 == v4) &&
						(newEdge->vertex2 == v2 || newEdge->vertex2 == v4))
				{
					edge24 = newEdge;
				}
				else if((newEdge->vertex1 == v2 || newEdge->vertex1 == v6) &&
						(newEdge->vertex2 == v2 || newEdge->vertex2 == v6))
				{
					edge26 = newEdge;
				}
				else if((newEdge->vertex1 == v3 || newEdge->vertex1 == v6) &&
						(newEdge->vertex2 == v3 || newEdge->vertex2 == v6))
				{
					edge36 = newEdge;
				}
				else if((newEdge->vertex1 == v3 || newEdge->vertex1 == v5) &&
						(newEdge->vertex2 == v3 || newEdge->vertex2 == v5))
				{
					edge35 = newEdge;
				}
			}
		}
		// store new edges
		newEdgeList.push_back(newEdge1);
		newEdgeList.push_back(newEdge2);
		newEdgeList.push_back(newEdge3);

		// store faces, and maintain adjacent edge list
		newFaceList.push_back(newFace1);
		newFace1->edge.push_back(newEdge1);
		newFace1->edge.push_back(edge14);
		newFace1->edge.push_back(edge15);

		newFaceList.push_back(newFace2);
		newFace2->edge.push_back(newEdge2);
		newFace2->edge.push_back(edge24);
		newFace2->edge.push_back(edge26);

		newFaceList.push_back(newFace3);
		newFace3->edge.push_back(newEdge3);
		newFace3->edge.push_back(edge36);
		newFace3->edge.push_back(edge35);

		newFaceList.push_back(newFace4);
		newFace4->edge.push_back(newEdge1);
		newFace4->edge.push_back(newEdge2);
		newFace4->edge.push_back(newEdge3);
	}

	// update adjacent face list in each edge
	for(std::vector<Face*>::const_iterator faceIt = newFaceList.begin(); faceIt != newFaceList.end(); ++faceIt)
	{
		Face* pFace = *faceIt;
		for(std::vector<Edge*>::iterator edgeIt = pFace->edge.begin(); edgeIt != pFace->edge.end(); ++ edgeIt)
		{
			(*edgeIt)->faceList.push_back(pFace);
		}
	}

	model.ClearFaceList();
	model.mFaceList = newFaceList;
	model.ClearEdgeList();
	model.mEdgeList = newEdgeList;
}