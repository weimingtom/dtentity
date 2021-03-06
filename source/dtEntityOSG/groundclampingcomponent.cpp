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

#include <dtEntityOSG/groundclampingcomponent.h>

#include <dtEntity/entity.h>
#include <dtEntityOSG/layerattachpointcomponent.h>
#include <dtEntity/nodemasks.h>
#include <dtEntityOSG/layercomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/stringid.h>
#include <dtEntity/systemmessages.h>
#include <iostream>
#include <osg/io_utils>
#include <osgSim/LineOfSight>

namespace dtEntityOSG
{


   const dtEntity::StringId GroundClampingComponent::TYPE(dtEntity::SID("GroundClamping"));
   const dtEntity::StringId GroundClampingComponent::ClampingModeId(dtEntity::SID("ClampingMode"));
   const dtEntity::StringId GroundClampingComponent::ClampingMode_DisabledId(dtEntity::SID("Disabled"));
   const dtEntity::StringId GroundClampingComponent::ClampingMode_KeepAboveTerrainId(dtEntity::SID("KeepAboveTerrain"));
   const dtEntity::StringId GroundClampingComponent::ClampingMode_SetHeightToTerrainHeightId(
      dtEntity::SID("SetHeightToTerrainHeight"));
   const dtEntity::StringId GroundClampingComponent::ClampingMode_SetHeightAndRotationToTerrainId(
      dtEntity::SID("SetHeightAndRotationToTerrain"));
   const dtEntity::StringId GroundClampingComponent::VerticalOffsetId(dtEntity::SID("VerticalOffset"));
   const dtEntity::StringId GroundClampingComponent::MinDistToCameraId(dtEntity::SID("MinDistToCamera"));
   const dtEntity::StringId GroundClampingComponent::MinMovementDeltaId(dtEntity::SID("MinMovementDelta"));

   ////////////////////////////////////////////////////////////////////////////
   GroundClampingComponent::GroundClampingComponent()
      : mTransformComponent(NULL)
      , mEntity(NULL)
      , mIntersector(new osgUtil::LineSegmentIntersector(osg::Vec3d(), osg::Vec3d()))
      , mDirty(true)
   {
      Register(ClampingModeId, &mClampingMode);
      Register(VerticalOffsetId, &mVerticalOffset);
      Register(MinDistToCameraId, &mMinDistToCamera);
      Register(MinMovementDeltaId, &mMinMovementDelta);

      mMinDistToCamera.Set(500);
      mMinMovementDelta.Set(0.2f);
      
   }
    
