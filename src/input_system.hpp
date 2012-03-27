#ifndef _INPUT_SYSTEM_HPP
#define _INPUT_SYSTEM_HPP

#include "entity_system.hpp"

class input_system_t : public system_t
{
public:
    bool keys[65536];
    bool buttons[65536];
    int mouse_x;
    int mouse_y;

    void init();
    void update(float dt);

    void key_down(int key);
    void key_up(int key);
    void button_down(int button);
    void button_up(int button);
    void mouse_move(int x, int y);
};

#endif // _INPUT_SYSTEM_HPP

