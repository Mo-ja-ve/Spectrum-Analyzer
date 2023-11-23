#include "WOPhysx_cameraCollider.h"

namespace Aftr {

    WOPhysx_cameraCollider::~WOPhysx_cameraCollider() { }

    WOPhysx_cameraCollider::WOPhysx_cameraCollider() : WO(), IFace(this) {  }


    WOPhysx_cameraCollider* WOPhysx_cameraCollider::New(const std::string& path, const Vector& scale, Aftr::MESH_SHADING_TYPE mst, physx::PxScene* gScene) {

        WOPhysx_cameraCollider* WOpx = new WOPhysx_cameraCollider();
        WOpx->init();
        WOpx->onCreate(path, scale, mst, gScene);

        return WOpx;
    }

    void WOPhysx_cameraCollider::init() {

        /***     physx init stuff      ***/
        gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorcallback);
        gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), 0, NULL);
        physx::PxSceneDesc gSceneDesc(gPhysics->getTolerancesScale());
        gSceneDesc.flags.setAll(physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS);

        gSceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
        gSceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);

        dummy_gScene = gPhysics->createScene(gSceneDesc);
    }

    void WOPhysx_cameraCollider::onCreate(const std::string& path, const Vector& scale, Aftr::MESH_SHADING_TYPE mst, physx::PxScene* gScene) {

        WO::onCreate(path, scale, mst);
        
        physx::PxMaterial* gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
        
        physx::PxRigidDynamic* cameraActor = physx::PxCreateDynamic(*gPhysics, physx::PxTransform(physx::PxVec3(0, 0, 10.0f)), physx::PxBoxGeometry(3.0f, 3.0f, 3.0f), *gMaterial, 10.0f);
        gScene->addActor(*cameraActor);
        physx::PxShape* shape = gPhysics->createShape(physx::PxBoxGeometry(3, 3, 3), *gMaterial, true);
        physx::PxTransform t({ 0, 0, 0 });

        actor = gPhysics->createRigidDynamic(t);
        actor->attachShape(*shape);

        actor->userData = this;
        gScene->addActor(*actor);
    }

    void WOPhysx_cameraCollider::updatePoseFromPhysx() {

        physx::PxMat44 m(this->actor->getGlobalPose().q);
        Mat4 m2;

        for (int i = 0; i < 16; i++) {
            m2[i] = m(i % 4, i / 4);
        }
        this->setDisplayMatrix(m2);
        this->setPosition(this->actor->getGlobalPose().p.x, this->actor->getGlobalPose().p.y, this->actor->getGlobalPose().p.z);
    }

    void WOPhysx_cameraCollider::setPosition(float x, float y, float z) {

        WO::setPosition(x, y, z);
        physx::PxTransform t = this->actor->getGlobalPose();
        t.p = physx::PxVec3(x, y, z);
        this->actor->setGlobalPose(t);
    }
}