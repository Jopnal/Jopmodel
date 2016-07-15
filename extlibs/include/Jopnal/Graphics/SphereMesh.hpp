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
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

//////////////////////////////////////////////

#ifndef JOP_SPHEREMESH_HPP
#define JOP_SPHEREMESH_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Graphics/Mesh.hpp>

//////////////////////////////////////////////


namespace jop
{
    class JOP_API SphereMesh : public Mesh
    {
    public:

        /// \copydoc jop::BoxMesh::BoxMesh()
        ///
        SphereMesh(const std::string& name);

        /// \brief Copy constructor
        ///
        /// \param other The other mesh to be copied
        /// \param newName Name of the new mesh
        ///
        SphereMesh(const SphereMesh& other, const std::string& newName);


        /// \brief Load a sphere
        ///
        /// \param radius The radius of the sphere
        /// \param rings How many rings will the sphere have
        /// \param sectors How many sectors the circle will get divided to
        /// \param normalizedTexCoords Normalize the texture coordinates?
        ///
        /// \return True if successful
        ///
        bool load(const float radius, const unsigned int rings, const unsigned int sectors, const bool normalizedTexCoords = true);


        /// \brief Get the radius
        ///
        /// \return The radius
        ///
        float getRadius() const;

        /// \brief Get the amount of rings
        ///
        /// \return The ring amount
        ///
        unsigned int getRings() const;

        /// \brief Get the amount of sectors
        ///
        /// \return The sector amount
        ///
        unsigned int getSectors() const;

        /// \brief Check if this sphere uses normalized texture coordinates
        ///
        /// \return True if normalized
        ///
        bool normalizedTexCoords() const;

    private:

        float m_radius;         ///< The radius
        unsigned int m_rings;   ///< Rings
        unsigned int m_sectors; ///< Sectors
        bool m_normTexCoords;   ///< Normalized texture coordinates
    };
}

#endif

/// \class SphereMesh
/// \ingroup Graphics
///
/// 