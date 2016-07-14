// Jopnal Model C++ Application
// Copyright(c) 2016 Team Jopnal
// 
// This software is provided 'as-is', without any express or implied
// warranty.In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions :
// 
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software.If you use this software
// in a product, an acknowledgement in the product documentation would be
// appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

//////////////////////////////////////////////

//Headers
#include <jopmodel/Material.hpp>

//////////////////////////////////////////////

namespace jopm
{
    Material::Material() :
        m_reflections   (),
        m_shininess     (1.f),
        m_reflectivity  (0.f),
        m_keypairs      ()
    {
        for (int i = 0; i < sizeof(m_reflections) / sizeof(m_reflections[0]); ++i)
        {
            m_reflections[i] = 0.f;
        }
    }
    Material::~Material()
    {
    }

    void Material::pushReflections(Material &jopmat, const aiColor3D& col, const int& refTypeIndex)
    {
        jopmat.m_reflections[refTypeIndex * 4 + 0] = col.r;
        jopmat.m_reflections[refTypeIndex * 4 + 1] = col.g;
        jopmat.m_reflections[refTypeIndex * 4 + 2] = col.b;
        jopmat.m_reflections[refTypeIndex * 4 + 3] = 1.0;
    }
}