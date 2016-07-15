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

#ifndef JOP_SKYBOX_HPP
#define JOP_SKYBOX_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Graphics/Drawable.hpp>
#include <Jopnal/Graphics/SphereMesh.hpp>
#include <Jopnal/Graphics/Material.hpp>

//////////////////////////////////////////////


namespace jop
{
    class JOP_API SkySphere : public Drawable
    {
    private:

        /// \brief Copy constructor
        ///
        /// \param other The other sky sphere to copy
        /// \param newObj The new object
        ///
        SkySphere(const SkySphere& other, Object& newObj);

        JOP_DISALLOW_COPY_MOVE(SkySphere);
        JOP_GENERIC_COMPONENT_CLONE(SkySphere);

    public:

        /// \brief Constructor
        ///
        /// You should only ever need to change the default radius if you're using
        /// a non-default near clipping plane with your cameras.
        ///
        /// \param obj The object component is bound to
        /// \param renderer The renderer
        /// \param radius The sphere radius
        ///
        SkySphere(Object& obj, Renderer& renderer, const float radius = 2.f);


        /// \copydoc Drawable::draw(const Camera*,const LightContainer&,Shader&)
        ///
        void draw(const Camera* camera, const LightContainer& lights, Shader& shader) const override;

        /// \brief Set the map
        ///
        /// The map should use spherical texture coordinates.
        ///
        /// \param map Reference to the map
        ///
        void setMap(const Texture2D& map);

        /// \brief Get the bound map
        ///
        /// \return Pointer to the map. Nullptr if none is bound
        ///
        const Texture* getMap() const;

    private:

        SphereMesh m_mesh;      ///< The mesh
        Material m_material;    ///< The material
    };
}

#endif