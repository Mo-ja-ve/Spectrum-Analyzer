
#ifndef WOPHYSXGROUND_H
#define WOPHYSXGROUND_H

#include "WO.h"
#include "Mat4.h"

namespace Aftr {

    class WOPhysxGround : public WO {
    public:
        ~WOPhysxGround();
        WOPhysxGround();

        static WOPhysxGround* New(const std::string& path, const Vector& scale, Aftr::MESH_SHADING_TYPE mst);
        void init();
        void onCreate(const std::string& path, const Vector& scale, Aftr::MESH_SHADING_TYPE mst);
        void updatePoseFromPhysx();
        void setPosition(float x, float y, float z);

        physx::PxDefaultAllocator gAllocator;
        physx::PxDefaultErrorCallback gErrorcallback;
        physx::PxRigidDynamic* actor = NULL;
        physx::PxFoundation* gFoundation = NULL;

        physx::PxPhysics* gPhysics;
        physx::PxScene* gScene;
    };
}

#endif