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

#ifndef JOPM_FILESYSTEM_HPP
#define JOPM_FILESYSTEM_HPP

//Headers
#include <dirent/dirent.h>

#include <cctype>
#include <direct.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

//////////////////////////////////////////////

namespace jopm
{
    class FileSystem{

    private:

        friend class Converter;

        struct pathInfo
        {
            std::string anyPath = "";
            bool fromRoot = false;
            int lastDot = -1;
            int lastFolder = -1;
        };

        FileSystem();

        std::string sortPaths(const int& argc, const char* argv[]);

        pathInfo sortAPath(const std::string& anyPath);

        bool sortArgs(const int& argc, const char* argv[]);

        bool argComp(std::string& a, std::string& b);

        std::string findTexture(const std::string& searchDir, const std::string& texName);

        std::string getTexture(const std::string& texPath);

        bool m_embedTex;
        bool m_verbose;
        bool m_argvNewPath;
        bool m_optimizeGraph;
        bool m_centered;
        
        std::string m_absTexSearchLoc;
        std::string m_parTexSearchLoc;
        std::string m_modelName;
        std::string m_absOutputPath;

        static std::vector<std::string> m_argCalls;

    };
}
#endif