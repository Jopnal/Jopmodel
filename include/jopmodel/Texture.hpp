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

#ifndef JOPM_TEXTURE_HPP
#define JOPM_TEXTURE_HPP

//Headers
#include <string>

//////////////////////////////////////////////

namespace jopm
{
	class Texture
	{
		friend class Converter;

    public:
        Texture();
        ~Texture();

    private:
        //First: full old path to the texture.
        //Second: 'modelname/texturename'
        std::pair<std::string, std::string> m_texturePaths;
		unsigned int m_type; //diffuse, specular etc.
		unsigned int m_wrapmode; //repeat, clamp etc.
        unsigned int m_texStart;
        unsigned int m_texLength;
        bool m_srgb;
        bool m_genmipmaps;
    };
}
#endif