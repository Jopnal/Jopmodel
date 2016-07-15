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

#ifndef JOP_CAMERA_HPP
#define JOP_CAMERA_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Core/Component.hpp>
#include <Jopnal/Graphics/RenderTexture.hpp>
#include <Jopnal/MathInclude.hpp>

//////////////////////////////////////////////


namespace jop
{
    class Renderer;
    class RenderTarget;

    class JOP_API Camera final : public Component
    {
    private:

        /// \brief Copy constructor
        ///
        /// \param other The other camera to copy
        /// \param newObj The new object
        ///
        Camera(const Camera& other, Object& newObj);

        JOP_DISALLOW_COPY_MOVE(Camera);
        JOP_GENERIC_COMPONENT_CLONE(Camera);

    public:

        typedef std::pair<float, float> ClippingPlanes;
        typedef std::pair<glm::vec2, glm::vec2> ViewPort;

        /// Union with the projection data
        ///
        union ProjectionData
        {
            struct
            {
                float sizeX;    ///< Orthographic projection width
                float sizeY;    ///< Orthographic projection height
            } orthographic;

            struct
            {
                float fov;          ///< The vertical field of view in radians
                float aspectRatio;  ///< The aspect ratio
            } perspective;
        };

        /// Projection mode
        ///
        enum class Projection
        {
            Orthographic,   ///< Usually used in 2D rendering
            Perspective     ///< Usually used in 3D rendering
        };

    public:
    
        /// \brief Constructor
        ///
        /// \param object The object this camera will be bound to
        /// \param renderer Reference to the renderer
        /// \param mode The initial projection mode
        ///
        Camera(Object& object, Renderer& renderer, const Projection mode);

        /// \brief Destructor
        ///
        ~Camera() override;


        /// \brief Get the projection matrix
        ///
        /// \return The projection matrix
        ///
        const glm::mat4& getProjectionMatrix() const;

        /// \brief Get the view matrix
        ///
        /// The matrix will be fetched straight from the object.
        ///
        /// \return The view matrix
        ///
        const glm::mat4& getViewMatrix() const;

        /// \brief Set the render mask
        ///
        /// \param mask The new mask to set
        ///
        Camera& setRenderMask(const uint32 mask);

        /// \brief Get the render mask
        ///
        /// \return The render mask
        ///
        uint32 getRenderMask() const;


        /// \brief Set the projection mode
        ///
        /// You need to accompany this call with calls to set the camera
        /// properties, as they won't be set to defaults automatically.
        /// Failing to set the properties will cause the projection to
        /// malfunction.
        ///
        /// \comm setProjectionMode
        ///
        /// \param mode The mode to be set
        ///
        Camera& setProjectionMode(const Projection mode);

        /// \brief Get the projection mode
        ///
        /// \return The projection mode
        ///
        Projection getProjectionMode() const;


        /// \brief Set the near and far clipping planes
        ///
        /// In perspective projection the clipping planes need to be positive.
        /// Otherwise, the projection result is undefined.
        ///
        /// \comm setClippingPlanes
        ///
        /// \param clipNear The near clipping plane
        /// \param clipFar The far clipping plane
        ///
        Camera& setClippingPlanes(const float clipNear, const float clipFar);

        /// \brief Get the values of the clipping planes
        ///
        /// The first value is the near clipping plane. The second value
        /// is the far clipping plane.
        ///
        /// \return Reference to the internal pair of clipping planes
        ///
        const ClippingPlanes& getClippingPlanes() const;


        /// \brief Brief set the size of the projection
        ///
        /// In perspective mode this call is equal to calling setAspectRatio(size.x / size.y)
        ///
        /// \param size The new size of the projection
        ///
        Camera& setSize(const glm::vec2& size);

        /// \brief Brief set the size of the projection
        ///
        /// \comm setSize
        ///
        /// \param x The width
        /// \param y The height
        ///
        Camera& setSize(const float x, const float y);

        /// \brief Get the size of the projection
        ///
        /// In perspective mode the return value is undefined.
        ///
        /// \return Size of the projection
        ///
        glm::vec2 getSize() const;

