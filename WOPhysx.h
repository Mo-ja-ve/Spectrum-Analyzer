//#pragma once

#ifndef WOPHYSX_H
#define WOPHYSX_H

#include "WO.h"
#include "Mat4.h"

namespace Aftr{

    class WOPhysx : public WO {
    public:
        ~WOPhysx();
        WOPhysx();

        static WOPhysx* New(const std::string& path, const Vector& scale, Aftr::MESH_SHADING_TYPE mst, physx::PxScene *gScene);
        void init();
        void onCreate( const std::string& path, const Vector& scale, Aftr::MESH_SHADING_TYPE mst, physx::PxScene *gScene);
        void updatePoseFromPhysx();
        void setPosition(float x, float y, float z);

        physx::PxDefaultAllocator gAllocator;
        physx::PxDefaultErrorCallback gErrorcallback;
        physx::PxRigidDynamic* actor = NULL;
        physx::PxFoundation* gFoundation = NULL;
      
        physx::PxPhysics* gPhysics;
        physx::PxScene* dummy_gScene;

        int pxWO_ID;
        bool physx_release;
    };
}

#endif