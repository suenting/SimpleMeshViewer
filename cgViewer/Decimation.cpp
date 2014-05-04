#include "Decimation.h"


namespace
{
	static float GetQuadricError(Vertex* v)
	{
		Math::Vector4f self = Math::Vector4f(v->x,v->y,v->z,0);
		Math::Vector4f temp = v->quadric * self;
		float out = self%temp;
		return out;
	}
}

Decimation::Decimation(void)
{
}

Decimation::~Decimation(void)
{
}

// assumes triangulated matrix
void Decimation::QuadraticMultipleChoiceEdgeDecimation(Model &model,size_t numCandidates, size_t numIterations)
{
	// zero all quadrics
	for(std::vector<Vertex*>::iterator vertexIter = model.mVertexList.begin(); vertexIter != model.mVertexList.end(); ++vertexIter)
	{
		(*vertexIter)->quadric = Math::Matrix4f::GetZeroMatrix();
	}

	// calculate the sum quadric for all faces surrounding each vertex
	for(std::vector<Face*>::const_iterator faceIter = model.mFaceList.begin(); faceIter != model.mFaceList.end(); ++ faceIter)
	{
		Face *face = *faceIter;
		Math::Vector3f pt1 = *face->vertex[0];
		Math::Vector3f pt2 = *face->vertex[1];
		Math::Vector3f pt3 = *face->vertex[2];
		Math::Vector3f v1 = pt2-pt1;
		Math::Vector3f v2 = pt3-pt1;
		Math::Vector3f normal = v1*v2;
		normal.NormalizeSelf();

		float a = normal.x;
		float b = normal.y;
		float c = normal.z;

		for(std::vector<Vertex*>::const_iterator vIt = face->vertex.begin(); vIt != face->vertex.end();++vIt)
		{
			Vertex* vertex = *vIt;
			float d = -1* (a*vertex->x + b*vertex->y + c*vertex->z);
			Math::Matrix4f q;
			q.m00 = a*a; q.m01 = a*b; q.m02 = a*c; q.m03 = a*d;
			q.m10 = a*b; q.m11 = b*b; q.m12 = b*c; q.m13 = b*d;
			q.m20 = a*c; q.m21 = b*c; q.m22 = c*c; q.m23 = c*d;
			q.m30 = a*d; q.m31 = b*d; q.m32 = c*d; q.m33 = d*d;
			vertex->quadric += q;
		}
	}
	for(int iteration = 0; iteration != numIterations; ++iteration)
	{

		float progress = iteration;
		progress = 1.f*((float)iteration/numIterations)*100.f;
		std::cout<<"Decimation progress: "<<progress<<std::endl;

		// early exit if nothing to remove
		if(model.mFaceList.size()<=0)
		{
			break;
		}

		// select x number of random candidates
		std::vector<Edge*> removeEdgeCandidates;
		for(int i = 0; i<numCandidates; ++i)
		{
			// select random edge
			Edge *e = model.mEdgeList[(rand()%model.mEdgeList.size())];
			removeEdgeCandidates.push_back(e);
		}

		//find the edge with the least error
		float currentError = 10000000000000000000.f;// fixme
		Edge *currentEdge = NULL;
		bool foundGoodEdge = false;
		for(std::vector<Edge*>::iterator edgeIter = removeEdgeCandidates.begin(); edgeIter != removeEdgeCandidates.end(); ++ edgeIter)
		{
			Edge *edge = *edgeIter;

			// get outer vertex
			std::vector<Vertex*> outerVerticies = edge->GetOuterVerticies();

			// ensure no outer vertex has degree == 3
			bool skip = false;
			for(std::vector<Vertex*>::iterator vIter = outerVerticies.begin(); vIter != outerVerticies.end(); ++vIter)
			{
				Vertex* pVertex = *vIter;
				if(pVertex->adjacentEdgeList.size() == 3)
				{
					skip = true;
					break;
				}
			}
			if(skip && foundGoodEdge)
			{
				continue;
			}
			if(!skip && !foundGoodEdge)
			{
				foundGoodEdge = true;
			}

			float v1Error = GetQuadricError(edge->vertex1);
			float v2Error = GetQuadricError(edge->vertex2);
			float totalError = fabs(v1Error + v2Error);
			if(totalError<currentError)
			{
				currentError = totalError;
				currentEdge = edge;
			}
		}

		// remove given edge
		if(NULL != currentEdge)
		{
			CollapseEdge(model,currentEdge);
		}
		else
		{
			std::cout<<"Couldn't Find Valid Edge To Remove"<<std::endl;
		}
	}
	model.GenerateDisplayLists();
}


