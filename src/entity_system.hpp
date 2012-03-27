#ifndef _ENTITY_SYSTEM_HPP
#define _ENTITY_SYSTEM_HPP

#include <list>
#include <string>
#include <map>
#include <set>
#include <typeinfo>
#include <cassert>

#include "math.hpp"

typedef std::list<class component_t *> component_list_t;
typedef std::list<int> entity_list_t;
typedef std::map<const std::type_info *, std::map<int, component_t *>, bool (*)(const std::type_info *a, const std::type_info *b)> component_storage_t;
typedef std::set<int> entity_storage_t;

class component_t
{
public:
    virtual void dummy_function_to_make_rtti_magic_work() {}
};

class meta_entity_t
{
public:
    int entity;
    class entity_manager_t *entity_manager;

    meta_entity_t(const std::string &name);

    void add_component(component_t *component);
    void remove_component(component_t *component);
    bool has_component_type(const std::type_info &type);
    bool has_component(component_t *component);
    template <typename T> T *get_component();
    component_list_t components();
};

class entity_manager_t
{
public:
    entity_storage_t entity_storage;
    component_storage_t component_storage;
    static class entity_manager_t *default_manager;

    int created_entity_count;

    entity_manager_t();

    int create_entity();
    //void destroy_entity(int entity);
    void add_component(int entity, component_t *component);
    void remove_component(int entity, component_t *component);
    bool has_component_type(int entity, const std::type_info &type);
    bool has_component(int entity, component_t *component);
    template <typename T> T *get_component(int entity);
    component_list_t components_of_entity(int entity);
    entity_list_t entities_possessing_component_type(const std::type_info &type);

    // iterator utility helpers
    template <typename T0> void iterate_nodes(int mandatory_components, void (*)(T0 *));
    template <typename T0, typename T1> void iterate_nodes(int mandatory_components, void (*)(T0 *, T1 *));
    template <typename T0, typename T1, typename T2> void iterate_nodes(int mandatory_components, void (*)(T0 *, T1 *, T2 *));
    template <typename T0, typename T1, typename T2, typename T3> void iterate_nodes(int mandatory_components, void (*)(T0 *, T1 *, T2 *, T3 *));
    template <typename T0, typename T1, typename T2, typename T3, typename T4> void iterate_nodes(int mandatory_components, void (*)(T0 *, T1 *, T2 *, T3 *, T4 *));
    template <typename T0> void iterate_nodes(int mandatory_components, void (*)(int, T0 *));
    template <typename T0, typename T1> void iterate_nodes(int mandatory_components, void (*)(int, T0 *, T1 *));
    template <typename T0, typename T1, typename T2> void iterate_nodes(int mandatory_components, void (*)(int, T0 *, T1 *, T2 *));
    template <typename T0, typename T1, typename T2, typename T3> void iterate_nodes(int mandatory_components, void (*)(int, T0 *, T1 *, T2 *, T3 *));
    template <typename T0, typename T1, typename T2, typename T3, typename T4> void iterate_nodes(int mandatory_components, void (*)(int, T0 *, T1 *, T2 *, T3 *, T4 *));
};

class system_t
{
    virtual void init() = 0;
    virtual void update(float dt) = 0;
};

template <typename T> T *meta_entity_t::get_component()
{
    return this->entity_manager->get_component<T>(this->entity);
}

template <typename T> T *entity_manager_t::get_component(int entity)
{
    std::map<int, component_t *> &m = this->component_storage[&typeid(T)];
    std::map<int, component_t *>::iterator res = m.find(entity);
    return (res == m.end()) ? NULL : (T *)res->second;
}

template <typename T0> void entity_manager_t::iterate_nodes(int mandatory_components, void (*callback)(int, T0 *))
{
    // don't be silly
    assert(mandatory_components >= 1);

    std::map<int, component_t *> &m = this->component_storage[&typeid(T0)];
    for (std::map<int, component_t *>::iterator it = m.begin(); it != m.end(); ++it)
    {
        int entity = it->first;
        T0 *c0 = (T0 *)it->second;

        callback(entity, c0);
    }
}

