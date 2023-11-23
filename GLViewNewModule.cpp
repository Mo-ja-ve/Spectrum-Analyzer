#include "GLViewNewModule.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
//#include "C:\repos\repo_distro\aburn\engine\src\external\imgui\imgui.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WO_imgui.h"
#include <chrono>
//#include "implot.h"

#include "WONVDynSphere.h"
#include "WOImGui.h" //GUI Demos also need to #include "AftrImGuiIncludes.h"
//#include "AftrImGuiIncludes.h"
#include "AftrGLRendererBase.h"
#include <complex>
//#include "C:\MinGW\include\complex.h"

using namespace irrklang;

using namespace Aftr;

const size_t binK = 512;
std::complex<float> out[binK];
float max = 0;

//template <typename T>
//dft_type fftCall(const std::complex<T>* samps, size_t nsamps);

#define ARRAY_LEN(a) sizeof(a)/sizeof(a[0])

typedef struct {//  our sound file has a right and left channel (Stereophonic sound)
	float left;
	float right;
} frameST;

static int callCount = 0;

size_t framesCount = 0;
frameST copyFrames[4800 * 2] = { 0 };
float	printFrames[4800 * 2] = { 0 };
//float dftPrint[4800 * 2] = { 0 };
std::vector<float> dft;

// Cycles per second (hz)
int frequency = 256;
// Audio frequency, for smoothing
float audioFrequency = 440.0f;
// Previous value, used to test if sine needs to be rewritten, and to smoothly modulate frequency
float oldFrequency = 1.0f;
// Index for audio rendering
float sineIdx = 0.0f;
int amp = 1;

int freqCut = 1;

void fft1(float in[], size_t stride, std::complex<float>* out, size_t n) {
	assert(n > 0);

	if (n == 1) {
		out[0] = in[0];
		return;
	}

	fft1(in, stride * 2, out, n / 2);
	fft1(in + stride, stride * 2, out + n / 2, n / 2);

	for (size_t k = 0; k < n / 2; k++) {
		float t = (float)k / n;

		std::complex<float> v = (std::complex<float>)(std::exp(std::complex<float>(0, (-2 * PI * t)))) * out[k + n / 2];

		std::complex<float> e = out[k];

		out[k] = e + v;
		out[k + n / 2] = e - v;
	}
}


std::vector<frameST> vecFrame(4800 * 2);


void buffCopy(void* buff, unsigned int frames) {//  continously dumps sound buffer information into copyFrames (so we can use it)
	//  dynamic memory allocation using if()s in case copyFrames[] isn't completely fully yet (SIZE = 4800*2)

//std::cout << std::endl << "CALL:  " << callCount++;
//std::cout << std::endl << "frames:  " << frames;
//std::cout << std::endl << "framesCount:  " << framesCount;
//std::cout << std::endl << "ARRAY_LEN(copyFrames): " << ARRAY_LEN(copyFrames);

	audioFrequency = frequency + (audioFrequency - frequency) * 0.95f;
	audioFrequency += 1.0f;
	audioFrequency -= 1.0f;

	short* d = (short*)buff;
	double time;

	float lowFilt = 120.0;
	float filtTemp = 0.0f;
	float tan = std::tan(PI * lowFilt / 4800);
	float a1 = ((tan - 1.0f) / (tan + 1.0f));
	float samp = 0.0f;
	float filteredSamp = 0.0f;

	for (unsigned int i = 0; i < frames; i++)
	{
		time = static_cast<double>(i) / 48000.0f;
		d[i] += (short)(amp * sinf(2 * PI * audioFrequency * time));
	}

	frameST* temp = static_cast<frameST*>(buff);
	if (frames <= vecFrame.size() - framesCount) {
		for(int i = 0; i < frames; i++)
			vecFrame[i] = temp[i];
		framesCount += frames;
	}
	else if (frames <= vecFrame.size()) {
		for(int i = 0; i < (vecFrame.size() - frames); i++)
			vecFrame[i] = vecFrame[i + frames];
		for (int i = 0; i < frames; i++)
			vecFrame[i + vecFrame.size() - frames] = temp[i];
	}
	else {
		for (int i = 0; i < vecFrame.size(); i++)
			vecFrame[i] = temp[i];
	}

	for (int i = 0; i < 2*4800; i++) {
		printFrames[i] = vecFrame[i].left;
	}

	if (framesCount >= binK) {
		float fftFrames[9600];

		for (int i = 0; i < 9600; i++)
			fftFrames[i] = printFrames[i];

		fft1(fftFrames, 1, out, binK);
	}

}


void GLViewNewModule::audioF() {

	InitAudioDevice();

	music = LoadMusicStream("C:/edmMusic.mp3");
	PlayMusicStream(music);
	//AttachAudioStreamProcessor(music.stream, buffCopy);

	while (1)
		UpdateMusicStream(music);
}


