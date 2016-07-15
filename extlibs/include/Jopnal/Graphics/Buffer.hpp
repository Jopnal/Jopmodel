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

#ifndef JOP_BUFFER_HPP
#define JOP_BUFFER_HPP

// Headers
#include <Jopnal/Header.hpp>

//////////////////////////////////////////////


namespace jop
{
    class JOP_API Buffer
    {
    public:
        
        /// \brief Enum class consisting of buffer types
        ///
        enum class Type
        {
            ArrayBuffer,
            ElementArrayBuffer,
            TextureBuffer,
            TransformFeedBackBuffer,
            UniformBuffer
        };

        /// \brief Enum of usage types
        ///
        enum Usage
        {
            StaticDraw,
            DynamicDraw,
            StreamDraw
        };

    public:

        /// \brief Constructor 
        ///
        /// \param type Buffer type 
        /// \param usage Usage type
        ///
        Buffer(const Type type, const Usage usage);

        /// \brief Constructor to another buffer
        ///
        /// \param other Reference to buffer constructor 
        ///
        Buffer(const Buffer& other);

        /// \brief Overloads reference to buffer
        ///
        /// \param other Reference to buffer constructor
        ///
        Buffer& operator =(const Buffer& other);

        /// \brief Move constructor 
        ///
        /// \param other 
        ///
        Buffer(Buffer&& other);

        /// \brief Overloads move constructor
        ///
        /// \param other 
        ///
        Buffer& operator =(Buffer&& other);

        /// \brief Destructor
        ///
        virtual ~Buffer() = 0;


        /// \brief Generates and binds buffers by its type
        ///
        void bind() const;

        /// \brief Unbinds buffer by its type
        ///
        /// \param type Name of the type that is unbound
        ///
        static void unbind(const Type type);

        /// \brief Clears this buffer
        ///
        void clear();

        /// \brief Destroys this buffer
        ///
        void destroy();

        /// \brief Get the allocated size in bytes
        ///
        std::size_t getAllocatedSize() const;

    protected:
  
        std::size_t m_bytesAllocated;   ///< Size of the allocated buffer
        mutable unsigned int m_buffer;  ///< Buffer's OpenGL handle
        const int m_bufferType;         ///< Type of the buffer
        const int m_usage;              ///< The usage type
    };
}
#endif

/// \class Buffer
/// \ingroup Graphics
///
/// 
