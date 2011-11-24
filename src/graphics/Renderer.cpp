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

	arx_assert(!RendererConfigurationStack.empty());
	RendererConfigurationStack.pop_back();
	arx_assert(RendererConfigurationStack.empty());
}

void Renderer::SetViewMatrix(const Vec3f & position, const Vec3f & dir, const Vec3f & up) {
	
	EERIEMATRIX mat;
	Util_SetViewMatrix(mat, position, dir, up);
	
	SetViewMatrix(mat);
}

Renderer::Renderer()
{
	// stack must always contain at least current state
	RendererConfiguration *config = new RendererConfiguration;
	memset(config, 0, sizeof(RendererConfiguration));
	RendererConfigurationStack.push_back(config);
}

void Renderer::PushRendererConfiguration()
{
	RendererConfiguration *old = RendererConfigurationStack.back();

	// allocate a new configuration state and copy the current configuration there
	RendererConfiguration *config = new RendererConfiguration;
	memcpy(&config->state, &old->state, sizeof(RendererConfiguration::state_struct));
	memset(&config->dirty, 0, sizeof(RendererConfiguration::dirty_flags));

	RendererConfigurationStack.push_back(config);
}

void Renderer::PopRendererConfiguration()
{
	arx_assert(!RendererConfigurationStack.empty());
	RendererConfiguration *config = RendererConfigurationStack.back();
	RendererConfigurationStack.pop_back();
	arx_assert(!RendererConfigurationStack.empty());
	RendererConfiguration *old = RendererConfigurationStack.back();

	for (int i = 0; i < nRenderStates; i++)
	{
		if (config->dirty.RenderState[i])
		{
			ApplyRenderState((RenderState)i, old->state.RenderState[i]);
		}
	}

	// ...

	delete config;
}
