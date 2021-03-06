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

#include <dtEntity/export.h>
#include <dtEntity/message.h>
#include <dtEntity/property.h>

namespace dtEntity
{

   /**
    * These messages can be sent by user code to trigger a reaction in
    * a dtEntity subsystem.
    */


   ////////////////////////////////////////////////////////////////////////////////
   class MessageFactory;

   /**
    * Adds all messages to message factory
	*/
	void DT_ENTITY_EXPORT RegisterCommandMessages(MessageFactory&);


   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Send this message to cause the map system to remove an entity from
    * scene and delete it
    */
   class DT_ENTITY_EXPORT DeleteEntityMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId UniqueIdId;

      DeleteEntityMessage();

      virtual Message* Clone() const { return CloneContainer<DeleteEntityMessage>(); }

      void SetUniqueId(const std::string& v) { mUniqueId.Set(v); }
      std::string GetUniqueId() const { return mUniqueId.Get(); }

   private:

      StringProperty mUniqueId;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Systems can react to this message by enabling their debug drawing.
    */
   class DT_ENTITY_EXPORT EnableDebugDrawingMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId EnableId;

      EnableDebugDrawingMessage();

      virtual Message* Clone() const { return CloneContainer<EnableDebugDrawingMessage>(); }

      bool GetEnable() const { return mEnable.Get(); }
      void SetEnable(bool v){ mEnable.Set(v); }

   private:

      BoolProperty mEnable;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Cause camera to be moved to entity
    */
   class DT_ENTITY_EXPORT MoveCameraToEntityMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;
      static const StringId DistanceId;
      static const StringId KeepCameraDirectionId;
      static const StringId ContextIdId;

      MoveCameraToEntityMessage();

      virtual Message* Clone() const { return CloneContainer<MoveCameraToEntityMessage>(); }

      EntityId GetAboutEntityId() const { return mAboutEntityId.Get(); }
      void SetAboutEntityId(EntityId id) { mAboutEntityId.Set(id); }

      // default is 10
      double GetDistance() const { return mDistance.Get(); }
      void SetDistance(double v) { mDistance.Set(v); }

      // if true: current eye direction of camera is preserved.
      // if false: rotate camera from current position to target entity, then
      // move until distance is reached
      // true by default
      bool GetKeepCameraDirection() const { return mKeepCameraDirection.Get(); }
      void SetKeepCameraDirection(bool v) { mKeepCameraDirection.Set(v); }

      // camera context
      int GetContextId() const { return mContextId.Get(); }
      void SetContextId(int v) { mContextId.Set(v); }

   private:

