/*
 * glExtension.c
 *
 *  Created on: 2016. 1. 13.
 *      Author: hunkyo.jung
 */

#include "glExtension.h"

/*
 * Replace the current matrix with the identity matrix
 */
void glExtLoadIdentity(float matrix[16])
{
	matrix[0]  = 1.0f;
	matrix[1]  = 0.0f;
	matrix[2]  = 0.0f;
	matrix[3]  = 0.0f;

	matrix[4]  = 0.0f;
	matrix[5]  = 1.0f;
	matrix[6]  = 0.0f;
	matrix[7]  = 0.0f;

	matrix[8]  = 0.0f;
	matrix[9]  = 0.0f;
	matrix[10] = 1.0f;
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;
}

/*
 * Multiply the current matrix by a general scaling matrix
 */
void glExtScale(float matrix[16], const float sx, const float sy, const float sz)
{
   matrix[0]  *= sx;
   matrix[1]  *= sx;
   matrix[2]  *= sx;
   matrix[3]  *= sx;

   matrix[4]  *= sy;
   matrix[5]  *= sy;
   matrix[6]  *= sy;
   matrix[7]  *= sy;

   matrix[8]  *= sz;
   matrix[9]  *= sz;
   matrix[10] *= sz;
   matrix[11] *= sz;
}

/*
 * Multiply the current matrix by a perspective matrix
 */
void glExtFrustum(GLfloat result[16], const float left, const float right, const float bottom, const float top, const float near, const float far)
{
    // First Column
    result[0] = 2.0 / (right - left);
    result[1] = 0.0;
    result[2] = 0.0;
    result[3] = 0.0;

    // Second Column
    result[4] = 0.0;
    result[5] = 2.0 / (top - bottom);
    result[6] = 0.0;
    result[7] = 0.0;

    // Third Column
    result[8] = 0.0;
    result[9] = 0.0;
    result[10] = -2.0 / (far - near);
    result[11] = 0.0;

    // Fourth Column
    result[12] = -(right + left) / (right - left);
    result[13] = -(top + bottom) / (top - bottom);
    result[14] = -(far + near) / (far - near);
    result[15] = 1;
}

/*
 * Set up a perspective projection matrix
 */
void glExtPerspective(GLfloat matrix[16], GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far)
{
    // Some calculus before the formula.
    float size = near * tanf(degreesToRadians(fovy/2.0));
    float left = -size / aspect;
    float right = size / aspect;
    float bottom = -size;
    float top = size;

    // First Column
    matrix[0] = 2 * near / (right - left);
    matrix[1] = 0.0;
    matrix[2] = 0.0;
    matrix[3] = 0.0;

    // Second Column
    matrix[4] = 0.0;
    matrix[5] = 2 * near / (top - bottom);
    matrix[6] = 0.0;
    matrix[7] = 0.0;

    // Third Column
    matrix[8] = (right + left) / (right - left);
    matrix[9] = (top + bottom) / (top - bottom);
    matrix[10] = -(far + near) / (far - near);
    matrix[11] = -1;

    // Fourth Column
    matrix[12] = 0.0;
    matrix[13] = 0.0;
    matrix[14] = -(2 * far * near) / (far - near);
    matrix[15] = 0.0;
}

/*
 * multiply the current matrix by a rotation matrix
 */
void glExtRotateX(float degrees, float matrix[16])
{
    float radians = degreesToRadians(degrees);

    glExtLoadIdentity(matrix);

    // Rotate X formula.
    matrix[5] = cosf(radians);
    matrix[6] = -sinf(radians);
    matrix[9] = -matrix[6];
    matrix[10] = matrix[5];
}

/*
 * multiply the current matrix by a rotation matrix
 */
void glExtRotateY(float degrees, float matrix[16])
{
    float radians = degreesToRadians(degrees);

    glExtLoadIdentity(matrix);

    // Rotate Y formula.
    matrix[0] = cosf(radians);
    matrix[2] = sinf(radians);
    matrix[8] = -matrix[2];
    matrix[10] = matrix[0];
}

/*
 * multiply the current matrix by a rotation matrix
 */
void glExtRotateZ(float degrees, float matrix[16])
{
    float radians = degreesToRadians(degrees);

    glExtLoadIdentity(matrix);

    // Rotate Z formula.
    matrix[0] = cosf(radians);
    matrix[1] = sinf(radians);
    matrix[4] = -matrix[1];
    matrix[5] = matrix[0];
}

/*
 * Multiply the current matrix with the specified matrix
 */
void glExtMultiply(float result[16], float m1[16], float m2[16])
{
	int i, row, column;
	float temp[16];
	for (column = 0; column < 4; column++)
	{
		for (row = 0; row < 4; row++)
		{
			temp[column * 4 + row] = 0.0f;
			for (i = 0; i < 4; i++)
				temp[column * 4 + row] += m1[i * 4 + row] * m2[column * 4 + i];
		}
	}
	for (i = 0; i < 16; i++)
		result[i] = temp[i];
}

/*
 * Multiply the current matrix by a translation matrix
 */
void glExtTranslate(float* result, const float translatex, const float translatey, const float translatez) {
	result[12] += result[0] * translatex + result[4] * translatey + result[8] * translatez;
	result[13] += result[1] * translatex + result[5] * translatey + result[9] * translatez;
	result[14] += result[2] * translatex + result[6] * translatey + result[10] * translatez;
	result[15] += result[3] * translatex + result[7] * translatey + result[11] * translatez;
}

/*
 * Get the length of a given vector
 */
float glExtGetLength(const float x, const float y, const float z) {
	return (float) sqrt(x*x + y*y +z*z);
}

/*
 * Define a viewing transformation
 */
void glExtLookat(float* result,
		const float eyex, const float eyey, const float eyez,
		const float centerx, const float centery, const float centerz,
		const float upx, const float upy, const float upz) {

	float fx = centerx - eyex;
	float fy = centery - eyey;
	float fz = centerz - eyez;

	// normalize f
	float rlf = 1.0f / glExtGetLength(fx, fy, fz);
	fx *= rlf;
	fy *= rlf;
	fz *= rlf;

	// compute s = f x up (x means "cross product")
    float sx = fy * upz - fz * upy;
    float sy = fz * upx - fx * upz;
    float sz = fx * upy - fy * upx;

    // and normalize s
    float rls = 1.0f / glExtGetLength(sx, sy, sz);
    sx *= rls;
    sy *= rls;
    sz *= rls;

    //The up vector must not be parallel to the line of sight from the eye point to the reference point.
    if((0 == sx)&&(0 == sy)&&(0 == sz))
    	return;

    // compute u = s x f
    float ux = sy * fz - sz * fy;
    float uy = sz * fx - sx * fz;
    float uz = sx * fy - sy * fx;

    result[0] = sx;
    result[1] = ux;
    result[2] = -fx;
    result[3] = 0.0f;

    result[4] = sy;
    result[5] = uy;
    result[6] = -fy;
    result[7] = 0.0f;

    result[8] = sz;
    result[9] = uz;
    result[10] = -fz;
    result[11] = 0.0f;

    result[12] = 0.0f;
    result[13] = 0.0f;
    result[14] = 0.0f;
    result[15] = 1.0f;

    glExtTranslate(result, -eyex, -eyey, -eyez);
}
