#include "ModelFactory.h"

namespace
{
	const size_t MAX_LINE_LENGTH = 128;

	struct TempFace
	{
		TempFace(size_t size)
		{
			Resize(size);
		}
		void Resize(size_t size)
		{
			vertex.resize(size);
			vertextTexture.resize(size);
			vertexNormal.resize(size);
		}
		std::vector<size_t> vertex;
		std::vector<size_t> vertextTexture;
		std::vector<size_t> vertexNormal;
	};

	typedef std::pair<size_t,size_t> TempEdgeKey;
	struct TempEdge
	{
		TempEdge()
		{
			vertex1 = -1;
			vertex2 = -1;
			centerVertex = -1;
			faceList.clear();
		}
		TempEdge(const TempEdge& edge)
		{
			vertex1 = edge.vertex1;
			vertex2 = edge.vertex2;
			centerVertex = edge.centerVertex;
			faceList= edge.faceList;
		}
		TempEdge(size_t v1, size_t v2)
		{
			vertex1 = v1;
			vertex2 = v2;
			centerVertex = -1;
			faceList.clear();
		}
		TempEdgeKey GetKey()const
		{
			TempEdgeKey key(std::min(vertex1,vertex2),std::max(vertex1,vertex2));
			return key;
		}
		static TempEdgeKey GetKey(size_t vertex1, size_t vertex2)
		{
			TempEdgeKey key(std::min(vertex1,vertex2),std::max(vertex1,vertex2));
			return key;
		}

		size_t vertex1;
		size_t vertex2;

		//used for subdivision scheme
		size_t centerVertex;

		// face indexes used for mesh decimation
		std::vector<Face*> faceList;
	};

}

ModelFactory::ModelFactory(void)
{
	mModels.clear();
}

ModelFactory::~ModelFactory(void)
{
	ClearAllModels();
}

void ModelFactory::ClearAllModels()
{
	// delete all models
	for(std::vector<Model*>::iterator i = mModels.begin(); i != mModels.end();++i)
	{
		delete *i;
	}
	mModels.clear();
}

ModelFactory* ModelFactory::Instance()
{
	static ModelFactory inst;
    return &inst;
}

