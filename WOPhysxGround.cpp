#include "WOPhysxGround.h"

/************************************************************/
/*              WOPHysx function definitions                */
/************************************************************/
namespace Aftr {

    WOPhysxGround::~WOPhysxGround() { }

    WOPhysxGround::WOPhysxGround() : WO(), IFace(this) {  }


    WOPhysxGround* WOPhysxGround::New(const std::string& path, const Vector& scale, Aftr::MESH_SHADING_TYPE mst) {

        WOPhysxGround* WOpx = new WOPhysxGround();
        WOpx->init();
        WOpx->onCreate(path, scale, mst);

        return WOpx;
    }

    void WOPhysxGround::init() {

        /***     physx init stuff      ***/
        gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorcallback);
        gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), 0, NULL);
        physx::PxSceneDesc gSceneDesc(gPhysics->getTolerancesScale());
        gSceneDesc.flags.setAll(physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS);
        gSceneDesc.gravity = physx::PxVec3(0.0f, 0.0f, -9.81f);

        gSceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
        gSceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);

        gScene = gPhysics->createScene(gSceneDesc);
    }

    void WOPhysxGround::onCreate(const std::string& path, const Vector& scale, Aftr::MESH_SHADING_TYPE mst) {

        WO::onCreate(path, scale, mst);
        //  createMaterial creates a material according to the paramaters which describe the properties of the new material 
        physx::PxMaterial* gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
        //  creating a static rigid object this time instead of dynamic rigid (because ground doesn't move (well most of the time))
        physx::PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, physx::PxPlane(0, 0, 1, 0), *gMaterial);
        gScene->addActor(*groundPlane);
        physx::PxShape* shape = gPhysics->createShape(physx::PxPlaneGeometry(), *gMaterial, true);
        physx::PxTransform t({ 0, 0, 0 });

        actor = gPhysics->createRigidDynamic(t);
        actor->attachShape(*shape);

        actor->userData = this;
        gScene->addActor(*actor);
    }

    void WOPhysxGround::updatePoseFromPhysx() {

        physx::PxMat44 m(this->actor->getGlobalPose().q);
        Mat4 m2;

        for (int i = 0; i < 16; i++) {
            m2[i] = m(i % 4, i / 4);
        }
        this->setDisplayMatrix(m2);
        this->setPosition(this->actor->getGlobalPose().p.x, this->actor->getGlobalPose().p.y, this->actor->getGlobalPose().p.z);
    }

    void WOPhysxGround::setPosition(float x, float y, float z) {

        WO::setPosition(x, y, z);
        physx::PxTransform t = this->actor->getGlobalPose();
        t.p = physx::PxVec3(x, y, z);
        this->actor->setGlobalPose(t);
    }
}