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

#ifndef JOP_SCENE_HPP
#define JOP_SCENE_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Core/Object.hpp>
#include <Jopnal/Utility/Message.hpp>
#include <vector>
#include <memory>

//////////////////////////////////////////////


namespace jop
{
    class Renderer;
    class World;
    class Window;
    class RenderTarget;

    class JOP_API Scene : private Object
    {
    public:

        using Object::createComponent;
        using Object::getComponent;
        using Object::getComponents;
        using Object::removeComponents;
        using Object::clearComponents;
        using Object::componentCount;

        using Object::createChild;
        using Object::getChildren;
        using Object::cloneChild;
        using Object::removeChildren;
        using Object::clearChildren;
        using Object::childCount;
        using Object::childCountRecursive;
        using Object::findChild;
        using Object::findChildWithPath;
        using Object::findChildren;
        using Object::findChildrenWithTag;

        using Object::adoptChild;
        using Object::setParent;

        using Object::isActive;
        using Object::setActive;

        using Object::getID;
        using Object::setID;

    public:

        JOP_DISALLOW_COPY_MOVE(Scene);

        friend class StateLoader;
        friend class Object;

    public:

        /// \brief Constructor
        ///
        /// \param ID Scene identifier
        ///
        Scene(const std::string& ID);

        /// \brief Virtual destructor
        ///
        virtual ~Scene();


        /// \brief Set a new renderer
        ///
        /// \param args Arguments to pass into the renderer's constructor
        ///
        /// \return Reference to the newly constructed renderer
        ///
        template<typename T, typename ... Args>
        T& setRenderer(Args&&... args);

        /// \brief Get the renderer
        ///
        /// \return Reference to the renderer
        ///
        Renderer& getRenderer() const;


        /// \brief Get the physics world
        ///
        /// \return Reference to the world
        ///
        World& getWorld() const;

        
        /// \brief Set the delta time scalar
        ///
        /// The delta time value will be multiplied by this value every frame.
        /// 
        /// \param scale The scale to set
        ///
        void setDeltaScale(const float scale);

        /// \brief Get the delta time scalar
        ///
        /// \return The delta time scalar
        ///
        float getDeltaScale() const;


        /// \brief Base sendMessage function
        ///
        /// This will handle message filtering and forwarding
        /// to the objects
        ///
        /// \param message String holding message
        ///
        /// \return The message result
        ///
        Message::Result sendMessage(const std::string& message);

        /// \brief Base sendMessage function
        ///
        /// This will handle message filtering and forwarding
        /// to the objects
        ///
        /// \param message String holding message
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


        /// \brief Update method for scene
        ///
        /// \param deltaTime The delta time
        ///
        void updateBase(const float deltaTime);

        /// \brief Method for drawing
        ///
        void drawBase();
        

        /// \brief Method for pre-updating
        ///
        /// This will be called before objects are updated.
        ///
        /// \param deltaTime The delta time
        ///
        virtual void preUpdate(const float deltaTime);

        /// \brief Method for post-updating
        ///
        /// This will be called after objects are updated.
        ///
        /// \param deltaTime The delta time
        ///
        virtual void postUpdate(const float deltaTime);
        
        /// \brief Method for pre-drawing
        ///
        /// This will be called before objects are drawn.
        ///
        virtual void preDraw();

        /// \brief Method for post drawing
        ///
        /// This will be called after objects are drawn.
        ///
        virtual void postDraw();


        /// \brief Get this scene as object
        ///
        /// This can be used to work around the privately inherited jop::Object.
        /// <b>Do not</b> ever call this unless you know what you're doing.
        ///
        /// \return Reference to this as Object
        ///
        Object& getAsObject();

        /// \copydoc getAsObject()
        ///
        const Object& getAsObject() const;

    private:

        /// \brief Virtual sendMessage
        ///
        virtual Message::Result sendMessageImpl(const Message& message);


        std::unique_ptr<Renderer> m_renderer;   ///< The renderer
        mutable World& m_world;                 ///< The physics world
        float m_deltaScale;                     ///< Delta time scalar
    };

    // Include the template implementation file
    #include <Jopnal/Core/Inl/Scene.inl>
}

#endif

/// \class Scene
/// \ingroup core