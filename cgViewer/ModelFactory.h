#ifndef ModelFactory_H
#define ModelFactory_H

#include "pch.h"
#include "Model.h"

class ModelFactory
{
public:
	static	ModelFactory* Instance();
	Model*	CreateModelOBJ(const char* path);
	void	ClearAllModels();
	void	SaveAllModels(const char* path);

private:
	ModelFactory(void);
	~ModelFactory(void);

	std::vector<Model*> mModels;

};

#endif //ModelFactory_h
