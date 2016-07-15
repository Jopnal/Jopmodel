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

#ifndef JOP_SUBSYSTEM_HPP
#define JOP_SUBSYSTEM_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Utility/SafeReferenceable.hpp>
#include <Jopnal/Utility/Message.hpp>
#include <string>
#include <memory>
#include <atomic>

//////////////////////////////////////////////


namespace jop
{
    class JOP_API Subsystem
    {
    private:

        JOP_DISALLOW_COPY_MOVE(Subsystem);

    public:

        /// \brief Constructor
        ///
        /// \param ID Identifier of this subsystem
        ///
        Subsystem(const std::string& ID);

        /// \brief Virtual destructor
        ///
        virtual ~Subsystem() = 0;


        /// \brief Pre-update
        ///
        /// This will be called before the engine calls the scene's update.
        ///
        /// \param deltaTime Delta time
        ///
        virtual void preUpdate(const float deltaTime);

        /// \brief Post-update
        ///
        /// This will be called after the engine calls the scene's update.
        /// This function can also be used as a pre-draw function.
        ///
        /// \param deltaTime Delta time
        ///
        virtual void postUpdate(const float deltaTime);

        /// \brief Post-draw
        ///
        /// This will be called after the engine calls the scene's draw.
        ///
        virtual void draw();


        /// \brief Function to handle messages
        ///
        /// \param message String holding the message
        ///
        /// \return The message result
        ///
        Message::Result sendMessage(const std::string& message);

        /// \brief Function to handle messages
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


        /// \brief Set this sub system active
        ///
        /// \param active True to set active
        ///
        Subsystem& setActive(const bool active);

        /// \brief Check if this sub system is active
        ///
        /// \return True if active
        ///
        bool isActive() const;


        /// \brief Get the ID
        ///
        /// \return Reference to the ID string
        ///
        const std::string& getID() const;

    protected:

        /// \brief Virtual sendMessage
        ///
        /// \param message The message
        ///
        /// \return The message result
        ///
        virtual Message::Result sendMessageImpl(const Message& message);

    private:

        const std::string m_ID;     ///< This subsystem's name
        std::atomic<bool> m_active; ///< Sets activity 
    };
}

#endif

/// \class SubSystem
/// \ingroup core
///
/// 