void Decimation::CollapseEdge(Model &model, Edge *edge)
{
	// remove faces connected to this edge
	std::vector<Face*> removeFaceList = edge->faceList;
	
	std::vector<Vertex*> removeVertexList;
	removeVertexList.push_back(edge->vertex1);
	removeVertexList.push_back(edge->vertex2);

	// inner optimization based of 3 samples : v1 , v2 , (v1+v2)/2
	Vertex* newVertex;
	Vertex tempVertexCenter = ((*edge->vertex1)+(*edge->vertex2))/2;
	if(	(GetQuadricError(&tempVertexCenter) <= GetQuadricError(edge->vertex1)) &&
		(GetQuadricError(&tempVertexCenter) <= GetQuadricError(edge->vertex2)))
	{
		newVertex = new Vertex(((*edge->vertex1)+(*edge->vertex2))/2);
		
	}
	else if((GetQuadricError(edge->vertex1) <= GetQuadricError(&tempVertexCenter)) &&
			(GetQuadricError(edge->vertex1) <= GetQuadricError(edge->vertex2)))
	{
		newVertex = new Vertex(*edge->vertex2);
	}
	else if((GetQuadricError(edge->vertex2) <= GetQuadricError(&tempVertexCenter)) &&
			(GetQuadricError(edge->vertex2) <= GetQuadricError(edge->vertex1)))
	{
		newVertex = new Vertex(*edge->vertex2);
	}
	else
	{
		assert(false);
		newVertex = new Vertex(((*edge->vertex1)+(*edge->vertex2))/2);
	}
	// create new vertex
	newVertex->quadric = edge->vertex1->quadric + edge->vertex2->quadric;

	std::vector<Vertex*> edgeVerticies;
	edgeVerticies.push_back(edge->vertex1);
	edgeVerticies.push_back(edge->vertex2);

	// remove faces, and remove face from edges adj face list
	RemoveFaceFromList(model,removeFaceList);

	// merge the verticies, if both verticies still exist in the model
	if((std::find(model.mVertexList.begin(),model.mVertexList.end(),edgeVerticies[0]) != model.mVertexList.end()) &&
	   (std::find(model.mVertexList.begin(),model.mVertexList.end(),edgeVerticies[1]) != model.mVertexList.end()))
	{
		model.mVertexList.push_back(newVertex);
		MergeVerticiesFromList( model, newVertex,edgeVerticies);
	}
	else
	{
		delete newVertex;
	}
}

