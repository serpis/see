#ifndef _RENDER_SYSTEM_HPP
#define _RENDER_SYSTEM_HPP

#include "entity_system.hpp"

class render_system_t : public system_t
{
public:
    void init();
    void update(float dt);
};


#endif // _RENDER_SYSTEM_HPP

