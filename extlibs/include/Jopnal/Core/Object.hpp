// Jopnal Engine C++ Library
// Copyright (c) 2016 Team Jopnal
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgement in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

//////////////////////////////////////////////

#ifndef JOP_OBJECT_HPP
#define JOP_OBJECT_HPP

//Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Utility/SafeReferenceable.hpp>
#include <Jopnal/Graphics/Transform.hpp>
#include <unordered_set>

//////////////////////////////////////////////


namespace jop
{
    class JOP_API Object : public SafeReferenceable<Object>
    {
    private:

        JOP_DISALLOW_COPY(Object);

        friend class StateLoader;
        friend class Component;

        enum : uint16
        {
            ActiveFlag          = 1,
            RemoveFlag          = 1 << 1,
            ChildrenRemovedFlag = 1 << 2,

            // Transformations
            IgnoreParent        = 1 << 3,
            MatrixDirty         = 1 << 4,
            InverseMatrixDirty  = 1 << 5,

            GlobalRotationDirty = 1 << 6,
            GlobalScaleDirty    = 1 << 7,
            GlobalPositionDirty = 1 << 8
        };

    public:

        /// \brief Constructor
        ///
        /// \param ID Object identifier
        ///
        Object(const std::string& ID);

        /// \brief Copy constructor
        /// 
        /// \param other The other object to copy
        /// \param newID The ID of the new object
        /// \param newTransform The transform of the new object
        ///
        Object(const Object& other, const std::string& newID, const Transform::Variables& newTransform);

        /// \brief Move constructor
        ///
        /// \param other The other object to move
        ///
        Object(Object&& other);

        /// \brief Move assignment operator
        ///
        /// \param other The other object to move
        ///
        /// \return Reference to self
        ///
        Object& operator =(Object&& other);

        /// \brief Destructor
        ///
        ~Object();


        /// \brief Get a component with the given id
        ///
        /// \param ID Component identifier to search with
        ///
        /// \return Pointer to the component, nullptr if the component wasn't found
        ///
        Component* getComponent(const std::string& ID);

        /// \copydoc getComponent()
        ///
        const Component* getComponent(const std::string& ID) const;

        
        /// \brief Get all components
        ///
        /// \return Reference to the internal vector with the components
        ///
        const std::vector<std::unique_ptr<Component>>& getComponents() const;

        /// \brief Get a component using type info
        ///
        /// \return Pointer to the component. Nullptr if not found
        ///
        template<typename T>
        T* getComponent();

        /// \copydoc getComponent()
        ///
        template<typename T>
        const T* getComponent() const;

        /// \brief Get a component using component ID
        ///
        /// \return Pointer to the component. Nullptr if not found
        ///
        template<typename T>
        T* getComponent(const std::string& ID);

        /// \copydoc getComponent()
        ///
        template<typename T>
        const T* getComponent(const std::string& ID) const;

        /// \brief Template function to create components
        ///
        /// \param args Arguments to use with construction
        ///
        template<typename T, typename ... Args>
        T& createComponent(Args&&... args);

        /// \brief Template function to clone a component
        ///
        /// \param object New object for the cloned component
        /// \param ID Identifier of the component to clone
        ///
        /// \return Pointer to the cloned component. Nullptr if not found
        ///
        Component* cloneComponent(Object& object, const std::string& ID) const;

        /// \brief Template function to duplicates component
        ///
        /// The same object will be used.
        ///
        /// \param ID Identifier of the component
        /// \param newID Identifier of the cloned component
        ///
        /// \return Pointer to the cloned component. Nullptr if not found
        ///
        Component* cloneComponent(const std::string& ID, const std::string& newID);

        /// \brief Remove components
        ///
        /// All components matching the identifier will be removed
        /// 
        /// \param ID The ID to search with
        ///
        /// \comm removeComponents
        ///
        /// \return Reference to self
        /// 
        Object& removeComponents(const std::string& ID);

        /// \brief Remove all components
        ///
        /// \comm clearComponents
        ///
        /// \return Reference to self
        /// 
        Object& clearComponents();

