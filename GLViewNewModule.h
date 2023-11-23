#pragma once

#include "GLView.h"
#include "../../../include/irrKlang/irrKlang.h"
#include "NetMsg.h"
#include "NetMessengerClient.h"
#include "../../../include/aftr/NetMsgCreateWO_2.h"
#include <thread>

//#include "WO_imgui.h"
//#include "implot.h"
#include "WOPhysx.h"
#include "WOPhysxGround.h"
#include "WOPhysx_cameraCollider.h"
#include "raudio.h"

using namespace irrklang;
    
namespace Aftr
{
   class Camera;

/**
   \class GLViewNewModule
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/

class GLViewNewModule : public GLView
{

public:
   static GLViewNewModule* New( const std::vector< std::string >& outArgs );
   virtual ~GLViewNewModule();
   virtual void updateWorld(); ///< Called once per frame
   virtual void loadMap(); ///< Called once at startup to build this module's scene
   virtual void createNewModuleWayPoints();
   virtual void onResizeWindow( GLsizei width, GLsizei height );
   virtual void onMouseDown( const SDL_MouseButtonEvent& e );
   virtual void onMouseUp( const SDL_MouseButtonEvent& e );
   virtual void onMouseMove( const SDL_MouseMotionEvent& e );
   virtual void onKeyDown( const SDL_KeyboardEvent& key );
   virtual void onKeyUp( const SDL_KeyboardEvent& key );

   virtual void audioContainer();

   void spawn_box();

   //InitAudioDevice();
   //Music music;
   //LoadMusicStream("C:/edmMusic.mp3");
   //PlayMusicStream(music);

   WOPhysxGround* WOpxObj_groundfloor;
   std::vector <WOPhysx*> physx_boxes;
   WOPhysx_cameraCollider* camPxActor;
   NetMessengerClient* client;
   NetMsgCreateWO_2 msg;
   void Vec2Float(Vector vec, float(*camF)[3]);
   
   void doThread();
   //void fftThread();
   
   std::thread* audioThread = nullptr;
   //std::thread* audioThreadFFT = nullptr;

   void audioF();

   Music music;
   //void audio(Music &m);

   int WOID = 8;
   irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
   irrklang::ISound* s_3d;
   //std::thread audioThread;

protected:
   GLViewNewModule( const std::vector< std::string >& args );
   virtual void onCreate();
};

/** \} */

} //namespace Aftr
