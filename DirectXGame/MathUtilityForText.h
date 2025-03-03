#pragma once

#include "Matrix4x4.h"
#include "Vector3.h"
#include <math.h>

Matrix4x4 Multply(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
Matrix4x4 MakeRotateXMatrix(float radian);
Matrix4x4 MakeRotateYMatrix(float radian);
// Matrix4x4 MakeRotateZMatrix(float radian);
Matrix4x4 MakeRotateZMatrix(float theta);

// Vector3同士の加算
Vector3& operator+=(Vector3& lhs, const Vector3& rhv);

// 自キャラと敵キャラの当たり判定
// bool IsColision(const AABB& aabb1, const AABB& aabb2);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate);

// 代入演算子オーバーロード
// Vector3の足し算
Vector3& operator+=(Vector3& lhs, const Vector3& rhv);

// 行列の掛け算
Matrix4x4 MatrixMultiply(Matrix4x4& m1, const Matrix4x4& m2);

// 2項演算子オーバーロード
// Vector3の足算
const Vector3 operator+(const Vector3& v1, const Vector3& v2);

// 代入演算子オーバーロード
// Vector3の足算
Vector3& operator*=(Vector3& v, float s);

// 2項演算子オーバーロード
// Vector3の足算
const Vector3 operator*(const Vector3& v, float s);

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

float EaseInOut(float x1, float x2, float t);
float Lerp(float x1, float x2, float t);
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

// ベクトル関数
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

Matrix4x4 Inverse(const Matrix4x4& matrix);