Model* ModelFactory::CreateModelOBJ(const char* path)
{
	char line[MAX_LINE_LENGTH];
	Model* pModel = new Model();
	mModels.push_back(pModel);

	// temp vars used for determining scale
	float minX = 10000000.f;
	float minY = 10000000.f;
	float minZ = 10000000.f;
	float maxX = -10000000.f;
	float maxY = -10000000.f;
	float maxZ = -10000000.f;

	// temp storage vars
	std::vector<TempFace>					faceList;
	std::map<TempEdgeKey,TempEdge>			edgeList;
	std::vector<Vertex>						vertexList;
	std::vector<Math::Vector3f>				vertexNormalList;
	std::vector<Math::Vector3f>				vertexTextureList;

	std::ifstream file(path);
	if (file.is_open())
	{    
		while ( !file.eof())
		{
			file.getline(line,MAX_LINE_LENGTH);
			// assumes .obj or .smf model format

			// pre initialize vars
			Vertex *vertex;
			Math::Vector3f *vertexNormal;
			Math::Vector3f *vertexTexture;
			switch(line[0])
			{
			case '#':	//comment
				// ignore line
				break;
			case 'v':	//vertex (v), vertex normal (vn), vertext texture (vt)
				switch(line[1])
				{
				case(' '):// vertex
					vertex = new Vertex();
					sscanf(line, "v %f %f %f", &vertex->x, &vertex->y, &vertex->z);
					pModel->mVertexList.push_back(vertex);

					if(vertex->x<minX)
					{
						minX = vertex->x;
					}
					if(vertex->y<minY)
					{
						minY = vertex->y;
					}
					if(vertex->z<minZ)
					{
						minZ = vertex->z;
					}
					if(vertex->x>maxX)
					{
						maxX = vertex->x;
					}
					if(vertex->y>maxY)
					{
						maxY = vertex->y;
					}
					if(vertex->z>maxZ)
					{
						maxZ = vertex->z;
					}
					break;
				case('n'):// vertex normal
					vertexNormal = new Math::Vector3f();
					sscanf(line, "vn %f %f %f", &vertexNormal->x, &vertexNormal->y, &vertexNormal->z);
					pModel->mVertexNormalList.push_back(vertexNormal);
					break;
				case('t'):// vertex texture
					vertexTexture = new Math::Vector3f();
					sscanf(line, "vt %f %f %f", &vertexTexture->x, &vertexTexture->y, &vertexTexture->z);
					pModel->mVertexTextureList.push_back(vertexTexture);
					break;
				default:// unknown
					// ignore line
					break;
				}
				break;
			case 'f':	// face
				// possible combinations include ( v//n, v/t/n, v/t, v )
				TempFace face(4);
				if(sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", 
					&face.vertex[0], &face.vertextTexture[0], &face.vertexNormal[0],
					&face.vertex[1], &face.vertextTexture[1], &face.vertexNormal[1],
					&face.vertex[2], &face.vertextTexture[2], &face.vertexNormal[2],
					&face.vertex[3], &face.vertextTexture[3], &face.vertexNormal[3]) == 12)
				{
					// v/t/n quad
					faceList.push_back(face);

				}
				else if(sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", 
					&face.vertex[0], &face.vertextTexture[0], &face.vertexNormal[0],
					&face.vertex[1], &face.vertextTexture[1], &face.vertexNormal[1],
					&face.vertex[2], &face.vertextTexture[2], &face.vertexNormal[2]) == 9)
				{
					// v/t/n triangle
					face.Resize(3);
					faceList.push_back(face);
				}
				else if (sscanf(line, "f %d/%d %d/%d %d/%d %d/%d", 
					&face.vertex[0], &face.vertextTexture[0],
					&face.vertex[1], &face.vertextTexture[1],
					&face.vertex[2], &face.vertextTexture[2],
					&face.vertex[3], &face.vertextTexture[3]) == 8)
				{
					// v/t quad
					face.vertexNormal.resize(0);
					faceList.push_back(face);
				}
				else if(	sscanf(line, "f %d//%d %d//%d %d//%d %d//%d", 
					&face.vertex[0], &face.vertexNormal[0],
					&face.vertex[1], &face.vertexNormal[1],
					&face.vertex[2], &face.vertexNormal[2],
					&face.vertex[3], &face.vertexNormal[3]) == 8)
				{
					// v//n quad
					face.vertextTexture.resize(0);
					faceList.push_back(face);
				}
				else if (sscanf(line, "f %d/%d %d/%d %d/%d", 
					&face.vertex[0], &face.vertextTexture[0],
					&face.vertex[1], &face.vertextTexture[1],
					&face.vertex[2], &face.vertextTexture[2]) == 6)
				{
					// v/t triangle
					face.Resize(3);
					face.vertexNormal.resize(0);
					faceList.push_back(face);
				}
				else if(	sscanf(line, "f %d//%d %d//%d %d//%d", 
					&face.vertex[0], &face.vertexNormal[0],
					&face.vertex[1], &face.vertexNormal[1],
					&face.vertex[2], &face.vertexNormal[2]) == 6)
				{
					// v//n triangle
					face.Resize(3);
					face.vertextTexture.resize(0);
					faceList.push_back(face);
				}
				else if (sscanf(line, "f %d %d %d %d", &face.vertex[0], &face.vertex[1], &face.vertex[2], &face.vertex[3]) == 4)
				{
					// v quad
					face.vertexNormal.resize(0);
					face.vertextTexture.resize(0);
					faceList.push_back(face);
				}
				else if (sscanf(line, "f %d %d %d", &face.vertex[0], &face.vertex[1], &face.vertex[2]) == 3)
				{
					// v triangle
					face.vertex.resize(3);
					face.vertexNormal.resize(0);
					face.vertextTexture.resize(0);
					faceList.push_back(face);
				}
				else
				{
					std::cerr<<"FAIL"<<std::endl;
				}
				break;
			}
		}
		file.close();

		
		for(std::vector<TempFace>::const_iterator tfIter = faceList.begin(); tfIter != faceList.end(); ++tfIter)
		{
			//Generate FaceList
			Face* nFace = new Face();
			TempFace tFace = *tfIter;
			for(std::vector<size_t>::iterator vIt = tFace.vertex.begin(); vIt != tFace.vertex.end(); ++vIt)
			{
				nFace->vertex.push_back(pModel->mVertexList[(*vIt)-1]);
			}
			pModel->mFaceList.push_back(nFace);

			//Generate TempEdgeList O(nLogn)
			for(int i = 0; i != tFace.vertex.size(); ++i)
			{
				size_t v1 = tFace.vertex[i];
				size_t v2 = tFace.vertex[((i+1)%tFace.vertex.size())];
				TempEdge edge((v1),(v2));
				TempEdgeKey key = edge.GetKey();
				if(edgeList[key].vertex1 == ~0)
				{

					edge.faceList.push_back(nFace);
					edgeList[key] = edge;
				}
				else
				{
					// ensure no duplicates
					if(std::find(edgeList[key].faceList.begin(),edgeList[key].faceList.end(),nFace) == edgeList[key].faceList.end())
					{
						edgeList[key].faceList.push_back(nFace);
					}
				}
			}
		}
		//Generate EdgeList
		for(std::map<TempEdgeKey,TempEdge>::const_iterator edgeIter = edgeList.begin();edgeIter != edgeList.end(); ++edgeIter)
		{
			TempEdge tEdge = edgeIter->second;
			Edge* edge = new Edge(pModel->mVertexList[tEdge.vertex1-1], pModel->mVertexList[tEdge.vertex2-1]);
			edge->faceList = tEdge.faceList;
			pModel->mEdgeList.push_back(edge);

			// also maintain the edge list within each face
			for(std::vector<Face*>::iterator faceIter = edge->faceList.begin(); faceIter != edge->faceList.end(); ++faceIter)
			{
				Face* updataFace = *faceIter;
				updataFace->edge.push_back(edge);
			}
			// also maintain the edge list within each vertex
			edge->vertex1->adjacentEdgeList.push_back(edge);
			edge->vertex2->adjacentEdgeList.push_back(edge);
		}
	}
	pModel->mInitialDepth = maxZ-minZ;
	pModel->mInitialWidth = maxY-minY;
	pModel->mInitialHeight = maxX-minX;
	pModel->mInitialX = (maxX-minX)/2;
	pModel->mInitialY = (maxY-minY)/2;
	pModel->GenerateDisplayLists();

	return pModel;
}