// assumes edge connecting the two verts are removed and merge vertex list size == 2
void Decimation::MergeVerticiesFromList(Model& model, Vertex* newVertex,std::vector<Vertex*> &mergeVertexList)
{
	std::set<Face*> adjacentFaceList;	// all verticies which will connect to the new vertex
	newVertex->adjacentEdgeList.clear();
	// construct the new vertex's adjacent edge list
	for(std::vector<Vertex*>::iterator vIt = mergeVertexList.begin(); vIt != mergeVertexList.end(); ++vIt)
	{
		Vertex* pVertex = *vIt;
		for(std::vector<Edge*>::iterator eIt = pVertex->adjacentEdgeList.begin(); eIt != pVertex->adjacentEdgeList.end(); ++eIt)
		{
			Edge* pEdge = *eIt;
			newVertex->adjacentEdgeList.push_back(pEdge);
			for(std::vector<Face*>::iterator fIt = pEdge->faceList.begin(); fIt != pEdge->faceList.end(); ++fIt)
			{
				Face* pFace = *fIt;
				adjacentFaceList.insert(pFace);
			}
		}
	}

	for(std::vector<Edge*>::iterator eIt = newVertex->adjacentEdgeList.begin(); eIt != newVertex->adjacentEdgeList.end(); ++eIt)
	{
		Edge* pEdge = *eIt;
		if(std::find(mergeVertexList.begin(),mergeVertexList.end(),pEdge->vertex1) != mergeVertexList.end())
		{
			pEdge->vertex1 = newVertex;
		}
		if(std::find(mergeVertexList.begin(),mergeVertexList.end(),pEdge->vertex2) != mergeVertexList.end())
		{
			pEdge->vertex2 = newVertex;
		}
	}

	for(std::set<Face*>::iterator fIt = adjacentFaceList.begin(); fIt != adjacentFaceList.end(); ++fIt)
	{
		Face* pFace = *fIt;
		for(std::vector<Vertex*>::iterator vIt = pFace->vertex.begin(); vIt != pFace->vertex.end(); ++vIt)
		{
			if(std::find(mergeVertexList.begin(),mergeVertexList.end(),(*vIt)) != mergeVertexList.end())
			{
				(*vIt) = newVertex;
			}
		}
	}
	//remove all verticies in merge vertex list
	for(std::vector<Vertex*>::iterator vIt = mergeVertexList.begin(); vIt != mergeVertexList.end(); ++vIt)
	{
		Vertex* pVertex = *vIt;
		std::vector<Vertex*>::iterator removeIt = std::remove(model.mVertexList.begin(), model.mVertexList.end(),pVertex);
		model.mVertexList.erase(removeIt,model.mVertexList.end());
	}

	// find all lined up edges which can be merged and merge them
	std::set<Edge*> noDuplicateEdgeList;
	std::vector<std::vector<Edge*> > pairedEdgeList;
	for(std::vector<Edge*>::iterator eIt1 = newVertex->adjacentEdgeList.begin();
		eIt1 != newVertex->adjacentEdgeList.end();
		++eIt1)
	{
		Edge* pEdge1 = *eIt1;
		if	(pEdge1->vertex1 == pEdge1->vertex2 || 
			(std::find(noDuplicateEdgeList.begin(),noDuplicateEdgeList.end(),pEdge1) != noDuplicateEdgeList.end()))
		{
			continue;
		}
		std::vector<Edge*> edgePair;
		edgePair.push_back(pEdge1);
		noDuplicateEdgeList.insert(pEdge1);

		for(std::vector<Edge*>::iterator eIt2 = newVertex->adjacentEdgeList.begin();
			eIt2 != newVertex->adjacentEdgeList.end();
			++eIt2)
		{
			Edge* pEdge2 = *eIt2;
			if(pEdge2->vertex1 == pEdge2->vertex2)
			{
				continue;
			}

			if(	(pEdge1->vertex1 == pEdge2->vertex1 || pEdge1->vertex1 == pEdge2->vertex2) &&
				(pEdge1->vertex2 == pEdge2->vertex1 || pEdge1->vertex2 == pEdge2->vertex2) && 
				 pEdge1 != pEdge2)
			{
				edgePair.push_back(pEdge2);
				assert(std::find(noDuplicateEdgeList.begin(),noDuplicateEdgeList.end(),pEdge2) == noDuplicateEdgeList.end());
				noDuplicateEdgeList.insert(pEdge2);
			}
		}
		if(edgePair.size()>1)
		{
			pairedEdgeList.push_back(edgePair);
		}
	}

	// merge edges if all edges still exist within the model
	for(std::vector<std::vector<Edge*> >::iterator pIt = pairedEdgeList.begin();
		pIt != pairedEdgeList.end();
		++pIt)
	{
		std::vector<Edge*> edgePair = *pIt;
		// if all edges still exist
		size_t edgesFound = 0;
		for(std::vector<Edge*>::iterator eIt = edgePair.begin(); eIt != edgePair.end(); ++eIt)
		{
			Edge* pEdge = *eIt;
			if(std::find(model.mEdgeList.begin(),model.mEdgeList.end(),pEdge) != model.mEdgeList.end())
			{
				++edgesFound;
			}
			else
			{
				// posibly break for performance
				assert(edgesFound == 0);
			}
		}

		if(edgesFound == edgePair.size())
		{
			MergeEdges(model,edgePair);
		}
		else
		{
			//assert(edgesFound == 0); //this hits...
		}
	}
}