   ////////////////////////////////////////////////////////////////////////////
   GroundClampingComponent::~GroundClampingComponent()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingComponent::OnAddedToEntity(dtEntity::Entity& e)
   {
      mEntity = &e;
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingComponent::Finished()
   {
      BaseClass::Finished();
      assert(mEntity != NULL);
      bool success = mEntity->GetEntityManager().GetComponent(mEntity->GetId(), mTransformComponent, true);
      if(!success)
      {
         LOG_ERROR("Ground clamping component depends on transform component!");
      }
      SetDirty(true);
   }
  
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////
   const dtEntity::StringId GroundClampingSystem::TYPE(dtEntity::SID("GroundClamping"));
   const dtEntity::StringId GroundClampingSystem::EnabledId(dtEntity::SID("Enabled"));
   const dtEntity::StringId GroundClampingSystem::IntersectLayerId(dtEntity::SID("IntersectLayer"));
   const dtEntity::StringId GroundClampingSystem::FetchLODsId(dtEntity::SID("FetchLODs"));

   ////////////////////////////////////////////////////////////////////////////
   GroundClampingSystem::GroundClampingSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
      , mRootNode(NULL)
      , mIntersectorGroup(new osgUtil::IntersectorGroup())
      , mEnabled(
           dtEntity::DynamicBoolProperty::SetValueCB(this, &GroundClampingSystem::SetEnabled),
           dtEntity::DynamicBoolProperty::GetValueCB(this, &GroundClampingSystem::GetEnabled)
        )
      , mEnabledVal(true)
      , mIntersectLayer(
           dtEntity::DynamicStringIdProperty::SetValueCB(this, &GroundClampingSystem::SetIntersectLayer),
           dtEntity::DynamicStringIdProperty::GetValueCB(this, &GroundClampingSystem::GetIntersectLayer)
        )
      , mCamera(NULL)
   {

      Register(EnabledId, &mEnabled);
      Register(IntersectLayerId, &mIntersectLayer);
      Register(FetchLODsId, &mFetchLODs);
      mFetchLODs.Set(true);

      mTickFunctor = dtEntity::MessageFunctor(this, &GroundClampingSystem::Tick);
      GetEntityManager().RegisterForMessages(dtEntity::EndOfFrameMessage::TYPE,
         mTickFunctor, dtEntity::FilterOptions::ORDER_DEFAULT, "GroundClampingSystem::Tick");

      mCameraAddedFunctor = dtEntity::MessageFunctor(this, &GroundClampingSystem::CameraAdded);
      GetEntityManager().RegisterForMessages(dtEntity::CameraAddedMessage::TYPE,
         mCameraAddedFunctor, "GroundClampingSystem::CameraAdded");

      mCameraRemovedFunctor = dtEntity::MessageFunctor(this, &GroundClampingSystem::CameraRemoved);
      GetEntityManager().RegisterForMessages(dtEntity::CameraRemovedMessage::TYPE,
         mCameraAddedFunctor, "GroundClampingSystem::CameraRemoved");

	   mMapLoadedFunctor = dtEntity::MessageFunctor(this, &GroundClampingSystem::MapLoaded);
      GetEntityManager().RegisterForMessages(dtEntity::MapLoadedMessage::TYPE,
         mMapLoadedFunctor, "GroundClampingSystem::MapLoaded");

	   GetEntityManager().RegisterForMessages(dtEntity::MapUnloadedMessage::TYPE,
         mMapLoadedFunctor, "GroundClampingSystem::MapUnLoaded");

      AddScriptedMethod("getTerrainHeight", dtEntity::ScriptMethodFunctor(this, &GroundClampingSystem::ScriptGetTerrainHeight));

      dtEntity::MapSystem* mapsys;
      GetEntityManager().GetEntitySystem(dtEntity::MapComponent::TYPE, mapsys);
      dtEntity::EntityId camid = mapsys->GetEntityIdByUniqueId("cam_0");
      if(camid != dtEntity::EntityId())
      {
         GetEntityManager().GetComponent(camid, mCamera);
      }
      else
      {
         dtEntityOSG::CameraSystem* camsys;
         bool success = GetEntityManager().GetES(camsys);
         assert(success);
         if(camsys->GetNumComponents() != 0)
         {
            mCamera = camsys->begin()->second;
         }
      }

      SetIntersectLayer(dtEntityOSG::LayerAttachPointSystem::DefaultLayerId);

   }

   ////////////////////////////////////////////////////////////////////////////
   GroundClampingSystem::~GroundClampingSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::SetEnabled(bool v)
   {
      mEnabledVal = v;
      if(v)
      {
         DirtyAll();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   bool GroundClampingSystem::GetEnabled() const
   {
      return mEnabledVal;
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::OnRemoveFromEntityManager(dtEntity::EntityManager& em)
   {
      GetEntityManager().UnregisterForMessages(dtEntity::EndOfFrameMessage::TYPE, mTickFunctor);
      GetEntityManager().UnregisterForMessages(dtEntity::CameraAddedMessage::TYPE, mCameraAddedFunctor);
      GetEntityManager().UnregisterForMessages(dtEntity::CameraRemovedMessage::TYPE, mCameraRemovedFunctor);
	   GetEntityManager().UnregisterForMessages(dtEntity::MapLoadedMessage::TYPE, mMapLoadedFunctor);
	   GetEntityManager().UnregisterForMessages(dtEntity::MapUnloadedMessage::TYPE, mMapLoadedFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::DirtyAll() 
   {
      ComponentStore::iterator i = mComponents.begin();
      for(; i != mComponents.end(); ++i)
      {
         i->second->SetDirty(true);
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::StringId GroundClampingSystem::GetIntersectLayer() const
   {
      return mIntersectLayerVal;
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::SetIntersectLayer(dtEntity::StringId layername)
   {
      mIntersectLayerVal = layername;
      dtEntityOSG::LayerAttachPointSystem* layersys;
      GetEntityManager().GetEntitySystem(dtEntityOSG::LayerAttachPointComponent::TYPE, layersys);
      dtEntityOSG::LayerAttachPointComponent* c;
      if(layersys->GetByName(layername, c))
      {
         mRootNode = c->GetGroup();
      }
      else
      {
         LOG_ERROR("Could not find intersect layer for ground clamping system!");
         mRootNode = NULL;
      }

      DirtyAll();
   }

   ////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* GroundClampingSystem::ScriptGetTerrainHeight(const dtEntity::PropertyArgs& args)
   {
      osg::Vec3d pos = args[0]->Vec3dValue();
      int voffset = 10000;
      if(args.size() > 1)
      {
         voffset = args[1]->IntValue();
      }
      bool found = ClampToTerrain(pos, voffset);
      if(found)
      {
         return new dtEntity::DoubleProperty(pos[2]);
      }
      else
      {
         return NULL;
      }
   } 

   ////////////////////////////////////////////////////////////////////////////
   bool GroundClampingSystem::ClampToTerrain(osg::Vec3d& v, int voffset)
   {
      if(!mRootNode.valid())
      {
         return false;
      }
      mIntersectionVisitor.reset();
      mIntersectionVisitor.setReadCallback(NULL);
      osgUtil::LineSegmentIntersector* intersector = 
         new osgUtil::LineSegmentIntersector(osg::Vec3d(v[0], v[1], v[2] + voffset),
         osg::Vec3d(v[0], v[1], v[2] - voffset));
      mIntersectionVisitor.setIntersector(intersector);
      mIntersectionVisitor.setTraversalMask(dtEntity::NodeMasks::TERRAIN);

      mRootNode->accept(mIntersectionVisitor);
	   if(intersector->containsIntersections())
	   {
		   v = intersector->getFirstIntersection().getWorldIntersectPoint();
		   return true;
	   }
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::CameraAdded(const dtEntity::Message& m)
   {
      const dtEntity::CameraAddedMessage& msg = static_cast<const dtEntity::CameraAddedMessage&>(m);
      if(msg.GetContextId() == 0)
      {
         GetEntityManager().GetComponent(msg.GetAboutEntityId(), mCamera);
         DirtyAll();
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::CameraRemoved(const dtEntity::Message& m)
   {
      const dtEntity::CameraAddedMessage& msg = static_cast<const dtEntity::CameraAddedMessage&>(m);
      if(msg.GetContextId() == 0)
      {
         mCamera = NULL;
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::MapLoaded(const dtEntity::Message& msg)
   {
	  SetIntersectLayer(mIntersectLayer.Get());
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::Tick(const dtEntity::Message& m)
   {
      const dtEntity::TickMessage& tick = static_cast<const dtEntity::TickMessage&>(m);
      float dt = tick.GetDeltaSimTime();
      double simTime = tick.GetSimulationTime();

      if(!mRootNode.valid() || mCamera == NULL || !mEnabled.Get() || mComponents.empty() )
      {
         return;
      }

      osg::Vec3d campos = mCamera->GetPosition();

      mIntersectorGroup->clear();
      mIntersectorGroup->reset();
      
      typedef std::list<std::pair<osgUtil::LineSegmentIntersector*, GroundClampingComponent*> > ISectToCompMap;
      ISectToCompMap iSectToCompMap;

      ComponentStore::iterator i = mComponents.begin();
      for(; i != mComponents.end(); ++i)
      {
         GroundClampingComponent* component = i->second;

         dtEntity::StringId mode = component->GetClampingMode();
         if(mode == GroundClampingComponent::ClampingMode_DisabledId)
         {
            continue;
         }

         dtEntityOSG::TransformComponent* transformcomp = component->GetTransformComponent();
         assert(transformcomp != NULL);
         osg::Vec3d translation = transformcomp->GetTranslation();

         // don't do clamping if camera is too far away
         float distx = translation[0] - campos[0];
         float disty = translation[1] - campos[1];
         float distToCam = sqrt(distx * distx + disty * disty);
         if(distToCam > component->GetMinDistToCamera())
         {
            component->SetDirty(true);
            continue;
         }

         osg::Vec3d lastpos = component->GetLastClampedPosition();
        
         double distMovedX = translation[0] - lastpos[0];
         double distMovedY = translation[1] - lastpos[1];

         // if only moved a little: Set height to last clamp height to override other
         // height modifiers
         if(!component->GetDirty() &&
            fabs(distMovedX) < component->GetMinMovementDelta() &&
            fabs(distMovedY) < component->GetMinMovementDelta()
            )
         {
            osg::Vec3 norml = component->GetLastClampedNormal();
            if(norml[2] != 0)
            {
               double slopeX = norml[0] / norml[2] * -1;
               double slopeY = norml[1] / norml[2] * -1;

               double deltah =  slopeX * distMovedX + slopeY * distMovedY;
               double newh = lastpos[2] + deltah;
               if((component->GetClampingMode() == GroundClampingComponent::ClampingMode_KeepAboveTerrainId && newh > translation[2]) ||
                     newh != translation[2])
               {
                  translation[2] = newh;
                  transformcomp->SetTranslation(translation);
               }

            }

            if(component->GetClampingMode() == GroundClampingComponent::ClampingMode_SetHeightAndRotationToTerrainId)
            {
               transformcomp->SetRotation(component->GetLastClampedAttitude());
            }
            transformcomp->Finished();
            continue;
         }

         osgUtil::LineSegmentIntersector* intersector = component->GetIntersector();
         intersector->setStart(osg::Vec3d(translation[0], translation[1], translation[2] + 10000));
         intersector->setEnd(osg::Vec3d(translation[0], translation[1], translation[2] - 10000));
         mIntersectorGroup->addIntersector(intersector);
         iSectToCompMap.push_back(std::make_pair(intersector, component));
         static int i = 0;
         ++i;
      }

      mIntersectionVisitor.reset();
      if(mFetchLODs.Get())
      {
         mIntersectionVisitor.setReadCallback(new osgSim::DatabaseCacheReadCallback);
      }
      else
      {
         mIntersectionVisitor.setReadCallback(NULL);
      }
      mIntersectionVisitor.setTraversalMask(dtEntity::NodeMasks::TERRAIN);
      mIntersectionVisitor.setIntersector(mIntersectorGroup.get());
      mRootNode->accept(mIntersectionVisitor);
      
      for(ISectToCompMap::iterator i = iSectToCompMap.begin(); i != iSectToCompMap.end(); ++i)
      {
         osgUtil::LineSegmentIntersector* isector = i->first;
         if(isector->containsIntersections())
         {
            GroundClampingComponent* component = i->second;
            HandleIntersection(component, isector->getFirstIntersection(), dt, simTime);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::HandleIntersection(GroundClampingComponent* component,
      const osgUtil::LineSegmentIntersector::Intersection& intersection, float dt, double simTime)
   {
      osg::Vec3d isectpos = intersection.getWorldIntersectPoint();         

      dtEntity::StringId mode = component->GetClampingMode();

      dtEntityOSG::TransformComponent* transformcomp = component->GetTransformComponent();
      osg::Vec3d translation = transformcomp->GetTranslation();

      float voffset = component->GetVerticalOffset();

      if(mode != GroundClampingComponent::ClampingMode_KeepAboveTerrainId || 
         translation[2] - voffset < isectpos[2])
      {
         translation[2] = isectpos[2] + voffset;
         transformcomp->SetTranslation(translation);
         component->SetLastClampedNormal(intersection.getWorldIntersectNormal());
      }
      else
      {
         component->SetLastClampedNormal(osg::Vec3());
      }

      if(mode == GroundClampingComponent::ClampingMode_SetHeightAndRotationToTerrainId)
      {
         osg::Quat rot = transformcomp->GetRotation();

         // get current up vector of entity in global coords
         osg::Vec3 up(0, 0, 1);
         up = rot * up;

         // make rotation from global up vec to surface normal
         osg::Quat addrot;
         addrot.makeRotate(up, intersection.getWorldIntersectNormal());
                           
         osg::Quat newrot = rot * addrot;
         transformcomp->SetRotation(newrot);  
         component->SetLastClampedAttitude(newrot);
      }               

      component->SetLastClampedPosition(translation);

      component->SetDirty(false);
      transformcomp->Finished();
   }
}
