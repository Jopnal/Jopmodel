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

#ifndef JOP_RECTANGLEMESH_HPP
#define JOP_RECTANGLEMESH_HPP

// Headers
#include <Jopnal/Jopnal.hpp>
#include <Jopnal/Graphics/Mesh.hpp>

//////////////////////////////////////////////


namespace jop
{
    class JOP_API RectangleMesh : public Mesh
    {
    public:

        /// \brief Default constructor
        ///
        /// Does not initialize the vertices.
        ///
        /// \param name Name of the rectangle
        ///
        RectangleMesh(const std::string& name);

        /// \brief Copy constructor
        ///
        /// \param other The other mesh to be copied
        /// \param newName Name of the new mesh
        ///
        RectangleMesh(const RectangleMesh& other, const std::string& newName);


        /// \brief Load this rectangle
        ///
        /// This will set up the vertices and create the buffers.
        ///
        /// \param size Size of the rectangle
        ///
        /// \return True if successful
        ///
        bool load(const float size);


        /// \brief Get the size
        ///
        /// \return The size
        ///
        float getSize() const;

    private:

        float m_size;   ///< This rectangle's size
    };
}
#endif

/// \class RectangleMesh
/// \ingroup Graphics
///
/// 