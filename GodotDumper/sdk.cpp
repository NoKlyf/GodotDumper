#include "sdk.h"
#include <cmath>
#define PI 3.14159265358979323846

__forceinline float DEG2RAD(const float deg)
{
	return deg * (float)PI / 180.f;
}

__forceinline float RAD2DEG(const float rad)
{
	return rad * 180.f / (float)PI;
}

static float get_fovy(float fovx, float aspect)
{
	return RAD2DEG(std::atan(aspect * std::tan(DEG2RAD(fovx) * 0.5f)) * 2.f);
}

void Projection::set_perspective(float fovy_degrees, float aspect, float z_near, float z_far, bool flip_fov)
{
	if (flip_fov)
		fovy_degrees = get_fovy(fovy_degrees, 1.f / aspect);

	float sine, cotangent, deltaz;
	float radians = DEG2RAD(fovy_degrees / 2.f);
	
	deltaz = z_far - z_near;
	sine = std::sinf(radians);
	
	if (deltaz == 0 || sine == 0 || aspect == 0)
		return;
	
	cotangent = std::cosf(radians) / sine;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			matrix[i][j] = (i == j) ? 1.f : 0.f;
	}
	
	matrix[0][0] = cotangent / aspect;
	matrix[1][1] = cotangent;
	matrix[2][2] = -(z_far + z_near) / deltaz;
	matrix[2][3] = -1.f;
	matrix[3][2] = -2.f * z_near * z_far / deltaz;
	matrix[3][3] = 0.f;
}

float Vector3::mag()
{
    return std::sqrtf(x * x + y * y + z * z);
}

void Vector3::normalize()
{
    float len = mag();
    if (len == 0.f)
        x = y = z = 0.f;
    else
    {
        x /= len;
        y /= len;
        z /= len;
    }
}

Vector3 Vector3::normalized() const
{
	Vector3 v = *this;
	v.normalize();

	return v;
}

float Vector3::dot(const Vector3& width) const
{
    return x * width.x + y * width.y + z * width.z;
}

Vector3 Vector3::cross(const Vector3& width) const
{
	return Vector3{
		(y * width.z) - (z * width.y),
		(z * width.x) - (x * width.y),
		(x * width.y) - (y * width.y)
	};
}

void Basis::orthonormalize()
{
    Vector3 x = get_column(0);
    Vector3 y = get_column(1);
    Vector3 z = get_column(2);

    x.normalize();
	y = (y - x * (x.dot(y)));
	y.normalize();
	z = (z - x * (x.dot(z)) - y * (y.dot(z)));
	z.normalize();

	set_column(0, x);
	set_column(1, y);
	set_column(2, z);
}

Basis Basis::orthonormalized() const
{
	Basis c = *this;
	c.orthonormalize();

	return c;
}

Vector3 Basis::xform_inv(const Vector3& p_vector) const
{
	return Vector3(
		(rows[0].x * p_vector.x) + (rows[1].x * p_vector.y) + (rows[2].x * p_vector.z),
		(rows[0].y * p_vector.x) + (rows[1].y * p_vector.y) + (rows[2].y * p_vector.z),
		(rows[0].z * p_vector.x) + (rows[1].z * p_vector.y) + (rows[2].z * p_vector.z));
}

Basis Basis::looking_at(const Vector3& target, const Vector3& up, bool use_model_front)
{
	Vector3 vz = target.normalized();
	if (!use_model_front)
		vz = -vz;

	Vector3 vx = up.cross(vz);
	vx.normalize();

	Vector3 vy = vz.cross(vx);

	Basis basis;
	basis.set_column(0, vx);
	basis.set_column(1, vy);
	basis.set_column(2, vz);

	return basis;
}

void Transform3D::orthonormalize()
{
	basis.orthonormalize();
}

Transform3D Transform3D::orthonormalized()
{
	Transform3D copy = *this;
	copy.orthonormalize();

	return copy;
}

void Transform3D::set_look_at(const Vector3& eye, const Vector3& target, const Vector3& up, bool use_model_front)
{
	basis = basis.looking_at(target - eye, up, use_model_front);
	origin = eye;
}

Plane Projection::xform4(const Plane& p_vec4) const
{
	Plane ret;

	ret.normal.x = matrix[0].x * p_vec4.normal.x + matrix[1].x * p_vec4.normal.y + matrix[2].x * p_vec4.normal.z + matrix[3].x * p_vec4.d;
	ret.normal.y = matrix[0].y * p_vec4.normal.x + matrix[1].y * p_vec4.normal.y + matrix[2].y * p_vec4.normal.z + matrix[3].y * p_vec4.d;
	ret.normal.z = matrix[0].z * p_vec4.normal.x + matrix[1].z * p_vec4.normal.y + matrix[2].z * p_vec4.normal.z + matrix[3].z * p_vec4.d;
	ret.d = matrix[0].w * p_vec4.normal.x + matrix[1].w * p_vec4.normal.y + matrix[2].w * p_vec4.normal.z + matrix[3].w * p_vec4.d;
	return ret;
}

float Vector2::distance(const Vector2& other) const
{
	float dx = x - other.x;
	float dy = y - other.y;

	return std::sqrtf(dx * dx + dy * dy);
}
