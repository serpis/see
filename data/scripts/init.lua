function v(...)
    local res = {}
    for i, v in ipairs(arg) do
        table.insert(res, v)
    end

    return res
end

function allocate_position_component(pos)
    return pos
end

function allocate_orientation_component(quat)
    return quat
end

function allocate_physics_component(rigid_body)
    return rigid_body
end

function allocate_cube_rigid_body(radius, mass)
    return { radius, mass }
end

function allocate_entity(components)
    return components
end

function allocate_box()
    local position = allocate_position_component(v(1.0, 1.0, 1.0))
    local orientation = allocate_orientation_component(v(0.0, 0.0, 0.0, 1.0))
    local physics = allocate_physics_component(allocate_cube_rigid_body(0.5, 1))

    return allocate_entity({ position, orientation, physics })
end

--print(allocate_box())