// assumes all edges in mergeEdgeList have same vertexs
void Decimation::MergeEdges(Model& model, std::vector<Edge*> &mergeEdgeList)
{
	//return;// currently broken

	// verify integrity of this mergeEdgeList
#ifdef DEBUG
	for(std::vector<Edge*>::iterator eIt = mergeEdgeList.begin(); eIt != mergeEdgeList.end(); ++eIt)
	{
		Edge* pEdge = *eIt;
		Edge* sEdge = *mergeEdgeList.begin();

		assert(pEdge->vertex1 == sEdge->vertex1 || pEdge->vertex1 == sEdge->vertex2);
		assert(pEdge->vertex2 == sEdge->vertex1 || pEdge->vertex2 == sEdge->vertex2);
	}
#endif


	Edge *newEdge = new Edge(mergeEdgeList.front()->vertex1, mergeEdgeList.front()->vertex2);
	for(std::vector<Edge*>::iterator eIt = mergeEdgeList.begin(); eIt != mergeEdgeList.end(); ++eIt)
	{
		Edge* pEdge = *eIt;
		// update faces of pEdge
		for(std::vector<Face*>::iterator fIt = pEdge->faceList.begin(); fIt != pEdge->faceList.end(); ++fIt)
		{
			Face* pFace = *fIt;
			// remove pEdge from face's edge list and add newEdge to it if newEdge isn't already there
			std::vector<Edge*>::iterator removeEdgeIter = std::remove(pFace->edge.begin(), pFace->edge.end(), pEdge);
			pFace->edge.erase(removeEdgeIter,pFace->edge.end());
			if(std::find(pFace->edge.begin(), pFace->edge.end(),newEdge) == pFace->edge.end())
			{
				pFace->edge.push_back(newEdge);
			}
			// insert this face into the edges face list if it isnt already there
			if(std::find(newEdge->faceList.begin(), newEdge->faceList.end(), pFace) == newEdge->faceList.end())
			{
				newEdge->faceList.push_back(pFace);
			}
		}
		// update verticies of pEdge
		std::vector<Edge*>::iterator removeEdgeIt = std::remove(pEdge->vertex1->adjacentEdgeList.begin(),
																pEdge->vertex1->adjacentEdgeList.end(),
																pEdge);
		pEdge->vertex1->adjacentEdgeList.erase(removeEdgeIt,pEdge->vertex1->adjacentEdgeList.end());

		removeEdgeIt = std::remove(	pEdge->vertex2->adjacentEdgeList.begin(),
									pEdge->vertex2->adjacentEdgeList.end(),
									pEdge);
		pEdge->vertex2->adjacentEdgeList.erase(removeEdgeIt,pEdge->vertex2->adjacentEdgeList.end());
	}

	// insert newEdge into its verticies adjEdgeList
	newEdge->vertex1->adjacentEdgeList.push_back(newEdge);
	newEdge->vertex2->adjacentEdgeList.push_back(newEdge);

	// add new edge to model
	model.mEdgeList.push_back(newEdge);

	// remove all edges in merge edge list from model
	for(std::vector<Edge*>::iterator eIt = mergeEdgeList.begin(); eIt != mergeEdgeList.end(); ++eIt)
	{
		Edge* pEdge = *eIt;
		std::vector<Edge*>::iterator removeModelEdgeIt = std::remove(model.mEdgeList.begin(),model.mEdgeList.end(),pEdge);
		assert(removeModelEdgeIt != model.mEdgeList.end());
		model.mEdgeList.erase(removeModelEdgeIt,model.mEdgeList.end());
		//std::cout<<pEdge<<std::endl;
		delete pEdge;
	}
}

