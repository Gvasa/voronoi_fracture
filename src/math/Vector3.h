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
#ifndef _vector3_h
#define _vector3_h

#include "Vector4.h"
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <cassert>

template <typename Real>
class Vector3
{
public:
  typedef Real ValueType;

public:
  // Constructors, destructor and assignment operators
  //! Default constructor, initializes to 0
  Vector3() {
    v[0] = v[1] = v[2] = Real(0.0);
  }
  /*Vector3(Real v0) {
    v[0] = v[1] = v[2] = v0;
  }*/
  Vector3(Real v0, Real v1, Real v2) {
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
  }
	/*
  //! Constructor that takes a Vector4
  Vector3(const Vector4<Real>& vec4) {
    v[0]=vec4[0]/vec4[3];
    v[1]=vec4[1]/vec4[3];
    v[2]=vec4[2]/vec4[3];
  }
	 */
  //! Copy constructor
  template <typename Real2>
  Vector3(const Vector3<Real2>& vec3) {
    *this = vec3;
  }
  //! Assignment operator = for vector
  template <typename Real2>
  Vector3& operator=(const Vector3<Real2>& vec3) {
    v[0]=(Real)vec3[0];
    v[1]=(Real)vec3[1];
    v[2]=(Real)vec3[2];
    return *this;
  }
  //! Assignment operator = for scalar
  template <typename Scalar>
  Vector3& operator=(Scalar s) {
    v[0]=(Real)s;
    v[1]=(Real)s;
    v[2]=(Real)s;
    return *this;
  }
  //! Constructor from std::string object
  Vector3(std::string str) {
    std::stringstream s;
    s << str;
    Load(s);
  }
  //! Destructor
  ~Vector3() { }
  //! Load a vector from an istream
  void Load(std::istream& is) {
    is >> v[0] >> v[1] >> v[2];
  }
  //! Save a vector from to a ostream
  void Save(std::ostream& os) const {
    os << v[0] << " " << v[1] << " " << v[2];
  }

  // Access operators
  Real& operator[](unsigned int i) {
    assert(i >= 0 && i <= 2 && "Index out of bounds in Vector3");
    return v[i];
  }
  Real operator[](unsigned int i) const {
    assert(i >= 0 && i <= 2 && "Index out of bounds in Vector3");
    return v[i];
  }
//   Real x() const {return v[0];}
//   Real y() const {return v[1];}
//   Real z() const {return v[2];}
//   Real& x() {return v[0];}
//   Real& y() {return v[1];}
//   Real& z() {return v[2];}
  Real *GetArrayPtr() { return v; }
  const Real *GetArrayPtr() const { return v; }

  // Algebraic operators
  Vector3 operator-(const Vector3& vec3) const {
    return Vector3(v[0]-vec3.v[0], v[1]-vec3.v[1], v[2]-vec3.v[2]);
  }
  Vector3 operator+(const Vector3& vec3) const {
    return Vector3(v[0]+vec3.v[0], v[1]+vec3.v[1], v[2]+vec3.v[2]);
  }
  Vector3 & operator+=(const Vector3& vec3) {
    v[0]+=vec3.v[0];
    v[1]+=vec3.v[1];
    v[2]+=vec3.v[2];
    return *this;
  }
  Vector3 & operator-=(const Vector3& vec3) {
    v[0]-=vec3.v[0];
    v[1]-=vec3.v[1];
    v[2]-=vec3.v[2];
    return *this;
  }
  //! Dot product
  Real operator*(const Vector3<Real>& vec3) const {
    return v[0]*vec3[0] + v[1]*vec3[1] + v[2]*vec3[2];
  }
  //! Scalar multiply for scaling the vector
  template<typename Scalar>
  Vector3<Real> operator*(Scalar s) const {
    return Vector3<Real>((Real)(s*v[0]), (Real)(s*v[1]), (Real)(s*v[2]));
  }
  template<typename Scalar>
  Vector3<Real>& operator*=(Scalar s) {
    v[0]*=(Real)s;
    v[1]*=(Real)s;
    v[2]*=(Real)s;
    return *this;
  }
  template<typename Scalar>
  Vector3<Real>& operator/=(Scalar s) {
    return *this *= 1.0/s;
  }
  //! Scaling of vector by division
  template<typename Scalar>
  Vector3<Real> operator/(Scalar s) const {
    Real invS = 1.0/s;
    return Vector3<Real>(v[0]*invS, v[1]*invS, v[2]*invS);
  }
  //! Multiplication with another vector entry by entry
  Vector3 EntryMult(const Vector3& vec3) const {
    return Vector3(v[0]*vec3.v[0], v[1]*vec3.v[1], v[2]*vec3.v[2]);
  }

