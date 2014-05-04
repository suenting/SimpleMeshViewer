// cgViewer.cpp : Defines the entry point for the console application.
//
#include "pch.h"
#include "World.h"

namespace
{
	static World* gWorld;
	int mainWindow;

	static void MyReshape(int w, int h)
	{
		  glViewport(0, 0, w, h);
		  glMatrixMode(GL_PROJECTION);
		  glLoadIdentity();

		  glMatrixMode(GL_MODELVIEW);
		  glLoadIdentity();
	}

	static void Idle(void)
	{	
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-1,1,-1,1,4,25000);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glutPostRedisplay();
		glutPostWindowRedisplay(mainWindow);// force window active
		
		gWorld->Update();
	}

	static void Display(void)
	{
		glClearColor(1.0f,1.0f,1.0f,1.0f);   /* set the background colour */
		/* OK, now clear the screen with the background colour */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gWorld->Draw();
		glFlush();
		glutSwapBuffers();
	}

	// glui input code
	enum
	{
		eHorse,
		eArmHand,
		eMan,
		eFish,
		eCube,
		eSphere
	};
	static void LoadModelCB(int model)
	{
		switch(model)
		{
		case eHorse:
			gWorld->LoadModel("horse.smf");
			break;
		case eArmHand:
			gWorld->LoadModel("armhand.smf");
			break;
		case eCube:
			gWorld->LoadModel("cube.smf");
			break;
		case eMan:
			gWorld->LoadModel("man.smf");
			break;
		case eFish:
			gWorld->LoadModel("bigfish.smf");
			break;
		case eSphere:
			gWorld->LoadModel("sphere.smf");
			break;
		}
	}
	static void ClearModelCB(int)
	{
		gWorld->ClearModels();
	}

	static GLUI_Translation * transX;
	static GLUI_Translation * transY;
	static GLUI_Translation * transZ;
	static float tx = 0;
	static float ty = 0;
	static float tz = 0;
	static void TranslateModelCB(int)
	{
		gWorld->TranslateModels((transX->get_x()-tx)/100.f,(transY->get_y()-ty)/100.f,(transZ->get_z()-tz)/100.f);
		tx = transX->get_x();
		ty = transY->get_y();
		tz = transZ->get_z();
	}
	static GLUI_Translation * rotateX;
	static GLUI_Translation * rotateY;
	static GLUI_Translation * rotateZ;
	static float rx = 0;
	static float ry = 0;
	static float rz = 0;
	static void RotateModelCB(int)
	{
		gWorld->RotateModels((rotateX->get_x()-rx)/100.f,(rotateY->get_y()-ry)/100.f,(rotateZ->get_z()-rz)/100.f);
		rx = rotateX->get_x();
		ry = rotateY->get_y();
		rz = rotateZ->get_z();
	}

	enum
	{
		eScaleAll,
		eScaleSingle
	};
	static GLUI_Translation * scaleXYZ;
	static GLUI_Translation * scaleX;
	static GLUI_Translation * scaleY;
	static GLUI_Translation * scaleZ;
	static float sx = 0;
	static float sy = 0;
	static float sz = 0;
	static float sa = 0;
	static void ScaleModelCB(int scaleType)
	{
		if(eScaleAll == scaleType)
		{
			float scaleAmount = (scaleXYZ->get_x()-sa)/100.f;
			gWorld->ScaleModels(1.f+scaleAmount,1.f+scaleAmount,1.f+scaleAmount);
			sa = scaleXYZ->get_x();
		}
		else
		{
			gWorld->ScaleModels(1.f+(scaleX->get_x()-sx)/100.f,1.f+(scaleY->get_y()-sy)/100.f,1.f+(scaleZ->get_z()-sz)/100.f);
			sx = scaleX->get_x();
			sy = scaleY->get_y();
			sz = scaleZ->get_z();
		}

	}

	// shading CBs
	enum eShadeType
	{
		eShaderSmooth,
		eShaderFlat,
		eShaderList,
		eLight,
		eDrawPoly,
		eDrawLine,
		eDrawPoint,
		eDrawPolyLine,
		eDrawNA,
	};
	static GLUI_Checkbox * smoothShade;
	static GLUI_Checkbox * flatShade;
	static GLUI_Checkbox * drawPoly;
	static GLUI_Checkbox * drawLine;
	static GLUI_Checkbox * drawPoint;
	static GLUI_Checkbox * light;
	static GLUI_Listbox * shaderListBox;
	static void UpdateDrawList()
	{
			if(drawPoly->get_int_val() == 1 && drawLine->get_int_val() == 1)
			{
				shaderListBox->set_int_val(eDrawPolyLine); 
			}
			else if (drawPoly->get_int_val() == 1 && drawLine->get_int_val() == 0)
			{
				shaderListBox->set_int_val(eDrawPoly); 
			}
			else if(drawPoly->get_int_val() == 0 && drawLine->get_int_val() == 1)
			{
				shaderListBox->set_int_val(eDrawLine); 
			}
			else if(drawPoly->get_int_val() == 0 && drawLine->get_int_val() == 0)
			{
				shaderListBox->set_int_val(eDrawNA); 
			}
	}
	void ShaderCB(int shadeType)
	{
		switch(shadeType)
		{
		case eShaderList:
			//update checkboxes
			if(shaderListBox->get_int_val() == eDrawPoly)
			{
				drawPoly->set_int_val(1);
				drawLine->set_int_val(0);
				gWorld->SetDrawPolygons(true);
				gWorld->SetDrawLines(false);
			}
			else if(shaderListBox->get_int_val() == eDrawLine)
			{
				drawPoly->set_int_val(0);
				drawLine->set_int_val(1);
				gWorld->SetDrawPolygons(false);
				gWorld->SetDrawLines(true);
			}
			else if(shaderListBox->get_int_val() == eDrawPolyLine)
			{
				drawPoly->set_int_val(1);
				drawLine->set_int_val(1);
				gWorld->SetDrawPolygons(true);
				gWorld->SetDrawLines(true);
			}
			else if(shaderListBox->get_int_val() == eDrawNA)
			{
				drawPoly->set_int_val(0);
				drawLine->set_int_val(0);
				gWorld->SetDrawPolygons(false);
				gWorld->SetDrawLines(false);
			}
			break;
		case eShaderSmooth:
			smoothShade->set_int_val(1);
			flatShade->set_int_val(0);
			gWorld->SetShadeModel(GL_SMOOTH);
			break;
		case eShaderFlat:
			smoothShade->set_int_val(0);
			flatShade->set_int_val(1);
			gWorld->SetShadeModel(GL_FLAT);
			break;
		case eLight:
			if(light->get_int_val() == 1)
			{
				gWorld->SetEnableLighting(true);
			}
			else
			{
				gWorld->SetEnableLighting(false);
			}
			break;
		case eDrawPoly:
			if(drawPoly->get_int_val() == 1)
			{
				gWorld->SetDrawPolygons(true);
			}
			else
			{
				gWorld->SetDrawPolygons(false);
			}
			UpdateDrawList();
			break;
		case eDrawLine:
			if(drawLine->get_int_val() == 1)
			{
				gWorld->SetDrawLines(true);
			}
			else
			{
				gWorld->SetDrawLines(false);
			}
			UpdateDrawList();
			break;
		case eDrawPoint:
			if(drawPoint->get_int_val() == 1)
			{
				gWorld->SetDrawPoints(true);
			}
			else
			{
				gWorld->SetDrawPoints(false);
			}
			break;

		}
	}

	static GLUI_FileBrowser * fileBrowser;
	static GLUI_Button * load;
	static GLUI_Button * save;
	static GLUI_EditText * stringPath;
	static GLUI_EditText * selectedTxt;
	static const char* cachedPath;
	enum
	{
		eEnable,
		eStringPath,
		eSave,
		eLoad,
	};
	static void SaveLoadCB(int saveload)
	{
		const char* path = fileBrowser->get_file();
		std::ifstream iStream;
		switch(saveload)
		{
		case eEnable:
			save->enable();
			load->enable();
			selectedTxt->set_text(path);
			cachedPath = path;
			break;
		case eStringPath:
			path = stringPath->get_text();
			selectedTxt->set_text(path);
			cachedPath = path;
			iStream.open(path, std::ifstream::in);
			if (iStream.is_open())
			{
				iStream.close();
			}
			if(iStream.fail())
			{
				load->disable();
			}
			else
			{
				load->enable();
			}
			if(strlen(path)>0)
			{
				save->enable();
			}
			break;
		case eLoad:
			gWorld->LoadModel(cachedPath);
			break;
		case eSave:
			gWorld->SaveAllModels(cachedPath);
			break;
		}
	}

	enum SubdivideType
	{
		ST_Loop_NOMASK,
		ST_Loop,
	};

	static void SubdivisionCB(int subdivisionType)
	{
		switch(subdivisionType)
		{
		case ST_Loop:
			gWorld->LoopSubdivideAllModels();
			break;
		case ST_Loop_NOMASK:
			gWorld->LoopSubdivideAllModels(false);
			break;
		}
	}
	static GLUI_Spinner * decimationIterations;
	static GLUI_Spinner * mcChoiceCount;

	static void DecimationCB(int decimationType)
	{
		gWorld->QuadricMCDecimation(mcChoiceCount->get_int_val(),decimationIterations->get_int_val());
	}

	static void InitUI()
	{
		// glui code
		GLUI *glui = GLUI_Master.create_glui( "Controls", 0, 810,0 );
		GLUI_Rollout * loader		= glui->add_rollout( "Save/Load", false );
		// debug dislikes this.... in msvc
#ifdef RELEASE
			GLUI_StaticText * saveLoadTxt = glui->add_statictext_to_panel(loader, "Select save/load file 'Requres Double Click'");
			fileBrowser = new GLUI_FileBrowser(loader, "",false, eEnable, SaveLoadCB);
			stringPath = glui->add_edittext_to_panel(loader, "",GLUI_EDITTEXT_TEXT,NULL, eStringPath, SaveLoadCB);
			GLUI_StaticText * saveLoadDispTxt = glui->add_statictext_to_panel(loader, "file selected:");
			selectedTxt   = glui->add_edittext_to_panel(loader, "");
			load = glui->add_button_to_panel( loader,"load", eLoad, SaveLoadCB);
			save = glui->add_button_to_panel( loader,"save", eSave, SaveLoadCB);
			load->disable();
			save->disable();
			selectedTxt->disable();
			selectedTxt->set_w(200);
#else
			//GLUI_StaticText * saveLoadTxt = glui->add_statictext_to_panel(loader, "Only Functional in Release Builds");
			glui->add_statictext_to_panel(loader, "Only Functional in Release Builds");
#endif
		GLUI_Rollout * spawner		= glui->add_rollout( "Spawn", true );
			GLUI_Button * horseModel = glui->add_button_to_panel( spawner,"horse.smf", eHorse, LoadModelCB);
			GLUI_Button * armHandModel = glui->add_button_to_panel( spawner,"armhand.smf", eArmHand, LoadModelCB);
			GLUI_Button * manModel = glui->add_button_to_panel( spawner,"man.smf", eMan, LoadModelCB);
			GLUI_Button * fishModel = glui->add_button_to_panel( spawner,"bigfish.smf", eFish, LoadModelCB);
			GLUI_Button * cubeModel = glui->add_button_to_panel( spawner,"cube.smf", eCube, LoadModelCB);
			GLUI_Button * sphereModel = glui->add_button_to_panel( spawner,"sphere.smf", eSphere, LoadModelCB);
			GLUI_Button * clearModel = glui->add_button_to_panel( spawner,"clear", 1, ClearModelCB);
			horseModel->set_w(200);
			armHandModel->set_w(200);
			cubeModel->set_w(200);
			manModel->set_w(200);
			fishModel->set_w(200);
			sphereModel->set_w(200);
			clearModel->set_w(200);
		GLUI_Rollout * shader		= glui->add_rollout( "Shading", false );
#ifdef RELEASE
			smoothShade	 = glui->add_checkbox_to_panel(shader, "Smooth Shade", NULL, eShaderSmooth, ShaderCB);
			flatShade	 = glui->add_checkbox_to_panel(shader, "Flat Shade", NULL, eShaderFlat, ShaderCB);
			smoothShade->set_int_val(1);
			glui->add_column_to_panel(shader, true);
			drawPoly	 = glui->add_checkbox_to_panel(shader, "Draw Polygons", NULL, eDrawPoly, ShaderCB);
			drawPoly->set_int_val(1);
			drawLine	 = glui->add_checkbox_to_panel(shader, "Draw Lines", NULL, eDrawLine, ShaderCB);
			drawLine->set_int_val(0);
			drawPoint	 = glui->add_checkbox_to_panel(shader, "Draw Points", NULL, eDrawPoint, ShaderCB);
			shaderListBox = glui->add_listbox_to_panel( shader,"Shade",NULL,eShaderList,ShaderCB);
			shaderListBox->add_item( eDrawPolyLine, "Draw Polygon+Line" );
			shaderListBox->add_item( eDrawPoly, "Draw Polygon" );
			shaderListBox->add_item( eDrawLine, "Draw Line" );
			shaderListBox->add_item( eDrawNA, "Draw NO Polygon/Line" );
			glui->add_column_to_panel(shader, true);
			light		 = glui->add_checkbox_to_panel(shader, "Enable Lighting", NULL, eLight, ShaderCB);
			light->set_int_val(1);
#else
			glui->add_statictext_to_panel(shader, "Only Functional in Release Builds");
#endif

		GLUI_Rollout * translater	= glui->add_rollout( "Translate", false );
			transX = glui->add_translation_to_panel( translater, "x", GLUI_TRANSLATION_X, NULL, 0, TranslateModelCB);
			glui->add_column_to_panel(translater, true);
			transY = glui->add_translation_to_panel( translater, "y", GLUI_TRANSLATION_Y, NULL, 0, TranslateModelCB);
			glui->add_column_to_panel(translater, true);
			transZ = glui->add_translation_to_panel( translater, "z", GLUI_TRANSLATION_Z, NULL, 0, TranslateModelCB);
			transX->set_x(0);
			transY->set_y(0);
			transZ->set_z(0);
		GLUI_Rollout * rotater		= glui->add_rollout( "Rotate", false );
			rotateX = glui->add_translation_to_panel( rotater, "x", GLUI_TRANSLATION_X, NULL, 0, RotateModelCB);
			glui->add_column_to_panel(rotater, true);
			rotateY = glui->add_translation_to_panel( rotater, "y", GLUI_TRANSLATION_Y, NULL, 0, RotateModelCB);
			glui->add_column_to_panel(rotater, true);
			rotateZ = glui->add_translation_to_panel( rotater, "z", GLUI_TRANSLATION_Z, NULL, 0, RotateModelCB);
			rotateX->set_x(0);
			rotateY->set_y(0);
			rotateZ->set_z(0);
		GLUI_Rollout * scaler		= glui->add_rollout( "Scale", false );
			scaleXYZ = glui->add_translation_to_panel( scaler, "xyz", GLUI_TRANSLATION_XY, NULL, eScaleAll, ScaleModelCB);
			glui->add_column_to_panel(scaler, true);
			scaleX = glui->add_translation_to_panel( scaler, "x", GLUI_TRANSLATION_X, NULL, eScaleSingle, ScaleModelCB);
			glui->add_column_to_panel(scaler, true);
			scaleY = glui->add_translation_to_panel( scaler, "y", GLUI_TRANSLATION_Y, NULL, eScaleSingle, ScaleModelCB);
			glui->add_column_to_panel(scaler, true);
			scaleZ = glui->add_translation_to_panel( scaler, "z", GLUI_TRANSLATION_Z, NULL, eScaleSingle, ScaleModelCB);
			scaleX->set_x(0);
			scaleY->set_y(0);
			scaleZ->set_z(0);
			scaleXYZ->set_x(0);
			scaleXYZ->set_y(0);
			scaleXYZ->set_z(0);
		GLUI_Rollout * subdivisions	= glui->add_rollout( "Subdivide (slow/assumes triangle mesh)", false );
			GLUI_Button * loop = glui->add_button_to_panel( subdivisions,"Loop", ST_Loop, SubdivisionCB);
			GLUI_Button * loopNoMask = glui->add_button_to_panel( subdivisions,"Loop No Mask (only creates new vertices/edges)", ST_Loop_NOMASK, SubdivisionCB);
			loop->set_w(200);
			loopNoMask->set_w(200);
		GLUI_Rollout * decimation	= glui->add_rollout( "Decimation (slow/assumes triangle mesh)", false );
			GLUI_Button * quarticMC = glui->add_button_to_panel( decimation,"quarticMC", 0, DecimationCB);
			decimationIterations = glui->add_spinner_to_panel( decimation, "iterations");
			mcChoiceCount = glui->add_spinner_to_panel( decimation, "number of choices");
			quarticMC->set_w(100);
			decimationIterations->set_int_limits( 1, 100000 );
			decimationIterations->set_w(400);
			mcChoiceCount->set_int_limits( 1, 100 );
			mcChoiceCount->set_int_val( 10 );
		GLUI_Button *exitButton		= glui->add_button( " Exit", 0, exit);
		loader->set_w(200);
		spawner->set_w(200);
		shader->set_w(200);
		translater->set_w(200);
		rotater->set_w(200);
		scaler->set_w(200);
		subdivisions->set_w(200);
		decimation->set_w(200);
		exitButton->set_w(200);
		glui->set_main_gfx_window( mainWindow );
	}
}

int main(int argc, char** argv) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition (0, 0);
	glutInitWindowSize(800,800);
	mainWindow = glutCreateWindow("CG Viewer");
	glutReshapeFunc (MyReshape);
	//glutIdleFunc(Idle);
	glutDisplayFunc(Display);

	// set current directory
	chdir("Models");
	InitUI();

	GLUI_Master.set_glutIdleFunc( Idle );

	// initialize world
	gWorld = new World();
	gWorld->Init();
	glutMainLoop();

	delete gWorld;
	return 0;         
}

