#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

#include "components.hpp"
#include "physics_system.hpp"
#include "heightmap.hpp"

static btBroadphaseInterface *broadphase;
static btDefaultCollisionConfiguration *collisionConfiguration;
static btCollisionDispatcher *dispatcher;
static btSequentialImpulseConstraintSolver *solver;
static btDiscreteDynamicsWorld *dynamicsWorld;

static btCollisionShape *groundShape;
static btCollisionShape *fallShape;
static btDefaultMotionState* groundMotionState;
static btRigidBody* groundRigidBody;
static btDefaultMotionState* fallMotionState;
static btRigidBody* fallRigidBody;

void physics_system_t::init()
{
    // Build the broadphase
    broadphase = new btDbvtBroadphase();
 
    // Set up the collision configuration and dispatcher
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
 
    // The actual physics solver
    solver = new btSequentialImpulseConstraintSolver;
 
    // The world.
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -10, 0));

    groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
    fallShape = new btSphereShape(1);

    groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo
    groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    groundRigidBody = new btRigidBody(groundRigidBodyCI);

    //dynamicsWorld->addRigidBody(groundRigidBody);

    fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));

    btScalar mass = 1;
    btVector3 fallInertia(0, 0, 0);
    fallShape->calculateLocalInertia(mass,fallInertia);

    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
    fallRigidBody = new btRigidBody(fallRigidBodyCI);
    //dynamicsWorld->addRigidBody(fallRigidBody);
}

void physics_system_t::update(float dt)
{
    entity_manager_t::default_manager->iterate_nodes<physics_component_t, position_component_t, orientation_component_t, player_component_t>(2, [](physics_component_t *physics, position_component_t *pos, orientation_component_t *ori, player_component_t *player)
    {
        btRigidBody *b = physics->rigid_body;
        btTransform transform = b->getCenterOfMassTransform();
        transform.setOrigin(btVector3(pos->xyz.x, pos->xyz.y, pos->xyz.z));
        if (!physics->in_system || player)
        {
            if (ori)
            {
                transform.setRotation(btQuaternion(ori->rotation.x, ori->rotation.y, ori->rotation.z, ori->rotation.w));
            }
            b->setCenterOfMassTransform(transform);
            b->getMotionState()->setWorldTransform(transform);
            b->activate(true);
        }

        if (!physics->in_system && player)
        {
            b->setCollisionFlags(b->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            b->setActivationState(DISABLE_DEACTIVATION);
        }


        if (!physics->in_system)
        {
            dynamicsWorld->addRigidBody(b);
            physics->in_system = true;
        }
    });

    dynamicsWorld->stepSimulation(dt, 10);

    entity_manager_t::default_manager->iterate_nodes<physics_component_t, position_component_t, orientation_component_t, player_component_t>(2, [](physics_component_t *physics, position_component_t *pos, orientation_component_t *ori, player_component_t *player)
    {
        btRigidBody *b = physics->rigid_body;
        btTransform transform = b->getCenterOfMassTransform();

        btVector3 origin = transform.getOrigin();
        pos->xyz.x = origin.getX();
        pos->xyz.y = origin.getY();
        pos->xyz.z = origin.getZ();

        if (ori && !player)
        {
            btQuaternion rotation = transform.getRotation();
            ori->rotation.x = rotation.getX();
            ori->rotation.y = rotation.getY();
            ori->rotation.z = rotation.getZ();
            ori->rotation.w = rotation.getW();
        }

        //dynamicsWorld->removeRigidBody(b);
    });
}

btRigidBody *physics_system_t::create_rigid_sphere(float radius, float mass)
{
    btCollisionShape *shape = new btSphereShape(radius);
    btDefaultMotionState *motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
    btVector3 inertia(0, 0, 0);
    shape->calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, shape, inertia);
    btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);

    return rigidBody;
}

btRigidBody *physics_system_t::create_rigid_cube(float radius, float mass)
{
    btCollisionShape *shape = new btBoxShape(btVector3(radius, radius, radius));
    btDefaultMotionState *motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
    btVector3 inertia(0, 0, 0);
    shape->calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, shape, inertia);
    btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);

    return rigidBody;
}

btRigidBody *physics_system_t::create_rigid_heightmap(class heightmap_t &heightmap)
{
    int ds_width = heightmap.width / 4;
    int ds_height = heightmap.height / 4;
    float *data = new float[ds_width * ds_height];
    float dy = heightmap.xyz_high.y - heightmap.xyz_low.y;
    for (int i = 0; i < ds_width * ds_height; i++)
    {
        int ds_x = i % ds_width;
        int ds_y = i / ds_width;

        int lookup = ds_x * 4 + ds_y * 4 * heightmap.width;

        data[i] = heightmap.xyz_low.y + dy * heightmap.heights[lookup];
    }
    btHeightfieldTerrainShape *shape = new btHeightfieldTerrainShape(ds_width, ds_height, data, 1.0f, -50, 50, 1, PHY_FLOAT, false);
    float scale_x = (heightmap.xyz_high.x - heightmap.xyz_low.x) / ds_width;
    float scale_z = (heightmap.xyz_high.z - heightmap.xyz_low.z) / ds_height;
    shape->setLocalScaling(btVector3(scale_x, 1.0f, scale_z));
    btDefaultMotionState *motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
    btVector3 inertia(0, 0, 0);
    shape->calculateLocalInertia(0.0f, inertia);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0.0f, motionState, shape, inertia);
    btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);

    //delete[] data;

    return rigidBody;
}

