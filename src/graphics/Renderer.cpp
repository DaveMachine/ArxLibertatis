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
	memset(&config, 0, sizeof(configuration));
}

Renderer::configuration::configuration(const configuration &old)
{
	memcpy(&state, &old.state, sizeof(state_struct));
	memset(&dirty, 0, sizeof(dirty_flags));
}

Renderer::configuration::configuration()
{
	memset(&state, 0, sizeof(state_struct));
	memset(&dirty, 0, sizeof(dirty_flags));
}

void Renderer::push()
{
	stack.push_back(configuration(stack.back()));
}

void Renderer::pop()
{
	configuration &config = stack.back();
	configuration &old = *(stack.end() - 1);

	for (int i = 0; i < nRenderStates; i++)
	{
		if (config.dirty.renderstate[i])
		{
			ApplyRenderState((RenderState)i, old.state.renderstate[i]);
		}
	}

	if (config.dirty.alphafunc)
	{
		ApplyAlphaFunc(config.state.alphafunc, config.state.alphafef);
	}

	if (config.dirty.blendfunc)
	{
		ApplyBlendFunc(config.state.blendsrcFactor, config.state.blenddstFactor);
	}

	// ...

	stack.pop_back();
	arx_assert(!stack.empty());
}
