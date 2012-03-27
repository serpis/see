#include <cstdio>

#include "entity_system.hpp"

// huzzah! circle dependencies!
#include "components.hpp"

bool type_info_comparator(const std::type_info *a, const std::type_info *b)
{
    return a->before(*b);
}

entity_manager_t *entity_manager_t::default_manager;


meta_entity_t::meta_entity_t(const std::string &name)
{
    this->entity_manager = entity_manager_t::default_manager;
    this->entity = this->entity_manager->create_entity();

    debug_name_component_t *debug_name = new debug_name_component_t;
    debug_name->name = name;

    this->add_component(debug_name);
}

void meta_entity_t::add_component(component_t *component)
{
    this->entity_manager->add_component(this->entity, component);
}

void meta_entity_t::remove_component(component_t *component)
{
    this->entity_manager->remove_component(this->entity, component);
}

bool meta_entity_t::has_component_type(const std::type_info &type)
{
    return this->entity_manager->has_component_type(this->entity, type);
}

bool meta_entity_t::has_component(component_t *component)
{
    return this->entity_manager->has_component(this->entity, component);
}

component_list_t meta_entity_t::components()
{
    return this->entity_manager->components_of_entity(this->entity);
}


entity_manager_t::entity_manager_t()
    : created_entity_count(0),
      component_storage(type_info_comparator)
{
    if (entity_manager_t::default_manager == NULL)
    {
        entity_manager_t::default_manager = this;
    }
}

int entity_manager_t::create_entity()
{
    // start counting at 1
    int entity = this->created_entity_count++ + 1;
    this->entity_storage.insert(entity);
    return entity;
}

void entity_manager_t::add_component(int entity, component_t *component)
{
    assert(!this->has_component_type(entity, typeid(*component)));
    std::map<int, component_t *> &m = this->component_storage[&typeid(*component)];
    m[entity] = component;
}

void entity_manager_t::remove_component(int entity, component_t *component)
{
    assert(this->has_component(entity, component));
    std::map<int, component_t *> &m = this->component_storage[&typeid(*component)];
    m.erase(entity);
}

bool entity_manager_t::has_component_type(int entity, const std::type_info &type)
{
    std::map<int, component_t *> &m = this->component_storage[&type];
    return m.find(entity) != m.end();
}

bool entity_manager_t::has_component(int entity, component_t *component)
{
    std::map<int, component_t *> &m = this->component_storage[&typeid(*component)];
    std::map<int, component_t *>::iterator res = m.find(entity);
    return res != m.end() && res->second == component;
}

component_list_t entity_manager_t::components_of_entity(int entity)
{
    component_list_t list;

    for (component_storage_t::iterator it = this->component_storage.begin(); it != this->component_storage.end(); ++it)
    {
        std::map<int, component_t *>::iterator res = it->second.find(entity);

        if (res != it->second.end())
        {
            list.push_back(res->second);
        }
    }

    return list;
}

entity_list_t entity_manager_t::entities_possessing_component_type(const std::type_info &type)
{
    entity_list_t list;

    std::map<int, component_t *> &m = this->component_storage[&type];

    for (std::map<int, component_t *>::iterator it = m.begin(); it != m.end(); ++it)
    {
        list.push_back(it->first);
    }

    return list;
}

