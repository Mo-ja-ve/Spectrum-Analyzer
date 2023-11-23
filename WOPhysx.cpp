#include "WOPhysx.h"

/************************************************************/
/*              WOPHysx function definitions                */
/************************************************************/
namespace Aftr {

    WOPhysx::~WOPhysx() { }

    WOPhysx::WOPhysx() : WO(), IFace(this) {  }


    WOPhysx* WOPhysx::New( const std::string& path, const Vector& scale, Aftr::MESH_SHADING_TYPE mst, physx::PxScene *gScene) {

        WOPhysx* WOpx = new WOPhysx();
        WOpx->init();
        WOpx->onCreate(path, scale, mst, gScene);

        return WOpx;
    }

    void WOPhysx::init() {

        /***     physx init stuff      ***/
        gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorcallback);
        gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), 0, NULL);
        physx::PxSceneDesc gSceneDesc(gPhysics->getTolerancesScale());
        gSceneDesc.flags.setAll(physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS);
        gSceneDesc.gravity = physx::PxVec3(0.0f, 0.0f ,-9.81f);

        gSceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
        gSceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);

        dummy_gScene = gPhysics->createScene(gSceneDesc);

        pxWO_ID = 0;
        physx_release = false;
    }

    void WOPhysx::onCreate(const std::string& path, const Vector& scale, Aftr::MESH_SHADING_TYPE mst, physx::PxScene *gScene) {

        WO::onCreate(path, scale, mst);
        physx::PxMaterial* gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

        physx::PxRigidDynamic* dynamic = physx::PxCreateDynamic(*gPhysics, physx::PxTransform(physx::PxVec3(0, 0, 10.0f)), physx::PxBoxGeometry(2.0f, 2.0f, 2.0f), *gMaterial, 10.0f);
        gScene->addActor(*dynamic);
        physx::PxShape* shape = gPhysics->createShape(physx::PxBoxGeometry(1.1, 1.1, 1.1), *gMaterial, true);
        physx::PxTransform t({ 0,0,0 });

        actor = gPhysics->createRigidDynamic(t);
        actor->attachShape(*shape);
       
        actor->userData = this;
        gScene->addActor(*actor);
    }

    void WOPhysx::updatePoseFromPhysx() {

        physx::PxMat44 m(this->actor->getGlobalPose().q);
        Mat4 m2;
        
        for (int i = 0; i < 16; i++) {
            m2[i] = m(i % 4, i / 4);
        }
        this->setDisplayMatrix(m2);
        this->setPosition(this->actor->getGlobalPose().p.x, this->actor->getGlobalPose().p.y, this->actor->getGlobalPose().p.z);
    }

    void WOPhysx::setPosition(float x, float y, float z) {

        WO::setPosition(x, y, z);
        physx::PxTransform t = this->actor->getGlobalPose();
        t.p = physx::PxVec3(x, y, z);
        this->actor->setGlobalPose(t);
    }
}