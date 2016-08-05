cmake_minimum_required(VERSION 2.8)

project(Jopmodel)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/include)

set(INCDIR ${CMAKE_CURRENT_SOURCE_DIR}/include/jopmodel)
set(SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/src/jopmodel)

add_executable(jopmodel
${SRCDIR}/main.cpp

${SRCDIR}/Converter.cpp
${SRCDIR}/FileSystem.cpp
${SRCDIR}/Material.cpp
${SRCDIR}/Mesh.cpp
${SRCDIR}/Model.cpp
${SRCDIR}/Texture.cpp

${INCDIR}/Converter.hpp
${INCDIR}/FileSystem.hpp
${INCDIR}/Material.hpp
${INCDIR}/Mesh.hpp
${INCDIR}/Model.hpp
${INCDIR}/Texture.hpp
)

include (ExternalProject)

#include_directories(${PROJECT_SOURCE_DIR}/extlibs/)

set(PREFIX_DIR "${PROJECT_SOURCE_DIR}/extlibs/${CMAKE_GENERATOR}")

#Assimp
ExternalProject_Add(Assimp

PREFIX ${PREFIX_DIR}

GIT_REPOSITORY "https://github.com/assimp/assimp.git"
GIT_TAG "master"

UPDATE_COMMAND ""
PATCH_COMMAND ""
INSTALL_COMMAND ""

CMAKE_ARGS 
-DASSIMP_BUILD_ASSIMP_TOOLS=OFF
-DASSIMP_BUILD_NO_EXPORT=ON
-DBUILD_SHARED_LIBS=OFF
-DASSIMP_BUILD_TESTS=OFF
-DASSIMP_BUILD_SAMPLES=OFF
-DLIBRARY_SUFFIX=
-DLIBRARY_OUTPUT_PATH=${PREFIX_DIR}/lib
)
add_dependencies(jopmodel Assimp)



ExternalProject_Get_Property(Assimp SOURCE_DIR)
include_directories(${SOURCE_DIR}/include)


target_link_libraries(jopmodel ${PREFIX_DIR}/lib/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}assimp${CMAKE_STATIC_LIBRARY_SUFFIX}
debug ${PREFIX_DIR}/lib/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstaticd${CMAKE_STATIC_LIBRARY_SUFFIX}
optimized ${PREFIX_DIR}/lib/${CMAKE_CFG_INTDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstatic${CMAKE_STATIC_LIBRARY_SUFFIX}
)






