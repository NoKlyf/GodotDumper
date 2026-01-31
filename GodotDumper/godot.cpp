#include "godot.h"
#include <Windows.h>

std::string gd::String::get_string()
{
    std::uint32_t* it = data;

    std::string result = "";
    while (it && *it != 0)
    {
        result.push_back(static_cast<char>(*it & 0xFF));
        ++it;
    }

    return result;
}

std::string gd::StringName::get_name()
{
    if (!ptr)
        return "No Name";

    return ptr->name.get_string();
}

bool gd::Object::inherits_from(AncestralClass ancestral_class)
{
    return ancestry & (std::uint32_t)ancestral_class;
}

gd::Node* gd::Node::find_child(std::string_view path)
{
    if (path.empty())
        return nullptr;

    Node* current = this;

    std::size_t start = 0;
    while (start < path.size())
    {
        std::size_t slash = path.find('/', start);
        std::string_view token = (slash == std::string_view::npos) ? path.substr(start) : path.substr(start, slash - start);

        if (token.empty())
            return nullptr;

        bool found = false;
        for (Node* child : current->children_cache)
        {
            if (!child)
                continue;

            if (child->get_name() == token)
            {
                current = child;
                found = true;
                break;
            }
        }

        if (!found)
            return nullptr;

        if (slash == std::string_view::npos)
            break;

        start = slash + 1;
    }

    return current;
}

std::string gd::Node::get_scene_file_path()
{
    return scene_file_path.get_string();
}

LocalVector<gd::Node*>& gd::Node::get_children()
{
    return children_cache;
}

std::string gd::Node::get_name()
{
    return name.get_name();
}

std::string gd::Node::get_class_name()
{
    if (!IsBadReadPtr(this, sizeof(this))) // I have to check for this because sometimes the vtable is null and crashes the game
        return mem->call_vfunc<gd::String, 10>(this).get_string();

    return "";
}

gd::Node* gd::Node::get_parent()
{
    return parent;
}

gd::Node* gd::Node::get_owner()
{
    return owner;
}

std::string gd::Window::get_title()
{
    return title.get_string();
}

std::string gd::Window::get_displayed_title()
{
    return displayed_title.get_string();
}

gd::Window* gd::SceneTree::get_root()
{
    return root;
}

gd::Node* gd::SceneTree::get_current_scene()
{
    return current_scene;
}

gd::SceneTree* gd::SceneTree::get_singleton()
{
    /*
     * To find the SceneTree singleton, look for the SceneTree constructor
     * Then find two lines like this
     * 
     * if (v4)
     *     qword_OFFSET = a1;
     * 
     * OFFSET is the offset to the singleton pointer
    */

#ifdef GODOT_VERSION_4_4
    static std::uint8_t* addr = mem->resolve_rel_addr(mem->find_pattern("48 39 1D ? ? ? ? 0F 84 ? ? ? ? 48 8B 8B ? ? ? ? 48 85 C9 0F 84"), 0x3, 0x7);
#elif defined GODOT_VERSION_4_3
    static std::uint8_t* addr = mem->resolve_rel_addr(mem->find_pattern("48 8B 05 ? ? ? ? 48 85 C0 74 ? 80 B8"), 0x3, 0x7);
#endif
    return *(SceneTree**)(addr);
}

Projection gd::Camera3D::get_camera_projection()
{
    if (mode != PROJECTION_PERSPECTIVE)
        return Projection{ };
    
    Vector2 viewport_size = { 1920.f, 1080.f };
    
    Projection cm;
    cm.set_perspective(fov, viewport_size.x / viewport_size.y, _near, _far, keep_aspect == KEEP_WIDTH);
    
    return cm;
}

bool gd::Camera3D::world_to_screen(const Vector3& world, Vector2& screen)
{
    if (is_position_behind(world))
        return false;

    Vector2 size = { 1920.f, 1080.f };
    Projection cm = get_camera_projection();

    Plane p(get_camera_transform().xform_inv(world), 1.f);

    p = cm.xform4(p);
    if (p.d == 0.f)
        return false;

    p.normal /= p.d;

    screen.x = (p.normal.x * 0.5f + 0.5f) * size.x;
    screen.y = (-p.normal.y * 0.5f + 0.5f) * size.y;

    return true;
}

bool gd::Camera3D::is_position_behind(const Vector3& world) const
{
    Vector3 eyedir = -global_transform.basis.get_column(2).normalized();
    return eyedir.dot(world - global_transform.origin) < _near;
}

void gd::Camera3D::look_at(const Vector3& world)
{
    global_transform.set_look_at(global_transform.origin, world);
}

Transform3D gd::Camera3D::get_camera_transform()
{
    Transform3D tr = global_transform.orthonormalized();
    tr.origin += tr.basis.get_column(1) * v_offset;
    tr.origin += tr.basis.get_column(0) * h_offset;

    return tr;
}

gd::Camera3D* gd::Viewport::get_camera_3d()
{
    return camera_3d;
}