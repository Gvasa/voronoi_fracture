/*************************************************************************************************
 *
 * Modeling and animation (TNM079) 2007
 * Code base for lab assignments. Copyright:
 *   Gunnar Johansson (gunnar.johansson@itn.liu.se)
 *   Ken Museth (ken.museth@itn.liu.se)
 *   Michael Bang Nielsen (bang@daimi.au.dk)
 *   Ola Nilsson (ola.nilsson@itn.liu.se)
 *   Andreas Sderstrm (andreas.soderstrom@itn.liu.se)
 *
 *************************************************************************************************/
#ifndef _matrix4x4_h
#define _matrix4x4_h

#include <algorithm>
#include <iterator>
#include <ostream>

template <typename Real> class Vector3;
template <typename Real> class Vector4;

template <typename Real>
class Matrix4x4
{
public:
  Matrix4x4();
  Matrix4x4(const Real m[4][4]);

  Matrix4x4 ToGLMatrix() const;
  bool IsSingular(Real epsilon = 1e-6) const;
  Real Determinant() const;
  Matrix4x4 Inverse() const;
  Matrix4x4 Transpose() const;
  Real* GetArrayPtr() { return &m[0][0]; }
  const Real* GetArrayPtr() const { return &m[0][0]; }

  bool CholeskyFactorization(Matrix4x4& U);

  // Input: i (row), j (column)
  Real &operator()(unsigned int i, unsigned int j);
  const Real &operator()(unsigned int i, unsigned int j) const;

  Vector4<Real> operator*(const Vector4<Real>& vec4) const;
  Matrix4x4 operator*(const Matrix4x4& m2) const;
  Matrix4x4& operator +=(const Matrix4x4 &m2);

  // Static methods
  static Matrix4x4 Scale(Real scale);
  static Matrix4x4 Scale(Real sx, Real sy, Real sz);
  // Angles in radians
  static Matrix4x4 RotationXYZ(Real rx, Real ry, Real rz);
  static Matrix4x4 Translation(Real tx, Real ty, Real tz);
  static Matrix4x4 Identity();

  //template <typename Real2>
  friend std::ostream& operator << (std::ostream &os, const Matrix4x4<Real> &m){
    os << "[";
    std::copy(m.m[0], m.m[0]+16, std::ostream_iterator<Real>(os, ", "));
    os << "]";
    return os;
  }

protected:
  Real m[4][4];

  static const Matrix4x4 identityMatrix;
};

template <typename Real>
Matrix4x4<Real> operator+(const Matrix4x4<Real> &m1, const Matrix4x4<Real> &m2){
  Matrix4x4<Real> m;
  m(0,0) = m1(0,0) + m2(0,0); m(0,1) = m1(0,1) + m2(0,1); m(0,2) = m1(0,2) + m2(0,2); m(0,3) = m1(0,3) + m2(0,3);
  m(1,0) = m1(1,0) + m2(1,0); m(1,1) = m1(1,1) + m2(1,1); m(1,2) = m1(1,2) + m2(1,2); m(1,3) = m1(1,3) + m2(1,3);
  m(2,0) = m1(2,0) + m2(2,0); m(2,1) = m1(2,1) + m2(2,1); m(2,2) = m1(2,2) + m2(2,2); m(2,3) = m1(2,3) + m2(2,3);
  m(3,0) = m1(3,0) + m2(3,0); m(3,1) = m1(3,1) + m2(3,1); m(3,2) = m1(3,2) + m2(3,2); m(3,3) = m1(3,3) + m2(3,3);

  return m;
}

#include "Matrix4x4_Impl.h"

#endif
