// need this for key constants
#include <GL/glfw.h>

#include "quat.h"

#include "components.hpp"
#include "input_system.hpp"

int mouse_x = 0;
int mouse_y = 0;

void input_system_t::init()
{
    memset(this->keys, 0, sizeof(this->keys));
    memset(this->buttons, 0, sizeof(this->buttons));
    this->mouse_x = 0;
    this->mouse_y = 0;
}

void input_system_t::update(float dt)
{
    entity_list_t players = entity_manager_t::default_manager->entities_possessing_component_type(typeid(player_component_t));
    assert(players.size() == 1);
    int player_entity = *players.begin();
    position_component_t *position = entity_manager_t::default_manager->get_component<position_component_t>(player_entity);
    orientation_component_t *orientation = entity_manager_t::default_manager->get_component<orientation_component_t>(player_entity);
    assert(position);
    assert(orientation);

    vec3_t<> forward = orientation->rotation.forward();
    vec3_t<> up = orientation->rotation.up();
    vec3_t<> right = orientation->rotation.right();

    //orientation->right = (orientation->forward.cross(vec3_t<>(0.0f, 1.0f, 0.0f))).normalized();
    //orientation->up = orientation->right.cross(orientation->forward);

    float speed = 5.0f;

    if (keys['W'])
    {
        position->xyz += dt*speed*forward;
    }
    if (keys['S'])
    {
        position->xyz -= dt*speed*forward;
    }
    if (keys['A'])
    {
        position->xyz -= dt*speed*right;
    }
    if (keys['D'])
    {
        position->xyz += dt*speed*right;
    }
    if (keys['Q'])
    {
        position->xyz += dt*speed*up;
    }
    if (keys['E'])
    {
        position->xyz -= dt*speed*up;
    }

    static int last_mouse_x = mouse_x;
    static int last_mouse_y = mouse_y;
    int mouse_dx = mouse_x - last_mouse_x;
    int mouse_dy = mouse_y - last_mouse_y;
    last_mouse_x = mouse_x;
    last_mouse_y = mouse_y;

    if (buttons[GLFW_MOUSE_BUTTON_RIGHT])
    {
        float mouse_sensitivity = 0.01f;

        float rotate_left = -mouse_sensitivity*mouse_dx;
        float rotate_up = -mouse_sensitivity*mouse_dy;

        static float total_rotate_left = 0.0f;
        total_rotate_left += rotate_left;
        static float total_rotate_up = 0.0f;
        total_rotate_up += rotate_up;

        orientation->rotation = quat_t<>(vec3_t<>(0.0f, 1.0f, 0.0f), total_rotate_left)
                              * quat_t<>(vec3_t<>(0.0f, 0.0f, 1.0f), total_rotate_up);

        /*quat l(cos(total_rotate_left / 2), 0.0f, sin(total_rotate_left / 2), 0.0f);
        quat u(cos(total_rotate_up / 2), 0.0f, 0.0f, sin(total_rotate_up / 2));

        quat r(1.0f);
        r = r * l;
        r = r * u;

        quat_t<> a = orientation->rotation;
        std::cout << "my (" << a.w << ", " << a.x << ", " << a.y << ", " << a.z << ", " << ")" << std::endl;
        std::cout << "th " << r << std::endl;*/
    }

    //orientation->forward = orientation->rotation.forward();
    //orientation->up = orientation->rotation.up();
    //orientation->right = orientation->rotation.right();

    /*if (keys[GLFW_KEY_UP])
    {
        orientation->forward = (mat4_t<>::rotation( dt*3.0f, orientation->right)*vec4_t<>(orientation->forward, 0.0f)).xyz();
    }
    if (keys[GLFW_KEY_DOWN])
    {
        orientation->forward = (mat4_t<>::rotation(-dt*3.0f, orientation->right)*vec4_t<>(orientation->forward, 0.0f)).xyz();
    }
    if (keys[GLFW_KEY_LEFT])
    {
        orientation->forward = (mat4_t<>::rotation( dt*3.0f, vec3_t<>(0.0f, 1.0f, 0.0f))*vec4_t<>(orientation->forward, 0.0f)).xyz();
    }
    if (keys[GLFW_KEY_RIGHT])
    {
        orientation->forward = (mat4_t<>::rotation(-dt*3.0f, vec3_t<>(0.0f, 1.0f, 0.0f))*vec4_t<>(orientation->forward, 0.0f)).xyz();
    }*/

    // Re-calculate camera coordinate system since forward changed
    //orientation->right = (orientation->forward.cross(vec3_t<>(0.0f, 1.0f, 0.0f))).normalized();
    //orientation->up = orientation->right.cross(orientation->forward);
}


void input_system_t::key_down(int key)
{
    this->keys[key] = true;
    if (key == 'P')
    {
        entity_manager_t *manager = entity_manager_t::default_manager;
        for (entity_storage_t::iterator it = manager->entity_storage.begin(); it != manager->entity_storage.end(); ++it)
        {
            int entity = *it;
            printf("entity %d\n", entity);
            component_list_t components = manager->components_of_entity(entity);
            for (component_list_t::iterator cit = components.begin(); cit != components.end(); ++cit)
            {
                print_component_contents(*cit);
            }
        }
    }
}

void input_system_t::key_up(int key)
{
    this->keys[key] = false;
}



void input_system_t::button_down(int button)
{
    this->buttons[button] = true;
}

void input_system_t::button_up(int button)
{
    this->buttons[button] = false;
}


void input_system_t::mouse_move(int x, int y)
{
    this->mouse_x = x;
    this->mouse_y = y;

    ::mouse_x = x;
    ::mouse_y = y;
}

