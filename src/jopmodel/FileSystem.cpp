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
#include <jopmodel/FileSystem.hpp>

//////////////////////////////////////////////

namespace jopm
{
    //Static definition
    const std::vector<std::string> FileSystem::m_argCalls = { "-", "/", "--" };


    FileSystem::FileSystem() :
        m_embedTex          (false),
        m_verbose           (false),
        m_argvNewPath       (false),
        m_optimizeGraph     (false),
        m_centered          (false),
        m_firstTex          (true),
        m_absTexSearchLoc   (),
        m_parTexSearchLoc   (),
        m_modelName         (),
        m_absOutputPath     (),
        m_binaryWriter      (0u),
        m_binaryLastSize    (0u)
    {}

    bool FileSystem::sortArgs(const int& argc, const char* argv[])
    {
        if (!std::strcmp(argv[1], "-h") || !std::strcmp(argv[1], "/h") || !std::strcmp(argv[1], "-help") || !std::strcmp(argv[1], "/help"))
        {
            std::cout <<
                "\nJOPMODEL CONVERTER\n"
                "Use this tool to convert your model files to be compatible with the Jopnal engine.\n"
                "See more in \"Jopnal.net\" and \"github.com/Jopnal\"\n\n"
                "ARGUMENTS:\n\n"
                "REQUIRED:\n"
                "First argument: file to load\n\n"
                "OPTIONAL:\n"

                "Second argument: path to write the new model file.\nWrites a file same name as the original model, ending in \".jopm\".\n"
                "Creates required directories.\n"
                "If not specified, creates the file in to the same directory as the original model.\n\n"

                "-et : --embed-textures = embed textures into the model file, default off\n\n"
                "-cn : --no-collapse    = collapse nodes, default on\n\n"
                "-cc : --no-center      = calculate local center of the object for Jopnal engine, default on\n"
                << std::endl;
            return false;
        }

        std::string triggeredPath;
        for (int i = 0; i < argc; ++i)
        {
            auto& a = std::string(argv[i]);
            auto& ac = m_argCalls;
            bool triggered = false;

            for (unsigned int j = 0; j < ac.size(); ++j)
            {
                if (argComp(a, ac[j] + "et") || argComp(a, ac[j] + "embed-textures"))
                {
                    triggered = true;
                    m_embedTex = true;
                }
                else if (argComp(a, ac[j] + "cn") || argComp(a, ac[j] + "no-collapse"))
                {
                    triggered = true;
                    m_optimizeGraph = true;
                }
                else if (argComp(a, ac[j] + "cc") || argComp(a, ac[j] + "no-center"))
                {
                    triggered = true;
                    m_centered = false;
                }
                else if (argComp(a, ac[j] + "v") || argComp(a, ac[j] + "verbose"))
                {
                    triggered = true;
                    m_verbose = true;
                }
                else if ((i == 2u) && (j == ac.size() - 1) && (triggered == false))
                {
                    triggeredPath = argv[i];
                    m_argvNewPath = true;
                }
            }
        }

        if (m_verbose)
        {
            std::string emb = m_embedTex ? "true" : "false";
            std::string cln = m_optimizeGraph ? "true" : "false";
            std::string cen = m_centered ? "true" : "false";

            std::cout << "Using verbose mode.\n\n" <<
                "Embedded textures: " << emb << "\n" <<
                "Collapsed nodes: " << cln << "\n" <<
                "Centered model: " << cen << "\n" <<
                std::endl;

            if (m_argvNewPath)
                std::cout << "Following triggered newPath: " << triggeredPath << std::endl;
        }

        return true;
    }

    bool FileSystem::argComp(std::string& a, std::string& b)
    {
        if (a.length() == b.length())
            return std::equal(b.begin(), b.end(), a.begin(), [](unsigned char _a, unsigned char _b){return std::tolower(_a) == std::tolower(_b); });
        return false;
    }

