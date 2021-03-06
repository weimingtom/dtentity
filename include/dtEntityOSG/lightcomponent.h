#pragma once

/* -*-c++-*-
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

#include <dtEntityOSG/export.h>
#include <dtEntity/entity.h>
#include <dtEntity/entityid.h>
#include <dtEntity/defaultentitysystem.h>
#include <dtEntity/dynamicproperty.h>
#include <dtEntityOSG/nodecomponent.h>


namespace osg
{
   class Light;
   class LightSource;
}

namespace dtEntityOSG
{      
  
   class DTENTITY_OSG_EXPORT LightComponent : public NodeComponent
   {

      typedef NodeComponent BaseClass;

   public:
      
      static const dtEntity::ComponentType TYPE;

      static const dtEntity::StringId LightNumId;
      static const dtEntity::StringId PositionId;
      static const dtEntity::StringId AmbientId;
      static const dtEntity::StringId DiffuseId;
      static const dtEntity::StringId SpecularId;
      static const dtEntity::StringId SpotCutoffId;
      static const dtEntity::StringId SpotExponentId;
      static const dtEntity::StringId DirectionId;
      static const dtEntity::StringId ConstantAttenuationId;
      static const dtEntity::StringId LinearAttenuationId;
      static const dtEntity::StringId QuadraticAttenuationId;

      LightComponent();
      virtual ~LightComponent();

      virtual dtEntity::ComponentType GetType() const { return TYPE; }
      
      virtual bool IsInstanceOf(dtEntity::ComponentType id) const
      { 
         return (id == TYPE); 
      }      

      osg::Light* GetLight() { return mLight; }

   private:

      osg::Vec4 GetLightPosition() const;
      osg::Vec4 GetAmbient() const;
      osg::Vec4 GetDiffuse() const;
      osg::Vec4 GetSpecular() const;
      osg::Vec3 GetDirection() const;

      osg::Light* mLight;
      dtEntity::DynamicIntProperty mLightNum;
      dtEntity::DynamicVec4Property mAmbient;
      dtEntity::DynamicVec4Property mDiffuse;
      dtEntity::DynamicVec4Property mSpecular;
      dtEntity::DynamicVec4Property mPosition;
      dtEntity::DynamicVec3Property mDirection;
      dtEntity::DynamicFloatProperty mConstantAttenuation;
      dtEntity::DynamicFloatProperty mLinearAttenuation;
      dtEntity::DynamicFloatProperty mQuadraticAttenuation;
      dtEntity::DynamicFloatProperty mSpotExponent;
     dtEntity:: DynamicFloatProperty mSpotCutOff;
   };
   
   
   ////////////////////////////////////////////////////////////////////////////
   
   class DTENTITY_OSG_EXPORT LightSystem
      : public dtEntity::DefaultEntitySystem<LightComponent>
   {
      typedef dtEntity::DefaultEntitySystem<LightComponent> BaseClass;
      
   public:
      static const dtEntity::ComponentType TYPE;

      LightSystem(dtEntity::EntityManager& em);     

   private:

   };
}
