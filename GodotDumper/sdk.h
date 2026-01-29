#pragma once
#include <cstdint>

#define CLASS_NO_CONSTRUCTOR(CLASS) \
	CLASS() = delete;                  \
	CLASS(CLASS&&) = delete;           \
	CLASS(const CLASS&) = delete;

#define CLASS_NO_ASSIGNMENT(CLASS)   \
	CLASS& operator=(CLASS&&) = delete; \
	CLASS& operator=(const CLASS&) = delete;

#define CLASS_NO_INITIALIZER(CLASS) \
	CLASS_NO_CONSTRUCTOR(CLASS)     \
	CLASS_NO_ASSIGNMENT(CLASS)

#define CLASS_NO_ALLOC()                               \
	void* operator new(const std::size_t size) = delete; \
	void operator delete(void* block) = delete;

#define GODOT_CLASS(CLASS) \
public: \
    CLASS_NO_INITIALIZER(CLASS) \
    CLASS_NO_ALLOC() \
private:

template <typename T, typename U = std::uint32_t>
class LocalVector
{
private:
    U count = 0;
    U capacity = 0;
    T* data = nullptr;

public:
    __forceinline T* ptr() { return data; }
    __forceinline const T* ptr() const { return data; }
    __forceinline U size() const { return count; }

    __forceinline const T& operator[](U p_index) const { return data[p_index]; }
    __forceinline T& operator[](U p_index) { return data[p_index]; }

    struct Iterator
    {
        __forceinline T& operator*() const { return *elem_ptr; }
        __forceinline T* operator->() const { return elem_ptr; }

        __forceinline Iterator& operator++()
        {
            elem_ptr++;
            return *this;
        }

        __forceinline Iterator& operator--()
        {
            elem_ptr--;
            return *this;
        }

        __forceinline bool operator==(const Iterator& b) const { return elem_ptr == b.elem_ptr; }
        __forceinline bool operator!=(const Iterator& b) const { return elem_ptr != b.elem_ptr; }

        Iterator(T* p_ptr) { elem_ptr = p_ptr; }
        Iterator() {}
        Iterator(const Iterator& p_it) { elem_ptr = p_it.elem_ptr; }

    private:
        T* elem_ptr = nullptr;
    };

    struct ConstIterator
    {
        __forceinline const T& operator*() const { return *elem_ptr; }
        __forceinline const T* operator->() const { return elem_ptr; }

        __forceinline ConstIterator& operator++()
        {
            elem_ptr++;
            return *this;
        }

        __forceinline ConstIterator& operator--()
        {
            elem_ptr--;
            return *this;
        }

        __forceinline bool operator==(const ConstIterator& b) const { return elem_ptr == b.elem_ptr; }
        __forceinline bool operator!=(const ConstIterator& b) const { return elem_ptr != b.elem_ptr; }

        ConstIterator(const T* p_ptr) { elem_ptr = p_ptr; }
        ConstIterator() {}
        ConstIterator(const ConstIterator& p_it) { elem_ptr = p_it.elem_ptr; }

    private:
        const T* elem_ptr = nullptr;
    };

    __forceinline Iterator begin() { return Iterator(data); }
    __forceinline Iterator end() { return Iterator(data + size()); }

    __forceinline ConstIterator begin() const { return ConstIterator(ptr()); }
    __forceinline ConstIterator end() const { return ConstIterator(ptr() + size()); }
};

class Vector2
{
public:
    float x, y;
};

class Vector3
{
public:
    float x, y, z;
};