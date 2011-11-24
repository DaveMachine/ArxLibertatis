/*
 * Copyright 2011 Arx Libertatis Team (see the AUTHORS file)
 *
 * This file is part of Arx Libertatis.
 *
 * Arx Libertatis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Arx Libertatis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Arx Libertatis.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ARX_GRAPHICS_RENDERER_H
#define ARX_GRAPHICS_RENDERER_H

#include <vector>

#include "platform/Flags.h"
#include "math/MathFwd.h"
#include "graphics/Color.h"

struct EERIEMATRIX;
struct TexturedVertex;
struct SMY_VERTEX;
struct SMY_VERTEX3;
class TextureContainer;
class TextureStage;
class Image;
class Texture;
class Texture2D;
template <class Vertex> class VertexBuffer;

class Renderer {
	
public:
	
	//! Render states
	enum RenderState {
		AlphaBlending = 0,
		AlphaTest,
		ColorKey,
		DepthTest,
		DepthWrite,
		Fog,
		Lighting,
		ZBias,
		nRenderStates,
	};
	
	//! Pixel comparison functions
	enum PixelCompareFunc {
		CmpNever,               //!< Never
		CmpLess,                //!< Less
		CmpEqual,               //!< Equal
		CmpLessEqual,           //!< Less Equal
		CmpGreater,             //!< Greater
		CmpNotEqual,            //!< Not Equal
		CmpGreaterEqual,        //!< Greater Equal
		CmpAlways               //!< Always
	};
	
	//! Pixel blending factor
	enum PixelBlendingFactor {
		BlendZero,              //!< Zero
		BlendOne,               //!< One
		BlendSrcColor,          //!< Source color
		BlendSrcAlpha,          //!< Source alpha
		BlendInvSrcColor,       //!< Inverse source color
		BlendInvSrcAlpha,       //!< Inverse source alpha
		BlendSrcAlphaSaturate,  //!< Source alpha saturate
		BlendDstColor,          //!< Destination color
		BlendDstAlpha,          //!< Destination alpha
		BlendInvDstColor,       //!< Inverse destination color
		BlendInvDstAlpha        //!< Inverse destination alpha
	};
	
	//! Culling 
	enum CullingMode {
		CullNone,
		CullCW,
		CullCCW
	};
	
	enum FillMode {
		FillPoint,
		FillWireframe,
		FillSolid
	};
	
	//! Fog
	enum FogMode {
		FogNone,
		FogExp,
		FogExp2,
		FogLinear
	};
	
	//! Target surface
	enum BufferType {
		ColorBuffer   = (1<<0),
		DepthBuffer   = (1<<1),
		StencilBuffer = (1<<2)
	};
	DECLARE_FLAGS(BufferType, BufferFlags);
	
	enum Primitive {
		TriangleList,
		TriangleStrip,
		TriangleFan,
		LineList,
		LineStrip
	};
	
	enum BufferUsage {
		Static,
		Dynamic,
		Stream
	};
	
	struct RendererConfiguration
	{
		struct dirty_flags
		{
			bool RenderState[nRenderStates];
			bool alphafunc;
			bool blendfunc;
		};

		struct state_struct
		{
			// set render state
			bool RenderState[nRenderStates];

			// set alpha func
			PixelCompareFunc alphafunc;
			float alphafef;

			// set blend func
			PixelBlendingFactor blendsrcFactor;
			PixelBlendingFactor blenddstFactor;
		};

		state_struct state;
		dirty_flags dirty;
	};

	Renderer();
	virtual ~Renderer();
	
	virtual void Initialize() = 0;
	
	// Scene begin/end...
	virtual bool BeginScene() = 0;
	virtual bool EndScene() = 0;
	
	// Matrices
	virtual void SetViewMatrix(const EERIEMATRIX & matView) = 0;
	void SetViewMatrix(const Vec3f & vPosition, const Vec3f & vDir, const Vec3f & vUp);
	virtual void GetViewMatrix(EERIEMATRIX & matView) const = 0;
	void pushViewMatrix() {}
	void popViewMatrix() {}
	virtual void SetProjectionMatrix(const EERIEMATRIX & matProj) = 0;
	virtual void GetProjectionMatrix(EERIEMATRIX & matProj) const = 0;
	void pushProjectionMatrix() {}
	void popProjectionMatrix() {}
	
	// Texture management
	virtual void ReleaseAllTextures() = 0;
	virtual void RestoreAllTextures() = 0;
	
	// Factory
	virtual Texture2D * CreateTexture2D() = 0;
	
	// Render states
	void PushRendererConfiguration();
	void PopRendererConfiguration();

	void SetRenderState(const RenderState &i, const bool &enable)
	{
		RendererConfiguration *config = RendererConfigurationStack.back();
		config->dirty.RenderState[i] = true;
		config->state.RenderState[i] = enable;
		ApplyRenderState(i, enable);
	}
	virtual void ApplyRenderState(const RenderState &i, const bool &enable) = 0;
	
	// Alphablending & Transparency
	void SetAlphaFunc(const PixelCompareFunc &func, const float &fef) // Ref = [0.0f, 1.0f]
	{
		RendererConfiguration *config = RendererConfigurationStack.back();
		config->dirty.alphafunc = true;
		config->state.alphafunc = func;
		config->state.alphafef = fef;
		ApplyAlphaFunc(func, fef);
	}

	virtual void ApplyAlphaFunc(const PixelCompareFunc &func, const float &fef) = 0; // Ref = [0.0f, 1.0f]

	void SetBlendFunc(const PixelBlendingFactor &srcFactor, const PixelBlendingFactor &dstFactor)
	{
		RendererConfiguration *config = RendererConfigurationStack.back();
		config->dirty.blendfunc = true;
		config->state.blendsrcFactor = srcFactor;
		config->state.blenddstFactor = dstFactor;
		ApplyBlendFunc(srcFactor, dstFactor);
	}
	
	virtual void ApplyBlendFunc(const PixelBlendingFactor &srcFactor, const PixelBlendingFactor &dstFactor) = 0;

	// Viewport
	virtual void SetViewport(const Rect & viewport) = 0;
	virtual Rect GetViewport() = 0;
	
	// Projection
	virtual void Begin2DProjection(float left, float right, float bottom, float top, float zNear, float zFar) = 0;
	virtual void End2DProjection() = 0;
	
	// Render Target
	virtual void Clear(BufferFlags bufferFlags, Color clearColor = Color::none, float clearDepth = 1.f, size_t nrects = 0, Rect * rect = 0) = 0;
	
	// Fog
	virtual void SetFogColor(Color color) = 0;
	virtual void SetFogParams(FogMode fogMode, float fogStart, float fogEnd, float fogDensity = 1.0f) = 0;
	virtual bool isFogInEyeCoordinates() = 0;
	
	// Rasterizer
	virtual void SetAntialiasing(bool enable) = 0;
	virtual void SetCulling(CullingMode mode) = 0;
	virtual void SetDepthBias(int depthBias) = 0;
	virtual void SetFillMode(FillMode mode) = 0;
	
	// Texturing
	inline unsigned int GetTextureStageCount() const { return m_TextureStages.size(); }
	TextureStage * GetTextureStage(unsigned int textureStage);
	void ResetTexture(unsigned int textureStage);
	void SetTexture(unsigned int textureStage, Texture * pTexture);
	void SetTexture(unsigned int textureStage, TextureContainer * pTextureContainer);
	
	virtual float GetMaxAnisotropy() const = 0;
	
	// Utilities...
	virtual void DrawTexturedRect(float x, float y, float w, float h, float uStart, float vStart, float uEnd, float vEnd, Color color) = 0;
	
	virtual VertexBuffer<TexturedVertex> * createVertexBufferTL(size_t capacity, BufferUsage usage) = 0;
	virtual VertexBuffer<SMY_VERTEX> * createVertexBuffer(size_t capacity, BufferUsage usage) = 0;
	virtual VertexBuffer<SMY_VERTEX3> * createVertexBuffer3(size_t capacity, BufferUsage usage) = 0;
	
	virtual void drawIndexed(Primitive primitive, const TexturedVertex * vertices, size_t nvertices, unsigned short * indices, size_t nindices) = 0;
	
	virtual bool getSnapshot(Image & image) = 0;
	virtual bool getSnapshot(Image & image, size_t width, size_t height) = 0;
	
protected:
	
	std::vector<TextureStage *> m_TextureStages;

private:
	
	std::vector<RendererConfiguration *> RendererConfigurationStack;
};

DECLARE_FLAGS_OPERATORS(Renderer::BufferFlags)

extern Renderer * GRenderer;

#endif // ARX_GRAPHICS_RENDERER_H