void GLViewNewModule::doThread() {

	this->audioThread = new std::thread(&GLViewNewModule::audioF, this);
	this->audioThread->detach();

	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::thread thr(AttachAudioStreamProcessor, music.stream, buffCopy);
	thr.detach();
}


GLViewNewModule* GLViewNewModule::New(const std::vector< std::string >& args)
{
	GLViewNewModule* glv = new GLViewNewModule(args);
	glv->init(Aftr::GRAVITY, Vector(0, 0, -1.0f), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE);
	glv->onCreate();
	return glv;
}


GLViewNewModule::GLViewNewModule(const std::vector< std::string >& args) : GLView(args)
{
	//Initialize any member variables that need to be used inside of LoadMap() here.
	//Note: At this point, the Managers are not yet initialized. The Engine initialization
	//occurs immediately after this method returns (see GLViewNewModule::New() for
	//reference). Then the engine invoke's GLView::loadMap() for this module.
	//After loadMap() returns, GLView::onCreate is finally invoked.

	//The order of execution of a module startup:
	//GLView::New() is invoked:
	//    calls GLView::init()
	//       calls GLView::loadMap() (as well as initializing the engine's Managers)
	//    calls GLView::onCreate()

	//GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
}


void GLViewNewModule::onCreate()
{
	//GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
	//At this point, all the managers are initialized. That is, the engine is fully initialized.

	if (this->pe != NULL)
	{
		//optionally, change gravity direction and magnitude here
		//The user could load these values from the module's aftr.conf
		this->pe->setGravityNormalizedVector(Vector(0, 0, -1.0f));
		this->pe->setGravityScalar(Aftr::GRAVITY);
	}
	this->setActorChaseType(STANDARDEZNAV); //Default is STANDARDEZNAV mode
	//this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1
}


GLViewNewModule::~GLViewNewModule()
{
	//Implicitly calls GLView::~GLView()
}


void GLViewNewModule::updateWorld()
{

	//GLView::updateWorld(); //Just call the parent's update world first.
						   //If you want to add additional functionality, do it after
						   //this call.
	//Aftr::Vector v_camPos(0, 0, 0);
	//Aftr::Vector v_lookDir(0, 0, 0);
	//Aftr::Vector v_up(0, 0, 0);

	//v_camPos = this->cam->getPosition();
	//irrklang::vec3df v_camPosinirr(v_camPos.x, v_camPos.y, v_camPos.z);

	//v_lookDir = this->cam->getLookDirection();
	//irrklang::vec3df v_lookDirinirr(v_lookDir.y, v_lookDir.x, v_lookDir.z);

	//v_up = this->cam->getNormalDirection();
	//irrklang::vec3df v_upinirr(v_up.x, v_up.y, v_up.z);

	//irrklang::vec3df veloc(0, 0, 0);
	//this->engine->setListenerPosition(v_camPosinirr, v_lookDirinirr, veloc, v_upinirr);
	//this->engine->setRolloffFactor(1);

	//worldLst->at(3)->moveRelative(Vector(-0.1, 0, 0));

	//Aftr::Vector Barrel_move_V(0, 0, 0);
	//Barrel_move_V = worldLst->at(3)->getPosition();
	//irrklang::vec3d Barrel_move_Vinirr(Barrel_move_V.x, Barrel_move_V.y, Barrel_move_V.z);

	//this->s_3d->setPosition(Barrel_move_Vinirr);

	//int numBoxes = physx_boxes.size();

	//Vector camvec = this->cam->getPosition();
	//float camF[3] = { 1, 2 , 3 };
	//Vec2Float(camvec, &camF);
	//camPxActor->setPosition(camF[0], camF[1], camF[2]);

	//WOpxObj_groundfloor->gScene->simulate(0.1);
	//WOpxObj_groundfloor->gScene->fetchResults(true);
	//physx::PxU32 numActors = 0;
	//physx::PxActor** actors = this->WOpxObj_groundfloor->gScene->getActiveActors(numActors);

	//std::vector <Vector> originalPos;
	//originalPos.resize(numBoxes);
	//for (int i = 0; i < numBoxes; i++) {
	//    originalPos[i] = physx_boxes[i]->getPosition();
	//}

	//for (physx::PxU32 i = 0; i < numActors; ++i) {
	//    physx::PxActor* actor = actors[i];
	//    //WOpxObj_ptr->updatePoseFromPhysx();
	//    for (int j = 0; j < numBoxes; j++) {
	//        physx_boxes[j]->updatePoseFromPhysx();
	//    }
	//    /*physx_boxes[1]->updatePoseFromPhysx();
	//    physx_boxes[2]->updatePoseFromPhysx();
	//    physx_boxes[3]->updatePoseFromPhysx();*/
	//    camPxActor->updatePoseFromPhysx();
	//}

	//Sound* s;
	//Music music = LoadMusicStream("edmMusic.mp3");

	//cam->setPosition(camPxActor->getPosition());
}