  // Comparison operators
  //! Greater than operator for lexicographic comparison
  bool operator >  ( const Vector3<Real> &A ) const;
  //! Less than operator for lexicographic comparison
  bool operator <  ( const Vector3<Real> &A ) const;
  //! Less than or equal operator for lexicographic comparison
  bool operator <= ( const Vector3<Real> &A ) const;
  //! Greater than or equal operator for lexicographic comparison
  bool operator >= ( const Vector3<Real> &A ) const;
  //! Equal operator for lexicographic comparison
  bool operator == ( const Vector3<Real> &A ) const;
  //! Not equal operator for lexicographic comparison
  bool operator != (const Vector3<Real>& other) const {return !(*this == other);}
  //! Templated equal operator for lexicographic comparison of vectors of different type
  template <typename Real2>
  bool operator==(const Vector3<Real2>& vec3) const {
    return v[0]==(Real)vec3[0] && v[1]==(Real)vec3[1] && v[2]==(Real)vec3[2];
  }

  // Geometric operators
  //! Norm of a vector
  Real Norm() const {
    return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
  }
  //! Length of a vector, specilization exists for type float, see below
  Real Length() const {
    return sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
  }
  //! Normalization of vector
  Vector3& Normalize() {
    Real len = Length();
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
    return *this;
  }
  // static methods
  static Vector3 Floor(const Vector3& vec3) {
    return Vector3(::floor(vec3[0]), ::floor(vec3[1]), ::floor(vec3[2]));
  }
  static Vector3 Ceil(const Vector3& vec3) {
    return Vector3(::ceil(vec3[0]), ::ceil(vec3[1]), ::ceil(vec3[2]));
  }

protected:
  Real v[3];
  // friends
  //! Cross product
  template<typename Real2>
  friend Vector3<Real2> Cross(const Vector3<Real2>& v1, const Vector3<Real2>& v2);
};

//! Specialization of length for float type, uses sqrtf instead.
template<>
inline float Vector3<float>::Length() const{
  return sqrtf( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
}

//! Cross product
template <typename Real2>
Vector3<Real2> Cross(const Vector3<Real2>& v1, const Vector3<Real2>& v2)
{
  Vector3<Real2> res;

  res[0] = v1[1] * v2[2] - v1[2] * v2[1];
  res[1] = v1[2] * v2[0] - v1[0] * v2[2];
  res[2] = v1[0] * v2[1] - v1[1] * v2[0];

  return res;
}

//! Less than operator for lexicographic comparison
template <class Real>
inline bool Vector3<Real>::operator < ( const Vector3<Real> &A ) const
{
  if ( v[0]<A[0] )
    return true;
  else if ( v[0]>A[0] )
    return false;
  else if ( v[1]<A[1] )
    return true;
  else if ( v[1]>A[1] )
    return false;
  else if ( v[2]<A[2] )
    return true;
  else
    return false;
}
template <class Real>
inline bool Vector3<Real>::operator > ( const Vector3<Real> &A ) const
{
  if ( v[0]>A[0] )
    return true;
  else if ( v[0]<A[0] )
    return false;
  else if ( v[1]>A[1] )
    return true;
  else if ( v[1]<A[1] )
    return false;
  else if ( v[2]>A[2] )
    return true;
  else
    return false;
}
template <class Real>
inline bool Vector3<Real>::operator <= ( const Vector3<Real> &A ) const
{
  if ( v[0]<A[0] )
    return true;
  else if ( v[0]>A[0] )
    return false;
  else if ( v[1]<A[1] )
    return true;
  else if ( v[1]>A[1] )
    return false;
  else if ( v[2]<=A[2] )
    return true;
  else
    return false;
}
template <class Real>
inline bool Vector3<Real>::operator >= ( const Vector3<Real> &A ) const
{
  if ( v[0]>A[0] )
    return true;
  else if ( v[0]<A[0] )
    return false;
  else if ( v[1]>A[1] )
    return true;
  else if ( v[1]<A[1] )
    return false;
  else if ( v[2]>=A[2] )
    return true;
  else
    return false;
}
template <class Real>
inline bool Vector3<Real>::operator == ( const Vector3<Real> &A ) const
{
  return ( v[0]==A[0] && v[1]==A[1] && v[2]==A[2] );
}



template <typename Real1, typename Real2>
Vector3<Real2> operator*(Real1 r, const Vector3<Real2>& v)
{
  Vector3<Real2> res;

  res[0] = v[0]*r;
  res[1] = v[1]*r;
  res[2] = v[2]*r;

  return res;
}

template <typename Real>
const Vector3<Real> operator-(const Vector3<Real>& vec3)
{
  return Vector3<Real>(-vec3[0], -vec3[1], -vec3[2]);
}

namespace std
{
  template <typename Real>
  ostream& operator<<(ostream& os, const Vector3<Real>& m)
  {
    m.Save(os);
    return os;
  }

  template <typename Real>
  istream& operator>>(istream& is, Vector3<Real>& m)
  {
    m.Load(is);
    return is;
  }
}


#endif
