/*
* dtEntity Game and Simulation Engine
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* Martin Scheffler
*/

#include <UnitTest++.h>
#include <dtEntity/dynamicproperty.h>

using namespace UnitTest;
using namespace dtEntity;

////////////////////////////////////////////////////////////////////////////////
bool s_bval = false;
void SetBool(bool v) { s_bval = v; }
bool GetBool() { return s_bval; }

TEST(SetValuesDynamicBool)
{
   DynamicBoolProperty prop = DynamicBoolProperty(DynamicBoolProperty::SetValueCB(SetBool), DynamicBoolProperty::GetValueCB(GetBool));
   prop.Set(true);
   CHECK_EQUAL(prop.Get(), true);
   prop.Set(false);
   CHECK_EQUAL(prop.Get(), false);
}

////////////////////////////////////////////////////////////////////////////////
float s_fval = 0;
void SetFloat(float v) { s_fval = v; }
float GetFloat() { return s_fval; }

TEST(SetValuesDynamicFloat)
{
   DynamicFloatProperty prop = DynamicFloatProperty(DynamicFloatProperty::SetValueCB(SetFloat), DynamicFloatProperty::GetValueCB(GetFloat));
   prop.Set(333);
   CHECK_EQUAL(prop.Get(), 333);
}

////////////////////////////////////////////////////////////////////////////////
float s_dval = 0;
void SetDouble(double v) { s_dval = v; }
double GetDouble() { return s_dval; }
TEST(SetValuesDynamicDouble)
{
   DynamicDoubleProperty prop = DynamicDoubleProperty(DynamicDoubleProperty::SetValueCB(SetDouble), DynamicDoubleProperty::GetValueCB(GetDouble));
   prop.Set(333);
   CHECK_EQUAL(prop.Get(), 333);
}

////////////////////////////////////////////////////////////////////////////////
int s_intval = 0;
void SetInt(int v) { s_intval = v; }
int GetInt() { return s_intval; }
TEST(SetValuesDynamicInt)
{
   DynamicIntProperty prop = DynamicIntProperty(DynamicIntProperty::SetValueCB(SetInt), DynamicIntProperty::GetValueCB(GetInt));
   prop.Set(-333);
   CHECK_EQUAL(prop.Get(), -333);
}

////////////////////////////////////////////////////////////////////////////////
unsigned int s_uintval = 0;
void SetUInt(unsigned int v) { s_uintval = v; }
unsigned int GetUInt() { return s_uintval; }
TEST(SetValuesDynamicUInt)
{
   DynamicUIntProperty prop = DynamicUIntProperty(DynamicUIntProperty::SetValueCB(SetUInt), DynamicUIntProperty::GetValueCB(GetUInt));
   prop.Set(333);
   CHECK_EQUAL(prop.Get(), (unsigned int)333);
}

////////////////////////////////////////////////////////////////////////////////
std::string s_strval = "bla";
void SetString(const std::string& v) { s_strval = v; }
std::string GetString() { return s_strval; }
TEST(SetValuesDynamicString)
{
   DynamicStringProperty prop = DynamicStringProperty(DynamicStringProperty::SetValueCB(SetString), DynamicStringProperty::GetValueCB(GetString));
   prop.Set("mystring");
   CHECK_EQUAL(prop.Get(), std::string("mystring"));
}

////////////////////////////////////////////////////////////////////////////////
StringId s_stridval = SID("bla");
void SetStringId(StringId v) { s_stridval = v; }
StringId GetStringId() { return s_stridval; }
TEST(SetValuesDynamicStringId)
{
   DynamicStringIdProperty prop = DynamicStringIdProperty(DynamicStringIdProperty::SetValueCB(SetStringId), DynamicStringIdProperty::GetValueCB(GetStringId));
   prop.Set(SID("mystring"));
   CHECK_EQUAL(prop.Get(), SID("mystring"));
}

////////////////////////////////////////////////////////////////////////////////
Vec2d s_v2dval(1,2);
void SetV2d(const Vec2d& v) { s_v2dval = v; }
Vec2d GetV2d() { return s_v2dval; }
TEST(SetValuesDynamicVec2d)
{
   DynamicVec2dProperty prop = DynamicVec2dProperty(DynamicVec2dProperty::SetValueCB(SetV2d), DynamicVec2dProperty::GetValueCB(GetV2d));
   prop.Set(Vec2d(4,5));
   CHECK(prop.Get() == Vec2d(4,5));
}

