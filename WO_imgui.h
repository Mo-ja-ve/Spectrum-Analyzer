#pragma once
#ifndef WO_IMGUI_H
#define WO_IMGUI_H

//#include "AftrConfig.h"
//#ifdef AFTR_CONFIG_USE_IMGUI

//#ifndef IMGUI_H
//#define IMGUI_H
//#include "C:\repos\repo_distro\aburn\engine\src\external\imgui\imgui.h"
//#endif
#include "WOImGuiAbstract.h"
#include <functional>
#include "C:\repos\repo_distro\aburn\engine\src\external\imgui\imgui_internal.h"
#include "C:\repos\repo_distro\aburn\engine\src\external\imgui\imgui_impl_opengl3.h"
#include "C:\repos\repo_distro\aburn\engine\src\external\imgui\imgui_impl_sdl.h"
//#include "WOGUI.h"
//#include "C:\repos\repo_distro\aburn\engine\src\external\imgui\AftrImGuiIncludes.h"

#include "WOPhysx.h"
#include "GLViewNewModule.h"
#include "WO.h"
#include "GLView.h"
#include "WorldContainer.h"

//Written By Scott Nykl

namespace Aftr
{
	class GLViewNewModule;
    class WO_imgui : public WOImGuiAbstract
    {
    public:

        ///This immediate mode, declaritive GUI, which employs a library called ImGui,
        ///uses functional composition to draw gui widgets. To add any gui widgets you
        ///desire, simply write a method that draws an ImGui TreeNode using the following
        ///syntax:
        /*
           { //Pretend this is in the GLView::loadMap() or some other convenient place

              WO_MyAwesomeWO* myWO = WO_MyAwesomeWO::New( ... );
                 //Assume that the WO_MyAwesomeWO above defines this method to draw the gui widgets:
                 void WO_MyAwesomeWO::my_ImGui_draw_method( int myParam )
                 {
                    if( ImGui::TreeNode( "My Gui Options" ) )
                    {
                       ImGui::Text( "This is my gui" );
                       //ImGui::Checkbox( "Add More Widgets here", &myVal);
                       ImGui::TreePop(); //ends your TreeNode (the "collapse" arrow hides all widgets
                                         //between here and the above call to ImGui::TreeNode(...)
                    }
                    ImGui::Separator(); //draws bottom separator
                 }

              //Now Create the gui and ensure it draws the above function/method via a lambda subscriber.
              //Here we are in the GLView::loadMap() method.
              int theParam = 42;
              WOImGui* gui = WOImGui::New( nullptr );
              gui->subscribe_drawImGuiWidget(
                 [this, theParam, gui]()
                 {
                    //Here is a super neato paradigm that could be extended to support real-time introspection.
                    //This is the glue code that is called from within the WOImGui instance...
                    //Whenever a WO wants to draw some Gui stuff about that WO's internal state,
                    //it simply asks the gui to call a function. That function is this lambda.
                    //Since we don't want to put the entire ImGui draw call right here (even though
                    //we could), let's just have the lambda call the corresponding WO's drawImGui_
                    //method.
                    this->myWO->my_ImGui_draw_method( theParam );
                 } );
              this->worldLst->push_back( gui );
           }
        */

        //    void show_text() { 
                //ImGuiIO& io = ImGui::GetIO();
            //    ImGUi::Begin("hello", );
            //    ImGui::BulletText("hello!"); 
            //}

        std::vector< std::string > args;

        using Callback_OnDrawImGui = std::function< void() >;

        //static WO_imgui* New(WOGUI* parentWOGUI, float width = 1.0f, float height = 1.0f);
        //virtual ~WO_imgui() override;
        //virtual void drawImGui_for_this_frame() override;
		void subscribe_drawImGuiWidget(Callback_OnDrawImGui callback) {
			this->subscribers.push_back(callback);
		}

        void physics_window(bool* show_physicsWindow);

        //void Vec2Float(Vector vec, float(*camF)[3]);

        //void showMenuBar(static bool *p_open);
        int physx_toggle;
        WOPhysx *physxWO_ptr;
        Aftr::GLViewNewModule* v = ManagerGLView::getGLViewT<GLViewNewModule>();
        int numberOfBoxes = 0;

		static WO_imgui* New(WOGUI* parentWOGUI, float width, float height) {

			WO_imgui* imgui = new WO_imgui(parentWOGUI);
			imgui->onCreate(width, height);
			imgui->physxWO_ptr = NULL;
			return imgui;
		}

		virtual ~WO_imgui() override { }

