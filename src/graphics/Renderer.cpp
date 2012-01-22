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

#include "graphics/Renderer.h"
#include "graphics/GraphicsUtility.h"
#include "graphics/texture/TextureStage.h"
#include "graphics/data/TextureContainer.h"
#include "graphics/texture/Texture.h"

Renderer * GRenderer;

TextureStage * Renderer::GetTextureStage(unsigned int textureStage) {
	return (textureStage < m_TextureStages.size()) ? m_TextureStages[textureStage] : NULL;
}

void Renderer::ResetTexture(unsigned int textureStage) {
	GetTextureStage(textureStage)->ResetTexture();
}

void Renderer::SetTexture(unsigned int textureStage, Texture * pTexture) {
	GetTextureStage(textureStage)->SetTexture(pTexture);
}

void Renderer::SetTexture(unsigned int textureStage, TextureContainer * pTextureContainer) {
	
	if(pTextureContainer && pTextureContainer->m_pTexture) {
		GetTextureStage(textureStage)->SetTexture(pTextureContainer->m_pTexture);
	} else {
		GetTextureStage(textureStage)->ResetTexture();
	}
}

Renderer::~Renderer() {
	for(size_t i = 0; i < m_TextureStages.size(); ++i) {
		delete m_TextureStages[i];
	}

	arx_assert(!stack.empty());
	stack.pop_back();
	arx_assert(stack.empty());
}

void Renderer::SetViewMatrix(const Vec3f & position, const Vec3f & dir, const Vec3f & up) {
	
	EERIEMATRIX mat;
	Util_SetViewMatrix(mat, position, dir, up);
	
	SetViewMatrix(mat);
}

Renderer::Renderer()
{
	// stack must always contain at least current state
	stack.push_back(configuration());
	configuration &config = stack.back();
	config.dirty.clear();
	config.state.clear();
}

void Renderer::push()
{
	// copy the current state into the new stack frame
	// TODO: could be used to check for state change
	// could also just check the old stack frame! (no copy needed)
	stack.push_back(configuration(stack.back()));
}

void Renderer::pop()
{
	// stack must always contain at least two frames for pop()
	arx_assert(stack.size() >= 2);

	// current configuration
	configuration &config = stack.back();
	// stored configuration
	configuration &old = *(stack.end() - 1);

	// optimization -
	// check if a value has been modified, if so restore it from the stack
	// this prevents Apply() being called without even potential for a state change

	// TODO: could check to ensure state change did occur here, or in Set()

	for (int i = 0; i < nRenderStates; i++) {
		if (config.dirty.renderstate[i])	{
			ApplyRenderState((RenderState)i, old.state.renderstate[i]);
		}
	}

	if (config.dirty.alphafunc) {
		ApplyAlphaFunc(old.state.alphafunc, old.state.alphafef);
	}

	if (config.dirty.antialiasing) {
		ApplyAntialiasing(old.state.antialiasing);
	}

	if (config.dirty.blendfunc) {
		ApplyBlendFunc(old.state.blendsrcFactor, old.state.blenddstFactor);
	}

	if (config.dirty.culling) {
		ApplyCulling(old.state.cullingmode);
	}

	if (config.dirty.depthbias) {
		ApplyDepthBias(old.state.depthbias);
	}

	if (config.dirty.fillmode) {
		ApplyFillMode(old.state.fillmode);
	}

	if (config.dirty.fogcolor) {
		ApplyFogColor(old.state.fogcolor);
	}

	if (config.dirty.fogparams) {
		ApplyFogParams(old.state.fogMode, old.state.fogStart, old.state.fogEnd, old.state.fogDensity);
	}

	// ...

	stack.pop_back();
}

Renderer::configuration::configuration()
{
	dirty.clear();
	state.clear();
}

Renderer::configuration::configuration(const configuration &old)
{
	dirty.clear();
	state = old.state;
}
