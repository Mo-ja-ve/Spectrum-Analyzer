#ifndef WOPHYSX_CAMERACOLLIDER_H
#define WOPHYSX_CAMERACOLLIDER_H

#include "WO.h"
#include "Mat4.h"

namespace Aftr{

    class WOPhysx_cameraCollider : public WO {
    public:
        ~WOPhysx_cameraCollider();
        WOPhysx_cameraCollider();

        static WOPhysx_cameraCollider* New(const std::string& path, const Vector& scale, Aftr::MESH_SHADING_TYPE mst, physx::PxScene* gScene);
        void init();
        void onCreate(const std::string& path, const Vector& scale, Aftr::MESH_SHADING_TYPE mst, physx::PxScene* gScene);
        void updatePoseFromPhysx();
        void setPosition(float x, float y, float z);

        physx::PxDefaultAllocator gAllocator;
        physx::PxDefaultErrorCallback gErrorcallback;
        physx::PxRigidDynamic* actor = NULL;
        physx::PxFoundation* gFoundation = NULL;

        physx::PxPhysics* gPhysics;
        physx::PxScene* dummy_gScene;
    };
}

#endif