    std::string FileSystem::sortPaths(const int& argc, const char* argv[])
    {
        std::string searchLoc = argv[1];
        std::string modelName = argv[1];
        std::string fileOutPath; //don't initialize yet
        std::string tempRoot;

        //execution path
        {
            std::vector<wchar_t> pathBuffer;
            DWORD copied = 0;
            do {
                pathBuffer.resize(pathBuffer.size() + MAX_PATH);
                copied = GetModuleFileNameW(0, &pathBuffer.at(0), pathBuffer.size());
            } while (copied >= pathBuffer.size());

            pathBuffer.resize(copied);
            tempRoot = std::string(pathBuffer.begin(), pathBuffer.end());
            pathInfo info = sortAPath(tempRoot);

            tempRoot.resize(info.lastFolder);

            if (m_verbose)
                std::cout << "Root path after execution path block is: " << tempRoot << std::endl;
        }
        //~execution path

        //argv[1]
        {
            //find out what kind of path we are given
            pathInfo info = sortAPath(searchLoc);

            //given argv[1] doesn't seem to be a file
            if (info.lastDot == -1)
            {
                std::cout << "Unknown parameters: first argument is not a file\n" << std::endl; //fix so that * can be used for recursion
                return ".jopm";
            }

            modelName = modelName.substr(info.lastFolder + 1, info.lastDot - (info.lastFolder + 1)); //name of the model == folder name to create

            //Cut the model file out
            searchLoc.resize(info.lastFolder == -1 ? 0 : info.lastFolder);

            //A directory structure was given but it doesn't start from root
            if (info.fromRoot == false)
            {
                m_parTexSearchLoc = searchLoc;
                std::string t_tempRoot = tempRoot;
                std::string::size_type i = t_tempRoot.find(searchLoc);
                if (i != std::string::npos)
                    t_tempRoot.erase(i, searchLoc.length());
                searchLoc = t_tempRoot;
            }

            //no directory structure was given, starting from working directory
            if (info.lastFolder == -1)
                searchLoc = tempRoot;

            if (info.fromRoot == true)
                m_parTexSearchLoc = searchLoc;

            m_absTexSearchLoc = searchLoc;

            if (m_verbose)
            {
                std::cout << "Model name after argv[1] block is: " << modelName << std::endl;
                std::cout << "Search location after argv[1] block is: " << searchLoc << std::endl;
            }
        }
        //~argv[1]

        //argv[2]
        {
            if (m_argvNewPath)
            {
                //find out what kind of path we are given
                fileOutPath = argv[2];
                pathInfo info = sortAPath(fileOutPath);

                //It's going to be a .jopm!
                if (info.lastDot > 0) //should work with ./directory
                    fileOutPath.resize(info.lastDot);

                if (info.fromRoot == false)
                {
                    std::string t_fileOutPath = tempRoot;
                    std::string::size_type i;

                    //Keep these 4 separate!
                    if (m_parTexSearchLoc.empty())
                        i = std::string::npos;
                    else
                        i = t_fileOutPath.find(m_parTexSearchLoc);

                    if (i != std::string::npos)
                    {
                        t_fileOutPath.erase(i, m_parTexSearchLoc.length());
                        fileOutPath = t_fileOutPath + fileOutPath;
                    }
                    else
                        fileOutPath = tempRoot + '\\' + fileOutPath;
                }
            }

            else
                fileOutPath = searchLoc;

            if (m_verbose)
            {
                std::string isNewPath = m_argvNewPath ? "true" : "false";
                std::cout << "There is a new location to place the converted model: " << isNewPath << std::endl;
                std::cout << "FileOutPath after argv[2] block is: " << fileOutPath << std::endl;
            }
        }
        //~argv[2]

        m_absOutputPath = fileOutPath + '\\' + modelName;
        m_modelName = modelName;
        return m_absOutputPath + ".jopm";
    }

    FileSystem::pathInfo FileSystem::sortAPath(const std::string& anyPath)
    {
        pathInfo newInfo;

        for (size_t i = 0; i < anyPath.size(); ++i)
        {
            if (anyPath[i] == '/' || anyPath[i] == '\\' || anyPath[i] == './' || anyPath[i] == '.\\')
                newInfo.lastFolder = i;
            else if (anyPath[i] == '.')
                newInfo.lastDot = i;
            else if (anyPath[i] == ':')
                newInfo.fromRoot = true;
        }
        return newInfo;
    }