        /// \brief Get amount of components
        ///
        /// \return Amount of components
        ///
        unsigned int componentCount() const;


        /// \brief Create a new child
        ///
        /// \comm createChild
        ///
        /// \param ID Id for the new object
        ///
        /// \return Reference to the newly created child
        ///
        WeakReference<Object> createChild(const std::string& ID);

        /// \brief Adopt a child
        ///
        /// This will move the child onto this object and remove it from its old parent.
        /// If the child's parent is equal to this, this function does nothing.
        ///
        /// \comm adoptChild
        ///
        /// \param child The child to adopt
        ///
        /// \return Reference to the adopted child. The old reference will become invalid
        ///
        WeakReference<Object> adoptChild(Object& child);

        /// \brief Get all children
        ///
        /// \return Reference to the internal vector with the components
        ///
        const std::vector<Object>& getChildren() const;

        /// \brief Clone a child with the given id
        ///
        /// The child object, if successfully cloned, will be added to the internal array
        /// and then returned.
        ///
        /// \param ID The id to search with
        /// \param clonedID The id of the cloned object
        ///
        /// \return Pointer to the newly cloned child object if the object was found, nullptr otherwise
        ///
        /// \comm cloneChild
        /// 
        WeakReference<Object> cloneChild(const std::string& ID, const std::string& clonedID);

        /// \copydoc cloneChild(const std::string&, const std::string&)
        ///
        /// \param newTransform New transform for the cloned child
        /// 
        WeakReference<Object> cloneChild(const std::string& ID, const std::string& clonedID, const Transform::Variables& newTransform);


        /// \brief Remove children with the given id
        ///
        /// The children will actually be removed only at the beginning of the next update call.
        ///
        /// \param ID The id to search with
        ///
        /// \return Reference to self
        ///
        /// \comm removeChildren
        /// 
        Object& removeChildren(const std::string& ID);

        /// \brief Remove all children with the given tag
        ///
        /// \copydetails removeChildren
        ///
        /// \param tag The tag to search with
        /// \param recursive Search recursively?
        ///
        /// \return Reference to self
        ///
        Object& removeChildrenWithTag(const std::string& tag, const bool recursive);

        /// \brief Remove all children
        ///
        /// \comm clearChild
        /// 
        /// the children will be removed immediately.
        ///
        /// \return Reference to self
        ///
        Object& clearChildren();

        /// \brief Mark this to be removed
        ///
        /// The object will be actually removed at the beginning of the next update call.
        ///
        /// \comm removeSelf
        ///
        void removeSelf();

        /// \brief Clone/instantiate this object
        ///
        /// The same ID and transform will be used.
        ///
        /// \return Reference to the cloned object
        ///
        WeakReference<Object> cloneSelf();

        /// \copybrief cloneSelf()
        ///
        /// The same transform will be used.
        ///
        /// \param newID The ID of the cloned object
        ///
        /// \return Reference to the cloned object
        /// 
        WeakReference<Object> cloneSelf(const std::string& newID);

        /// \copybrief cloneSelf()
        ///
        /// \param newID The ID of the cloned object
        /// \param newTransform The transform of the cloned object
        ///
        /// \return Reference to the cloned object
        ///
        WeakReference<Object> cloneSelf(const std::string& newID, const Transform::Variables& newTransform);

        /// \brief Check if this object has been marked to be removed
        ///
        /// \return True if this is to be removed
        ///
        bool isRemoved() const;

        /// \brief Get amount of children
        ///
        /// \return Amount of children
        ///
        unsigned int childCount() const;

        /// \brief Get amount of children recursively
        ///
        /// Goes through the children and their children all the way down the tree
        /// and return the total amount of children
        ///
        /// \return Amount of children, summed recursively
        ///
        unsigned int childCountRecursive() const;


        /// \brief Get this object's parent
        ///
        /// If the result is empty, it means that this is a scene.
        ///
        /// \return Reference to the parent
        ///
        WeakReference<Object> getParent() const;