		virtual void drawImGui_for_this_frame() override {
			//Aftr::GLViewNewModule* v = ManagerGLView::getGLViewT<GLViewNewModule>();

			static bool show_metrics = false;
			static bool show_menu = false;
			static bool show_physicsObjects_menu;
			static bool* p_open = NULL;

			if (show_metrics) { ImGui::ShowMetricsWindow(); }

			// Demonstrate the various window flags. Typically you would just use the default!
			static bool no_titlebar = false;
			static bool no_scrollbar = false;
			static bool no_menu = false;
			static bool no_move = false;
			static bool no_resize = false;
			static bool no_collapse = false;
			static bool no_close = false;
			static bool no_nav = false;
			static bool no_background = false;
			static bool no_bring_to_front = false;

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

			// Main body of the Demo window starts here.
			if (!ImGui::Begin("Interface", p_open, window_flags))
			{
				// Early out if the window is collapsed, as an optimization.
				ImGui::End();
				return;
			}

			if (show_physicsObjects_menu) {
				ImGui::Text("MAP OBJECTS & PHYSICS CONTROLS");

				if (ImGui::Button("TOGGLE GRAVITY")) {

					static bool boolFlag_gravity = false;
					switch (boolFlag_gravity) {
					case false:
						for (int i = 0; i < v->physx_boxes.size(); i++)
							v->physx_boxes[i]->actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
						v->camPxActor->actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
						boolFlag_gravity = true;
						break;

					case true:
						for (int i = 0; i < v->physx_boxes.size(); i++)
							v->physx_boxes[i]->actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
						v->camPxActor->actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
						boolFlag_gravity = false;
						break;
					}
				}
				ImGui::SameLine();
				ImGui::Text("Red Box count: %d", numberOfBoxes);
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();

				if (ImGui::CollapsingHeader("RED BOXES"))
				{
					ImGuiIO& io = ImGui::GetIO();

					if (ImGui::TreeNode("Configuration"))
					{

						if (ImGui::Button("SPAWN BOX")) {
							this->v->spawn_box();
							this->numberOfBoxes++;
						}

						static int clicked = 0;
						if (ImGui::Button("DELETE BOX"))
							clicked++;

						if (clicked & 1)
						{
							static char str0[3] = ""; ImGui::SameLine();
							ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0)); ImGui::SameLine();
							if (ImGui::Button("DELETE")) {
								int boxNum = (int)str0[0] - 48;
								if (boxNum >= 0 && boxNum < v->physx_boxes.size()) {
									std::cout << std::endl << "BOX NUMBER: " << v->physx_boxes[boxNum]->pxWO_ID << std::endl;
									v->getWorldContainer()->eraseViaWOIndex(v->physx_boxes[boxNum]->pxWO_ID);

									v->physx_boxes[boxNum]->actor->release();
									v->physx_boxes.erase(v->physx_boxes.begin() + boxNum);
									v->WOID--;

									if (v->physx_boxes.size() != 0)
										for (int i = 0; i < v->physx_boxes.size(); i++)
											v->physx_boxes[i]->pxWO_ID--;
								}
							}
						}

						if (ImGui::Button("MOVE")) {
							//this->physxWO_ptr->setPosition(10.0f, 10.0f, 10.0f);
						}

						if (ImGui::Button("TOGGLE PHYSICS")) {

							static bool flag = false;
							switch (flag) {
							case false:
								for (int i = 0; i < this->v->physx_boxes.size(); i++)
									this->v->physx_boxes[i]->actor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
								flag = true;
								break;

							case true:
								for (int i = 0; i < this->v->physx_boxes.size(); i++)
									this->v->physx_boxes[i]->actor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, false);
								flag = false;
								break;
							}
						}

						if (io.ConfigFlags & ImGuiConfigFlags_NoMouse) // Create a way to restore this flag otherwise we could be stuck completely!
						{
							if (fmodf((float)ImGui::GetTime(), 0.40f) < 0.20f)
							{
								ImGui::SameLine();
								ImGui::Text("<<PRESS SPACE TO DISABLE>>");
							}
							if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space)))
								io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
						}

						ImGui::TreePop();
						ImGui::Separator();
					}

					if (ImGui::TreeNode("BOX LIST"))
					{
						if (ImGui::Button("BOXES")) {
							for (int i = 0; i < v->getWorldContainer()->size(); i++) {
								std::cout << std::endl << "RED BOX at : " << i << std::endl;
								if (v->getWorldContainer()->at(i)->getLabel() == "red box") {
									ImGui::Text("BOX: %d", i);
									std::cout << std::endl << "RED BOX at : " << i << std::endl;
									std::cout << std::endl << "RED BOX at : " << i << std::endl;
									std::cout << std::endl << "RED BOX at : " << i << std::endl;
									std::cout << std::endl << "RED BOX at : " << i << std::endl;
								}
							}
						}
						ImGui::TreePop();
						ImGui::Separator();
					}
				}
			}

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Menu"))
				{
					ImGui::MenuItem("Metrics", NULL, &show_metrics);
					ImGui::MenuItem("Objects & Physics", NULL, &show_physicsObjects_menu);
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

		}

		void showMenuBar(static bool* p_open) {


		}

		void Vec2Float(Vector vec, float(*camF)[3]) {
			(*camF)[0] = vec.x;
			(*camF)[1] = vec.y;
			(*camF)[2] = vec.z;
		}

    protected:
        //void onCreate(float width, float height);
        //WO_imgui(WOGUI* parentWOGUI);
		WO_imgui(WOGUI* parentWOGUI) : IFace(this), WOImGuiAbstract(parentWOGUI) { }
		void onCreate(float width, float height) {

			WOImGuiAbstract::onCreate(width, height);
		}

    private:

        std::vector< Callback_OnDrawImGui > subscribers;
    };

}

#endif