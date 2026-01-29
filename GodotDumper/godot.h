#pragma once
#include "sdk.h"
#include "memory.h"
#include <string>

namespace gd
{
    class String
    {
        GODOT_CLASS(String)

        std::uint32_t* data; // The data is stored in a COW struct (CowData<uint32_t>)

    public:
        std::string get_string();
    };

    class StringName
    {
        GODOT_CLASS(StringName);

        struct _Data {
            std::uint32_t refcount;
            std::uint32_t static_count;
            const char* cname; // This field got removed in Godot 4.5
            String name;
        };

        _Data* ptr;
    public:
        std::string get_name();
    };

    class Object
    {
        GODOT_CLASS(Object);

        PAD(0x5C); // TODO: Check if this is the correct offset
        std::uint32_t ancestry; // Bitfield to check if the node inherits a base class

#ifdef GODOT_VERSION_4_4
        PAD(0xC8);
#elif defined GODOT_VERSION_4_3
        PAD(0xB8);
#endif

    public:
        enum class AncestralClass : std::uint32_t
        {
            REF_COUNTED = 1 << 0,
            NODE = 1 << 1,
            RESOURCE = 1 << 2,
            SCRIPT = 1 << 3,

            CANVAS_ITEM = 1 << 4,
            CONTROL = 1 << 5,
            NODE_2D = 1 << 6,
            COLLISION_OBJECT_2D = 1 << 7,
            AREA_2D = 1 << 8,

            NODE_3D = 1 << 9,
            VISUAL_INSTANCE_3D = 1 << 10,
            GEOMETRY_INSTANCE_3D = 1 << 11,
            COLLISION_OBJECT_3D = 1 << 12,
            PHYSICS_BODY_3D = 1 << 13,
            MESH_INSTANCE_3D = 1 << 14,
        };

    public:
        bool inherits_from(AncestralClass ancestral_class);

    public:
        template <typename T = Node>
        T* as();
    };

    class Node : public Object
    {
        GODOT_CLASS(Node);

        String scene_file_path; // Only available if the Node is the current loaded scene
        PAD(0x10);
        Node* parent;
        Node* owner;
        PAD(0x38);
        LocalVector<Node*> children_cache;
        PAD(0x50);
        StringName name;

    public:
        Node* find_child(std::string_view path);

        template <typename T = Node>
        T* find_child(std::string_view path);

    public:
        std::string get_scene_file_path();
        std::string get_name();
        std::string get_class_name();

        Node* get_parent();
        Node* get_owner();

        LocalVector<Node*>& get_children();

        template <typename T = Node>
        LocalVector<T*>& get_children();
    };

    class Node2D : public Node
    {
        GODOT_CLASS(Node2D);

#ifdef GODOT_VERSION_4_4
        PAD(0x304);
#elif defined GODOT_VERSION_4_3
        PAD(0x2A4);
#endif

    public:
        Vector2 position;
    };

    class Node3D : public Node
    {
        GODOT_CLASS(Node3D);

#if GODOT_VERSION_4_4
        PAD(0x250);
#elif defined GODOT_VERSION_4_3
        PAD(0x1F8);
#endif

    public:
        Vector3 global_position; // Actual position inside the world
    private: PAD(0x30); public:
        Vector3 position; // Position relative to the parent Node
    };

    class Camera3D : public Node3D
    {
        GODOT_CLASS(Camera3D);

        PAD(0x6E);

    public:
        enum ProjectionType
        {
            PROJECTION_PERSPECTIVE,
            PROJECTION_ORTHOGONAL,
            PROJECTION_FRUSTUM
        };

        enum KeepAspect
        {
            KEEP_WIDTH,
            KEEP_HEIGHT
        };

    public:
        bool force_change;
        bool current;

        // This should be a Viewport* but the struct doesn't contian anything useful as of now so i'm using Node*
        Node* viewport;

        ProjectionType mode;

        float fov;
        float size;

        Vector2 frustum_offset;

        float _near;
        float _far;

        float v_offset;
        float h_offset;

        KeepAspect keep_aspect;
    };

    // This should inherit Viewport
    class Window : public Node
    {
        GODOT_CLASS(Window);

#ifdef GODOT_VERSION_4_4
        PAD(0x748);
#elif defined GODOT_VERSION_4_3
        PAD(0x738);
#endif
        String title; // The game's name
        String displayed_title; // The name displayed as the window's title

    public:
        std::string get_title();
        std::string get_displayed_title();
    };

    class SceneTree
    {
        GODOT_CLASS(SceneTree);

#ifdef GODOT_VERSION_4_4
        PAD(0x318);
#elif defined GODOT_VERSION_4_3
        PAD(0x2B8)
#endif
        Window* root;
        PAD(0xE8);
        Node* current_scene;

    public:
        Window* get_root();
        Node* get_current_scene();

    public:
        static SceneTree* get_singleton();
    };

    template <typename T>
    inline T* Object::as()
    {
        return reinterpret_cast<T*>(this);
    }

    template <typename T>
    inline T* Node::find_child(std::string_view path)
    {
        return find_child(path)->as<T>();
    }

    template <typename T>
    LocalVector<T*>& Node::get_children()
    {
        return reinterpret_cast<LocalVector<T*>>(children_cache);
    }
}