        /// \brief Set a new parent
        ///
        /// This is equivalent to calling newParent.adoptChild(this)
        ///
        /// \comm setParent
        ///
        /// \param newParent The new parent
        ///
        /// \return Reference to this. The old reference will become invalid
        ///
        WeakReference<Object> setParent(Object& newParent);


        /// \brief Get the scene this objects is bound to
        ///
        /// You should avoid calling this when you can use jop::Engine::getCurrentScene
        /// or jop::Engine::getSharedScene.
        ///
        /// \return Reference to the scene
        ///
        Scene& getScene();

        /// \copydoc getScene
        ///
        const Scene& getScene() const;


        /// \brief Method to send messages
        ///
        /// Forwards messages to this object's components
        ///
        /// \param message String holding the message
        ///
        /// \return The message result
        ///
        Message::Result sendMessage(const std::string& message);

        /// \brief Method to send messages
        ///
        /// Forwards messages to this object's components
        ///
        /// \param message String holding the message
        /// \param returnWrap Pointer to hold extra data
        ///
        /// \return The message result
        ///
        Message::Result sendMessage(const std::string& message, Any& returnWrap);

        /// \brief Function to handle messages
        ///
        /// \param message The message
        ///
        /// \return The message result
        ///
        Message::Result sendMessage(const Message& message);

        /// \brief Function to find child returns weak reference of the child object
        ///
        /// \param ID Unique object identifier
        /// \param recursive Tells if search if recursive
        /// \param strict Tells if search is strict, meaning the ID has to match exactly
        ///
        /// \return Objects child reference, empty if not found
        ///
        WeakReference<Object> findChild(const std::string& ID, const bool recursive = false, const bool strict = true) const;

        /// \brief Function to find all child objects 
        /// 
        /// \param ID Unique object identifier
        /// \param recursive Tells if search is recursive
        /// \param strict Tells if search is strict, meaning the ID has to match exactly
        ///
        /// \return Vector consisting all objects children, empty if none were found
        ///
        std::vector<WeakReference<Object>> findChildren(const std::string &ID, const bool recursive, const bool strict) const;

        /// \brief Finds children by given tag
        ///
        /// \param tag Object identifier
        /// \param recursive Tells if search is recursive
        ///
        /// \return vector consisting objects children, empty if none were found
        ///
        std::vector<WeakReference<Object>> findChildrenWithTag(const std::string& tag, const bool recursive) const;

        /// \brief Find child via path
        /// 
        /// \param path String that includes multiple ID:s 
        ///
        /// \return Weak reference to child, empty if not found
        //
        WeakReference<Object> findChildWithPath(const std::string& path) const;

        /// \brief Makes path to children including all parents
        ///
        /// \return String with the path
        ///
        /// \see findChildWithPath
        ///
        std::string makeSearchPath() const;

        /// \brief Set this object active/inactive
        ///
        /// \param active Activity flag to set
        ///
        /// \return Reference to self
        ///
        /// \comm setActive
        /// 
        Object& setActive(const bool active);

        /// \brief Check if this object is active
        ///
        /// \return True if active
        ///
        bool isActive() const;


        /// \brief Get this object's id
        ///
        /// \return Reference to the internal id string
        ///
        const std::string& getID() const;

        /// \brief method for setting the ID
        ///
        /// \param ID unique object identifier
        ///
        /// \return Reference to self
        ///
        /// \comm setID
        /// 
        Object& setID(const std::string& ID);


        /// \brief Add a tag
        ///
        /// \comm addTag
        ///
        /// \param tag Name of the added tag
        ///
        /// \return Reference to self
        ///
        Object& addTag(const std::string& tag);

        /// \brief Remove a tag
        ///
        /// \comm removeTag
        ///
        /// \param tag Name of the tag to be removed
        ///
        /// \return Reference to self
        ///
        Object& removeTag(const std::string& tag);

        /// \brief Clear tags
        ///
        /// \comm clearTag
        ///
        /// \return Reference to self
        ///
        Object& clearTags();

        /// \brief Finds out if object has tag name tag
        ///
        /// \param tag The tag to check
        ///
        /// \return True if found
        ///
        bool hasTag(const std::string& tag) const;