    std::string FileSystem::findTexture(const std::string& searchDir, const std::string& texName)
    {
        DIR *dir;
        struct dirent *ent;

        //Anyone here?
        if ((dir = opendir(searchDir.c_str())) != NULL)
        {
            //Are we there yet?
            while ((ent = readdir(dir)) != NULL)
            {
                if (std::string(ent->d_name) != "." && std::string(ent->d_name) != "..")
                {
                    switch (ent->d_type)
                    {
                    case DT_UNKNOWN:
                    case DT_REG:
                    {
                        //check file
                        if (std::string(ent->d_name) == texName)
                        {
                            //texture found
                            return searchDir + '\\' + ent->d_name;
                        }
                        break;
                    }
                    //go check the subfolder
                    case DT_DIR:
                    {
                        std::string temp = findTexture(searchDir + '\\' + ent->d_name, texName);
                        if (!temp.empty())
                            return temp;
                    }
                    }
                }
            }
        }
        return "";
    }

    std::pair<std::string, std::string> FileSystem::getTexture(const std::string& texPath)
    {
        if (m_firstTex)
        {
            //Create directory tree
            if (!makeDirs(m_absOutputPath))
            {
                std::cout << "Can not place files next to the original model. Check file paths and priviledges." << std::endl;
                assert(false);
            }
            m_firstTex = false;
        }

        std::string texLoc;
        std::string textureName = texPath;
        bool texAlreadyMoved = false;

        //get the name of the texture resource
        pathInfo info = sortAPath(texPath);
        textureName = textureName.substr(info.lastFolder + 1, texPath.size());

        //no need to go in here when embedding
        if (!m_embedTex)
        {
            //Check if the texture has already been moved...
            texLoc = findTexture(m_absOutputPath, textureName);
            if (!texLoc.empty())
                texAlreadyMoved = true;
        }
        //...if not, go find the texture location
        if (!texAlreadyMoved)
        {
            //The Search
            texLoc = findTexture(m_absTexSearchLoc, textureName);

            if (!texLoc.empty())
            {
                std::ifstream src(texLoc, std::ios::binary);
                if (m_embedTex)
                {
                    //get the size to write
                    unsigned int temp = m_binaryWriter;
                    m_binaryWriter += static_cast<unsigned int>(src.rdbuf()->pubseekoff(0, src.end));
                    m_binaryLastSize = m_binaryWriter - temp;
                }
                else
                {
                    std::ofstream dest(m_absOutputPath + '\\' + textureName, std::ios::binary | std::ios::trunc);
                    dest << src.rdbuf();
                    dest.close();
                }
                src.close();
            }
            else
                std::cout << "Failed to find texture: " << textureName << " from " << m_absTexSearchLoc << std::endl;
        }
        //m_textureName = textureName;
        //Changing the path to be compatible with the Jopnal Engine
        return std::make_pair(texLoc, m_modelName + '/' + textureName);
    }

    bool FileSystem::makeDirs(const std::string& dirsToMake)
    {
        std::string outDir = dirsToMake;
        pathInfo info = sortAPath(outDir);

        if (m_embedTex)
            outDir.resize(info.lastFolder);

        outDir += '\\';

        std::string tempPath = outDir;
        for (size_t i = 0; i < outDir.size(); ++i)
        {
            if (outDir[i] == '\\')
            {
                tempPath.resize(i);

                //Reset error.
                _set_errno(0);
                //Can create only one directory at a time.
                int result = _mkdir(tempPath.c_str());

                if (result != 0 && errno == ENOENT)
                {
                    std::cout << "Can not create directory: " << tempPath << "\nPlacing files next to the original model: " << m_absTexSearchLoc << std::endl;
                    if (!argComp(m_absOutputPath, m_absTexSearchLoc))
                    {
                        //Keep!
                        m_absOutputPath = m_absTexSearchLoc;
                        return makeDirs(m_absOutputPath);
                    }
                }

                if (m_verbose)
                {
                    if (errno == EEXIST)
                        std::cout << "Directory exists: " << tempPath << std::endl;

                    if (result == 0)
                        std::cout << "Created directory: " << tempPath << std::endl;
                }
                tempPath = outDir;
            }
        }
        return true;
    }
}