void Decimation::RemoveVertexFromList(Model& model, std::vector<Vertex*> &removeVertexList)
{
	for(std::vector<Vertex*>::const_iterator vIt = removeVertexList.begin(); vIt != removeVertexList.end(); ++vIt)
	{
		Vertex* pVertex = *vIt;
		std::vector<Vertex*>::iterator removeVIt = std::find(model.mVertexList.begin(), model.mVertexList.end(), pVertex);
		//assert(removeVIt != model.mVertexList.end());
		if(removeVIt != model.mVertexList.end())
		{
			model.mVertexList.erase(removeVIt);
			delete pVertex;
		}
	}
}

void Decimation::RemoveEdgeFromList(Model& model, std::vector<Edge*> &removeEdgeList)
{
	for(std::vector<Edge*>::const_iterator eIt = removeEdgeList.begin(); eIt != removeEdgeList.end(); ++eIt)
	{
		Edge* pEdge = *eIt;
		std::vector<Edge*>::iterator removeEIt = std::find(model.mEdgeList.begin(), model.mEdgeList.end(), pEdge);
		assert(removeEIt != model.mEdgeList.end());
		if(removeEIt != model.mEdgeList.end())
		{
			model.mEdgeList.erase(removeEIt);
		}
		//update verticies connected to this edge
		std::vector<Edge*>::iterator removeEdgeIt;
		removeEdgeIt = std::remove(pEdge->vertex1->adjacentEdgeList.begin(),pEdge->vertex1->adjacentEdgeList.end(),pEdge);
		pEdge->vertex1->adjacentEdgeList.erase(removeEdgeIt,pEdge->vertex1->adjacentEdgeList.end());

		removeEdgeIt = std::remove(pEdge->vertex2->adjacentEdgeList.begin(),pEdge->vertex2->adjacentEdgeList.end(),pEdge);
		pEdge->vertex2->adjacentEdgeList.erase(removeEdgeIt,pEdge->vertex2->adjacentEdgeList.end());

		//if a vertex has no edges they can be removed
		std::vector<Vertex*> removeVertexList;
		if(pEdge->vertex1->adjacentEdgeList.size() == 0)
		{
			removeVertexList.push_back(pEdge->vertex1);
		}
		if(pEdge->vertex2->adjacentEdgeList.size() == 0)
		{
			removeVertexList.push_back(pEdge->vertex2);
		}
		RemoveVertexFromList(model,removeVertexList);
		delete pEdge;
		pEdge = NULL;
	}
}

void Decimation::RemoveFaceFromList(Model& model, std::vector<Face*> &removeFaceList)
{
	for(std::vector<Face*>::const_iterator fIt = removeFaceList.begin(); fIt != removeFaceList.end(); ++fIt)
	{
		Face* pFace = *fIt;
		std::vector<Face*>::iterator removefIt;
		removefIt = std::find(model.mFaceList.begin(), model.mFaceList.end(), (*fIt));
		assert(removefIt != model.mFaceList.end());
		if(removefIt != model.mFaceList.end())
		{
			model.mFaceList.erase(removefIt);
		}

		// remove this face from edges adjacent to this face
		for(std::vector<Edge*>::iterator eIt = pFace->edge.begin(); eIt != pFace->edge.end(); ++eIt)
		{
			Edge *pEdge = *eIt;
			std::vector<Face*>::iterator removeFaceIter = std::remove(pEdge->faceList.begin(),pEdge->faceList.end(),pFace);
			pEdge->faceList.erase(removeFaceIter,pEdge->faceList.end());

			// if an edge doesn't have any adjacent faces it can also be removed
			if(pEdge->faceList.empty())
			{
				std::vector<Edge*> tempRemoveEdgeList;
				tempRemoveEdgeList.push_back(pEdge);
				RemoveEdgeFromList(model,tempRemoveEdgeList);
			}
		}

		delete pFace;
		pFace = NULL;
	}
}