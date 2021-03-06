#pragma once

/* -*-c++-*-
* dtEntity Game and Simulation Engine
*
* Copyright (c) 2013 Martin Scheffler
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies 
* or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/

#include <osg/ref_ptr>
#include <dtEntity/entityid.h>
#include <dtEntity/export.h>
#include <dtEntity/messagepump.h>
#include <map>
#include <vector>
#include <assert.h>
#include <OpenThreads/ReadWriteMutex>

namespace dtEntity
{
   class Component;
   class Entity;
   class EntitySystem;
   class Message;

   /**
    * Entity manager is a container for entity systems. 
    * Each entity system holds entity components of a specific type.
    * The entity manager gives access to the entity systems and can be used to
    * create, retrieve and delete components.
    *
    * Also, Entity manager is a central point to send and register to messages.
    * Messages can be sent (emitted) immediately to all registrants, or they can be 
    * posted to a message queue to be emitted at a later time. 
    */
   class DT_ENTITY_EXPORT EntityManager 
   {
   
   public:

      /**
       * Constructor.
       */
      EntityManager();

      // Destructor
      ~EntityManager();

      /**
       * can be used to clean up wrappers for components or
       * execute other actions
       */
      class ComponentDeletedCallback
      {
      public:
         virtual void ComponentDeleted(ComponentType t, EntityId id) = 0;
         virtual ~ComponentDeletedCallback() {}
      };

      typedef std::vector<ComponentDeletedCallback*> ComponentDeletedCallbacks;


      /**
       * If user executes CreateComponent and no entity system with given component
       * type is registered then all EntitySystemRequestCallbacks are executed
       * until one returns true. Callback can be used to add entity systems lazily.
       */
      class EntitySystemRequestCallback
      {
      public:
         virtual bool CreateEntitySystem(EntityManager* em, ComponentType t) = 0;

         virtual ~EntitySystemRequestCallback() {}
      };

      typedef std::vector<EntitySystemRequestCallback*> EntitySystemRequestCallbacks;

      /**
       * Get entity object for entity ID.
       * @param id Unique id of the entity to retrieve
	   * @param entity Will receive pointer to entity object
	   * @return true if entity with this id was found
       * @threadsafe
       */
      bool GetEntity(EntityId id, Entity*& entity);

      /**
       * return true if entity with this ID exists
       */
      bool EntityExists(EntityId id) const;

      /**
       * Fill vector with entity ids of all existing entities
       */
      void GetEntityIds(std::vector<EntityId>& toFill);

      /**
       * delete entity object and tell all entity systems to delete their
       * components for this entity
       * @param id id of the entity to delete
       * @return true if success
       * @threadsafe
       */
      bool KillEntity(EntityId id);

      /**
       * Create a new entity with a new unique EntityId
       * All entities are deleted when entity manager is deleted.
       * This method is thread safe so that entities can be created from
       * worker threads. Creation and deletion of entities are
       * thread-safe, all other entity system/component methods are
       * non-tread safe.
       * @param entity Receives pointer to newly created entity
       * @return true if success
       * @threadsafe
       */
      bool CreateEntity(Entity*& entity);

      /**
       * Loops through all components of origin and creates them on target entity
       * @param target ID of an existing entity with no components
       * @param origin Clone components of this entity
       * @return true if success
       */
      bool CloneEntity(EntityId target, EntityId origin);

      /**
       * @param id Check if an entity with this id exists
       * @return true if such an entity exists
       */
      bool HasEntity(EntityId id) const;

      /**
       * returns true while at least one entity exists
       */
      bool HasEntities() const;

      /**
       * DEPRECATED! Use MapSystem::AddToScene
       */
      bool AddToScene(EntityId eid);

      /**
       * DEPRECATED! Use MapSystem::RemoveFromScene
       */
      bool RemoveFromScene(EntityId eid);

      /**
       * @param id Check if an entity system of this component type is active
       * @return true if such a system exists
       */
      bool HasEntitySystem(ComponentType id) const;

      /**
       * @param s Add this entity system to the manager
       */
      bool AddEntitySystem(EntitySystem& s);

      /**
       * @param s Remove this entity system
       * @return true if entity system was registered, else false
       */
      bool RemoveEntitySystem(EntitySystem& s);

      /**
       * Get entity system of given type.
       * Returns NULL if not found.
       */
      EntitySystem* GetEntitySystem(ComponentType id) const;

      /**
       * @param id Get entity system for this component type
       * @param es receives entity system if found
       * @return true if entity system was found
       */
      template <typename T>
      bool GetEntitySystem(ComponentType id, T*& es) const;

      template <typename T>
      bool GetES(T*& es) const;

      /**
       * Fill vector with all entity systems registered with
       * the entity manager
       */
      void GetEntitySystems(std::vector<EntitySystem*>& toFill);
      void GetEntitySystems(std::vector<const EntitySystem*>& toFill) const;

      /**
       * @param eid Get component of this entity
       * @param t Get component with this component type
       * @param component Receives component if found
	   * @param searchDerived If true, return component that inherits t if one exists. Inheritance
	   *                      is defined by return value of EntitySystem::GetBaseType
       * @return true if component was found
       */
      bool GetComponent(EntityId eid, ComponentType t, Component*& component, bool searchDerived = false);

      /**
       * @param eid Get component of this entity
       * @param component Receives component of type T if found
       * @param searchDerived If true, return component that inherits t if one exists. Inheritance
	   *                      is defined by return value of EntitySystem::GetBaseType
       * @return true if success
       */
      template <typename T>
      bool GetComponent(EntityId eid, T*& component, bool searchDerived = false);

      /**
       * Get all components of given entity
       * Warning: slow. Iterates over all entity systems.
       * @param eid Get components of this entity
       * @param toFill receives components
       */
      void GetComponents(EntityId eid, std::vector<Component*>& toFill);
      void GetComponents(EntityId eid, std::vector<const Component*>& toFill) const;

      /**
       * @param eid Check if component exists for this entity
       * @param t Type of component to check for
       * @param searchDerived If true, search for component that inherits t. Inheritance
	   *                      is defined by return value of EntitySystem::GetBaseType
       * @return true if exists
       */
      bool HasComponent(EntityId eid, ComponentType t, bool searchDerived = false) const;

      /**
       * Create a component of given type for entity
       * @param eid Add component to this entity
       * @param id Add component of this type
       * @param component Receives newly created component if success
       * @return true if success
       */
      bool CreateComponent(EntityId eid, ComponentType id, Component*& component);
      
      /**
       * Create a component of given type for entity 
       * @param eid Add component to this entity
       * @param component Receives newly created component of type T if success
       * @return true if success
       */      
      template <typename T>
      bool CreateComponent(EntityId eid, T*& component);

      /**
       * Delete component.
       */
      bool DeleteComponent(EntityId eid, Component& component);

      /**
       * Delete component.
       */
      bool DeleteComponent(EntityId eid, ComponentType componentType);

	  /**
	   * @return default message pump for inter-system communication
	   */
      MessagePump& GetMessagePump()
      {
        return mMessagePump;
      }
      

      // See messagepump.h for documentation
      inline void RegisterForMessages(MessageType msgtype, MessageFunctor& ftr, unsigned int options = FilterOptions::DEFAULT, const std::string& funcname = "")
      {
         mMessagePump.RegisterForMessages(msgtype, ftr, options, funcname);
      }

      // See messagepump.h for documentation
      inline void RegisterForMessages(MessageType msgtype, MessageFunctor& ftr, const std::string& funcname)
      {
         mMessagePump.RegisterForMessages(msgtype, ftr, FilterOptions::DEFAULT, funcname);
      }

      // See messagepump.h for documentation
      inline bool UnregisterForMessages(MessageType msgtype, MessageFunctor& ftr)
      {
         return mMessagePump.UnregisterForMessages(msgtype, ftr);
      }

      // See messagepump.h for documentation
      inline void EmitMessage(const Message& msg)
      {
         mMessagePump.EmitMessage(msg);
      }

      // See messagepump.h for documentation
      inline void EnqueueMessage(const Message& msg, double time = 0)
      {
         mMessagePump.EnqueueMessage(msg, time);
      }

      // See messagepump.h for documentation
      void EmitQueuedMessages(double simtime)
      {
         mMessagePump.EmitQueuedMessages(simtime);
      }

      void AddDeletedCallback(ComponentDeletedCallback* cb);
      bool RemoveDeletedCallback(ComponentDeletedCallback* cb);

      void AddEntitySystemRequestCallback(EntitySystemRequestCallback* cb);
      bool RemoveEntitySystemRequestCallback(EntitySystemRequestCallback* cb);


   private:

      // Returns next id and increments internal counter
      EntityId GetNextAvailableID();

      unsigned int mNextAvailableId;

      /**
       * Look in type hierarchy map if a component derived from type exists
       */
      bool GetDerived(EntityId eid, ComponentType ctype, Component*& comp) const;

      // storage for entity objects.
      typedef std::map<EntityId,Entity*> EntityMap;
      EntityMap mEntities;

      // controls access to mEntities.
      mutable OpenThreads::ReadWriteMutex mEntityMutex;

      // Storage for entity systems
      typedef std::map<ComponentType, EntitySystem*> EntitySystemStore;
      EntitySystemStore mEntitySystemStore;

      // stores inheritance tree for component types
      typedef std::multimap<ComponentType, ComponentType> TypeHierarchyMap;
      TypeHierarchyMap mTypeHierarchy;

	  // for publish-subscribe of messages
      MessagePump mMessagePump;

      ComponentDeletedCallbacks mDeletedCallbacks;

      EntitySystemRequestCallbacks mEntitySystemRequestCallbacks;

   };


   ////////////////////////////////////////////////////////////////////////////////
   template <typename T>
   bool EntityManager::CreateComponent(EntityId eid, T*& component)
   {
      assert(eid > 0);
      Component* c;
      if(!this->CreateComponent(eid, T::TYPE, c))
      {
         return false;
      }
      component = static_cast<T*>(c);
      return true;
   }


   ////////////////////////////////////////////////////////////////////////////////
   template <typename T>
   bool EntityManager::GetComponent(EntityId eid, T*& component, bool searchDerived)
   {
      Component* c;
      if(this->GetComponent(eid, T::TYPE, c, searchDerived))
      {
         assert(dynamic_cast<T*>(c) != NULL);
         component = static_cast<T*>(c);
         return true;
      }
      return false;

   }

   ////////////////////////////////////////////////////////////////////////////////
   template <typename T>
   bool EntityManager::GetEntitySystem(ComponentType id, T*& es) const
   {
      EntitySystem* s = GetEntitySystem(id);
      if(s == NULL) return false;
      assert(dynamic_cast<T*>(s) != NULL);
      es = static_cast<T*>(s);
      
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   template <typename T>
   bool EntityManager::GetES(T*& es) const
   {
      ComponentType ctype = T::TYPE;
      EntitySystemStore::const_iterator i = mEntitySystemStore.find(ctype);
      if(i == mEntitySystemStore.end())
      {
         es = NULL;
         return false;
      }
      EntitySystem* s = i->second;
      assert(dynamic_cast<T*>(s) != NULL);
      es = static_cast<T*>(s);
      return true;
   }

}
