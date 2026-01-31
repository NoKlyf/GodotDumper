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

public:
    float distance(const Vector2& other) const;
};

class Vector3
{
public:
    float x, y, z;

public:
    constexpr const float& operator[](int index) const
    {
        switch (index)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            return 0.f;
        }
    }

    constexpr float& operator[](int index)
    {
        switch (index)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        }
    }

public:
    constexpr Vector3& operator+=(const Vector3& v);
    constexpr Vector3 operator+(const Vector3& v) const;
    constexpr Vector3& operator-=(const Vector3& v);
    constexpr Vector3 operator-(const Vector3& v) const;
    constexpr Vector3& operator*=(const Vector3& v);
    constexpr Vector3 operator*(const Vector3& v) const;
    constexpr Vector3& operator/=(const Vector3& v);
    constexpr Vector3 operator/(const Vector3& v) const;

    constexpr Vector3& operator*=(float scalar);
    constexpr Vector3 operator*(float scalar) const;
    constexpr Vector3& operator/=(float scalar);
    constexpr Vector3 operator/(float scalar) const;

    constexpr Vector3 operator-() const;

public:
    float mag();
    void normalize();
    Vector3 normalized() const;

    float dot(const Vector3& width) const;
    Vector3 cross(const Vector3& width) const;
};

constexpr Vector3& Vector3::operator+=(const Vector3& p_v)
{
    x += p_v.x;
    y += p_v.y;
    z += p_v.z;
    return *this;
}

constexpr Vector3 Vector3::operator+(const Vector3& p_v) const
{
    return Vector3(x + p_v.x, y + p_v.y, z + p_v.z);
}

constexpr Vector3& Vector3::operator-=(const Vector3& p_v)
{
    x -= p_v.x;
    y -= p_v.y;
    z -= p_v.z;
    return *this;
}

constexpr Vector3 Vector3::operator-(const Vector3& p_v) const
{
    return Vector3(x - p_v.x, y - p_v.y, z - p_v.z);
}

constexpr Vector3& Vector3::operator*=(const Vector3& p_v)
{
    x *= p_v.x;
    y *= p_v.y;
    z *= p_v.z;
    return *this;
}

constexpr Vector3 Vector3::operator*(const Vector3& p_v) const
{
    return Vector3(x * p_v.x, y * p_v.y, z * p_v.z);
}

constexpr Vector3& Vector3::operator/=(const Vector3& p_v)
{
    x /= p_v.x;
    y /= p_v.y;
    z /= p_v.z;
    return *this;
}

constexpr Vector3 Vector3::operator/(const Vector3& p_v) const
{
    return Vector3(x / p_v.x, y / p_v.y, z / p_v.z);
}

constexpr Vector3& Vector3::operator*=(float p_scalar) {
    x *= p_scalar;
    y *= p_scalar;
    z *= p_scalar;
    return *this;
}

constexpr Vector3 operator*(float p_scalar, const Vector3& p_vec)
{
    return p_vec * p_scalar;
}

constexpr Vector3 Vector3::operator*(float p_scalar) const
{
    return Vector3(x * p_scalar, y * p_scalar, z * p_scalar);
}

constexpr Vector3& Vector3::operator/=(float p_scalar)
{
    x /= p_scalar;
    y /= p_scalar;
    z /= p_scalar;
    return *this;
}

constexpr Vector3 Vector3::operator/(float p_scalar) const
{
    return Vector3(x / p_scalar, y / p_scalar, z / p_scalar);
}

constexpr Vector3 Vector3::operator-() const
{
    return Vector3(-x, -y, -z);
}

class Vector4
{
public:
    float x, y, z, w;

public:
    constexpr const float& operator[](int index) const
    {
        switch (index)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        default:
            return 0.f;
        }
    }

    constexpr float& operator[](int index)
    {
        switch (index)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
            return w;
        }
    }
};

class Plane
{
public:
    Vector3 normal;
    float d;
};

class Projection
{
public:
    Vector4 matrix[4];

public:
    Plane xform4(const Plane& p_vec4) const;
    void set_perspective(float fovy_degrees, float aspect, float z_near, float z_far, bool flip_fov);
};

class Basis
{
public:
    Vector3 rows[3];

public:
    void orthonormalize();
    Basis orthonormalized() const;

    Vector3 xform_inv(const Vector3& p_vector) const;

    Basis looking_at(const Vector3& target, const Vector3& up, bool use_model_front);

public:
    __forceinline Vector3 get_column(int index) const
    {
        return Vector3(rows[0][index], rows[1][index], rows[2][index]);
    }

    __forceinline void set_column(int index, const Vector3& value)
    {
        rows[0][index] = value.x;
        rows[1][index] = value.y;
        rows[2][index] = value.z;
    }

public:
    constexpr const Vector3& operator[](int row) const { return rows[row]; }
    constexpr Vector3& operator[](int row) { return rows[row]; }
};

class Transform3D
{
public:
    Basis basis;
    Vector3 origin;

public:
    __forceinline Vector3 xform_inv(const Vector3& p_vector) const
    {
        Vector3 v = p_vector - origin;

        return Vector3(
            (basis.rows[0][0] * v.x) + (basis.rows[1][0] * v.y) + (basis.rows[2][0] * v.z),
            (basis.rows[0][1] * v.x) + (basis.rows[1][1] * v.y) + (basis.rows[2][1] * v.z),
            (basis.rows[0][2] * v.x) + (basis.rows[1][2] * v.y) + (basis.rows[2][2] * v.z));
    }

public:
    void orthonormalize();
    Transform3D orthonormalized();
    
    void set_look_at(const Vector3& eye, const Vector3& target, const Vector3& up = { 0.f, 1.f, 0.f }, bool use_model_front = false);
};