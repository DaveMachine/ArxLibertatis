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

#include <deque>
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
	
	///! Render states
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
	
	///! Pixel comparison functions
	enum PixelCompareFunc {
		CmpNever,               ///!< Never
		CmpLess,                ///!< Less
		CmpEqual,               ///!< Equal
		CmpLessEqual,           ///!< Less Equal
		CmpGreater,             ///!< Greater
		CmpNotEqual,            ///!< Not Equal
		CmpGreaterEqual,        ///!< Greater Equal
		CmpAlways               ///!< Always
	};
	
	///! Pixel blending factor
	enum PixelBlendingFactor {
		BlendZero,              ///!< Zero
		BlendOne,               ///!< One
		BlendSrcColor,          ///!< Source color
		BlendSrcAlpha,          ///!< Source alpha
		BlendInvSrcColor,       ///!< Inverse source color
		BlendInvSrcAlpha,       ///!< Inverse source alpha
		BlendSrcAlphaSaturate,  ///!< Source alpha saturate
		BlendDstColor,          ///!< Destination color
		BlendDstAlpha,          ///!< Destination alpha
		BlendInvDstColor,       ///!< Inverse destination color
		BlendInvDstAlpha        ///!< Inverse destination alpha
	};
	
	///! Culling 
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
	
	///! Fog
	enum FogMode {
		FogNone,
		FogExp,
		FogExp2,
		FogLinear
	};
	
	///! Target surface
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
	
	struct configuration
	{
		configuration();
		configuration(const configuration &old);

		struct dirty_flags
		{
			void clear()
			{
				for (int i = 0; i < nRenderStates; i++)
				{
					renderstate[i] = false;
				}

				alphafunc = false;
				blendfunc = false;
				fogcolor = false;
				fogparams = false;
				antialiasing = false;
				culling = false;
				depthbias = false;
				fillmode = false;
			}
			bool renderstate[nRenderStates];
			bool alphafunc;
			bool blendfunc;
			bool fogcolor;
			bool fogparams;
			bool antialiasing;
			bool culling;
			bool depthbias;
			bool fillmode;
		};

		struct state_struct
		{
			void clear()
			{
				for (int i = 0; i < nRenderStates; i++)
				{
					renderstate[i] = false;
				}

				alphafunc = CmpNever;
				alphafef = 0.0f;

				blendsrcFactor = BlendZero;
				blenddstFactor = BlendZero;

				fogcolor.gray(0.0f);

				fogMode = FogNone;
				fogStart = 0.0f;
				fogEnd = 0.0f;
				fogDensity = 0.0f;

				antialiasing = false;

				cullingmode = CullNone;

				depthbias = 0;

				fillmode = FillPoint;
			}

			/// render state
			bool renderstate[nRenderStates];

			/// alpha func
			PixelCompareFunc alphafunc;
			float alphafef;

			/// blend func
			PixelBlendingFactor blendsrcFactor;
			PixelBlendingFactor blenddstFactor;

			/// fog color
			Color fogcolor;

			/// fog params
			FogMode fogMode;
			float fogStart;
			float fogEnd;
			float fogDensity;

			/// antialiasing
			bool antialiasing;

			/// culling
			CullingMode cullingmode;

			/// depth bias
			int depthbias;

			/// fill mode
			FillMode fillmode;
		};

		state_struct state;
		dirty_flags dirty;
	};

	Renderer();
	virtual ~Renderer();
	
	virtual void Initialize() = 0;
	
	/// Scene begin/end...
	virtual bool BeginScene() = 0;
	virtual bool EndScene() = 0;
	
	/// Matrices
	virtual void SetViewMatrix(const EERIEMATRIX & matView) = 0;
	void SetViewMatrix(const Vec3f & vPosition, const Vec3f & vDir, const Vec3f & vUp);
	virtual void GetViewMatrix(EERIEMATRIX & matView) const = 0;
	void pushViewMatrix();
	void popViewMatrix();

	virtual void SetProjectionMatrix(const EERIEMATRIX & matProj) = 0;
	virtual void GetProjectionMatrix(EERIEMATRIX & matProj) const = 0;
	void pushProjectionMatrix();
	void popProjectionMatrix();
	
	/// Texture management
	virtual void ReleaseAllTextures() = 0;
	virtual void RestoreAllTextures() = 0;
	
	/// Factory
	virtual Texture2D * CreateTexture2D() = 0;
	
	/// Viewport
	virtual void SetViewport(const Rect & viewport) = 0;
	virtual Rect GetViewport() = 0;
	
	/// Projection
	virtual void Begin2DProjection(float left, float right, float bottom, float top, float zNear, float zFar) = 0;
	virtual void End2DProjection() = 0;
	
	/// Render Target
	virtual void Clear(BufferFlags bufferFlags, Color clearColor = Color::none, float clearDepth = 1.f, size_t nrects = 0, Rect * rect = 0) = 0;
	
	/// Texturing
	inline unsigned int GetTextureStageCount() const { return m_TextureStages.size(); }
	TextureStage * GetTextureStage(unsigned int textureStage);
	void ResetTexture(unsigned int textureStage);
	void SetTexture(unsigned int textureStage, Texture * pTexture);
	void SetTexture(unsigned int textureStage, TextureContainer * pTextureContainer);
	
	virtual float GetMaxAnisotropy() const = 0;
	
	/// Utilities...
	virtual void DrawTexturedRect(float x, float y, float w, float h, float uStart, float vStart, float uEnd, float vEnd, Color color) = 0;
	
	virtual VertexBuffer<TexturedVertex> * createVertexBufferTL(size_t capacity, BufferUsage usage) = 0;
	virtual VertexBuffer<SMY_VERTEX> * createVertexBuffer(size_t capacity, BufferUsage usage) = 0;
	virtual VertexBuffer<SMY_VERTEX3> * createVertexBuffer3(size_t capacity, BufferUsage usage) = 0;
	
	virtual void drawIndexed(Primitive primitive, const TexturedVertex * vertices, size_t nvertices, unsigned short * indices, size_t nindices) = 0;
	
	virtual bool getSnapshot(Image & image) = 0;
	virtual bool getSnapshot(Image & image, size_t width, size_t height) = 0;

	// below this point state is tracked by the stack

	/// Render states
	bool GetRenderState(const RenderState &i) {
		configuration &config = stack.back();
		return config.state.renderstate[i];
	}

	void SetRenderState(const RenderState &i, const bool &enable)
	{
		configuration &config = stack.back();
		config.dirty.renderstate[i] = true;
		config.state.renderstate[i] = enable;
		ApplyRenderState(i, enable);
	}
	virtual void ApplyRenderState(const RenderState &i, const bool &enable) = 0;
	
	/// Alphablending & Transparency
	void SetAlphaFunc(const PixelCompareFunc &func, const float &fef) // Ref = [0.0f, 1.0f]
	{
		configuration &config = stack.back();
		config.dirty.alphafunc = true;
		config.state.alphafunc = func;
		config.state.alphafef = fef;
		ApplyAlphaFunc(func, fef);
	}
	virtual void ApplyAlphaFunc(const PixelCompareFunc &func, const float &fef) = 0; // Ref = [0.0f, 1.0f]

	void SetBlendFunc(const PixelBlendingFactor &srcFactor, const PixelBlendingFactor &dstFactor)
	{
		configuration &config = stack.back();
		config.dirty.blendfunc = true;
		config.state.blendsrcFactor = srcFactor;
		config.state.blenddstFactor = dstFactor;
		ApplyBlendFunc(srcFactor, dstFactor);
	}
	virtual void ApplyBlendFunc(const PixelBlendingFactor &srcFactor, const PixelBlendingFactor &dstFactor) = 0;

	/// Fog
	void SetFogColor(Color color)
	{
		configuration &config = stack.back();
		config.dirty.fogcolor = true;
		config.state.fogcolor = color;
		ApplyFogColor(color);
	}
	virtual void ApplyFogColor(Color color) = 0;

	void SetFogParams(FogMode fogMode, float fogStart, float fogEnd, float fogDensity = 1.0f)
	{
		configuration &config = stack.back();
		config.dirty.fogparams = true;
		config.state.fogMode = fogMode;
		config.state.fogStart = fogStart;
		config.state.fogEnd = fogEnd;
		config.state.fogDensity = fogDensity;
		ApplyFogParams(fogMode, fogStart, fogEnd, fogDensity);
	}
	virtual void ApplyFogParams(FogMode fogMode, float fogStart, float fogEnd, float fogDensity = 1.0f) = 0;

	virtual bool isFogInEyeCoordinates() = 0;
	
	/// Rasterizer
	void SetAntialiasing(bool enable)
	{
		configuration &config = stack.back();
		config.dirty.antialiasing = true;
		config.state.antialiasing = enable;
		ApplyAntialiasing(enable);
	}
	virtual void ApplyAntialiasing(bool enable) = 0;

	void SetCulling(CullingMode mode)
	{
		configuration &config = stack.back();
		config.dirty.culling = true;
		config.state.cullingmode = mode;
		ApplyCulling(mode);
	}
	virtual void ApplyCulling(CullingMode mode) = 0;

	void SetDepthBias(int depthBias)
	{
		configuration &config = stack.back();
		config.dirty.depthbias = true;
		config.state.depthbias = depthBias;
		ApplyDepthBias(depthBias);
	}
	virtual void ApplyDepthBias(int depthBias) = 0;

	void SetFillMode(FillMode mode)
	{
		configuration &config = stack.back();
		config.dirty.fillmode = true;
		config.state.fillmode = mode;
		ApplyFillMode(mode);
	}
	virtual void ApplyFillMode(FillMode mode) = 0;
	
	/// stack

	void push();
	void pop();

protected:
	
	std::vector<TextureStage *> m_TextureStages;

private:
	
	std::deque<configuration> stack;
};

DECLARE_FLAGS_OPERATORS(Renderer::BufferFlags)

extern Renderer * GRenderer;

#endif // ARX_GRAPHICS_RENDERER_H
