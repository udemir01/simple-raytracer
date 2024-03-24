#include <float.h>

#define F32Max FLT_MAX
#define F32Min -FLT_MAX

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef char s8;
typedef short s16;
typedef int s32;
typedef s32 b32;
typedef s32 b32x;
typedef float f32;

union v3 {
    struct {
        f32 X, Y, Z;
    };

    struct {
        f32 R, G, B;
    };

    f32 Elements[3];

    inline f32 &operator[](int Index) { return Elements[Index]; }
};

static inline v3
V3(f32 X, f32 Y, f32 Z)
{
	v3 result;
	result.X = X;
	result.Y = Y;
	result.Z = Z;

	return result;
}

static inline b32
eq_v3(v3 left, v3 right)
{
	return left.X == right.X && left.Y == right.Y && left.Z == right.Z;
}

static inline v3
add_v3(v3 left, v3 right)
{
    v3 result;
    result.X = left.X + right.X;
    result.Y = left.Y + right.Y;
    result.Z = left.Z + right.Z;

    return result;
}

static inline v3
sub_v3(v3 left, v3 right)
{
    v3 result;
    result.X = left.X - right.X;
    result.Y = left.Y - right.Y;
    result.Z = left.Z - right.Z;

    return result;
}

static inline v3
div_v3(v3 left, v3 right)
{
    v3 result;
    result.X = left.X / right.X;
    result.Y = left.Y / right.Y;
    result.Z = left.Z / right.Z;

    return result;
}

static inline v3
div_f(v3 left, f32 right)
{
    v3 result;
    result.X = left.X / right;
    result.Y = left.Y / right;
    result.Z = left.Z / right;

    return result;
}

static inline v3
mul_v3(v3 left, v3 right)
{
    v3 result;
    result.X = left.X * right.X;
    result.Y = left.Y * right.Y;
    result.Z = left.Z * right.Z;

    return result;
}

static inline v3
mul_f(v3 left, f32 right)
{
    v3 result;
    result.X = left.X * right;
    result.Y = left.Y * right;
    result.Z = left.Z * right;

    return result;
}

static inline f32
dot_v3(v3 left, v3 right)
{
    return (left.X * right.X) + (left.Y * right.Y) + (left.Z * right.Z);
}

static inline v3
cross(v3 left, v3 right)
{
    v3 result;
    result.X = (left.Y * right.Z) - (left.Z * right.Y);
    result.Y = (left.Z * right.X) - (left.X * right.Z);
    result.Z = (left.X * right.Y) - (left.Y * right.X);

    return result;
}

static inline f32
inv_sqrtf(f32 number)
{
	u32 i;
	f32 x2, y;
	const f32 threehalfs = 1.5f;

	x2 = number * 0.5f;
	y = number;
	i = * (u32 *) &y;
	i = 0x5f3759df - (i >> 1);
	y = * (f32 *) &i;
	y *= threehalfs - (x2 * y * y);
	y *= threehalfs - (x2 * y * y);

	return y;
}

static inline f32
sqrtf(f32 number)
{
    return 1.0f / inv_sqrtf(number);
}

static inline v3 normal_v3(v3 A)
{
    return mul_f(A, inv_sqrtf(dot_v3(A, A)));
}

static inline float len_v3(v3 A)
{
    return sqrtf(dot_v3(A, A));
}

static inline b32
operator!=(v3 left, v3 right)
{
    return !eq_v3(left, right);
}

static inline b32
operator==(v3 left, v3 right)
{
    return eq_v3(left, right);
}

static inline v3
operator+(v3 left, v3 right)
{
    return add_v3(left, right);
}

static inline v3
operator-(v3 left, v3 right)
{
    return sub_v3(left, right);
}

static inline v3
operator*(v3 left, v3 right)
{
    return mul_v3(left, right);
}

static inline v3
operator*(v3 left, f32 right)
{
    return mul_f(left, right);
}

static inline v3
operator*(f32 left, v3 right)
{
    return mul_f(right, left);
}

static inline v3
operator/(v3 left, v3 right)
{
    return div_v3(left, right);
}

static inline v3
operator/(v3 left, f32 right)
{
    return div_f(left, right);
}

static inline v3
&operator+=(v3 &left, v3 right)
{
    return left = left + right;
}

static inline v3
&operator-=(v3 &left, v3 right)
{
    return left = left - right;
}

static inline v3
&operator*=(v3 &left, v3 right)
{
    return left = left * right;
}

static inline v3
&operator*=(v3 &left, f32 right)
{
    return left = left * right;
}

static inline v3
&operator/=(v3 &left, v3 right)
{
    return left = left / right;
}

static inline v3
&operator/=(v3 &left, f32 right)
{
    return left = left / right;
}

static inline v3
operator-(v3 in)
{
    v3 result;
    result.X = -in.X;
    result.Y = -in.Y;
    result.Z = -in.Z;

    return result;
}
