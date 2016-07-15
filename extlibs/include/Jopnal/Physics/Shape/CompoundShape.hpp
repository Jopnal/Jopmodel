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

#ifndef JOP_COMPOUNDSHAPE_HPP
#define JOP_COMPOUNDSHAPE_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Physics/Shape/CollisionShape.hpp>

//////////////////////////////////////////////


namespace jop
{
    class Transform;

    class JOP_API CompoundShape final : public CollisionShape
    {
    public:

        /// \brief Constructor
        ///
        /// \param name Name of the resource
        ///
        CompoundShape(const std::string& name);


        /// \brief Add a child shape
        ///
        /// \param childShape Reference to a valid shape
        /// \param childTransform Local transform for the child
        ///
        void addChild(CollisionShape& childShape, const Transform::Variables& childTransform);
    };
}

#endif