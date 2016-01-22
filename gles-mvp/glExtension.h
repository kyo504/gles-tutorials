/*
 * glExtension.h
 *
 *  Created on: 2016. 1. 13.
 *      Author: hunkyo.jung
 */

#ifndef GLEXTENTION_H_
#define GLEXTENTION_H_

#include <math.h>
#include <GLES2/gl2.h>

#define kPI180     0.017453 // Pre-calculated value of PI / 180.
#define k180PI    57.295780 // Pre-calculated value of 180 / PI.
#define degreesToRadians(x) (x * kPI180) // Converts degrees to radians.
#define radiansToDegrees(x) (x * k180PI) // Converts radians to degrees.

void glExtLoadIdentity(float matrix[16]);
void glExtScale(float matrix[16], const float sx, const float sy, const float sz);
void glExtFrustum(float result[16], const float left, const float right, const float bottom, const float top, const float near, const float far);
void glExtPerspective(GLfloat matrix[16], GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far);
void glExtRotateX(float degrees, float matrix[16]);
void glExtRotateY(float degrees, float matrix[16]);
void glExtRotateZ(float degrees, float matrix[16]);
void glExtMultiply(float result[16], float m1[16], float m2[16]);
void glExtTranslate(GLfloat result[16], GLfloat x, GLfloat y, GLfloat z);
float glExtGetLength(const float x, const float y, const float z);
void glExtLookat(float* result, const float eyex, const float eyey, const float eyez, const float centerx, const float centery, const float centerz, const float upx, const float upy, const float upz);

#endif /* GLEXTENTION_H_ */
