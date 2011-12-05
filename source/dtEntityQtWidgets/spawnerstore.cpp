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

#include <dtEntityQtWidgets/spawnerstore.h>
#include <dtEntity/applicationcomponent.h>
#include <dtEntity/basemessages.h>
#include <dtEntity/cameracomponent.h>
#include <dtEntity/entity.h>
#include <dtEntity/entitymanager.h>
#include <dtEntity/layerattachpointcomponent.h>
#include <dtEntity/mapcomponent.h>
#include <dtEntity/nodemasks.h>
#include <dtEntity/spawner.h>
#include <dtEntity/windowmanager.h>
#include <dtEntityQtWidgets/messages.h>
#include <osgUtil/LineSegmentIntersector>
#include <osgViewer/View>
#include <osgDB/FileUtils>
#include "ui_spawnerstore.h"

namespace dtEntityQtWidgets
{

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   SpawnerList::SpawnerList()
   {
      setDragEnabled(true);
      setIconSize(QSize(80, 80));
      setMovement(QListView::Snap);
      setWrapping(true);
      setGridSize(QSize(120,120));
      setViewMode(QListView::IconMode);
      setWordWrap(true);

      QFont sansFont("Helvetica [Cronyx]", 8);
      setFont(sansFont);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerList::mousePressEvent(QMouseEvent *event)
    {
        QListWidgetItem* item = itemAt(event->pos());
        if (!item)
            return;

        emit spawnerClicked(item);

        //QPoint hotSpot = event->pos() - item->pos();

        QByteArray itemData;
        QMimeData *mimeData = new QMimeData;
        mimeData->setData("application/x-spawner", itemData);
        mimeData->setText(QString("SPAWNER|%1").arg(item->text()));

		  QDrag drag(this);
		  drag.setMimeData(mimeData);
		  drag.setPixmap(item->icon().pixmap(QSize(40, 40)));

        drag.exec(Qt::CopyAction);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   SpawnerStoreView::SpawnerStoreView(QWidget* parent)
      : QWidget(parent)
   {
      Ui_SpawnerStore ui;
      ui.setupUi(this);
      mCategories = ui.mCategories;
      mSpawnerList = new SpawnerList();
      layout()->addWidget(mSpawnerList);
      connect(mSpawnerList, SIGNAL(spawnerClicked( QListWidgetItem*)), this, SLOT(OnItemClicked(QListWidgetItem*)));
      connect(mCategories, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(CategoryChanged(const QString&)));
   }

   ////////////////////////////////////////////////////////////////////////////////
   SpawnerStoreView::~SpawnerStoreView()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreView::CategoryChanged(const QString& category)
   {
      ShowHideByCategory();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreView::OnItemClicked(QListWidgetItem* item)
   {
      emit SpawnerClicked(item->text(), item->data(Qt::UserRole).toString());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreView::ShowHideByCategory()
   {
      QString category = mCategories->currentText();
      for(int i = 0; i < mSpawnerList->count(); ++i)
      {
         QListWidgetItem* item = mSpawnerList->item(i);
         if(category == "All" || item->data(Qt::UserRole).toString() == category)
         {
            item->setHidden(false);
         }
         else
         {
            item->setHidden(true);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreView::OnAddSpawner(const QString& name, const QString& category, const QString& iconpath)
   {
      QListWidgetItem* item;
      if(iconpath != "")
      {
         QString iconrealpath = osgDB::findDataFile(iconpath.toStdString()).c_str();
         QIcon icon(iconrealpath);
         item = new QListWidgetItem(icon, name, mSpawnerList);
      }
      else
      {
         item = new QListWidgetItem(name, mSpawnerList);
      }
      item->setData(Qt::UserRole, category);

      mSpawnerList->addItem(item);

      if(mCategories->findText(category) == -1)
      {
         mCategories->addItem(category);
      }
      ShowHideByCategory();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreView::OnRemoveSpawner(const QString& name, const QString& category)
   {
      QList<QListWidgetItem*> items = mSpawnerList->findItems(name, Qt::MatchExactly);
      QListWidgetItem* item;
      foreach(item, items)
      {
         if(item->data(Qt::UserRole).toString() == category)
         {
            mSpawnerList->removeItemWidget(item);
            delete item;
            return;
         }
      }
      ShowHideByCategory();
   }


   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   SpawnerStoreController::SpawnerStoreController(dtEntity::EntityManager* entityManager)
   : mEntityManager(entityManager)
   {
      
   }

   ////////////////////////////////////////////////////////////////////////////////
   SpawnerStoreController::~SpawnerStoreController()
   {
      mEntityManager->UnregisterForMessages(dtEntity::SpawnerAddedMessage::TYPE, mSpawnerAddedFunctor);
      mEntityManager->UnregisterForMessages(dtEntity::SpawnerAddedMessage::TYPE, mSpawnerAddedFunctor);
      mEntityManager->UnregisterForMessages(dtEntity::SpawnerModifiedMessage::TYPE, mSpawnerModifiedFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreController::SetupSlots(SpawnerStoreView* view)
   {
      connect(this, SIGNAL(AddSpawner(const QString&, const QString&, const QString&)),
              view, SLOT(OnAddSpawner(const QString&, const QString&, const QString&)));

      connect(this, SIGNAL(RemoveSpawner(const QString&, const QString&)),
              view, SLOT(OnRemoveSpawner(const QString&, const QString&)));

      connect(view, SIGNAL(SpawnerClicked(const QString&, const QString&)),
              this, SLOT(OnSpawnerClicked(const QString&, const QString&)));

   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreController::Init()
   {

      mSpawnerAddedFunctor = dtEntity::MessageFunctor(this, &SpawnerStoreController::OnSpawnerAdded);
      mEntityManager->RegisterForMessages(dtEntity::SpawnerAddedMessage::TYPE, mSpawnerAddedFunctor);

      mSpawnerRemovedFunctor = dtEntity::MessageFunctor(this, &SpawnerStoreController::OnSpawnerRemoved);
      mEntityManager->RegisterForMessages(dtEntity::SpawnerRemovedMessage::TYPE, mSpawnerRemovedFunctor);

      mSpawnerModifiedFunctor = dtEntity::MessageFunctor(this, &SpawnerStoreController::OnSpawnerModified);
      mEntityManager->RegisterForMessages(dtEntity::SpawnerModifiedMessage::TYPE, mSpawnerModifiedFunctor);

      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);

      std::map<std::string, dtEntity::Spawner*> spawners;
      mtsystem->GetAllSpawners(spawners);
      std::map<std::string, dtEntity::Spawner*>::const_iterator i;
      for(i = spawners.begin(); i != spawners.end(); ++i)
      {
         if(i->second->GetAddToSpawnerStore())
         {
            emit AddSpawner(i->second->GetName().c_str(), i->second->GetGUICategory().c_str(),
                            i->second->GetIconPath().c_str());
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreController::OnTextDroppedOntoGLWidget(const QPointF& pos, const QString& text)
   {
      QStringList l = text.split("|");
      if(l.size() != 2 || l.front() != "SPAWNER")
      {
         return;
      }
      QString spawnerName = l.back();

      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);


      dtEntity::Spawner* spawner;
      if(!mtsystem->GetSpawner(spawnerName.toStdString(), spawner))
      {
         LOG_ERROR("Could not spawn by drag and drop: Spawner not found!");
         return;
      }

      dtEntity::ApplicationSystem* appsys;
      mEntityManager->GetEntitySystem(dtEntity::ApplicationSystem::TYPE, appsys);
      
      osg::Vec3 pickray = appsys->GetWindowManager()->GetPickRay("defaultView", pos.x(), pos.y());
      
      dtEntity::CameraComponent* cam;
      mEntityManager->GetComponent(mtsystem->GetEntityIdByUniqueId("cam_0"), cam);
      osg::Vec3 start = cam->GetPosition();
     
      osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi = new osgUtil::LineSegmentIntersector(start, start + pickray * 100000);

      osgUtil::IntersectionVisitor iv(lsi.get());
      iv.setTraversalMask(dtEntity::NodeMasks::PICKABLE);
      iv.setUseKdTreeWhenAvailable(true);

      dtEntity::LayerAttachPointSystem* layersys;
      mEntityManager->GetEntitySystem(dtEntity::LayerAttachPointComponent::TYPE, layersys);
      dtEntity::LayerAttachPointComponent* sceneLayer = layersys->GetDefaultLayer();
      osg::Node* sceneNode = sceneLayer->GetGroup();
      sceneNode->accept(iv);

      bool found = lsi->containsIntersections();

      osg::Vec3 spawnPosition;
      if(found)
      {
         spawnPosition = lsi->getIntersections().begin()->getWorldIntersectPoint();
      }
      else
      {
         spawnPosition = start + pickray * 10;
      }

      dtEntity::Entity* entity;
      mEntityManager->CreateEntity(entity);
      spawner->Spawn(*entity);

      dtEntity::MapComponent* mc;
      if(entity->GetComponent(mc))
      {
         mc->SetEntityName(spawner->GetName());
      }
      mEntityManager->AddToScene(entity->GetId());

      dtEntity::TransformComponent* tcomp;      
      if(mEntityManager->GetComponent(entity->GetId(), tcomp, true))
      {
         tcomp->SetTranslation(spawnPosition);
      }
      
      dtEntity::RequestEntitySelectMessage msg;
      msg.SetAboutEntityId(entity->GetId());
      mEntityManager->EmitMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreController::OnSpawnerClicked(const QString& name, const QString& category)
   {
      SpawnerSelectedMessage msg;
      msg.SetName(name.toStdString());
      mEntityManager->EmitMessage(msg);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreController::OnSpawnerAdded(const dtEntity::Message& m)
   {
      const dtEntity::SpawnerAddedMessage& msg =
         static_cast<const dtEntity::SpawnerAddedMessage&>(m);

      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);

      std::map<std::string, dtEntity::Spawner*> spawners;
      dtEntity::Spawner* spawner;
      if(!mtsystem->GetSpawner(msg.GetName(), spawner))
      {
         return;
      }


      if(spawner->GetAddToSpawnerStore())
      {
         emit AddSpawner(spawner->GetName().c_str(), spawner->GetGUICategory().c_str(),
                         spawner->GetIconPath().c_str());
      }

   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreController::OnSpawnerModified(const dtEntity::Message& m)
   {
      const dtEntity::SpawnerModifiedMessage& msg =
         static_cast<const dtEntity::SpawnerModifiedMessage&>(m);

      emit RemoveSpawner(msg.GetName().c_str(), msg.GetOldCategory().c_str());

      dtEntity::SpawnerAddedMessage m2;
      m2.SetName(msg.GetName());
      m2.SetMapName(msg.GetMapName());
      OnSpawnerAdded(m2);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SpawnerStoreController::OnSpawnerRemoved(const dtEntity::Message& m)
   {
      const dtEntity::SpawnerRemovedMessage& msg =
         static_cast<const dtEntity::SpawnerRemovedMessage&>(m);

      dtEntity::MapSystem* mtsystem;
      bool success = mEntityManager->GetEntitySystem(dtEntity::MapComponent::TYPE, mtsystem);
      assert(success);

      std::map<std::string, dtEntity::Spawner*> spawners;
      dtEntity::Spawner* spawner;
      if(!mtsystem->GetSpawner(msg.GetName(), spawner))
      {
         return;
      }

      emit RemoveSpawner(spawner->GetName().c_str(), spawner->GetGUICategory().c_str());
   }
} 
