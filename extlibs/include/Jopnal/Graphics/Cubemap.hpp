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

#ifndef JOP_CUBEMAP_HPP
#define JOP_CUBEMAP_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Graphics/Texture.hpp>

//////////////////////////////////////////////


namespace jop
{
    class JOP_API Cubemap final : public Texture
    {
    public:

        /// \brief Constructor
        ///
        /// \param name Name of this resource
        ///
        Cubemap(const std::string& name);


        /// \brief Load a cube map from files
        ///
        /// \param right Right side
        /// \param left Left side
        /// \param top Top side
        /// \param bottom Bottom side
        /// \param back Back side
        /// \param front Front side
        /// \param srgb Use SRGB color space?
        ///
        /// \return True if successful
        ///
        bool load(const std::string& right, const std::string& left,
                  const std::string& top, const std::string& bottom,
                  const std::string& back, const std::string& front,
                  const bool srgb);

        /// \brief Load an empty cube map
        ///
        /// \param size The size of a single face
        /// \param bpp The pixel depth in bytes
        /// \param srgb Use SRGB color space?
        ///
        /// \return True if successful
        ///
        bool load(const glm::uvec2& size, const unsigned int bpp, const bool srgb);

        /// \brief Get the texture size
        ///
        /// \return The size
        ///
        glm::uvec2 getSize() const override;


        /// \brief Get the error texture
        ///
        /// \return Reference to the texture
        ///
        static Cubemap& getError();

        /// \brief Get the default texture
        ///
        /// \return Reference to the texture
        ///
        static Cubemap& getDefault();

    private:

        glm::uvec2 m_size;              ///< Size
        unsigned int m_bytesPerPixel;   ///< Pixel depth in bytes
    };
}

#endif