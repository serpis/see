#ifndef _PHYSICS_SYSTEM_HPP
#define _PHYSICS_SYSTEM_HPP

#include "entity_system.hpp"

class physics_system_t : public system_t
{
public:
    void init();
    void update(float dt);
    class btRigidBody *create_rigid_sphere(float radius, float mass);
    class btRigidBody *create_rigid_cube(float radius, float mass);
    class btRigidBody *create_rigid_heightmap(class heightmap_t &heightmap);
};

#endif // _PHYSICS_SYSTEM_HPP

