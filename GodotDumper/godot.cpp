#include "godot.h"

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
    return mem->call_vfunc<gd::String, 10>(this).get_string();
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