template <typename T0, typename T1> void entity_manager_t::iterate_nodes(int mandatory_components, void (*callback)(int, T0 *, T1 *))
{
    // don't be silly
    assert(mandatory_components >= 1);

    std::map<int, component_t *> &m = this->component_storage[&typeid(T0)];
    for (std::map<int, component_t *>::iterator it = m.begin(); it != m.end(); ++it)
    {
        int entity = it->first;
        T0 *c0 = (T0 *)it->second;

        T1 *c1 = this->get_component<T1>(entity);
        if (mandatory_components >= 2 && c1 == NULL)
        {
            continue;
        }

        callback(entity, c0, c1);
    }
}

template <typename T0, typename T1, typename T2> void entity_manager_t::iterate_nodes(int mandatory_components, void (*callback)(int, T0 *, T1 *, T2 *))
{
    // don't be silly
    assert(mandatory_components >= 1);

    std::map<int, component_t *> &m = this->component_storage[&typeid(T0)];
    for (std::map<int, component_t *>::iterator it = m.begin(); it != m.end(); ++it)
    {
        int entity = it->first;
        T0 *c0 = (T0 *)it->second;

        T1 *c1 = this->get_component<T1>(entity);
        if (mandatory_components >= 2 && c1 == NULL)
        {
            continue;
        }

        T2 *c2 = this->get_component<T2>(entity);
        if (mandatory_components >= 3 && c2 == NULL)
        {
            continue;
        }

        callback(entity, c0, c1, c2);
    }
}

template <typename T0, typename T1, typename T2, typename T3> void entity_manager_t::iterate_nodes(int mandatory_components, void (*callback)(int, T0 *, T1 *, T2 *, T3 *))
{
    // don't be silly
    assert(mandatory_components >= 1);

    std::map<int, component_t *> &m = this->component_storage[&typeid(T0)];
    for (std::map<int, component_t *>::iterator it = m.begin(); it != m.end(); ++it)
    {
        int entity = it->first;
        T0 *c0 = (T0 *)it->second;

        T1 *c1 = this->get_component<T1>(entity);
        if (mandatory_components >= 2 && c1 == NULL)
        {
            continue;
        }

        T2 *c2 = this->get_component<T2>(entity);
        if (mandatory_components >= 3 && c2 == NULL)
        {
            continue;
        }

        T3 *c3 = this->get_component<T3>(entity);
        if (mandatory_components >= 4 && c3 == NULL)
        {
            continue;
        }

        callback(entity, c0, c1, c2, c3);
    }
}

template <typename T0, typename T1, typename T2, typename T3, typename T4> void entity_manager_t::iterate_nodes(int mandatory_components, void (*callback)(int, T0 *, T1 *, T2 *, T3 *, T4 *))
{
    // don't be silly
    assert(mandatory_components >= 1);

    std::map<int, component_t *> &m = this->component_storage[&typeid(T0)];
    for (std::map<int, component_t *>::iterator it = m.begin(); it != m.end(); ++it)
    {
        int entity = it->first;
        T0 *c0 = (T0 *)it->second;

        T1 *c1 = this->get_component<T1>(entity);
        if (mandatory_components >= 2 && c1 == NULL)
        {
            continue;
        }

        T2 *c2 = this->get_component<T2>(entity);
        if (mandatory_components >= 3 && c2 == NULL)
        {
            continue;
        }

        T3 *c3 = this->get_component<T3>(entity);
        if (mandatory_components >= 4 && c3 == NULL)
        {
            continue;
        }

        T4 *c4 = this->get_component<T4>(entity);
        if (mandatory_components >= 5 && c4 == NULL)
        {
            continue;
        }

        callback(entity, c0, c1, c2, c3, c4);
    }
}

