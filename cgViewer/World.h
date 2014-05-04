#ifndef WORLD_H
#define WORLD_H

#include "pch.h"
#include "ModelFactory.h"
class InputManager;
class World
{
public:
	World(void);
	~World(void);

	void Init();
	void Draw()const;
	void Update();
	void LoadModel(const char * modelPath);
	void SaveAllModels(const char * path);
	void ClearModels();

	// subdivision
	void LoopSubdivideAllModels(bool mask = true);

	// decimation
	void QuadricMCDecimation(int numberChoices, int iterations);

	// model manipulation
	void TranslateModels(float x,float y,float z);
	void RotateModels(float x,float y,float z);
	void ScaleModels(float x,float y,float z);

	// lighting manipulation
	void SetShadeModel(int shadeModel);
	void SetEnableLighting(bool lighting);
	void SetDrawPolygons(bool b);
	void SetDrawLines(bool b);
	void SetDrawPoints(bool b);
private:

	InputManager * pInputManager;

	std::vector<Model*> models;// weak ptrs to models
	mutable int prevTime;
	mutable int fps;
	mutable int displayFPS;

	bool		mEnableLighting;
	int			mShadeModel;
	bool		mDrawPolygons;
	bool		mDrawLines;
	bool		mDrawPoints;
};

#endif