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

#ifndef JOPM_MESH_HPP
#define JOPM_MESH_HPP

//Headers
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

//////////////////////////////////////////////

namespace jopm
{
    class Mesh
    {
        friend class Converter;

    public:
        Mesh();
        ~Mesh();

    private:
        uint32_t m_vertexComponents;
        uint32_t m_matIndex;
        unsigned int m_type; //triangles, lines etc.
        uint32_t m_components; //bitfield
        unsigned int m_meshStart;
        unsigned int m_meshLength; //bytes
        unsigned int m_meshStartIndex;
        unsigned int m_meshLengthIndex;
        unsigned int m_meshSizeIndex;
        unsigned int m_vertexSize;

        std::vector<unsigned char> m_vertexBuffer;
        std::vector<unsigned char> m_indexBuffer;
        std::pair<glm::vec3, glm::vec3> m_localBB;
    };
}
#endif