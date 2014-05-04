#include "Model.h"

Model::Model(void)
{
	mTransform = Math::Matrix4f::GetIdentity();
}

Model::~Model(void)
{
	// delete lists
	glDeleteLists(mPolygonDisplayList,1);
	glDeleteLists(mLineDisplayList,1);
	glDeleteLists(mPointDisplayList,1);

	ClearEdgeList();
	ClearFaceList();
	ClearVertexList();
	ClearVertexTextureList();
	ClearVertexNormalList();
}

void Model::Draw(GLenum mode)const
{
	glPushMatrix();
	// apply transformation
	float glMatrix[16];
	mTransform.GetGLMultMatrix(glMatrix);
	glMultMatrixf(glMatrix);

	if(mode == GL_POLYGON)
	{
		glCallList(mPolygonDisplayList);
	}
	if(mode == GL_LINES)
	{
		glCallList(mLineDisplayList);
	}
	if(mode == GL_POINTS)
	{
		glCallList(mPointDisplayList);
	}
	glPopMatrix();
}

void Model::SetTransform(Math::Matrix4f &newTransform)
{
	mTransform = newTransform;
}

size_t Model::GetPolygonCount()const
{
	return mFaceList.size();
}

size_t Model::GetVertexCount()const
{
	return mVertexList.size();
}

size_t Model::GetEdgeCount() const
{
	return mEdgeList.size();
}

Math::Matrix4f Model::GetTransform()const
{
	return mTransform;
}

float Model::GetInitialDepth() const
{
	return mInitialDepth;
}
float Model::GetInitialHeight() const
{
	return mInitialHeight;
}
float Model::GetInitialWidth() const
{
	return mInitialWidth;
}
float Model::GetInitialX()const
{
	return mInitialX;
}
float Model::GetInitialY()const
{
	return mInitialY;
}

void Model::GenerateDisplayLists()
{

	glDeleteLists(mPolygonDisplayList,1);
	glDeleteLists(mLineDisplayList,1);
	glDeleteLists(mPointDisplayList,1);

	bool doGen = true;
	while(doGen)
	{
		doGen = false;
		mPolygonDisplayList = glGenLists(1);
		glNewList(mPolygonDisplayList,GL_COMPILE);
			GenerateDisplayListsHelper(GL_POLYGON);
		glEndList();
		if(glGetError() != GL_NO_ERROR)
		{
			doGen = true;
		}
	}

	doGen = true;
	while(doGen)
	{
		doGen = false;
		mLineDisplayList = glGenLists(1);
		glNewList(mLineDisplayList,GL_COMPILE);
			GenerateDisplayListsHelper(GL_LINE_LOOP);
		glEndList();
		if(glGetError() != GL_NO_ERROR)
		{
			doGen = true;
		}
	}

	doGen = true;
	while(doGen)
	{
		doGen = false;
		mPointDisplayList = glGenLists(1);
		glNewList(mPointDisplayList,GL_COMPILE);
			GenerateDisplayListsHelper(GL_POINTS);
		glEndList();
		if(glGetError() != GL_NO_ERROR)
		{
			doGen = true;
		}
	}
}

void Model::GenerateDisplayListsHelper(GLenum mode)
{
	for(std::vector<Face*>::const_iterator i = mFaceList.begin(); i != mFaceList.end(); ++i)
	{
		Face *face = *i;
		glBegin(mode);
		if(GL_POLYGON == mode)
		{
			// calculate normal
			float nx = 0;
			float ny = 0;
			float nz = 0;
			int count = 0;
			for(std::vector<Math::Vector3f*>::const_iterator vnIter = face->vertexNormal.begin(); vnIter != face->vertexNormal.end(); ++vnIter )
			{
				nx += (*vnIter)->x;
				ny += (*vnIter)->y;
				nz += (*vnIter)->z;
				count++;
			}
			// currently disable use of existing normal
			if(count>0 && false)
			{
				// use predefined normals
				nx = nx/count;
				ny = ny/count;
				nz = nz/count;
				glNormal3f(nx,ny,nz);
			}
			else
			{
				// calculate surface normal
				Math::Vector3f pt1 = *face->vertex[0];
				Math::Vector3f pt2 = *face->vertex[1];
				Math::Vector3f pt3 = *face->vertex[2];
				Math::Vector3f v1 = pt2-pt1;
				Math::Vector3f v2 = pt3-pt1;
				Math::Vector3f normal = v1*v2;
				glNormal3f(normal.x,normal.y,normal.z);
			}
		}

		for(std::vector<Vertex*>::const_iterator index = face->vertex.begin(); index != face->vertex.end(); ++index)
		{
			glVertex3f( (*index)->x, (*index)->y, (*index)->z);
		}
		glEnd();
	}
}

void Model::ClearEdgeList()
{
	for(std::vector<Edge*>::iterator it = mEdgeList.begin(); it != mEdgeList.end(); ++it)
	{
		delete(*it);
	}
	mEdgeList.clear();
}
void Model::ClearFaceList()
{
	for(std::vector<Face*>::iterator it = mFaceList.begin(); it != mFaceList.end(); ++it)
	{
		delete(*it);
	}
	mFaceList.clear();
}
void Model::ClearVertexList()
{
	for(std::vector<Vertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it)
	{
		delete(*it);
	}
	mVertexList.clear();
}
void Model::ClearVertexNormalList()
{
	for(std::vector<Math::Vector3f*>::iterator it = mVertexNormalList.begin(); it != mVertexNormalList.end(); ++it)
	{
		delete(*it);
	}
	mVertexNormalList.clear();
}
void Model::ClearVertexTextureList()
{
	for(std::vector<Math::Vector3f*>::iterator it = mVertexTextureList.begin(); it != mVertexTextureList.end(); ++it)
	{
		delete(*it);
	}
	mVertexTextureList.clear();
}

//utils

std::vector<Vertex*> Edge::GetOuterVerticies()const
{
	std::vector<Vertex*> outerVerticies;
	for(std::vector<Face*>::const_iterator faceIter = faceList.begin(); faceIter != faceList.end(); ++faceIter)
	{
		Face* pFace = *faceIter;
		for(std::vector<Vertex*>::iterator vIter = pFace->vertex.begin(); vIter != pFace->vertex.end(); ++vIter)
		{
			Vertex* pVert = *vIter;
			if(pVert != vertex1 && pVert != vertex2)
			{
				outerVerticies.push_back(pVert);
			}
		}
	}
	return outerVerticies;
}

