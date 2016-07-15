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

#ifndef JOP_MESSAGE_HPP
#define JOP_MESSAGE_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Utility/Assert.hpp>
#include <Jopnal/Utility/Any.hpp>
#include <string>
#include <typeinfo>
#include <sstream>
#include <unordered_set>

//////////////////////////////////////////////


namespace jop
{
    class JOP_API Message
    {
    private:

        JOP_DISALLOW_COPY_MOVE(Message);

    public:

        /// System filtering bits
        ///
        enum Filter : unsigned short
        {
            Engine = 1,
            Subsystem = Engine << 1,
            SharedScene = Subsystem << 1,
            Scene = SharedScene << 1,
            Object = Scene << 1,
            Component = Object << 1,
            Command = Component << 1,
            Custom = Command << 1,
            Global = 0xFFFF
        };

        /// Message result
        ///
        enum class Result
        {
            Continue,   ///< Continue to process the message
            Escape      ///< Return from sendMessage
        };

    public:

        /// \brief Constructor
        /// 
        /// \param message The message. Can include just a filter or be empty,
        ///                             in which case you must push the command & arguments
        ///                             in a separate context.
        /// \param ptr The return value pointer
        ///
        Message(const std::string& message, Any& ptr);


        /// \brief Push an argument value
        ///
        /// \param arg The argument to push
        ///
        /// \return Reference to this
        ///
        template<typename T>
        Message& push(const T& arg);

        /// \brief Push a string argument
        ///
        /// This can be used to push the command, if the message passed to the constructor didn't have it.
        ///
        /// \param str The argument to push
        ///
        /// \return Reference to this
        ///
        Message& push(const std::string& str);

        /// \copydoc push(const std::string&)
        ///
        Message& push(const char* str);

        /// \brief Push a pointer argument
        ///
        /// The pointer will be converted into a hex string and then
        /// interpreted by the command handler to recreate the pointer.
        ///
        /// \param ptrArg The pointer to push
        ///
        /// \return Reference to self
        ///
        Message& pushPointer(const void* ptrArg);

        /// \brief Push a reference argument
        ///
        /// Essentially does the same as pushPointer().
        ///
        /// \param ref The reference to push
        ///
        /// \return Reference to self
        ///
        template<typename T>
        Message& pushReference(const T& ref);


        /// \brief Get a string with the command and arguments
        ///
        /// \return A new string with the internal buffer's contents
        ///
        const std::string& getString() const;


        /// \brief Check if the given bit field should pass the filter
        ///
        /// \param filter The bits to check. Only one match is required for a pass
        ///
        /// \return True if the filter was passed
        ///
        bool passFilter(const unsigned short filter) const;

        /// \brief Check if the given id should pass the filter
        ///
        /// \param id The id to check
        ///
        /// \return True if the filter was passed
        ///
        bool passFilter(const std::string& id) const;

        /// \brief Check if the given tags should pass the filter
        ///
        /// \param tags The tags to check
        ///
        /// \return True if the filter was passed
        ///
        bool passFilter(const std::unordered_set<std::string>& tags) const;

        /// \brief Set the filter
        ///
        /// \param filter A filter field in the correct form
        ///
        /// \return Reference to self
        ///
        Message& setFilter(const std::string& filter);

        /// \brief Get the wrapper containing the return value pointer
        ///
        /// \return Reference to the internal return value pointer
        ///
        Any& getReturnWrapper() const;

        /// \brief Check the internal command/argument buffer
        ///
        /// \return True if the buffer has at least a command
        ///
        operator bool() const;

    private:

        mutable std::ostringstream m_command;                                       ///< Buffer containing the command and arguments in string form
        mutable std::string m_commandStr;                                           ///< String with the command & arguments
        std::string m_idPattern;                                                    ///< The id filter to compare any passed ids against
        std::unordered_set<std::string> m_tags;                                     ///< Tags to compare against
        mutable Any& m_ptr;                                                         ///< Any object to store a possible return value
        unsigned short m_filterBits;                                                ///< Bit field with the system filter bits
        bool (*m_idMatchMethod)(const std::string&, const std::string&);            ///< Function to use in comparing the filter id and the passed id
        bool (*m_tagMatchMethod)(const decltype(m_tags)&, const decltype(m_tags)&); ///< Function to use in comparing the filter tags and the passed tags
    };

    // Include the template implementation file
    #include <Jopnal/Utility/Inl/Message.inl>
}

#endif

/// \class Resource
/// \ingroup utility