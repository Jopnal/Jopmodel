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
#include <jopmodel/Mesh.hpp>

//////////////////////////////////////////////

namespace jopm
{
    Mesh::Mesh() :
        m_vertexComponents  (0),
        m_matIndex          (0),
        m_type              (0),
        m_components        (0),
        m_meshStart         (0),
        m_meshLength        (0),
        m_meshStartIndex    (0),
        m_meshLengthIndex   (0),
        m_meshSizeIndex     (0),
        m_vertexSize        (0),
        m_vertexBuffer      (),
        m_indexBuffer       (),
        m_localBB           ()
    {
        m_localBB = std::make_pair(glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX), glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX));
    }

    Mesh::~Mesh()
    {
    }
}
