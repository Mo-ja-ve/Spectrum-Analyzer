#include "NetMsgCreateWO_2.h"
#ifdef AFTR_CONFIG_USE_BOOST

#include <iostream>
#include <sstream>
#include <string>
#include "AftrManagers.h"
#include "Vector.h"
#include "WO.h"
#include "GLView.h"
#include "WorldContainer.h"
#include "NetMsgCreateWOReply.h"

using namespace Aftr;

NetMsgMacroDefinition( NetMsgCreateWO_2 );

NetMsgCreateWO_2::NetMsgCreateWO_2()
{
   this->xPos = 0;
   this->yPos = 0;
   this->zPos = 0;

   this->WOindex = 0;
}

NetMsgCreateWO_2::~NetMsgCreateWO_2()
{
}

bool NetMsgCreateWO_2::toStream( NetMessengerStreamBuffer& os ) const
{

   os << this->xPos;
   os << this->yPos;
   os << this->zPos;

   os << this->WOindex;
   os << this->trans_str[0];
   os << this->box_index;

   return true;
}

bool NetMsgCreateWO_2::fromStream( NetMessengerStreamBuffer& is )
{
   is >> this->xPos;
   is >> this->yPos;
   is >> this->zPos;

   is >> this->WOindex;
   is >> this->trans_str[0];
   is >> this->box_index;

   return true;
}

void NetMsgCreateWO_2::onMessageArrived()
{

	//std::cout << "Message has arrived... " << this->toString() << "\n";
	 //std::cout << "Message has arrived... " << std::endl;
	//WO* wo = WO::New( ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl" );
	/*std::cout << "Payload\n";
	std::cout << xPos << ",";
	std::cout << yPos << ",";
	std::cout << zPos << ".\n";*/
	//wo->setPosition( Vector( xPos, yPos, zPos ) );
	//wo->setLabel( "Created by IPC Net Msg" );

	 //for (int i = 0; i < 16; i++) {
	 //	m2[i] = (int)mat4_str[i];
	  //  }
	//m2 = m2.fromString(mat4_str, true);
	//std::cout << std::endl << "mat 4 string: " << m2.toString() << std::endl;
	//physx::PxTransform t = m2;

	//t.p.x = std::stof(trans_str[0], &sz);
	//t.p.y = std::stof(trans_str[1], &sz);
	//t.p.z = std::stof(trans_str[2], &sz);
	//t.q.w = std::stof(trans_str[3], &sz);
	//t.p   = physx::PxVec3(t.p.x, t.p.x, t.p.x);
	//m2.fromString(trans_str[0]);

	std::cout << std::endl << "xPos " << xPos;
	std::cout << std::endl << "yPos " << yPos;
	std::cout << std::endl << "zPos " << zPos;
	std::cout << std::endl << "WOindex " << WOindex;

	ManagerGLView::getGLView()->getWorldContainer()->at(8)->setPosition(xPos, yPos, zPos);
	//ManagerGLView::getGLView()->getWorldContainer()->at(WOindex)->setPose(m2);

	//ManagerGLView::getGLView()

	//ManagerGLView::getGLView()->getWorldContainer()->at(WOindex)->setDisplayMatrix(m2);
	//ManagerGLView::getGLView()->getWorldContainer()->at(WOindex)->setPose(m2);
	//ManagerGLView::getGLView()->getWorldContainer()->at(WOindex)->
	//std::cout << std::endl << "WO INDEX:  " << WOindex << std::endl;
	//auto msg = std::make_shared<NetMsgCreateWOReply>();
	//msg->woID = ManagerGLView::getGLView()->getWorldContainer()->at(WOindex)->getID();
	//this->sendReply(msg);
}

std::string NetMsgCreateWO_2::toString() const
{
   
	std::stringstream ss;

	ss << NetMsg::toString();
	//ss << "   Payload: " << Vector( xPos, yPos, zPos ).toString() << "...\n";
	ss << "Payload\n";   
	ss << xPos << ",";
	ss << yPos << ",";
	ss << zPos << ".\n";

   return ss.str();
}

#endif
