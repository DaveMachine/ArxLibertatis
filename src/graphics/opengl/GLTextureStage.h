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

#ifndef ARX_GRAPHICS_OPENGL_GLTEXTURESTAGE_H
#define ARX_GRAPHICS_OPENGL_GLTEXTURESTAGE_H

#include "graphics/opengl/OpenGLUtil.h"
#include "graphics/texture/TextureStage.h"

class OpenGLRenderer;
class GLTexture2D;

class GLTextureStage : public TextureStage {
	
public:
	
	GLTextureStage(OpenGLRenderer * renderer, unsigned textureStage);
	~GLTextureStage();
	
	void SetTexture(Texture * pTexture);
	void ResetTexture();
	
	void SetColorOp(TextureOp op, TextureArg arg0, TextureArg arg1) {
		setOp(Color, op, arg0, arg1);
	}

	void SetColorOp(TextureOp op) {
		setOp(Color, op);
	}

	void SetAlphaOp(TextureOp op, TextureArg arg0, TextureArg arg1) {
		setOp(Alpha, op, arg0, arg1);
	}

	void SetAlphaOp(TextureOp op) {
		setOp(Alpha, op);
	}

	void SetWrapMode(WrapMode _wrapMode) {
		wrapMode = _wrapMode;
	}

	void SetMinFilter(FilterMode filterMode) {
		minFilter = filterMode;
	}

	void SetMagFilter(FilterMode filterMode) {
		magFilter = filterMode;
	}

	void SetMipFilter(FilterMode filterMode) {
		mipFilter = filterMode;
	}

	void SetMipMapLODBias(float bias);
	
	void apply();
	
private:
	
	inline bool isEnabled() { return ((ops[Color] != OpDisable) || (ops[Alpha] != OpDisable)); }
	
	OpenGLRenderer * renderer;
	
	enum OpType {
		Color,
		Alpha
	};
	
	enum Arg {
		Arg0,
		Arg1
	};
	
	TextureOp ops[2];
	TextureArg args[2][2];
	
	void setArg(OpType alpha, Arg idx, TextureArg arg);
	
	void setOp(OpType alpha, GLenum op, GLint scale);
	void setOp(OpType alpha, TextureOp op);
	void setOp(OpType alpha, TextureOp op, TextureArg arg0, TextureArg arg1);
	
	GLTexture2D * tex;
	GLTexture2D * current;
	
	WrapMode wrapMode;
	FilterMode minFilter;
	FilterMode magFilter;
	FilterMode mipFilter;
	
	friend class GLTexture2D;
};

#endif // ARX_GRAPHICS_OPENGL_GLTEXTURESTAGE_H
