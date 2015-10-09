/*************************************************************************************************
 *
 * Modeling and animation (TNM079) 2007
 * Code base for lab assignments. Copyright:
 *   Gunnar Johansson (gunnar.johansson@itn.liu.se)
 *   Ken Museth (ken.museth@itn.liu.se)
 *   Michael Bang Nielsen (bang@daimi.au.dk)
 *   Ola Nilsson (ola.nilsson@itn.liu.se)
 *   Andreas Söderström (andreas.soderstrom@itn.liu.se)
 *
 *************************************************************************************************/
#ifndef _vector4_h
#define _vector4_h

#include <iostream>
#include <iterator>
#include <cassert>

template <typename Real>
class Vector4
{
public:
  Vector4() { v[0] = v[1] = v[2] = v[3] = Real(0.0); }
  //Vector4(Real c) { v[0] = v[1] = v[2] = v[3] = c; }
  Vector4(Real v0, Real v1, Real v2, Real v3) { v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3; }
  ~Vector4() { }

  void load(std::istream& is) { is >> v[0] >> v[1] >> v[2] >> v[3]; }
  void save(std::ostream& os) const { os << v[0] << " " << v[1] << " " << v[2] << " " << v[3]; }

  //  Real& operator[](unsigned int i) { return v[i]; }
  //  Real operator[](unsigned int i) const { return v[i]; }
  Real& operator[](unsigned int i) {
    assert(i >= 0 && i <= 3 && "Index out of bounds in Vector4");
    return v[i];
  }
  Real operator[](unsigned int i) const {
    assert(i >= 0 && i <= 3 && "Index out of bounds in Vector4");
    return v[i];
  }

  Real * GetArrayPtr() { return &v[0]; }
  const Real * GetArrayPtr() const { return &v[0]; }

  Vector4 operator+(const Vector4& vec4) const { return Vector4(v[0]+vec4.v[0],v[1]+vec4.v[1],v[2]+vec4.v[2],v[3]+vec4.v[3]); }
  Vector4 operator-(const Vector4& vec4) const { return Vector4(v[0]-vec4.v[0],v[1]-vec4.v[1],v[2]-vec4.v[2],v[3]-vec4.v[3]); }
  Vector4 entryMult(const Vector4& vec4) const { return Vector4(v[0]*vec4.v[0],v[1]*vec4.v[1],v[2]*vec4.v[2],v[3]*vec4.v[3]); }

  //template<typename Real2>
  //friend std::ostream& std::operator << (std::ostream &os, const Vector4<Real2> &m);

protected:
  Real v[4];
};

template <typename Real1, typename Real2>
Vector4<Real2> operator*(Real1 r, const Vector4<Real2>& v)
{
  Vector4<Real2> res;

  res[0] = v[0]*r;
  res[1] = v[1]*r;
  res[2] = v[2]*r;
  res[3] = v[3]*r;

  return res;
}
template <typename Real>
Real operator*(const Vector4<Real> &v1, const Vector4<Real>& v2)
{
  return v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2]+v1[3]*v2[3];
}

namespace std
{
  template <typename Real>
  ostream& operator<<(ostream& os, const Vector4<Real>& m)
  {
    //std::copy(m.v[0], m.v+4, std::ostream_iterator<Real>(os, " "));
    os << m.v[0] << " " << m.v[1] << " " << m.v[2] << " " << m.v[3] << std::endl;
    return os;
  }

//   template <typename Real>
//   istream& operator>>(istream& is, Vector4<Real>& m)
//   {
//     std::copy(m.m[0], m.m[0]+16, std::ostream_iterator<Real>(os, ", "));
//     return is;
//   }
}

#endif
