#include "World.h"
#include "Subdivision.h"
#include "Decimation.h"

namespace
{
	static void RenderBitmapString(float x, float y, float z, void *font, char *string)
	{  
		char *c;
		glRasterPos3f(x, y, z);
		for (c=string; *c != '\0'; c++) 
		{
			glutBitmapCharacter(font, *c);
		}
	}

	// lighting params
	static GLfloat LightAmbient[]=		{ 2.25f, 2.25f, 2.25f, 1.0f };
	static GLfloat LightDiffuse[]=		{ 0.5f, 0.5f, 0.5f, 1.0f };
	static GLfloat LightPosition[]=		{ 0.0f, 0.0f, 50000.0f, 1.0f };

/*
	if(displayFPS==-10)
	{
		loading = true;
		sprintf(outString,"loading...");
	}
	else if(displayFPS == -20)
	{
		clearing = true;
		sprintf(outString,"clearing...");
	}
	else if(displayFPS == -30)
	{
		saving = true;
		sprintf(outString,"saving...");
	}
	else if(displayFPS == -40)
	{
		loopSubdividing = true;
		sprintf(outString,"LOOP Subdivision In Progress...");
	}
	else if(displayFPS == -50)
	{
		QuadricMCDecimating = true;
*/

	// message states
	enum MSG_STATE
	{
		MS_LOADING = -10,
		MS_CLEARING = -20,
		MS_SAVING = -30,
		MS_LOOP_SUBDIV = -40,
		MS_QUADRIC_DEC = -50,
	};

	// state vars
	static bool loading = false;
	static bool clearing = false;
	static bool saving = false;
	static bool loopSubdividing = false;
	static bool loopSubdivMask = true;
	static bool QuadricMCDecimating = false;
	static size_t quadricMCChoiceCount = 1;
	static size_t quadricDecimatingIterations = 1;

	static const char* cachedModelPath;
}

World::World(void)
{
	prevTime = 0;
	fps = 0;
	displayFPS = 0;
}

World::~World(void)
{
}

void World::Init()
{
	//glEnable(GL_TEXTURE_2D);						// Enable Texture Mapping
	glClearDepth(1.0f);								// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);						// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);							// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		// Setup The Ambient Light
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// Setup The Diffuse Light
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);	// Position The Light
	glEnable(GL_LIGHT1);								// Enable Light One
	glEnable(GL_NORMALIZE);

	mEnableLighting = true;
	mShadeModel		= GL_SMOOTH;
	mDrawPolygons	= true;
	mDrawLines		= false;
	mDrawPoints		= false;

	glLineWidth(2);
	glPointSize(5);
	srand((unsigned)time(0)); 
}

void World::Draw()const
{
	size_t polygons = 0;
	size_t verticies = 0;
	size_t edges = 0;
	
	for(std::vector<Model*>::const_iterator it = models.begin(); it != models.end();++it)
	{
		// draw all models, default shade grey
		if(mDrawPolygons)
		{
			glColor3f(0.7f,0.7f,0.7f);
			(*it)->Draw(GL_POLYGON);
		}

		// draw black lines
		glDisable(GL_LIGHTING);
		if(mDrawLines)
		{
			glColor3f(0.f,0.f,0.f);
			(*it)->Draw(GL_LINES);
		}

		// draw blue points
		if(mDrawPoints)
		{
			glColor3f(0.f,0.f,1.f);
			(*it)->Draw(GL_POINTS);
		}
		if(mEnableLighting)
		{
			glEnable(GL_LIGHTING);
		}

		// gathers stats from all models
		polygons += (*it)->GetPolygonCount();
		verticies += (*it)->GetVertexCount();
		edges += (*it)->GetEdgeCount();
	}

	// gather and render stats
	glDisable(GL_LIGHTING);
	char outString[100];
	sprintf(outString,"polygons: %d",polygons);
	glColor3f(0.f,0.f,0.f);
	RenderBitmapString(-250, 235, 1500,GLUT_BITMAP_TIMES_ROMAN_24, outString);

	sprintf(outString,"verticies: %d",verticies);
	glColor3f(0.f,0.f,0.f);
	RenderBitmapString(-250, 215, 1500,GLUT_BITMAP_TIMES_ROMAN_24, outString);

	sprintf(outString,"edges: %d",edges);
	glColor3f(0.f,0.f,0.f);
	RenderBitmapString(-250, 195, 1500,GLUT_BITMAP_TIMES_ROMAN_24, outString);

	if(displayFPS == MS_LOADING)
	{
		loading = true;
		sprintf(outString,"loading...");
	}
	else if(displayFPS == MS_CLEARING)
	{
		clearing = true;
		sprintf(outString,"clearing...");
	}
	else if(displayFPS == MS_SAVING)
	{
		saving = true;
		sprintf(outString,"saving...");
	}
	else if(displayFPS == MS_LOOP_SUBDIV)
	{
		loopSubdividing = true;
		sprintf(outString,"LOOP Subdivision In Progress...");
	}
	else if(displayFPS == MS_QUADRIC_DEC)
	{
		QuadricMCDecimating = true;
		sprintf(outString,"Quadric M/C Mesh Decimation In Progress...");
	}
	else
	{
		sprintf(outString,"~frame rate: %d",displayFPS);
	}
	glColor3f(0.f,0.f,0.f);
	RenderBitmapString(-250, 175, 1500,GLUT_BITMAP_TIMES_ROMAN_24, outString);

	fps++;

	// a second has passed
	if(time(NULL)!=prevTime)
	{
		prevTime = time(NULL);
		displayFPS = fps;
		fps = 0;
	}
	if(mEnableLighting)
	{
		glEnable(GL_LIGHTING);
	}
}