        /// \brief Update method for object - forwarded for its components
        ///
        /// \param deltaTime Double holding delta time
        ///
        void update(const float deltaTime);



        ////////////////////////////////////////////////////////////////////////////////
        // Transformations
        ////////////////////////////////////////////////////////////////////////////////

    public:

        /// \brief Get the transform
        ///
        /// \return Reference to the internal transform
        ///
        const Transform& getTransform() const;

        /// \brief Get the inverse transform
        ///
        /// \return Reference to the internal inverse transform
        ///
        const Transform& getInverseTransform() const;


        /// \brief Get the local transformation variables
        ///
        /// \return Reference to the internal variables
        ///
        const Transform::Variables& getLocalTransformVars() const;

        /// \brief Get the global transformation variables
        ///
        /// \return Reference to the internal variables
        ///
        const Transform::Variables& getGlobalTransformVars() const;


        /// \brief Set the rotation
        ///
        /// This version uses euler angles
        ///
        /// \param x The X angle
        /// \param y The Y angle
        /// \param z The Z angle
        ///
        /// \return Reference to self
        ///
        Object& setRotation(const float x, const float y, const float z);

        /// \brief Set the rotation
        ///
        /// \param rotation Rotation vector in euler angles
        ///
        /// \return Reference to self
        ///
        Object& setRotation(const glm::vec3& rotation);

        /// \brief Set the rotation using angle-axis
        ///
        /// \param angle The angle
        /// \param axis The axis
        ///
        /// \return Reference to self
        ///
        Object& setRotation(const float angle, const glm::vec3& axis);

        /// \brief Set the rotation
        ///
        /// \param rotation Quaternion with the rotation to set
        ///
        /// \return Reference to self
        ///
        Object& setRotation(const glm::quat& rotation);

        /// \brief Get the local rotation
        ///
        /// \return Quaternion with the rotation
        ///
        const glm::quat& getLocalRotation() const;

        /// \brief Get the global rotation
        ///
        /// \return The global rotation
        ///
        const glm::quat& getGlobalRotation() const;


        /// \brief Get the global front vector
        ///
        /// \return The global front vector
        ///
        glm::vec3 getGlobalFront() const;

        /// \brief Get the global right vector
        ///
        /// \return The global right vector
        ///
        glm::vec3 getGlobalRight() const;

        /// \brief Get the global right vector
        ///
        /// \return The global up vector
        ///
        glm::vec3 getGlobalUp() const;

        /// \brief Get the local front vector
        ///
        /// \return The local front vector
        ///
        glm::vec3 getLocalFront() const;

        /// \brief Get the local right vector
        ///
        /// \return The local right vector
        ///
        glm::vec3 getLocalRight() const;

        /// \brief Get the local up vector
        ///
        /// \return The local up vector
        ///
        glm::vec3 getLocalUp() const;


        /// \brief Set the scale
        ///
        /// 1.f means the original scale
        ///
        /// \param x The X component
        /// \param y The Y component
        /// \param z The Z component
        ///
        /// \return Reference to self
        ///
        Object& setScale(const float x, const float y, const float z);

        /// \brief Set the scale
        ///
        /// \param scale Vector with the scale to set
        ///
        /// \return Reference to self
        ///
        Object& setScale(const glm::vec3& scale);

        /// \brief Set the scale
        ///
        /// This call is equal to setScale(delta, delta, delta)
        ///
        /// \param delta The new scale
        ///
        /// \return Reference to self
        ///
        Object& setScale(const float delta);

        /// \brief Get the local scale
        ///
        /// \return Vector with the scale
        ///
        const glm::vec3& getLocalScale() const;

        /// \brief Get the global scale
        ///
        /// \return The global scale
        ///
        const glm::vec3& getGlobalScale() const;


        /// \brief Set the position
        ///
        /// \param x The X component
        /// \param y The Y component
        /// \param z The Z component
        ///
        /// \return Reference to self
        ///
        Object& setPosition(const float x, const float y, const float z);

        /// \brief Set the position
        ///
        /// \param position Vector with the position to set
        ///
        /// \return Reference to self
        ///
        Object& setPosition(const glm::vec3& position);

