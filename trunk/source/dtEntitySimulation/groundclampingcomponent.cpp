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

#include <dtEntitySimulation/groundclampingcomponent.h>

#include <dtEntity/basemessages.h>
#include <dtEntity/entity.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/nodemasks.h>
#include <dtEntity/layercomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/stringid.h>
#include <iostream>

namespace dtEntitySimulation
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

   ////////////////////////////////////////////////////////////////////////////
   GroundClampingComponent::GroundClampingComponent()
      : mTransformComponent(NULL)
      , mEntity(NULL)
      , mIntersector(new osgUtil::LineSegmentIntersector(osg::Vec3d(), osg::Vec3d()))
   {
      Register(ClampingModeId, &mClampingMode);
      Register(VerticalOffsetId, &mVerticalOffset);

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
   void GroundClampingComponent::OnFinishedSettingProperties()
   {
      assert(mEntity != NULL);
      bool success = mEntity->GetEntityManager().GetComponent(mEntity->GetId(), mTransformComponent, true);
      if(!success)
      {
         LOG_ERROR("Ground clamping component depends on transform component!");
      }
   }
  
   ////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////

   const dtEntity::StringId GroundClampingSystem::EnabledId(dtEntity::SID("Enabled"));

   GroundClampingSystem::GroundClampingSystem(dtEntity::EntityManager& em)
      : BaseClass(em)
      , mSceneNode(NULL)
      , mTerrainId(0)
      , mIntersectorGroup(new osgUtil::IntersectorGroup())
   {
      //mIntersectionVisitor.setReadCallback(mLos.getDatabaseCacheReadCallback());
      Register(EnabledId, &mEnabled);
      mEnabled.Set(true);

      mTickFunctor = dtEntity::MessageFunctor(this, &GroundClampingSystem::Tick);
      GetEntityManager().RegisterForMessages(dtEntity::EndOfFrameMessage::TYPE,
         mTickFunctor, dtEntity::FilterOptions::PRIORITY_DEFAULT, "GroundClampingSystem::Tick");

      mMapLoadedFunctor = dtEntity::MessageFunctor(this, &GroundClampingSystem::OnMapLoaded);
      em.RegisterForMessages(dtEntity::MapLoadedMessage::TYPE, mMapLoadedFunctor, "GroundClampingComponent::OnMapLoaded");

      mMapUnloadedFunctor = dtEntity::MessageFunctor(this, &GroundClampingSystem::OnMapUnloaded);
      em.RegisterForMessages(dtEntity::MapBeginUnloadMessage::TYPE, mMapUnloadedFunctor, "GroundClampingComponent::OnMapUnloaded");

      AddScriptedMethod("getTerrainHeight", dtEntity::ScriptMethodFunctor(this, &GroundClampingSystem::ScriptGetTerrainHeight));

      dtEntity::MapSystem* ms;
      if(em.GetEntitySystem(dtEntity::MapComponent::TYPE, ms) && ms->GetCurrentScene() != "") 
      {
         dtEntity::SceneLoadedMessage msg;
         OnMapLoaded(msg);
      }      
   }

   ////////////////////////////////////////////////////////////////////////////
   GroundClampingSystem::~GroundClampingSystem()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::OnRemoveFromEntityManager(dtEntity::EntityManager& em)
   {
     GetEntityManager().UnregisterForMessages(dtEntity::EndOfFrameMessage::TYPE, mTickFunctor);

    em.UnregisterForMessages(dtEntity::MapLoadedMessage::TYPE, mMapLoadedFunctor);


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
   void GroundClampingSystem::OnMapLoaded(const dtEntity::Message& msg)
   {
      dtEntity::MapSystem* ms;
      GetEntityManager().GetEntitySystem(dtEntity::MapComponent::TYPE, ms);
      mTerrainId = ms->GetEntityIdByUniqueId("Terrain");
      if(mTerrainId == 0) 
      {
         LOG_DEBUG("Cannot find terrain entity, ground clamping not possible!");
         mSceneNode = NULL;
         return;
      }
      dtEntity::LayerComponent* lc;
      if(!GetEntityManager().GetComponent(mTerrainId, lc))
      {
         LOG_ERROR("Terrain entity has no layer component!");
         mSceneNode = NULL;
         mTerrainId = 0;
         return;
      }
      mSceneNode = lc->GetAttachedComponentNode();
      const dtEntity::Entity* entity = dynamic_cast<const dtEntity::Entity*>(mSceneNode->getUserData());
      assert(entity != NULL);
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::OnMapUnloaded(const dtEntity::Message& msg)
   {
      OnMapLoaded(msg);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool GroundClampingSystem::ClampToTerrain(osg::Vec3d& v, int voffset)
   {
      if(!mSceneNode.valid())
      {
         return false;
      }
      mIntersectionVisitor.reset();
      osgUtil::LineSegmentIntersector* intersector = 
         new osgUtil::LineSegmentIntersector(osg::Vec3d(v[0], v[1], v[2] + voffset),
         osg::Vec3d(v[0], v[1], v[2] - voffset));
      mIntersectionVisitor.setIntersector(intersector);
      mSceneNode->accept(mIntersectionVisitor);

      osgUtil::LineSegmentIntersector::Intersections::const_iterator isect;
      for(isect = intersector->getIntersections().begin(); isect != intersector->getIntersections().end(); ++isect)
      {
         for(osg::NodePath::const_reverse_iterator j = isect->nodePath.rbegin(); j != isect->nodePath.rend(); ++j)
         {
            const osg::Node* node = *j;

            const osg::Referenced* referenced = node->getUserData();

            if(referenced == NULL) continue;
            const dtEntity::Entity* entity = static_cast<const dtEntity::Entity*>(referenced);
            if(entity != NULL && entity->GetId() == mTerrainId)
            {
               osg::Vec3d isectpos = isect->localIntersectionPoint;         
               v[2] = isectpos[2];
               return true;
            }
            if(mSceneNode.get() == node)
            {
               return false;
            }
         }
      }      
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::Tick(const dtEntity::Message& msg)
   {
      if(!mSceneNode.valid() || mComponents.empty() || !mEnabled.Get())
      {
         return;
      }

      dtEntity::LayerAttachPointSystem* layersys;
      GetEntityManager().GetEntitySystem(dtEntity::LayerAttachPointComponent::TYPE, layersys);
      dtEntity::LayerAttachPointComponent* sceneLayer = layersys->GetDefaultLayer();
      mSceneNode = sceneLayer->GetGroup();

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

         dtEntity::TransformComponent* transformcomp = component->GetTransformComponent();
         assert(transformcomp != NULL);
         osg::Vec3d translation = transformcomp->GetTranslation();
         osgUtil::LineSegmentIntersector* intersector = component->GetIntersector();
         intersector->setStart(osg::Vec3d(translation[0], translation[1], translation[2] + 10000));
         intersector->setEnd(osg::Vec3d(translation[0], translation[1], translation[2] - 10000));
         mIntersectorGroup->addIntersector(intersector);
         iSectToCompMap.push_back(std::make_pair(intersector, component));
      }

      mIntersectionVisitor.reset();
      mIntersectionVisitor.setIntersector(mIntersectorGroup.get());
      mSceneNode->accept(mIntersectionVisitor);
      
      for(ISectToCompMap::iterator i = iSectToCompMap.begin(); i != iSectToCompMap.end(); ++i)
      {
         osgUtil::LineSegmentIntersector* isector = i->first;
         GroundClampingComponent* component = i->second;
         HandleIntersections(component, isector->getIntersections());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   void GroundClampingSystem::HandleIntersections(GroundClampingComponent* component,
      const osgUtil::LineSegmentIntersector::Intersections& intersections)
   {
      osgUtil::LineSegmentIntersector::Intersections::const_iterator isect;
      for(isect = intersections.begin(); isect != intersections.end(); ++isect)
      {
         for(osg::NodePath::const_reverse_iterator j = isect->nodePath.rbegin(); j != isect->nodePath.rend(); ++j)
         {
            const osg::Node* node = *j;
            const osg::Referenced* referenced = node->getUserData();

            if(referenced == NULL) continue;
            const dtEntity::Entity* entity = dynamic_cast<const dtEntity::Entity*>(referenced);
            if(entity != NULL && entity->GetId() == mTerrainId)
            {
               osg::Vec3d isectpos = isect->localIntersectionPoint;         
         
               dtEntity::StringId mode = component->GetClampingMode();

               dtEntity::TransformComponent* transformcomp = component->GetTransformComponent();
               osg::Vec3d translation = transformcomp->GetTranslation();

               float voffset = component->GetVerticalOffset();

               if(mode != GroundClampingComponent::ClampingMode_KeepAboveTerrainId || 
                  translation[2] - voffset < isectpos[2])
               {
                  translation[2] = isectpos[2] + voffset;
                  transformcomp->SetTranslation(translation);
               }

               if(mode == GroundClampingComponent::ClampingMode_SetHeightAndRotationToTerrainId)
               {
                  osg::Quat rot = transformcomp->GetRotation();

                  // get current up vector of entity in global coords
                  osg::Vec3 up(0, 0, 1);
                  up = rot * up;

                  // make rotation from global up vec to surface normal
                  osg::Quat addrot;
                  addrot.makeRotate(up, isect->localIntersectionNormal);
                  rot = rot * addrot;
                  transformcomp->SetRotation(rot);
                  
               }
               return;
            }
         }
      }
   }
}