void World::Update()
{
	// update camera position
	gluLookAt(0,0,2500,0,0,0,0,1,0);
	glEnable(mEnableLighting);// update lighting params
	glShadeModel(mShadeModel);// Enable Smooth Shading
	// ensure loading begins after a loading message has been displayed
	if(loading)
	{
		Model* pModel = ModelFactory::Instance()->CreateModelOBJ(cachedModelPath);
		Math::Matrix4f temp = Math::Matrix4f::GetIdentity();

		// translate model to center of screen (0,0)
		// std::cout<<pModel->GetInitialX()<<" "<<pModel->GetInitialY()<<std::endl;
		Math::Matrix4f transMatrix = Math::GetTranslateMatrix( -1*(pModel->GetInitialX()+1), -1*(pModel->GetInitialY()+1), 0);
		temp *= transMatrix;

		float maxSize = std::max(std::max(pModel->GetInitialDepth(), pModel->GetInitialHeight()),pModel->GetInitialWidth() );
		float scaler = (500.f/maxSize);
		// scale model to any size at most 500
		Math::Matrix4f scalerMatrix = Math::GetScaleMatrix(scaler,scaler,scaler);
		temp *= scalerMatrix;

		pModel->SetTransform(temp);
		models.push_back(pModel);
		loading = false;
		displayFPS = 0;
		fps = 0;
	}
	if(clearing)
	{
		ModelFactory::Instance()->ClearAllModels();
		models.clear();
		clearing = false;
		displayFPS = 0;
		fps = 0;
	}
	if(saving)
	{
		ModelFactory::Instance()->SaveAllModels(cachedModelPath);
		saving = false;
		displayFPS = 0;
		fps = 0;
	}
	if(loopSubdividing)
	{
		for(std::vector<Model*>::const_iterator it = models.begin(); it != models.end();++it)
		{
			Model* pModel = (*it);
			Subdivision::LoopSubdivision(*pModel, loopSubdivMask);
		}
		loopSubdividing = false;
		displayFPS = 0;
		fps = 0;
	}
	if(QuadricMCDecimating)
	{
		for(std::vector<Model*>::const_iterator it = models.begin(); it != models.end();++it)
		{
			Model* pModel = (*it);
			Decimation::QuadraticMultipleChoiceEdgeDecimation(*pModel,quadricMCChoiceCount,quadricDecimatingIterations);
		}
		QuadricMCDecimating = false;
		displayFPS = 0;
		fps = 0;
	}
}

void World::LoadModel(const char * modelPath)
{
	displayFPS = MS_LOADING;
	fps = MS_LOADING;
	cachedModelPath = modelPath;
}

void World::ClearModels()
{
	displayFPS = MS_CLEARING;
	fps = MS_CLEARING;
}

void World::SaveAllModels(const char * path)
{
	displayFPS = MS_SAVING;
	fps = MS_SAVING;
	cachedModelPath = path;
}

void World::LoopSubdivideAllModels(bool mask)
{
	displayFPS = MS_LOOP_SUBDIV;
	fps = MS_LOOP_SUBDIV;
	loopSubdivMask = mask;
}

void World::QuadricMCDecimation(int numberChoices, int iterations)
{
	displayFPS = MS_QUADRIC_DEC;
	fps = MS_QUADRIC_DEC;
	quadricMCChoiceCount = numberChoices;
	quadricDecimatingIterations = iterations;
}

void World::TranslateModels(float x,float y,float z)
{
	for(std::vector<Model*>::const_iterator it = models.begin(); it != models.end();++it)
	{
		Model* pModel = (*it);
		Math::Matrix4f temp = pModel->GetTransform();
		Math::Matrix4f translate = Math::GetTranslateMatrix(x,y,z);
		temp *= translate;
		pModel->SetTransform(temp);
	}
}

void World::RotateModels(float x,float y,float z)
{
	for(std::vector<Model*>::const_iterator it = models.begin(); it != models.end();++it)
	{
		Model* pModel = (*it);
		Math::Matrix4f temp = pModel->GetTransform();
		Math::Matrix4f rotation;
		rotation = Math::GetRotationMatrixX(x);
		temp *= rotation;
		rotation = Math::GetRotationMatrixY(y);
		temp *= rotation;
		rotation = Math::GetRotationMatrixZ(z);
		temp *= rotation;
		pModel->SetTransform(temp);
	}
}

void World::ScaleModels(float x,float y,float z)
{
	for(std::vector<Model*>::const_iterator it = models.begin(); it != models.end();++it)
	{
		Model* pModel = (*it);
		Math::Matrix4f temp = pModel->GetTransform();
		Math::Matrix4f scale = Math::GetScaleMatrix(x,y,z);
		temp *= scale;
		pModel->SetTransform(temp);
	}
}

void World::SetShadeModel(int shadeModel)
{
	mShadeModel = shadeModel;
}

void World::SetEnableLighting(bool lighting)
{
	mEnableLighting = lighting;
}

void World::SetDrawPolygons(bool b)
{
	mDrawPolygons = b;
}

void World::SetDrawLines(bool b)
{
	mDrawLines = b;
}

void World::SetDrawPoints(bool b)
{
	mDrawPoints = b;
}