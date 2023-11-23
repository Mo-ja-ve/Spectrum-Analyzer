#pragma once

#include "NetMsg.h"
#include "WOPhysx.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr
{

class NetMsgCreateWO_2 : public NetMsg
{
public:
   NetMsgMacroDeclaration( NetMsgCreateWO_2);

   NetMsgCreateWO_2();
   virtual ~NetMsgCreateWO_2();
   virtual bool toStream( NetMessengerStreamBuffer& os ) const;
   virtual bool fromStream( NetMessengerStreamBuffer& is );
   virtual void onMessageArrived();
   virtual std::string toString() const;

   float xPos;
   float yPos;
   float zPos;

   //below types I added
   std::vector <WOPhysx*> box_ptr;
   std::string trans_str[4];
   physx::PxTransform t;
   Mat4 m2;
   int box_index = 0;
   int WOindex   = 0;
   int *pointer_address = 0;

protected:

};

} //namespace Aftr

#endif
