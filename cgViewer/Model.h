#ifndef MODEL_H
#define MODEL_H

#include "pch.h"


struct Vertex;
struct Face;
struct Edge;


struct Vertex
	:public Math::Vector3f
{
	Vertex()
	{
		x = -1;
		y = -1;
		z = -1;
		subDivAdjFaceList.clear();
		adjacentEdgeList.clear();
		quadric = Math::Matrix4f::GetZeroMatrix();
	}
	Vertex(const Vertex& vertex)
	{
		x = vertex.x;
		y = vertex.y;
		z = vertex.z;
		subDivAdjFaceList=vertex.subDivAdjFaceList;
		adjacentEdgeList = vertex.adjacentEdgeList;
		quadric = vertex.quadric;
	}
	Vertex(const Math::Vector3f& vertex)
	{
		x = vertex.x;
		y = vertex.y;
		z = vertex.z;
		subDivAdjFaceList.clear();
		adjacentEdgeList.clear();
		quadric = Math::Matrix4f::GetZeroMatrix();
	}

	Math::Matrix4f quadric;					// used for decimation
	std::vector<Edge*> adjacentEdgeList;	// all edges which use this vertex

	std::vector<Face> subDivAdjFaceList;	// used for subdivision
};

struct Face
{
	Face()
	{
	}
	std::vector<Vertex*> vertex;
	std::vector<Math::Vector3f*> vertextTexture;
	std::vector<Math::Vector3f*> vertexNormal;

	// edge list used for subdivision
	std::vector<Edge*>	edge;
};

struct Edge
{
	Edge(Vertex *v1, Vertex *v2)
	{
		vertex1 = v1;
		vertex2 = v2;
		faceList.clear();
	}
	Edge(const Edge& edge)
	{
		vertex1 = edge.vertex1;
		vertex2 = edge.vertex2;
		centerVertex = edge.centerVertex;
		faceList= edge.faceList;
	}

	std::vector<Vertex*> GetOuterVerticies()const;

	Vertex* vertex1;
	Vertex* vertex2;

	// temp vars used for subdivision scheme
	Vertex* centerVertex;
	Edge*	leftChild;
	Edge*	rightChild;

	// face indexes used for mesh decimation
	std::vector<Face*> faceList;
};


class Model
{
friend class ModelFactory;
friend class Subdivision;
friend class Decimation;

public:
	void									Draw(GLenum mode)const;
	Math::Matrix4f							GetTransform()const;
	void									SetTransform(Math::Matrix4f& newTransform);

	size_t									GetPolygonCount()const;
	size_t									GetVertexCount()const;
	size_t									GetEdgeCount()const;
	float									GetInitialHeight()const;
	float									GetInitialWidth()const;
	float									GetInitialDepth()const;
	float									GetInitialX()const;
	float									GetInitialY()const;

	void									ClearFaceList();
	void									ClearEdgeList();
	void									ClearVertexList();
	void									ClearVertexTextureList();
	void									ClearVertexNormalList();
private:
	Model(void);
	~Model(void);
	void									GenerateDisplayLists();
	void									GenerateDisplayListsHelper(GLenum mode);
	std::vector<Face*>						mFaceList;
	std::vector<Edge*>						mEdgeList;
	std::vector<Vertex*>					mVertexList;
	std::vector<Math::Vector3f*>			mVertexNormalList;
	std::vector<Math::Vector3f*>			mVertexTextureList;

	Math::Matrix4f							mTransform;
	float									mInitialHeight;
	float									mInitialWidth;
	float									mInitialDepth;

	// used for initial reposition
	float									mInitialX;
	float									mInitialY;

	GLuint									mPolygonDisplayList;
	GLuint									mPointDisplayList;
	GLuint									mLineDisplayList;

};
#endif