void GLViewNewModule::onResizeWindow(GLsizei width, GLsizei height)
{
	GLView::onResizeWindow(width, height); //call parent's resize method.
}


void GLViewNewModule::onMouseDown(const SDL_MouseButtonEvent& e)
{
	GLView::onMouseDown(e);
}


void GLViewNewModule::onMouseUp(const SDL_MouseButtonEvent& e)
{
	GLView::onMouseUp(e);
}


void GLViewNewModule::onMouseMove(const SDL_MouseMotionEvent& e)
{
	GLView::onMouseMove(e);
}


void GLViewNewModule::onKeyDown(const SDL_KeyboardEvent& key)
{
	GLView::onKeyDown(key);
	if (key.keysym.sym == SDLK_0)
		this->setNumPhysicsStepsPerRender(1);
	static int count = 0;
	if (key.keysym.sym == SDLK_3) {
		count++;
		aftrColor4f new_specular(worldLst->at(4)->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0).getSpecular());
		aftrColor4f new_ambient(worldLst->at(4)->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0).getAmbient());
		new_specular[3] += 0.2f;
		new_ambient[3] += 0.2f;

		worldLst->at(4)->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0).setSpecular(new_specular);
		worldLst->at(4)->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0).setAmbient(new_ambient);

		worldLst->at(4)->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0).getMultiTextureSet().at(0).setTexRepeats(count);

		new_specular = worldLst->at(4)->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0).getSpecular();
		new_ambient = worldLst->at(4)->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0).getAmbient();
		new_specular[3] -= 0.2f;
		new_ambient[3] -= 0.2f;

		worldLst->at(4)->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0).setSpecular(new_specular);
		worldLst->at(4)->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0).setAmbient(new_ambient);



		new_specular = worldLst->at(4)->getModel()->getModelDataShared()->getModelMeshes().at(3)->getSkins().at(0).getSpecular();
		new_ambient = worldLst->at(4)->getModel()->getModelDataShared()->getModelMeshes().at(3)->getSkins().at(0).getAmbient();
		new_specular[3] -= 0.2f;
		new_ambient[3] -= 0.2f;

		worldLst->at(4)->getModel()->getModelDataShared()->getModelMeshes().at(3)->getSkins().at(0).setSpecular(new_specular);
		worldLst->at(4)->getModel()->getModelDataShared()->getModelMeshes().at(3)->getSkins().at(0).setAmbient(new_ambient);
	}

	if (key.keysym.sym == SDLK_1)
	{
		std::cout << std::endl << std::endl << "key 1 pressed!" << std::endl << std::endl;
	}
}


void GLViewNewModule::onKeyUp(const SDL_KeyboardEvent& key)
{
	GLView::onKeyUp(key);
}