void ModelFactory::SaveAllModels(const char * path)
{
	// delete file if it exists
	remove(path);
	std::ofstream oStream(path);
	std::map<Vertex*,size_t> vertexIndexMap;
	if (oStream.is_open())
	{
		size_t polygons = 0;
		size_t verticies = 0;
		for(std::vector<Model*>::const_iterator it = mModels.begin(); it != mModels.end();++it)
		{
			polygons += (*it)->GetPolygonCount();
			verticies += (*it)->GetVertexCount();
		}
		oStream<<"# "<<verticies<<" "<<polygons<<std::endl;
		size_t index = 1;
		for(std::vector<Model*>::const_iterator it = mModels.begin(); it != mModels.end();++it)
		{
			Model* pModel = (*it);
			for(std::vector<Vertex*>::const_iterator vIt = pModel->mVertexList.begin(); vIt != pModel->mVertexList.end(); ++vIt)
			{
				Vertex* pVertex = *vIt;
				oStream<<"v "<<pVertex->x<<" "<<pVertex->y<<" "<<pVertex->z<<std::endl;
				vertexIndexMap[pVertex] = index;
				++index;
			}
		}
		for(std::vector<Model*>::const_iterator it = mModels.begin(); it != mModels.end();++it)
		{
			Model* pModel = (*it);
			for(std::vector<Face*>::const_iterator fIt = pModel->mFaceList.begin(); fIt != pModel->mFaceList.end(); ++fIt)
			{
				Face* pFace = (*fIt);
				oStream<<"f";
				for(std::vector<Vertex*>::const_iterator fvIt = pFace->vertex.begin(); fvIt != pFace->vertex.end(); ++fvIt)
				{
					Vertex* pFVertex = *fvIt;
					oStream<<" "<<vertexIndexMap[pFVertex];
				}
				oStream<<std::endl;
			}
		}
		oStream.close();
	}
	else
	{
		std::cerr<<"Error: couldn't open save file"<<std::endl;
	}
}