        /// \brief Set the aspect ratio
        ///
        /// This call is only valid in perspective mode.
        ///
        /// \comm setAspectRatio
        ///
        /// \param ratio The new aspect ratio to be set
        ///
        Camera& setAspectRatio(const float ratio);

        /// \brief Get the aspect ratio
        ///
        /// In orthographic mode the return value is undefined
        ///
        /// \return The aspect ratio
        ///
        float getAspectRatio() const;


        /// \brief Set the vertical field of view
        ///
        /// The minimum value is glm::radians(1) and maximum glm::radians(179).
        /// The value will be clamped inside this range.
        ///
        /// \comm setFieldOfView
        ///
        /// \param fovY The new field of view value
        ///
        Camera& setFieldOfView(const float fovY);

        /// \brief Get the field of view value
        ///
        /// \return The field of view value
        ///
        float getFieldOfView() const;


        /// \brief Set the view port
        ///
        /// The values are in relative coordinates. For example, [0.5,0.5] and [1.0,1.0]
        /// will select the right half of the screen
        ///
        /// \comm setViewport
        ///
        /// \param start The start coordinates
        /// \param end The end coordinates
        ///
        /// \return Reference to self
        ///
        Camera& setViewport(const glm::vec2& start, const glm::vec2& end);

        /// \brief Get the view port
        ///
        /// first = start
        /// second = size
        ///
        /// \see setViewport
        ///
        /// \return Reference to the view port
        ///
        const ViewPort& getViewport() const;

        /// \brief Apply this camera's view port
        ///
        /// \param mainTarget The main render target. This will be used to calculate the absolute coordinates
        ///
        void applyViewport(const RenderTarget& mainTarget) const;


        /// \brief Set this camera to use a render texture
        ///
        /// This will effectively cause whatever is drawn using this camera to render into the frame buffer.
        ///
        /// \param enable Enable the render texture? If this is false and a render texture already exists,
        ///               it will be destroyed and the rest of the arguments are ignored
        /// \param color The desired color attachment type
        /// \param size Size of the render texture
        /// \param depth The desired depth attachment type
        /// \param stencil The desired stencil attachment
        ///
        /// \return True if set/unset successfully
        ///
        bool enableRenderTexture(const bool enable,
                                 const glm::uvec2& size = glm::uvec2(0),
                                 const RenderTexture::ColorAttachment color = RenderTexture::ColorAttachment::RGBA2D,
                                 const RenderTexture::DepthAttachment depth = RenderTexture::DepthAttachment::None,
                                 const RenderTexture::StencilAttachment stencil = RenderTexture::StencilAttachment::None);

        /// \brief Get the internal render texture
        ///
        /// The returned RenderTexture will not be valid if it wasn't set before
        /// by using enableRenderTexture().
        ///
        /// \return Reference to the render texture
        ///
        const RenderTexture& getRenderTexture() const;


        /// \brief Get a ray for mouse picking purposes
        ///
        /// The resulting ray is normalized
        ///
        /// \param mouseCoords Mouse coordinates. Origin is assumed to be in the upper left corner
        /// \param target The render target (main usually). This is used to get the correct view port dimensions
        ///
        /// \return Normalized ray pointing from camera to the pointed position
        ///
        glm::vec3 getPickRay(const glm::vec2& mouseCoords, const RenderTarget& target) const;

    private:

        mutable glm::mat4 m_projectionMatrix;   ///< The projection matrix
        RenderTexture m_renderTexture;          ///< RenderTexture used for off-screen rendering
        ViewPort m_viewPort;                    ///< Viewport in relative coordinates
        ProjectionData m_projData;              ///< Union with data for orthographic and perspective projections
        ClippingPlanes m_clippingPlanes;        ///< The clipping planes
        Renderer& m_rendererRef;                ///< Reference to the renderer
        uint32 m_renderMask;                    ///< The render mask
        Projection m_mode;                      ///< Projection mode
        mutable bool m_projectionNeedUpdate;    ///< Flag to mark if the projection needs to be updated
    };
}

#endif

/// \class Camera
/// \ingroup graphics
///
/// 