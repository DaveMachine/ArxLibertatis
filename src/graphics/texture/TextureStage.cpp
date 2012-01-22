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

#include "graphics/texture/TextureStage.h"

#include "platform/Platform.h"

TextureStage::TextureStage(unsigned int stage) : mStage(stage) 
{ 
	// stack must always contain at least current state
	stack.push_back(configuration());
	configuration &config = stack.back();
	config.dirty.clear();
	config.state.clear();
}

void TextureStage::push()
{

	stack.push_back(configuration(stack.back()));
}

void TextureStage::pop()
{
	// current configuration
	configuration &config = stack.back();
	// stored configuration
	configuration &old = *(stack.end() - 1);

	// optimization -
	// check if a value has been modified, if so restore it from the stack
	// this prevents Apply() being called without even potential for a state change

	// TODO: could check to ensure state change did occur here, or in Set()

	// ...

	stack.pop_back();
	arx_assert(!stack.empty());
}

TextureStage::configuration::configuration()
{
	dirty.clear();
	state.clear();
}

TextureStage::configuration::configuration(const configuration &old)
{
	dirty.clear();
	state = old.state;
}