////////////////////////////////////////////////////////////////////////////////
Vec3d s_v3dval(1,2,3);
void SetV3d(const Vec3d& v) { s_v3dval = v; }
Vec3d GetV3d() { return s_v3dval; }
TEST(SetValuesDynamicVec3d)
{
   DynamicVec3dProperty prop = DynamicVec3dProperty(DynamicVec3dProperty::SetValueCB(SetV3d), DynamicVec3dProperty::GetValueCB(GetV3d));
   prop.Set(Vec3d(4,5,6));
   CHECK(prop.Get() == Vec3d(4,5,6));
}

////////////////////////////////////////////////////////////////////////////////
Vec4d s_v4dval(1,2,3,4);
void SetV4d(const Vec4d& v) { s_v4dval = v; }
Vec4d GetV4d() { return s_v4dval; }
TEST(SetValuesDynamicVec4d)
{
   DynamicVec4dProperty prop = DynamicVec4dProperty(DynamicVec4dProperty::SetValueCB(SetV4d), DynamicVec4dProperty::GetValueCB(GetV4d));
   prop.Set(Vec4d(4,5,6,7));
   CHECK(prop.Get() == Vec4d(4,5,6,7));
}

////////////////////////////////////////////////////////////////////////////////
Vec2f s_v2val(1,2);
void SetV2(const Vec2f& v) { s_v2val = v; }
Vec2f GetV2() { return s_v2val; }
TEST(SetValuesDynamicVec2)
{
   DynamicVec2Property prop = DynamicVec2Property(DynamicVec2Property::SetValueCB(SetV2), DynamicVec2Property::GetValueCB(GetV2));
   prop.Set(Vec2f(4,5));
   CHECK(prop.Get() == Vec2f(4,5));
}

////////////////////////////////////////////////////////////////////////////////
Vec3f s_v3val(1,2,3);
void SetV3(const Vec3f& v) { s_v3val = v; }
Vec3f GetV3() { return s_v3val; }
TEST(SetValuesDynamicVec3)
{
   DynamicVec3Property prop = DynamicVec3Property(DynamicVec3Property::SetValueCB(SetV3), DynamicVec3Property::GetValueCB(GetV3));
   prop.Set(Vec3f(4,5,6));
   CHECK(prop.Get() == Vec3f(4,5,6));
}
////////////////////////////////////////////////////////////////////////////////
Vec4f s_v4val(1,2,3,4);
void SetV4(const Vec4f& v) { s_v4val = v; }
Vec4f GetV4() { return s_v4val; }
TEST(SetValuesDynamicVec4)
{
   DynamicVec4Property prop = DynamicVec4Property(DynamicVec4Property::SetValueCB(SetV4), DynamicVec4Property::GetValueCB(GetV4));
   prop.Set(Vec4f(4,5,6,7));
   CHECK(prop.Get() == Vec4f(4,5,6,7));
}

////////////////////////////////////////////////////////////////////////////////
Quat s_qval(1,2,3,4);
void SetQ(const Quat& v) { s_qval = v; }
Quat GetQ() { return s_qval; }
TEST(SetValuesDynamicQuat)
{
   DynamicQuatProperty prop = DynamicQuatProperty(DynamicQuatProperty::SetValueCB(SetQ), DynamicQuatProperty::GetValueCB(GetQ));
   prop.Set(Quat(4,5,6,7));
   CHECK(prop.Get() == Quat(4,5,6,7));
}

////////////////////////////////////////////////////////////////////////////////
Matrix s_mval(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
void SetM(const Matrix& v) { s_mval = v; }
Matrix GetM() { return s_mval; }
TEST(SetValuesDynamicMatrix)
{
   DynamicMatrixProperty prop = DynamicMatrixProperty(DynamicMatrixProperty::SetValueCB(SetM), DynamicMatrixProperty::GetValueCB(GetM));
   prop.Set(Matrix(4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19));
   CHECK(prop.Get() == Matrix(4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19));
}
