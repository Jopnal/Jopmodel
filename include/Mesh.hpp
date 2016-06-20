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


#ifndef JOP_MESH_HPP
#define JOP_MESH_HPP

#include <cstdint>
#include <vector>

namespace jop
{
	class Mesh
	{
	public:
		enum VertexComponent : uint32_t
		{
			Position = 1,
			TexCoords = 1 << 1,
			Normal = 1 << 2,
			Tangents = 1 << 3,
			Color = 1 << 4
		};

		Mesh();
		~Mesh();

	private:

		friend class Converter;

		std::vector<unsigned char> m_vertexBuffer;
		std::vector<unsigned char> m_indexBuffer;
		uint32_t m_vertexComponents;

		int m_type; //triangles, lines etc.
		uint32_t m_components = 0; //bitfield

		unsigned int m_startIndex = 0;
		unsigned int m_start = 0;
		unsigned int m_lengthIndex = 0;
		unsigned int m_length = 0; //bytes
	};
}
#endif