      UIntProperty mAboutEntityId;
      DoubleProperty mDistance;
      BoolProperty mKeepCameraDirection;
      IntProperty mContextId;
   };

   ///////////////////////////////////////////////////////////////////////////////
   /**
    * Cause camera to be moved to a specific position
    */
   class DT_ENTITY_EXPORT MoveCameraToPositionMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId PositionId;
      static const StringId LookAtId;
      static const StringId UpId;
      static const StringId ContextIdId;

      MoveCameraToPositionMessage();

      virtual Message* Clone() const { return CloneContainer<MoveCameraToPositionMessage>(); }

      Vec3d GetPosition() const { return mPosition.Get(); }
      void SetPosition(const Vec3d& v) { mPosition.Set(v); }

      // if same as position: Look in direction of movement from last position
      Vec3d GetLookAt() const { return mLookAt.Get(); }
      void SetLookAt(const Vec3d& v) { mLookAt.Set(v); }

      Vec3f GetUp() const { return mUp.Get(); }
      void SetUp(const Vec3f& v) { mUp.Set(v); }

      // camera context
      int GetContextId() const { return mContextId.Get(); }
      void SetContextId(int v) { mContextId.Set(v); }

   private:

      Vec3dProperty mPosition;
      Vec3dProperty mLookAt;
      Vec3Property mUp;
      IntProperty mContextId;
   };

   ///////////////////////////////////////////////////////////////////////////////
   /**
    * Cause an animation to be played
    */
   class DT_ENTITY_EXPORT PlayAnimationMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;
      static const StringId AnimationNameId;

      PlayAnimationMessage();

      virtual Message* Clone() const { return CloneContainer<PlayAnimationMessage>(); }

      EntityId GetAboutEntityId() const { return mAboutEntityId.Get(); }
      void SetAboutEntityId(EntityId id) { mAboutEntityId.Set(id); }

      std::string GetAnimationName() const { return mAnimationName.Get(); }
      void SetAnimationName(const std::string& id) { mAnimationName.Set(id); }

   private:

      UIntProperty mAboutEntityId;
      StringProperty mAnimationName;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT RequestEntityDeselectMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;

      RequestEntityDeselectMessage();

      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<RequestEntityDeselectMessage>(); }

      void SetAboutEntityId(EntityId id) { mAboutEntity.Set(id); }
      EntityId GetAboutEntityId() const { return mAboutEntity.Get(); }

   private:

      UIntProperty mAboutEntity;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT RequestEntitySelectMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;
      static const StringId UseMultiSelectId;

      RequestEntitySelectMessage();

      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<RequestEntitySelectMessage>(); }

      void SetAboutEntityId(EntityId id) { mAboutEntity.Set(id); }
      EntityId GetAboutEntityId() const { return mAboutEntity.Get(); }

      void SetUseMultiSelect(bool v) { mUseMultiSelect.Set(v); }
      bool GetUseMultiSelect() const { return mUseMultiSelect.Get(); }

   private:

      UIntProperty mAboutEntity;
      BoolProperty mUseMultiSelect;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DT_ENTITY_EXPORT RequestToggleEntitySelectionMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId AboutEntityId;

      RequestToggleEntitySelectionMessage();

      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<RequestToggleEntitySelectionMessage>(); }

      void SetAboutEntityId(EntityId id) { mAboutEntity.Set(id); }
      EntityId GetAboutEntityId() const { return mAboutEntity.Get(); }

   private:

      UIntProperty mAboutEntity;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Application system reacts to this message by retrieving a component of type t
   * from entity identified by id and applying the property values of mProperty
   * to the component.
   */
   class DT_ENTITY_EXPORT SetComponentPropertiesMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId EntityUniqueIdId;
      static const StringId ComponentTypeId;
      static const StringId PropertiesId;

      SetComponentPropertiesMessage();

      /**
       * Initialize with single property. Prop is cloned!
       */
      SetComponentPropertiesMessage(const std::string& entityUniqueId,
         const std::string& componenttype, const std::string& propname, Property& prop);

      virtual Message* Clone() const { return CloneContainer<SetComponentPropertiesMessage>(); }

      std::string GetEntityUniqueId() const { return mEntityUniqueId.Get(); }
      void SetEntityUniqueId(const std::string& id) { mEntityUniqueId.Set(id); }

      void SetComponentType(const std::string& t) { mComponentType.Set(t); }
      std::string GetComponentType() const { return mComponentType.Get(); }

	  /**
		* Group of properties that will be applied to entity system
		*/
      void SetComponentProperties(PropertyGroup& v) { mProperties.Set(v); }
      const PropertyGroup& GetComponentProperties() const { return mProperties.Get(); }

   private:
      StringProperty mComponentType;
      GroupProperty mProperties;
      StringProperty mEntityUniqueId;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Application system reacts to this message by retrieving the entity system
   * of type t and applying the property values of mProperty to the entity system.
   */
   class DT_ENTITY_EXPORT SetSystemPropertiesMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId ComponentTypeId;
      static const StringId PropertiesId;

      SetSystemPropertiesMessage();

      /**
       * Initialize with single property. Prop is cloned!
       */
      SetSystemPropertiesMessage(const std::string& componenttype, const std::string& propname, Property& prop);

      virtual Message* Clone() const { return CloneContainer<SetSystemPropertiesMessage>(); }

      void SetComponentType(const std::string& t) { mComponentType.Set(t); }
      std::string GetComponentType() const { return mComponentType.Get(); }

	  /**
		* Group of properties that will be applied to entity system
		*/
      void SetSystemProperties(PropertyGroup& v) { mProperties.Set(v); }
      const PropertyGroup& GetSystemProperties() const { return mProperties.Get(); }

   private:
      StringProperty mComponentType;
      GroupProperty mProperties;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * User can send this message to cause the map system to spawn an entity from the given spawner.
    * The contained unique id will be assigned to the spawned entity.
    */
   class DT_ENTITY_EXPORT SpawnEntityMessage
      : public Message
   {
   public:
      
      static const MessageType TYPE;
      static const StringId UniqueIdId;
      static const StringId EntityNameId;
      static const StringId SpawnerNameId;
      static const StringId AddToSceneId;
      SpawnEntityMessage();      
      
      virtual Message* Clone() const { return CloneContainer<SpawnEntityMessage>(); }

      void SetUniqueId(const std::string& v) { mUniqueId.Set(v); }
      std::string GetUniqueId() const { return mUniqueId.Get(); }

      std::string GetSpawnerName() const { return mSpawnerName.Get(); }
      void SetSpawnerName(const std::string& v) { mSpawnerName.Set(v); }

      std::string GetEntityName() const { return mEntityName.Get(); }
      void SetEntityName(const std::string& v) { mEntityName.Set(v); }

      bool GetAddToScene() const { return mAddToScene.Get(); }
      void SetAddToScene(bool v) { mAddToScene.Set(v); }

   private:

      StringProperty mUniqueId;
      StringProperty mEntityName;
      StringProperty mSpawnerName;
      BoolProperty mAddToScene;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /**
    * Send this to cause the tool system to switch to using the named tool
    */
   class DT_ENTITY_EXPORT ToolActivatedMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId ToolNameId;

      ToolActivatedMessage();

      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<ToolActivatedMessage>(); }

      void SetToolName(const std::string& n) { mToolName.Set(n); }
      std::string GetToolName() const { return mToolName.Get(); }

   private:

      StringProperty mToolName;
   };

   ////////////////////////////////////////////////////////////////////////////////
   /** send this when you have added or removed a tool from tool manager
     * mTools property should have data in this format:
     * mtools contains a number of group properties.
     * Each group property has entries
     * StringProperty "ToolName" - name of tool as registered in tool manager
     * StringProperty "IconPath" - path to icon to display for tool
     * StringProperty "Shortcut" - keyboard shortcut for tool, example: "Ctrl+t"
     */
   class DT_ENTITY_EXPORT ToolsUpdatedMessage
      : public Message
   {
   public:

      static const MessageType TYPE;
      static const StringId ToolsId;
      static const StringId ToolNameId;
      static const StringId IconPathId;
      static const StringId ShortcutId;

      ToolsUpdatedMessage();

      // Create a copy of this message on the heap
      virtual Message* Clone() const { return CloneContainer<ToolsUpdatedMessage>(); }

      void SetTools(const PropertyArray& n) { mTools.Set(n); }
      PropertyArray GetTools() const { return mTools.Get(); }

   private:

      ArrayProperty mTools;
   };
}