template <typename T0> void entity_manager_t::iterate_nodes(int mandatory_components, void (*callback)(T0 *))
{
    // don't be silly
    assert(mandatory_components >= 1);

    std::map<int, component_t *> &m = this->component_storage[&typeid(T0)];
    for (std::map<int, component_t *>::iterator it = m.begin(); it != m.end(); ++it)
    {
        int entity = it->first;
        T0 *c0 = (T0 *)it->second;

        callback(c0);
    }
}

template <typename T0, typename T1> void entity_manager_t::iterate_nodes(int mandatory_components, void (*callback)(T0 *, T1 *))
{
    // don't be silly
    assert(mandatory_components >= 1);

    std::map<int, component_t *> &m = this->component_storage[&typeid(T0)];
    for (std::map<int, component_t *>::iterator it = m.begin(); it != m.end(); ++it)
    {
        int entity = it->first;
        T0 *c0 = (T0 *)it->second;

        T1 *c1 = this->get_component<T1>(entity);
        if (mandatory_components >= 2 && c1 == NULL)
        {
            continue;
        }

        callback(c0, c1);
    }
}

template <typename T0, typename T1, typename T2> void entity_manager_t::iterate_nodes(int mandatory_components, void (*callback)(T0 *, T1 *, T2 *))
{
    // don't be silly
    assert(mandatory_components >= 1);

    std::map<int, component_t *> &m = this->component_storage[&typeid(T0)];
    for (std::map<int, component_t *>::iterator it = m.begin(); it != m.end(); ++it)
    {
        int entity = it->first;
        T0 *c0 = (T0 *)it->second;

        T1 *c1 = this->get_component<T1>(entity);
        if (mandatory_components >= 2 && c1 == NULL)
        {
            continue;
        }

        T2 *c2 = this->get_component<T2>(entity);
        if (mandatory_components >= 3 && c2 == NULL)
        {
            continue;
        }

        callback(c0, c1, c2);
    }
}

template <typename T0, typename T1, typename T2, typename T3> void entity_manager_t::iterate_nodes(int mandatory_components, void (*callback)(T0 *, T1 *, T2 *, T3 *))
{
    // don't be silly
    assert(mandatory_components >= 1);

    std::map<int, component_t *> &m = this->component_storage[&typeid(T0)];
    for (std::map<int, component_t *>::iterator it = m.begin(); it != m.end(); ++it)
    {
        int entity = it->first;
        T0 *c0 = (T0 *)it->second;

        T1 *c1 = this->get_component<T1>(entity);
        if (mandatory_components >= 2 && c1 == NULL)
        {
            continue;
        }

        T2 *c2 = this->get_component<T2>(entity);
        if (mandatory_components >= 3 && c2 == NULL)
        {
            continue;
        }

        T3 *c3 = this->get_component<T3>(entity);
        if (mandatory_components >= 4 && c3 == NULL)
        {
            continue;
        }

        callback(c0, c1, c2, c3);
    }
}

template <typename T0, typename T1, typename T2, typename T3, typename T4> void entity_manager_t::iterate_nodes(int mandatory_components, void (*callback)(T0 *, T1 *, T2 *, T3 *, T4 *))
{
    // don't be silly
    assert(mandatory_components >= 1);

    std::map<int, component_t *> &m = this->component_storage[&typeid(T0)];
    for (std::map<int, component_t *>::iterator it = m.begin(); it != m.end(); ++it)
    {
        int entity = it->first;
        T0 *c0 = (T0 *)it->second;

        T1 *c1 = this->get_component<T1>(entity);
        if (mandatory_components >= 2 && c1 == NULL)
        {
            continue;
        }

        T2 *c2 = this->get_component<T2>(entity);
        if (mandatory_components >= 3 && c2 == NULL)
        {
            continue;
        }

        T3 *c3 = this->get_component<T3>(entity);
        if (mandatory_components >= 4 && c3 == NULL)
        {
            continue;
        }

        T4 *c4 = this->get_component<T4>(entity);
        if (mandatory_components >= 5 && c4 == NULL)
        {
            continue;
        }

        callback(c0, c1, c2, c3, c4);
    }
}

#endif // _ENTITY_SYSTEM_HPP

