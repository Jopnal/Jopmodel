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

#ifndef JOP_MATERIAL_HPP
#define JOP_MATERIAL_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Graphics/Color.hpp>
#include <Jopnal/Core/Resource.hpp>
#include <memory>
#include <array>

//////////////////////////////////////////////


namespace jop
{
	class Camera;
    class Shader;
    class Texture;


    class JOP_API Material final : public Resource
    {
    public:

        JOP_DISALLOW_COPY_MOVE(Material);

        Material(const Material& other, const std::string& newName);

    public:

        /// Attribute data type
        ///
        typedef uint32 AttribType;

        /// Bit values to describe a material's attributes
        ///
        struct Attribute
        {
            enum : AttribType
            {
                // Misc
                AmbientConstant = 1,
                DiffuseAlpha    = AmbientConstant   << 1,
                VertexColor     = DiffuseAlpha      << 1, 

                // Maps
                DiffuseMap      = 1                 << 4,
                SpecularMap     = DiffuseMap        << 1,
                EmissionMap     = SpecularMap       << 1,
                EnvironmentMap  = EmissionMap       << 1,
                ReflectionMap   = EnvironmentMap    << 1,
                OpacityMap      = ReflectionMap     << 1,
                GlossMap        = OpacityMap        << 1,
                //NormalMap       = 1 << 7, // TODO: Implement
                //ParallaxMap     = 1 << 8, // TODO: Implement
                //AmbientMap    = 1 << 10, // TODO: Implement
                //LightMap      = 1 << 11, // TODO: Implement

                // Lighting models
                Phong           = 1 << 18,
                BlinnPhong      = Phong | Phong << 1,   //
                Gouraud         = Phong,                // TODO: Implement these, requires shader work
                Flat            = Phong,                //

                // Bundles
                Default         = DiffuseMap,
                DefaultLighting = AmbientConstant | BlinnPhong,

                // For internal functionality, do not use
                __SkySphere     = 1  << 29,
                __SkyBox        = 1  << 30,
                __RecordEnv     = 1u << 31,
                __Lighting      = Phong | BlinnPhong | Gouraud | Flat
            };
        };

        /// The reflection attribute
        ///
        enum class Reflection
        {
            Ambient,
            Diffuse,
            Specular,
            Emission,

            Solid = Emission
        };

        /// The map attribute
        ///
        enum class Map
        {
            Diffuse = 1,
            Specular,
            Emission,
            Environment,
            Reflection,
            //Normal
            //Parallax
            Opacity,
            Gloss,
            
            /// For internal use. Never use this
            Last
        };

        /// Predefined material properties
        ///
        enum class Preset
        {
            Emerald,
            Jade,
            Obsidian,
            Pearl,
            Ruby,
            Turquoise,
            Brass,
            Bronze,
            Chrome,
            Copper,
            Gold,
            Silver,
            BlackPlastic,
            CyanPlastic,
            GreenPlastic,
            RedPlastic,
            WhitePlastic,
            YellowPlastic,
            BlackRubber,
            CyanRubber,
            GreenRubber,
            RedRubber,
            WhiteRubber,
            YellowRubber
        };

    public:

        /// \brief Default constructor
        ///
        /// \param name Name of this material
        /// \param autoAttributes Set attributes automatically?
        ///
        Material(const std::string& name, const bool autoAttributes = true);

        /// \brief Overloaded constructor for initializing with attributes
        ///
        /// \param name Name of this material
        /// \param attributes The initial attributes
        /// \param autoAttributes Set attributes automatically
        ///
        Material(const std::string& name, const AttribType attributes, const bool autoAttributes = true);
        

        /// \brief Send this material to a shader
        ///
        /// \param shader Reference to the shader to send this material to
        /// \param camera The camera to use
        ///
        void sendToShader(Shader& shader, const Camera* camera) const;

        /// \brief Get the shader
        ///
        /// \return Pointer to the shader
        ///
        Shader* getShader() const;


        /// \brief Set a reflection value
        ///
        /// \param reflection The reflection attribute
        /// \param color The reflection color
        ///
        /// \return Reference to self
        ///
        Material& setReflection(const Reflection reflection, const Color color);

        /// \brief Set the reflection values
        ///
        /// \param ambient The ambient reflection
        /// \param diffuse The diffuse reflection
        /// \param specular The specular reflection
        /// \param emission The emission value
        ///
        /// \return Reference to self
        ///
        Material& setReflection(const Color ambient, const Color diffuse, const Color specular, const Color emission);

        /// \brief Set the reflection values using a preset
        ///
        /// \param preset The preset to use
        ///
        /// \return Reference to self
        ///
        Material& setReflection(const Preset preset);
   
        /// \brief Get a reflection value
        ///
        /// \param reflection The reflection attribute
        ///
        /// \return The reflection value
        ///
        Color getReflection(const Reflection reflection) const;

        /// \brief Set the shininess value
        ///
        /// \param value The shininess value
        ///
        /// \return Reference to self
        ///
        Material& setShininess(const float value);

        /// \brief Get the shininess value
        ///
        /// \return The shininess value
        ///
        float getShininess() const;


        /// \brief Set the reflectivity
        ///
        /// Do not confuse this with reflection. What this defines is how
        /// strongly an environment map is reflected. 0 means there's no
        /// reflection and 1 makes the surface mirror-like. This attribute
        /// has no effect if there's no environment map
        ///
        /// \param reflectivity The reflectivity value
        ///
        /// \return Reference to self
        ///
        Material& setReflectivity(const float reflectivity);

        /// \brief Get the reflectivity value
        ///
        /// \return The reflectivity value
        ///
        float getReflectivity() const;


        /// \brief Set a map
        ///
        /// \param map The map attribute
        /// \param tex Reference to the texture
        ///
        /// \return Reference to self
        ///
        Material& setMap(const Map map, const Texture& tex);

        /// \brief Remove a map
        ///
        /// \param map The map to remove
        ///
        /// \return Reference to self
        ///
        Material& removeMap(const Map map);

        /// \brief Get a map
        ///
        /// \param map The map attribute
        ///
        /// \return Pointer to the texture. Nullptr if none bound
        ///
        const Texture* getMap(const Map map) const;


        /// \brief Set the attribute bit field
        ///
        /// \param attribs The attributes to set
        ///
        /// \return Reference to self
        ///
        Material& setAttributeField(const AttribType attribs);

        /// \brief Get the attribute bit field
        ///
        /// \return The attribute bit field
        ///
        AttribType getAttributeField() const;

        /// \brief Check if this material has a specific attribute
        ///
        /// \param attrib The attribute to check
        ///
        /// \return True if does have the attribute
        ///
        bool hasAttribute(const AttribType attrib) const;

        bool hasAttributes(const AttribType attribs) const;

        bool compareAttributes(const AttribType attribs) const;

        Material& addAttributes(const AttribType attribs);

        Material& removeAttributes(const AttribType attribs);


        /// \brief Get the default material
        ///
        /// \return Reference to the default material
        ///
        static Material& getDefault();

    private:

        std::array<Color, 4> m_reflection;  ///< The reflection values
        float m_reflectivity;               ///< The reflectivity value
        AttribType m_attributes;            ///< The attribute bit field
        float m_shininess;                  ///< The shininess factor
        std::array
        <
            WeakReference<const Texture>,
            static_cast<int>(Map::Last) - 1
        > m_maps;                               ///< An array with the bound maps
        mutable WeakReference<Shader> m_shader; ///< Shader fitting the attributes
        mutable bool m_attributesChanged;       ///< Have the attributes been changed?
        bool m_autoAttribs;                     ///< Use automatic attributes?
    };
}

#endif