void Aftr::GLViewNewModule::loadMap()
{
	this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
	this->actorLst = new WorldList();
	this->netLst = new WorldList();

	Aftr::Vector v_camPos(0, 0, 0);
	Aftr::Vector v_lookDir(0, 0, 0);
	Aftr::Vector v_up(0, 0, 0);

	ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
	ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
	ManagerOpenGLState::enableFrustumCulling = false;
	Axes::isVisible = true;
	this->glRenderer->isUsingShadowMapping(false); //set to TRUE to enable shadow mapping, must be using GL 3.2+

	this->cam->setPosition(15, 15, 10);

	std::string shinyRedPlasticCube(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl");
	std::string wheeledCar(ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl");
	std::string grass(ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl");
	std::string human(ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl");

	std::string barrel(ManagerEnvironmentConfiguration::getSMM() + "/models/barrel1.wrl");

	std::string boxbox(ManagerEnvironmentConfiguration::getLMM() + "/models/fallout-new-vegas-destroyed-diner/source/PC Computer - Fallout New Vegas - Diner Destroyed/Diner/dinernosign.obj");

	std::string second_diner_texture(ManagerEnvironmentConfiguration::getSMM() + "/models/fallout-new-vegas-destroyed-diner/source/PC Computer - Fallout New Vegas - Diner Destroyed/Diner/edgetrim01_alpha.png");

	//SkyBox Textures readily available
	//std::vector< std::string > skyBoxImageNames; //vector to store texture paths
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_winter+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/early_morning+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_afternoon+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_cloudy3+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_day2+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_deepsun+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_evening+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_morning2+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_noon+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_warp+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_gray_matter+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_easter+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_hot_nebula+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_ice_field+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_lemon_lime+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_milk_chocolate+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_solar_bloom+6.jpg" );
	//skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_thick_rb+6.jpg" );

	//{
	//   //Create a light
	//   float ga = 0.1f; //Global Ambient Light level for this module
	//   ManagerLight::setGlobalAmbientLight( aftrColor4f( ga, ga, ga, 1.0f ) );
	//   WOLight* light = WOLight::New();
	//   light->isDirectionalLight( true );
	//   light->setPosition( Vector( 0, 0, 100 ) );
	//   //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
	//   //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
	//   light->getModel()->setDisplayMatrix( Mat4::rotateIdentityMat( { 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD ) );
	//   light->setLabel( "Light" );
	//   worldLst->push_back( light );
	//}

	//{
	//   //Create the SkyBox
	//   WO* wo = WOSkyBox::New( skyBoxImageNames.at( 0 ), this->getCameraPtrPtr() );
	//   wo->setPosition( Vector( 0, 0, 0 ) );
	//   wo->setLabel( "Sky Box" );
	//   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	//   worldLst->push_back( wo );
	//}

	{
		////Create the infinite grass plane (the floor)
		//WO* wo = WO::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
		//wo->setPosition( Vector( 0, 0, 0 ) );
		//wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
		//wo->upon_async_model_loaded( [wo]()
		//   {
		//      ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 );
		//      grassSkin.getMultiTextureSet().at( 0 ).setTexRepeats( 5.0f );
		//      grassSkin.setAmbient( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Color of object when it is not in any light
		//      grassSkin.setDiffuse( aftrColor4f( 1.0f, 1.0f, 1.0f, 1.0f ) ); //Diffuse color components (ie, matte shading color of this object)
		//      grassSkin.setSpecular( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Specular color component (ie, how "shiney" it is)
		//      grassSkin.setSpecularCoefficient( 10 ); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
		//   } );
		//wo->setLabel( "Grass" );
		//worldLst->push_back( wo );
	}

	//audioContainer();

	//WOPhysxGround* WOpxGround = WOPhysxGround::New(grass, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
	//WOpxGround->setPosition(0, 0, 0);
	//WOpxGround->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	//WOpxGround->upon_async_model_loaded( [WOpxGround](){
	//      ModelMeshSkin& grassSkin = WOpxGround->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 );
	//      grassSkin.getMultiTextureSet().at( 0 ).setTexRepeats( 5.0f );
	//      grassSkin.setAmbient( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Color of object when it is not in any light
	//      grassSkin.setDiffuse( aftrColor4f( 1.0f, 1.0f, 1.0f, 1.0f ) ); //Diffuse color components (ie, matte shading color of this object)
	//      grassSkin.setSpecular( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Specular color component (ie, how "shiney" it is)
	//      grassSkin.setSpecularCoefficient( 10 ); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
	//} );
	//worldLst->push_back(WOpxGround);
	//WOpxObj_groundfloor = static_cast<WOPhysxGround*>(WOpxGround->actor->userData);

	//{
	//   //Create the infinite grass plane that uses the Open Dynamics Engine (ODE)
	//   WO* wo = WOStatic::New( grass, Vector(1,1,1), MESH_SHADING_TYPE::mstFLAT );
	//   ((WOStatic*)wo)->setODEPrimType( ODE_PRIM_TYPE::PLANE );
	//   wo->setPosition( Vector(0,0,0) );
	//   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	//   wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).getMultiTextureSet().at(0)->setTextureRepeats( 5.0f );
	//   wo->setLabel( "Grass" );
	//   worldLst->push_back( wo );
	//}

	//{
	//   //Create the infinite grass plane that uses NVIDIAPhysX(the floor)
	//   WO* wo = WONVStaticPlane::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
	//   wo->setPosition( Vector( 0, 0, 0 ) );
	//   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	//   wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 ).getMultiTextureSet().at( 0 )->setTextureRepeats( 5.0f );
	//   wo->setLabel( "Grass" );
	//   worldLst->push_back( wo );
	//}

	//{
	//   //Create the infinite grass plane (the floor)
	//   WO* wo = WONVPhysX::New( shinyRedPlasticCube, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
	//   wo->setPosition( Vector( 0, 0, 50.0f ) );
	//   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	//   wo->setLabel( "Grass" );
	//   worldLst->push_back( wo );
	//}

	//{
	//   WO* wo = WONVPhysX::New( shinyRedPlasticCube, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
	//   wo->setPosition( Vector( 0, 0.5f, 75.0f ) );
	//   wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	//   wo->setLabel( "Grass" );
	//   worldLst->push_back( wo );
	//}

	//{
	//   WO* wo = WONVDynSphere::New( ManagerEnvironmentConfiguration::getVariableValue( "sharedmultimediapath" ) + "/models/sphereRp5.wrl", Vector( 1.0f, 1.0f, 1.0f ), mstSMOOTH );
	//   wo->setPosition( 0, 0, 100.0f );
	//   wo->setLabel( "Sphere" );
	//   this->worldLst->push_back( wo );
	//}

	//{
	//   WO* wo = WOHumanCal3DPaladin::New( Vector( .5, 1, 1 ), 100 );
	//   ((WOHumanCal3DPaladin*)wo)->rayIsDrawn = false; //hide the "leg ray"
	//   ((WOHumanCal3DPaladin*)wo)->isVisible = false; //hide the Bounding Shell
	//   wo->setPosition( Vector( 20, 20, 20 ) );
	//   wo->setLabel( "Paladin" );
	//   worldLst->push_back( wo );
	//   actorLst->push_back( wo );
	//   netLst->push_back( wo );
	//   this->setActor( wo );
	//}
	//
	//{
	//   WO* wo = WOHumanCyborg::New( Vector( .5, 1.25, 1 ), 100 );
	//   wo->setPosition( Vector( 20, 10, 20 ) );
	//   wo->isVisible = false; //hide the WOHuman's bounding box
	//   ((WOHuman*)wo)->rayIsDrawn = false; //show the 'leg' ray
	//   wo->setLabel( "Human Cyborg" );
	//   worldLst->push_back( wo );
	//   actorLst->push_back( wo ); //Push the WOHuman as an actor
	//   netLst->push_back( wo );
	//   this->setActor( wo ); //Start module where human is the actor
	//}

	//{
	//   //Create and insert the WOWheeledVehicle
	//   std::vector< std::string > wheels;
	//   std::string wheelStr( "../../../shared/mm/models/WOCar1970sBeaterTire.wrl" );
	//   wheels.push_back( wheelStr );
	//   wheels.push_back( wheelStr );
	//   wheels.push_back( wheelStr );
	//   wheels.push_back( wheelStr );
	//   WO* wo = WOCar1970sBeater::New( "../../../shared/mm/models/WOCar1970sBeater.wrl", wheels );
	//   wo->setPosition( Vector( 5, -15, 20 ) );
	//   wo->setLabel( "Car 1970s Beater" );
	//   ((WOODE*)wo)->mass = 200;
	//   worldLst->push_back( wo );
	//   actorLst->push_back( wo );
	//   this->setActor( wo );
	//   netLst->push_back( wo );
	//}

	//Make a Dear Im Gui instance via the WOImGui in the engine... This calls
	//the default Dear ImGui demo that shows all the features... To create your own,
	//inherit from WOImGui and override WOImGui::drawImGui_for_this_frame(...) (among any others you need).

	 //WO_imgui* WOgui = WO_imgui::New(nullptr, 10.0, 10.0);
	 ////WOgui->physxWO_ptr = physx_boxes[0];
	 //WOgui->subscribe_drawImGuiWidget([this, WOgui]() {
	 //    WOgui->v = this;
	 //    //WOgui->drawImGui_for_this_frame();
	 //    bool show_metrics = false;
	 //    bool show_physicsObjects_menu
	 //    if (ImGui::BeginMenuBar())
	 //    {
	 //        if (ImGui::BeginMenu("Menu"))
	 //        {
	 //            ImGui::MenuItem("Metrics", NULL, &show_metrics);
	 //            ImGui::MenuItem("Objects & Physics", NULL, &show_physicsObjects_menu);
	 //            ImGui::EndMenu();
	 //        }

	 //        ImGui::EndMenuBar();
	 //    }
	 //    //WOImGui::draw_AftrImGui_Demo( WOgui ); //Displays a small Aftr Demo from C:/repos/aburn/engine/src/aftr/WOImGui.cpp
	 //    worldLst->push_back(WOgui);
	 // });

	//random barrel models i added in for first module project
	//WO* wo = WO::New(barrel, Vector(1, 1, 1), MESH_SHADING_TYPE::mstSMOOTH);
	//wo->setPosition(149, 149, 0.5);
	//wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	//wo->setLabel("barrel");
	//worldLst->push_back(wo);
	//std::cout<< std::endl<< std::endl<< std::endl<< std::endl<<"WORLD LIST SIZ"<<worldLst->size()<< std::endl<< std::endl<< std::endl;


   //v_camPos = this->cam->getPosition();
   //irrklang::vec3df v_camPosinirr(v_camPos.x, v_camPos.y, v_camPos.z);

   //v_lookDir = this->cam->getLookDirection();
   //irrklang::vec3df v_lookDirinirr(v_lookDir.y, v_lookDir.x, v_lookDir.z);

   //v_up = this ->cam->getNormalDirection();
   //irrklang::vec3df v_upinirr(v_up.x, v_up.y, v_up.z);

   // irrklang::vec3df veloc(0, 0, 0);
   // this->engine->setListenerPosition(v_camPosinirr, v_lookDirinirr, veloc, v_upinirr);
   // this->engine->setRolloffFactor(1);

   // worldLst->at(3)->moveRelative(Vector(-0.1, 0, 0));

   // Aftr::Vector Barrel_move_V(0, 0, 0);
   // Barrel_move_V = worldLst->at(3)->getPosition();
   // irrklang::vec3d Barrel_move_Vinirr(Barrel_move_V.x, Barrel_move_V.y, Barrel_move_V.z);

	//this->s_3d->setPosition(Barrel_move_Vinirr);

	//audioContainer();

  /*  wo = WO::New(boxbox, Vector(0.01, 0.01, 0.01), MESH_SHADING_TYPE::mstAUTO);
	wo->setPosition(138, 130, 2.5);
	wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	worldLst->push_back(wo);

	wo->upon_async_model_loaded([wo]() {
		std::string second_diner_texture(ManagerEnvironmentConfiguration::getSMM() + "/models/fallout-new-vegas-destroyed-diner/source/PC Computer - Fallout New Vegas - Diner Destroyed/Diner/edgetrim01_alpha.png");
		ModelMeshSkin& dinerSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at(1)->getSkins().at(0);
		dinerSkin.getMultiTextureSet().at(0).setTexRepeats(1.0f);
		dinerSkin.setAmbient(aftrColor4f(0.4f, 0.4f, 0.4f, 10.01f));
		dinerSkin.setSpecular(aftrColor4f(0.4f, 0.4f, 0.4f, 10.01f));

		ModelMeshSkin diner_skin(*ManagerTex::loadTexAsync(second_diner_texture));
		diner_skin.setMeshShadingType(MESH_SHADING_TYPE::mstSMOOTH);
		wo->getModel()->getSkins().at(0)
			= std::move(diner_skin);
	 });*/


	 //float camF[3];
	 //Vec2Float(cam->getPosition(), &camF);

	 //std::cout << std::endl << std::endl << std::endl << std::endl << "WORLD LIST SIZ" << worldLst->size() << std::endl << std::endl << std::endl;
	 //WOPhysx_cameraCollider* WOpxobj_cam = WOPhysx_cameraCollider::New(shinyRedPlasticCube, Vector(0.001, 0.001, 0.001), MESH_SHADING_TYPE::mstSMOOTH, WOpxGround->gScene);
	 //WOpxobj_cam->setPosition(camF[0], camF[1], camF[2]);
	 //WOpxobj_cam->renderOrderType = RENDER_ORDER_TYPE::roTRANSPARENT;
	 //worldLst->push_back(WOpxobj_cam);
	 ////aftrColor4f new_specular(-10.0f);
	 ////aftrColor4f new_ambient(-10.0f);
	 ////WOpxobj_cam->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).setSpecular(new_specular);
	 ////WOpxobj_cam->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).setAmbient(new_ambient);

	 //camPxActor = static_cast<WOPhysx_cameraCollider*>(WOpxobj_cam->actor->userData);

	 //createNewModuleWayPoints();

	WOImGui* gui = WOImGui::New(nullptr);
	gui->setLabel("My Gui");
	gui->subscribe_drawImGuiWidget(
		[this, gui]() //this is a lambda, the capture clause is in [], the input argument list is in (), and the body is in {}
		{
			//gui->v = this;
			static bool show_metrics = false;
			static bool show_menu = false;
			static bool show_physicsObjects_menu;
			static bool show_audio_menu = false;
			static bool* p_open = NULL;
			//static Music music;
			//static std::thread* audioThread = nullptr;

			if (show_metrics) { ImGui::ShowMetricsWindow(); }

			// Demonstrate the various window flags. Typically you would just use the default!
			static bool no_titlebar = false;
			static bool no_scrollbar = false;
			static bool no_menu = false;
			static bool audio_play = false;
			static bool no_move = false;
			static bool no_resize = false;
			static bool no_collapse = false;
			static bool no_close = false;
			static bool no_nav = false;
			static bool no_background = false;
			static bool no_bring_to_front = false;
			static bool playToggle = false;
			static bool pauseToggle = false;

			ImGuiWindowFlags window_flags = 0;
			if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
			if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
			if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
			if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
			if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
			if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
			if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
			if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
			if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
			if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

			// We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! We only do it to make the Demo applications a little more welcoming.
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

			// Most "big" widgets share a common width settings by default.
			//ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);    // Use 2/3 of the space for widgets and 1/3 for labels (default)
			ImGui::PushItemWidth(ImGui::GetFontSize() * -12);           // Use fixed width for labels (by passing a negative value), the rest goes to widgets. We choose a width proportional to our font size.


			ImGui::Begin("Interface", p_open, window_flags);
			if (show_physicsObjects_menu) {
				ImGui::Text("MAP OBJECTS & PHYSICS CONTROLS");

				if (ImGui::Button("TOGGLE GRAVITY")) {

					static bool boolFlag_gravity = false;
					switch (boolFlag_gravity) {
					case false:
						for (int i = 0; i < physx_boxes.size(); i++)
							physx_boxes[i]->actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
						camPxActor->actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
						boolFlag_gravity = true;
						break;

					case true:
						for (int i = 0; i < physx_boxes.size(); i++)
							physx_boxes[i]->actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
						camPxActor->actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
						boolFlag_gravity = false;
						break;
					}
				}
				ImGui::SameLine();
				ImGui::Text("Red Box count:");
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
			}

			if (show_audio_menu) {
				ImGui::Text("AUDIO CONTROLS");
				if (ImGui::Button("PLAY")) {

					if (!playToggle) {
						this->doThread();
						playToggle = !playToggle;

					}
				}

				if (ImGui::Button("PAUSE")) {

					if (playToggle) {
						if (!pauseToggle) {
							PauseMusicStream(music);
						}
						else {
							ResumeMusicStream(music);
						}
						pauseToggle = !pauseToggle;
					}
				}

				ImGui::SliderInt("Frequency", &frequency, 1, 10'000);
				ImGui::SliderInt("AMP", &amp, 1, 100);
				ImGui::SliderInt("Frequency Cut", &freqCut, 1, 256);


				if (playToggle) {
					ImGui::Text("Frames: %d", music.frameCount);
					ImGui::Text("Stream Sample Rate: %d", music.stream.sampleRate);
					ImGui::Text("Stream Sample Size: %d", music.stream.sampleSize);

					int   bar_data[11];
					float x_data[1000];
					float y_data[1000];
					ImPlot::BeginPlot("Audio Wave Form");


					float temp[binK];

					for (int i = 0; i < binK; i++)
						if (max < std::abs(out[i]))
							max = std::abs(out[i]);

					for (int i = 0; i < binK; i++)
						temp[i] = (std::abs(out[i]) / max) * 100;

					ImPlot::PlotLine("Wave", printFrames, ARRAY_LEN(copyFrames));

					ImPlot::EndPlot();

					ImPlot::BeginPlot("Audio Spectrum");

					ImPlot::PlotLine("Audio Spectrum", temp, binK / 2);

					ImPlot::EndPlot();
				}

			}
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Menu"))
				{
					ImGui::MenuItem("Metrics", NULL, &show_metrics);
					ImGui::MenuItem("Objects & Physics", NULL, &show_physicsObjects_menu);
					ImGui::MenuItem("Audio Menu", NULL, &show_audio_menu);

					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			ImGui::End();
			// if (ImGui::CollapsingHeader("RED BOXES"))
			// {
			   //  ImGuiIO& io = ImGui::GetIO();

			   //  if (ImGui::TreeNode("Configuration"))
			   //  {

				  //   if (ImGui::Button("SPAWN BOX")) {
				  //	   gui->v->spawn_box();
				  //	   gui->numberOfBoxes++;
				  //   }

				  //   static int clicked = 0;
				  //   if (ImGui::Button("DELETE BOX"))
				  //	   clicked++;

				  //   if (clicked & 1)
				  //   {
				  //	   static char str0[3] = ""; ImGui::SameLine();
				  //	   ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0)); ImGui::SameLine();
				  //	   if (ImGui::Button("DELETE")) {
				  //		   int boxNum = (int)str0[0] - 48;
				  //		   if (boxNum >= 0 && boxNum < gui->v->physx_boxes.size()) {
				  //			   std::cout << std::endl << "BOX NUMBER: " << gui->v->physx_boxes[boxNum]->pxWO_ID << std::endl;
				  //			   gui->v->getWorldContainer()->eraseViaWOIndex(gui->v->physx_boxes[boxaNum]->pxWO_ID);

				  //			   gui->v->physx_boxes[boxNum]->actor->release();
				  //			   gui->v->physx_boxes.erase(gui->v->physx_boxes.begin() + boxNum);
				  //			   gui->v->WOID--;

				  //			   if (gui->v->physx_boxes.size() != 0)
				  //				   for (int i = 0; i < gui->v->physx_boxes.size(); i++)
				  //					   gui->v->physx_boxes[i]->pxWO_ID--;
				  //		   }
				  //	   }
				  //   }

				  //   if (ImGui::Button("MOVE")) {
				  //	   //this->physxWO_ptr->setPosition(10.0f, 10.0f, 10.0f);
				  //   }

				  //   if (ImGui::Button("TOGGLE PHYSICS")) {

				  //	   static bool flag = false;
				  //	   switch (flag) {
				  //	   case false:
				  //		   for (int i = 0; i < gui->v->physx_boxes.size(); i++)
				  //			   gui->v->physx_boxes[i]->actor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
				  //		   flag = true;
				  //		   break;

				  //	   case true:
				  //		   for (int i = 0; i < gui->v->physx_boxes.size(); i++)
				  //			   gui->v->physx_boxes[i]->actor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, false);
				  //		   flag = false;
				  //		   break;
				  //	   }
				  //   }

				  //   if (io.ConfigFlags & ImGuiConfigFlags_NoMouse) // Create a way to restore this flag otherwise we could be stuck completely!
				  //   {
				  //	   if (fmodf((float)ImGui::GetTime(), 0.40f) < 0.20f)
				  //	   {
				  //		   ImGui::SameLine();
				  //		   ImGui::Text("<<PRESS SPACE TO DISABLE>>");
				  //	   }
				  //	   if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space)))
				  //		   io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
				  //   }

				  //   ImGui::TreePop();
				  //   ImGui::Separator();
			   //  }

			   //  if (ImGui::TreeNode("BOX LIST"))
			   //  {
				  //   if (ImGui::Button("BOXES")) {
				  //	   for (int i = 0; i < gui->v->getWorldContainer()->size(); i++) {
				  //		   std::cout << std::endl << "RED BOX at : " << i << std::endl;
				  //		   if (gui->v->getWorldContainer()->at(i)->getLabel() == "red box") {
				  //			   ImGui::Text("BOX: %d", i);
				  //			   std::cout << std::endl << "RED BOX at : " << i << std::endl;
				  //			   std::cout << std::endl << "RED BOX at : " << i << std::endl;
				  //			   std::cout << std::endl << "RED BOX at : " << i << std::endl;
				  //			   std::cout << std::endl << "RED BOX at : " << i << std::endl;
				  //		   }
				  //	   }
				  //   }
				  //   ImGui::TreePop();
				  //   ImGui::Separator();
			   //  }
			// }
		 //}


		 //if (ImGui::BeginMenuBar())
		 //{
			// if (ImGui::BeginMenu("Menu"))
			// {
			   //  ImGui::MenuItem("Metrics", NULL, &show_metrics);
			   //  ImGui::MenuItem("Objects & Physics", NULL, &show_physicsObjects_menu);
			   //  ImGui::EndMenu();
			// }

			// ImGui::EndMenuBar();
		 //}
		});

	worldLst->push_back(gui);
	//std::thread audioThread(audio);
	//audioThread.join();

	//static std::thread* audioThread = nullptr;

	//int music = 0;
	//audioThread = new std::thread(audio, music);
	//audioThread->detach();

	//int inter = 0;
	//audioThread = new std::thread(&GLViewNewModule::testF);
	//audioThread->detach();
	//this->doThread();
}


// Function that will be executed in a separate thread
//void audio(Music &music) {
//	InitAudioDevice();
//
//	music = LoadMusicStream("C:/edmMusic.mp3");
//	
//	PlayMusicStream(music);
//	while (1)
//		UpdateMusicStream(music);
//}

void GLViewNewModule::createNewModuleWayPoints()
{
	// Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
	WayPointParametersBase params(this);
	params.frequency = 5000;
	params.useCamera = true;
	params.visible = true;
	WOWayPointSpherical* wayPt = WOWayPointSpherical::New(params, 3);
	wayPt->setPosition(Vector(50, 0, 3));
	worldLst->push_back(wayPt);
}

void GLViewNewModule::audioContainer() {

	//std::string sound4(ManagerEnvironmentConfiguration::getSMM() + "/sounds/sound4.wav");
	//char* cstr = new char[sound4.length() + 1];
	//std::strcpy(cstr, sound4.c_str());

	//std::string sound3D(ManagerEnvironmentConfiguration::getSMM() + "/sounds/sound5.wav");

	//ISoundEngine* engine = createIrrKlangDevice();
	//ISound* s = engine->play2D(cstr, true);

	//ISoundEngine* engine3D = createIrrKlangDevice();

	//irrklang::vec3df b_position(149, 149, 0.5);
	//
	//s_3d = engine->play3D(cstr, b_position, true, false, true);

	//irrklang::vec3df veloc(-10, 0, 0);

	//engine->setDopplerEffectParameters(1.0);

	//s_3d->setVelocity(veloc);
}

void GLViewNewModule::Vec2Float(Vector vec, float(*camF)[3]) {
	(*camF)[0] = vec.x;
	(*camF)[1] = vec.y;
	(*camF)[2] = vec.z;
}

void GLViewNewModule::spawn_box() {

	std::string shinyRedPlasticCube(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl");

	float camPosF[3];

	Vector distance(20, 20, 40);

	Vector spawnPosition = this->cam->getPosition() + (this->cam->getLookDirection() * distance);

	Vec2Float(spawnPosition, &camPosF);

	WOPhysx* WOpxobj_spawn = WOPhysx::New(shinyRedPlasticCube, Vector(0.5, 0.5, 0.5), MESH_SHADING_TYPE::mstSMOOTH, WOpxObj_groundfloor->gScene);
	WOpxobj_spawn->setPosition(camPosF[0], camPosF[1], camPosF[2] + 5);

	WOpxobj_spawn->renderOrderType = RENDER_ORDER_TYPE::roTRANSPARENT;
	WOpxobj_spawn->setLabel("red box");
	this->worldLst->push_back(WOpxobj_spawn);
	WOID++;


	unsigned int numBoxes = this->physx_boxes.size();
	this->physx_boxes.resize(numBoxes + 1);

	physx_boxes[physx_boxes.size() - 1] = static_cast<WOPhysx*>(WOpxobj_spawn->actor->userData);
	physx_boxes[physx_boxes.size() - 1]->pxWO_ID = WOID;
}