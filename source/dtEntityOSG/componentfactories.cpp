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

#include <dtEntityOSG/componentfactories.h>
#include <dtEntity/componentpluginmanager.h>
#include <dtEntityOSG/dtentityosg_config.h>

#include <dtEntityOSG/cameracomponent.h>
#include <dtEntityOSG/groundclampingcomponent.h>
#include <dtEntityOSG/groupcomponent.h>
#include <dtEntityOSG/layerattachpointcomponent.h>
#include <dtEntityOSG/layercomponent.h>
#include <dtEntityOSG/lightcomponent.h>
#include <dtEntityOSG/manipulatorcomponent.h>
#include <dtEntityOSG/matrixtransformcomponent.h>
#include <dtEntityOSG/nodecomponent.h>
#include <dtEntityOSG/osganimationcomponent.h>
#include <dtEntityOSG/particlecomponent.h>
#include <dtEntityOSG/pathcomponent.h>
#include <dtEntityOSG/pickshapecomponent.h>
#include <dtEntityOSG/positionattitudetransformcomponent.h>
#include <dtEntityOSG/shadercomponent.h>
#include <dtEntityOSG/shadowcomponent.h>
#include <dtEntityOSG/skyboxcomponent.h>
#include <dtEntityOSG/staticmeshcomponent.h>
#include <dtEntityOSG/textlabelcomponent.h>
#include <dtEntityOSG/texturelabelcomponent.h>
#include <dtEntityOSG/transformcomponent.h>

#if OSGEPHEMERIS_FOUND
#include <dtEntityOSG/osgephemeriscomponent.h>
#endif

namespace dtEntityOSG
{
   void RegisterStandardFactories(dtEntity::ComponentPluginManager& pluginManager)
   {
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<CameraSystem>("Camera"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<GroundClampingSystem>("GroundClamping"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<GroupSystem>("Group"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<LayerSystem>("Layer"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<LayerAttachPointSystem>("LayerAttachPoint"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<LightSystem>("Light"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<ManipulatorSystem>("Manipulator"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<MatrixTransformSystem>("MatrixTransform"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<NodeSystem>("Node"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<OSGAnimationSystem>("OSGAnimation"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<ParticleSystem>("Particle"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<PathSystem>("Path"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<PickShapeSystem>("PickShape"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<PositionAttitudeTransformSystem>("PositionAttitudeTransform"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<ShaderSystem>("Shader"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<ShadowSystem>("Shadow"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<SkyBoxSystem>("SkyBox"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<StaticMeshSystem>("StaticMesh"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<TextLabelSystem>("TextLabel"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<TextureLabelSystem>("TextureLabel"));
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<TransformSystem>("Transform"));

#if OSGEPHEMERIS_FOUND
      pluginManager.AddFactory(new dtEntity::ComponentPluginFactoryImpl<OSGEphemerisSystem>("OSGEphemeris"));
#endif

   }
}