        /// \brief Get the local position
        ///
        /// \return Vector with the position
        ///
        const glm::vec3& getLocalPosition() const;

        /// \brief Get the global position
        ///
        /// \return The global position
        ///
        const glm::vec3& getGlobalPosition() const;


        /// \brief Set this transform to look at a certain point
        ///
        /// The rotation is applied locally.
        ///
        /// \param point The point to look at
        ///
        /// \return Reference to self
        ///
        Object& lookAt(const glm::vec3& point);

        /// \copydoc lookAt
        ///
        /// \param up A custom up vector
        ///
        Object& lookAt(const glm::vec3& point, const glm::vec3& up);

        /// \brief Set this transform to look at a certain point
        ///
        /// \param x The X point
        /// \param y The Y point
        /// \param z The Z point
        ///
        /// \return Reference to self
        ///
        Object& lookAt(const float x, const float y, const float z);


        /// \brief Move this object
        ///
        /// \param x The X component
        /// \param y The Y component
        /// \param z The Z component
        ///
        /// \returns Reference to self
        ///
        Object& move(const float x, const float y, const float z);

        /// \brief Move this object
        ///
        /// \param offset The movement offset
        ///
        /// \return Reference to self
        ///
        Object& move(const glm::vec3& offset);


        /// \brief Rotate this object
        ///
        /// \param x The X component
        /// \param y The Y component
        /// \param z The Z component
        ///
        /// \returns Reference to self
        ///
        Object& rotate(const float x, const float y, const float z);

        /// \brief Rotate this object
        ///
        /// \param rotation The rotation offset
        ///
        /// \return Reference to self
        ///
        Object& rotate(const glm::quat& rotation);

        /// \copydoc rotate(const glm::quat&)
        ///
        Object& rotate(const glm::vec3& rotation);

        /// \brief Rotate this object using an axis-angle
        ///
        /// \param angle The angle
        /// \param axis The axis
        /// 
        /// \return Reference to self
        ///
        Object& rotate(const float angle, const glm::vec3& axis);


        /// \brief Scale this object
        ///
        /// \param x The X component
        /// \param y The Y component
        /// \param z The Z component
        ///
        /// \returns Reference to self
        ///
        Object& scale(const float x, const float y, const float z);

        /// \brief Scale this object
        ///
        /// \param scale The scale multiplier
        ///
        /// \return Reference to self
        ///
        Object& scale(const glm::vec3& scale);

        /// \brief Scale this object
        ///
        /// This call is equal to scale(delta, delta, delta)
        ///
        /// \param delta The scale modifier
        ///
        /// \return Reference to self
        ///
        Object& scale(const float delta);


        /// \brief Set this node to ignore its parent
        ///
        /// Nodes that ignore their parent will not take
        /// into account the parent's transformation.
        ///
        /// \param ignore The flag to set
        ///
        /// \return Reference to self
        ///
        Object& setIgnoreParent(const bool ignore);

        /// \brief Check if this node ignores its parent
        ///
        /// \return True if ignores parent
        ///
        bool ignoresParent() const;

    private:

        void sweepRemoved();

        bool flagSet(const uint16 flag) const;

        void setFlags(const uint16 flags) const;

        void clearFlags(const uint16 flags) const;

        void propagateFlags(const uint16 flags);


        // Transformation
        mutable Transform m_transform;
        mutable Transform m_inverseTransform;
        mutable Transform::Variables m_locals;
        mutable Transform::Variables m_globals;

        // Object
        std::vector<Object> m_children;                         ///< Container holding this object's children
        std::vector<std::unique_ptr<Component>> m_components;   ///< Container holding components
        std::unordered_set<std::string> m_tags;                 ///< Container holding tags
        std::string m_ID;                                       ///< Unique object identifier
        WeakReference<Object> m_parent;                         ///< The parent
        mutable uint16 m_flags;                                 ///< Flags
    };

    // Include the template implementation file
    #include <Jopnal/Core/Inl/Object.inl>
}

#endif

/// \class Object 
/// \ingroup graphics