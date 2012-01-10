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
/* Based on:
===========================================================================
ARX FATALIS GPL Source Code
Copyright (C) 1999-2010 Arkane Studios SA, a ZeniMax Media company.

This file is part of the Arx Fatalis GPL Source Code ('Arx Fatalis Source Code'). 

Arx Fatalis Source Code is free software: you can redistribute it and/or modify it under the terms of the GNU General Public 
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

Arx Fatalis Source Code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Arx Fatalis Source Code.  If not, see 
<http://www.gnu.org/licenses/>.

In addition, the Arx Fatalis Source Code is also subject to certain additional terms. You should have received a copy of these 
additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Arx 
Fatalis Source Code. If not, please request a copy in writing from Arkane Studios at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing Arkane Studios, c/o 
ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.
===========================================================================
*/
// Copyright (c) 1999-2000 ARKANE Studios SA. All rights reserved

#include "game/Spells.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <limits>
#include <map>
#include <set>
#include <utility>

#include "core/Application.h"
#include "core/Config.h"
#include "core/Core.h"
#include "core/GameTime.h"
#include "core/Localisation.h"

#include "game/Damage.h"
#include "game/Equipment.h"
#include "game/NPC.h"
#include "game/Player.h"
#include "game/Inventory.h"

#include "gui/Speech.h"
#include "gui/Menu.h"
#include "gui/Interface.h"
#include "gui/MiniMap.h"

#include "graphics/BaseGraphicsTypes.h"
#include "graphics/Color.h"
#include "graphics/Draw.h"
#include "graphics/GraphicsTypes.h"
#include "graphics/Math.h"
#include "graphics/Renderer.h"
#include "graphics/Vertex.h"
#include "graphics/data/Mesh.h"
#include "graphics/data/TextureContainer.h"
#include "graphics/effects/SpellEffects.h"
#include "graphics/particle/ParticleEffects.h"
#include "graphics/particle/ParticleSystem.h"
#include "graphics/spells/Spells01.h"
#include "graphics/spells/Spells02.h"
#include "graphics/spells/Spells03.h"
#include "graphics/spells/Spells04.h"
#include "graphics/spells/Spells05.h"
#include "graphics/spells/Spells06.h"
#include "graphics/spells/Spells07.h"
#include "graphics/spells/Spells09.h"
#include "graphics/spells/Spells10.h"

#include "input/Input.h"

#include "io/resource/ResourcePath.h"
#include "io/log/Logger.h"

#include "math/Angle.h"
#include "math/Vector2.h"
#include "math/Vector3.h"

#include "physics/Collisions.h"

#include "platform/Platform.h"
#include "platform/String.h"

#include "scene/Light.h"
#include "scene/Scene.h"
#include "scene/GameSound.h"
#include "scene/Interactive.h"

#include "script/Script.h"

using std::abs;
using std::string;

static const float DEC_FOCAL = 50.0f;
static const float IMPROVED_FOCAL = 320.0f;

void MakeSpCol();
extern long WILLRETURNTOCOMBATMODE;
extern long TRUE_PLAYER_MOUSELOOK_ON;
long passwall=0;
long WILLRETURNTOFREELOOK = 0;
long GLOBAL_MAGIC_MODE=1;
bool bPrecastSpell = false;
extern std::string LAST_FAILED_SEQUENCE;
enum ARX_SPELLS_RuneDirection
{
	AUP,
	AUPRIGHT,
	ARIGHT,
	ADOWNRIGHT,
	ADOWN,
	ADOWNLEFT,
	ALEFT,
	AUPLEFT
};
long sp_arm=0;
long cur_arm=0;
long cur_sos=0;
static void ApplyPasswall();
static void ApplySPArm();
static void ApplySPuw();
static void ApplySPRf();
static void ApplySPMax();
static void ApplySPWep();
static void ApplySPBow();
static void ApplyCurPNux();
static void ApplyCurMr();
static void ApplyCurSOS(); 
 
 
extern long FistParticles;
extern long ParticleCount;
extern long sp_max;
short uw_mode=0;
short uw_mode_pos=0;
extern long MAGICMODE;
extern INTERACTIVE_OBJ * CURRENT_TORCH;
extern float GLOBAL_SLOWDOWN;
extern void ARX_SPSound();
extern float sp_max_y[64];
extern Color sp_max_col[64];
extern char	sp_max_ch[64];
extern long sp_max_nb;
long cur_mega=0;
float sp_max_start = 0;
long sp_wep=0;

extern bool bRenderInCursorMode;

bool bOldLookToggle;
extern float SLID_START;

long BH_MODE = 0;
void EERIE_OBJECT_SetBHMode()
{
	if (BH_MODE)
		BH_MODE=0;
	else
	{
		BH_MODE=1;
		MakeCoolFx(&player.pos);
		MakeSpCol();
		strcpy(sp_max_ch,"!!!_Super-Deformed_!!!");
		sp_max_nb=strlen(sp_max_ch);
		sp_max_start=arxtime.get_updated();
			}
}
struct Scan {
	short SlotDir; 
};

struct SYMBOL_DRAW {
	unsigned long	starttime;
	Vec3f		lastpos;
	short			lasttim;
	short			duration;
	char			sequence[32];
	char			cPosStartX;
	char			cPosStartY;
};

extern bool FrustrumsClipSphere(EERIE_FRUSTRUM_DATA * frustrums,EERIE_SPHERE * sphere);
extern bool bGToggleCombatModeWithKey;
void ARX_INTERFACE_Combat_Mode(long i);

static float ARX_SPELLS_GetManaCost(Spell _lNumSpell,long _lNumSpellTab);

///////////////Spell Interpretation
SPELL spells[MAX_SPELLS];
short ARX_FLARES_broken(1);
long CurrSlot(1);
long CurrPoint(0);
long cur_mx=0;
long cur_pnux=0;
long cur_pom=0;
long cur_rf=0;
long cur_mr=0;
long cur_sm=0;
long cur_bh=0;

static float LASTTELEPORT(0.0F);
long snip=0;
Vec2s Lm;

static const long MAX_POINTS(200);
static Vec2s plist[MAX_POINTS];
std::string SpellMoves;
Rune SpellSymbol[MAX_SPELL_SYMBOLS];
size_t CurrSpellSymbol=0;

static Scan spell[MAX_SLOT + 1];

long lMaxSymbolDrawSizeX;
long lMaxSymbolDrawSizeY;

unsigned char ucFlick=0;

bool GetSpellPosition(Vec3f * pos,long i)
{
 

	switch (spells[i].type)
	{
				//*********************************************************************************************
		// LEVEL 1 SPELLS -----------------------------------------------------------------------------
		case SPELL_MAGIC_SIGHT: // Launching MAGIC_SIGHT			
		break;
		//----------------------------------------------------------------------------------------------
		case SPELL_MAGIC_MISSILE: // Launching MAGIC_MISSILE
		break;
		//----------------------------------------------------------------------------------------------
		case SPELL_IGNIT:// Launching IGNIT
		break;
		//---------------------------------------------------------------------------------------------
		case SPELL_DOUSE:// Launching DOUSE
		break;
		//---------------------------------------------------------------------------------------------
		case SPELL_ACTIVATE_PORTAL:// Launching ACTIVATE_PORTAL
		break;			
		//*************************************************************************************************
		// LEVEL 2 SPELLS -----------------------------------------------------------------------------
		case SPELL_HEAL:// Launching HEAL
		break;
		//---------------------------------------------------------------------------------------------
		case SPELL_DETECT_TRAP:// Launching DETECT_TRAP
		break;
		//---------------------------------------------------------------------------------------------
		case SPELL_ARMOR:// Launching ARMOR

			if (ValidIONum(spells[i].target))
			{
				*pos = inter.iobj[spells[i].target]->pos;
				return true;
			}

		break;
		//------------------------------------------------------------------------------------------------
		case SPELL_LOWER_ARMOR:// Launching LOWER_ARMOR

			if (ValidIONum(spells[i].target))
			{
				*pos = inter.iobj[spells[i].target]->pos;
				return true;
			}

		break;
		//------------------------------------------------------------------------------------------------
		case SPELL_HARM:// Launching HARM
		break;			
		//**********************************************************************************************
		// LEVEL 3 SPELLS -----------------------------------------------------------------------------
		case SPELL_SPEED:// Launching SPEED

			if (ValidIONum(spells[i].target))
			{
				*pos = inter.iobj[spells[i].target]->pos;
				return true;
			}

		break;
		//--------------------------------------------------------------------------------------------------
		case SPELL_DISPELL_ILLUSION:// Launching DISPELL_ILLUSION (REVEAL)
		break;
		//----------------------------------------------------------------------------------------------
		case SPELL_FIREBALL:// Launching FIREBALL
		break;
		//-------------------------------------------------------------------------------------------------
		case SPELL_CREATE_FOOD:// Launching CREATE_FOOD
		break;
		//----------------------------------------------------------------------------------------------
		case SPELL_ICE_PROJECTILE:// Launching ICE_PROJECTILE
		break;
		//***********************************************************************************************	
		// LEVEL 4 SPELLS -----------------------------------------------------------------------------
		case SPELL_BLESS:// Launching BLESS

			if (ValidIONum(spells[i].target))
			{
				*pos = inter.iobj[spells[i].target]->pos;
				return true;
			}

		break;
		//-----------------------------------------------------------------------------------------------
		case SPELL_DISPELL_FIELD:// Launching DISPELL_FIELD
		break;
		//-----------------------------------------------------------------------------------------------
		case SPELL_FIRE_PROTECTION:// Launching FIRE_PROTECTION

			if (ValidIONum(spells[i].target))
			{
				*pos = inter.iobj[spells[i].target]->pos;
				return true;
			}

		break;
		//----------------------------------------------------------------------------------------------
		case SPELL_COLD_PROTECTION:// Launching COLD_PROTECTION

			if (ValidIONum(spells[i].target))
			{
				*pos = inter.iobj[spells[i].target]->pos;
				return true;
			}

		break;
		//----------------------------------------------------------------------------------------------
		case SPELL_TELEKINESIS:// Launching TELEKINESIS
		break;
		//-----------------------------------------------------------------------------------------------
		case SPELL_CURSE:// Launching CURSE

			if (ValidIONum(spells[i].target))
			{
				*pos = inter.iobj[spells[i].target]->pos;
				return true;
			}

		break;
		//*********************************************************************
		// LEVEL 5 SPELLS -----------------------------------------------------------------------------
		case SPELL_RUNE_OF_GUARDING:
		break;
		//----------------------------------------------------------------------------
		case SPELL_LEVITATE:
		break;
		//----------------------------------------------------------------------------
		case SPELL_CURE_POISON:
		break;
		//----------------------------------------------------------------------------
		case SPELL_REPEL_UNDEAD:
		break;
		//----------------------------------------------------------------------------
		case SPELL_POISON_PROJECTILE:
		break;
		//***************************************************************************
		// LEVEL 6 -----------------------------------------------------------------------------
		case SPELL_RISE_DEAD:
		break;
		//----------------------------------------------------------------------------
		case SPELL_PARALYSE:

			if (ValidIONum(spells[i].target))
			{
				*pos = inter.iobj[spells[i].target]->pos;
				return true;
			}

		break;
		//----------------------------------------------------------------------------
		case SPELL_CREATE_FIELD:
		break;
		//----------------------------------------------------------------------------
		case SPELL_DISARM_TRAP:
		break;
		//----------------------------------------------------------------------------
		case SPELL_SLOW_DOWN:

			if (ValidIONum(spells[i].target))
			{
				*pos = inter.iobj[spells[i].target]->pos;
				return true;
			}

		break;
		//****************************************************************************************
		// LEVEL 7 SPELLS -----------------------------------------------------------------------------
		case SPELL_FLYING_EYE:
		{	
			*pos = eyeball.pos;
			return true;
		}	
		break;
		//----------------------------------------------------------------------------
		case SPELL_FIRE_FIELD:
			CSpellFx *pCSpellFX;
			pCSpellFX = spells[i].pSpellFx;

			if (pCSpellFX)
			{
				CFireField *pFireField = (CFireField *) pCSpellFX;
					
				*pos = pFireField->pos;
				return true;
			}

		break;
		//----------------------------------------------------------------------------
		case SPELL_ICE_FIELD:
		break;
		//----------------------------------------------------------------------------
		case SPELL_LIGHTNING_STRIKE:
		break;
		//----------------------------------------------------------------------------
		case SPELL_CONFUSE:

			if (ValidIONum(spells[i].target))
			{
				*pos = inter.iobj[spells[i].target]->pos;
				return true;
			}

		break;
		//*********************************************************************************
		// LEVEL 8 SPELLS -----------------------------------------------------------------------------
		case SPELL_INVISIBILITY:

			if (ValidIONum(spells[i].target))
			{
				*pos = inter.iobj[spells[i].target]->pos;
				return true;
			}

		break;
		//----------------------------------------------------------------------------
		case SPELL_MANA_DRAIN:				

			if (ValidIONum(spells[i].target))
			{
				*pos = inter.iobj[spells[i].target]->pos;
				return true;
			}

		break;
		//----------------------------------------------------------------------------
		case SPELL_EXPLOSION:
		break;
		//----------------------------------------------------------------------------
		case SPELL_ENCHANT_WEAPON:
		break;			
		//----------------------------------------------------------------------------
		case SPELL_LIFE_DRAIN:

			if (ValidIONum(spells[i].target))
			{
				*pos = inter.iobj[spells[i].target]->pos;
				return true;
			}

		break;
		//*****************************************************************************************
		// LEVEL 9 SPELLS -----------------------------------------------------------------------------
		case SPELL_SUMMON_CREATURE:
		break;
		//----------------------------------------------------------------------------
		case SPELL_NEGATE_MAGIC:
		break;
		//----------------------------------------------------------------------------
		case SPELL_INCINERATE:

			if (ValidIONum(spells[i].target))
			{
				*pos = inter.iobj[spells[i].target]->pos;
				return true;
			}

		break;
		//----------------------------------------------------------------------------
		case SPELL_MASS_PARALYSE:
		break;
		//----------------------------------------------------------------------------
		//********************************************************************************************
		// LEVEL 10 SPELLS -----------------------------------------------------------------------------
		case SPELL_MASS_LIGHTNING_STRIKE:
		break;
		//----------------------------------------------------------------------------
		case SPELL_CONTROL_TARGET:
		break;
		//----------------------------------------------------------------------------
		case SPELL_FREEZE_TIME:
		break;
		//----------------------------------------------------------------------------
		case SPELL_MASS_INCINERATE:
		break;
		//----------------------------------------------------------------------------
		case SPELL_TELEPORT:
		break;
		//----------------------------------------------------------------------------
		default: break;
	}

	if (ValidIONum(spells[i].caster))
	{
		*pos = inter.iobj[spells[i].caster]->pos;
		return true;
	}

	return false;
}

void LaunchAntiMagicField(long ident)
{
	for(size_t n = 0 ; n < MAX_SPELLS; n++) {
		if (	(spells[n].exist) 
			&&	(spells[ident].caster_level >= spells[n].caster_level)
			&&	((long)n != ident))
		{
			Vec3f pos; 
			GetSpellPosition(&pos,n);

			if(distSqr(pos, inter.iobj[spells[ident].caster]->pos) < square(600.f)) {
				if (spells[n].type==SPELL_CREATE_FIELD)
				{
					if ((spells[ident].caster==0) && (spells[n].caster==0))				
						spells[n].tolive=0;
				}
				else 	spells[n].tolive=0;
			}
		}
	}
}

//-----------------------------------------------------------------------------
void ARX_SPELLS_AddSpellOn(const long &caster, const long &spell)
{
	if (caster < 0 ||  spell < 0 || !inter.iobj[caster]) return;

	INTERACTIVE_OBJ *io = inter.iobj[caster];
	void *ptr;

	ptr = realloc(io->spells_on, sizeof(long) * (io->nb_spells_on + 1));

	if (!ptr) return;

	io->spells_on = (long *)ptr;
	io->spells_on[io->nb_spells_on] = spell;
	io->nb_spells_on++;
}

//-----------------------------------------------------------------------------
long ARX_SPELLS_GetSpellOn(const INTERACTIVE_OBJ * io, Spell spellid)
{
	if (!io) return -1;

	for (long i(0); i < io->nb_spells_on; i++)
	{		
		if (	(spells[io->spells_on[i]].type == spellid)
			&&	(spells[io->spells_on[i]].exist)	)
			return io->spells_on[i];
	}

	return -1;
}

//-----------------------------------------------------------------------------
void ARX_SPELLS_RemoveSpellOn(const long &caster, const long &spell)
{
	if (caster < 0 || spell < 0) return;

	INTERACTIVE_OBJ *io = inter.iobj[caster];

	if (!io || !io->nb_spells_on) return;

	if (io->nb_spells_on == 1) 
	{
		free(io->spells_on);
		io->spells_on = NULL;
		io->nb_spells_on = 0;
		return;
	}

	long i(0);

	for (; i < io->nb_spells_on; i++)
		if (io->spells_on[i] == spell) break;

	if ( i >= io->nb_spells_on) return;

	io->nb_spells_on--;
	memcpy(&io->spells_on[i], &io->spells_on[i + 1], sizeof(long) * (io->nb_spells_on - i));

	io->spells_on = (long *)realloc(io->spells_on, sizeof(long) * io->nb_spells_on);
}
void ARX_SPELLS_RemoveMultiSpellOn(long spell_id)
{
	for (long i=0;i<inter.nbmax;i++)
	{
		ARX_SPELLS_RemoveSpellOn(i,spells[spell_id].type);
	}
}
//-----------------------------------------------------------------------------
void ARX_SPELLS_RemoveAllSpellsOn(INTERACTIVE_OBJ *io)
{
	free(io->spells_on), io->spells_on = NULL, io->nb_spells_on = 0;
}

//-----------------------------------------------------------------------------
void ARX_SPELLS_RequestSymbolDraw(INTERACTIVE_OBJ *io, const string & name, float duration) {
	
	const char * sequence;
	int iPosX = 0;
	int iPosY = 0;

	if(name == "aam")              iPosX = 0, iPosY = 2, sequence = "6666";
	else if(name == "cetrius")     iPosX = 1, iPosY = 1, sequence = "33388886666";
	else if(name == "comunicatum") iPosX = 0, iPosY = 0, sequence = "6666622244442226666";
	else if(name == "cosum")       iPosX = 0, iPosY = 2, sequence = "66666222244448888";
	else if(name == "folgora")     iPosX = 0, iPosY = 3, sequence = "99993333";
	else if(name == "fridd")       iPosX = 0, iPosY = 4, sequence = "888886662222";
	else if(name == "kaom")        iPosX = 3, iPosY = 0, sequence = "44122366";
	else if(name == "mega")        iPosX = 2, iPosY = 4, sequence = "88888";
	else if(name == "morte")       iPosX = 0, iPosY = 2, sequence = "66666222";
	else if(name == "movis")       iPosX = 0, iPosY = 0, sequence = "666611116666";
	else if(name == "nhi")         iPosX = 4, iPosY = 2, sequence = "4444";
	else if(name == "rhaa")        iPosX = 2, iPosY = 0, sequence = "22222";
	else if(name == "spacium")     iPosX = 4, iPosY = 0, sequence = "44444222266688";
	else if(name == "stregum")     iPosX = 0, iPosY = 4, sequence = "8888833338888";
	else if(name == "taar")        iPosX = 0, iPosY = 1, sequence = "666222666";
	else if(name == "tempus")      iPosX = 0, iPosY = 4, sequence = "88886662226668866";
	else if(name == "tera")        iPosX = 0, iPosY = 3, sequence = "99922266";
	else if(name == "vista")       iPosX = 1, iPosY = 0, sequence = "333111";
	else if(name == "vitae")       iPosX = 0, iPosY = 2, sequence = "66666888";
	else if(name == "yok")         iPosX = 0, iPosY = 0, sequence = "222226666888";
	else if(name == "akbaa")       iPosX = 0, iPosY = 0, sequence = "22666772222";
	else return;

	io->symboldraw = (SYMBOL_DRAW *)realloc(io->symboldraw, sizeof(SYMBOL_DRAW));

	if (!io->symboldraw) return;

	SYMBOL_DRAW *sd = io->symboldraw;

	sd->duration = (short)std::max(1l, long(duration));
	strcpy(sd->sequence, sequence);

	sd->starttime = (unsigned long)(arxtime);
	sd->lasttim = 0;
	sd->lastpos.x = io->pos.x - EEsin(radians(MAKEANGLE(io->angle.b - 45.0F + iPosX*2))) * 60.0F;
	sd->lastpos.y = io->pos.y - 120.0F - iPosY*5;
	sd->lastpos.z = io->pos.z + EEcos(radians(MAKEANGLE(io->angle.b - 45.0F + iPosX*2))) * 60.0F;
	
	sd->cPosStartX = checked_range_cast<char>(iPosX);
	sd->cPosStartY = checked_range_cast<char>(iPosY);
	
	io->GameFlags &= ~GFLAG_INVISIBILITY;
}

static void ARX_SPELLS_RequestSymbolDraw2(INTERACTIVE_OBJ *io, Rune symb, float duration)
{
	const char * sequence;
	int iPosX = 0;
	int iPosY = 0;

	switch (symb)
	{
		case RUNE_AAM   :
			iPosX = 0, iPosY = 2, sequence = "6666";
			break;
		case RUNE_CETRIUS:
			iPosX = 0, iPosY = 1, sequence = "33388886666";
			break;
		case RUNE_COMUNICATUM:
			iPosX = 0, iPosY = 0, sequence = "6666622244442226666";
			break;
		case RUNE_COSUM:
			iPosX = 0, iPosY = 2, sequence = "66666222244448888";
			break;
		case RUNE_FOLGORA:
			iPosX = 0, iPosY = 3, sequence = "99993333";
			break;
		case RUNE_FRIDD:
			iPosX = 0, iPosY = 4, sequence = "888886662222";
			break;
		case RUNE_KAOM:
			iPosX = 3, iPosY = 0, sequence = "44122366";
			break;
		case RUNE_MEGA:
			iPosX = 2, iPosY = 4, sequence = "88888";
			break;
		case RUNE_MORTE:
			iPosX = 0, iPosY = 2, sequence = "66666222";
			break;
		case RUNE_MOVIS:
			iPosX = 0, iPosY = 0, sequence = "666611116666";
			break;
		case RUNE_NHI:
			iPosX = 4, iPosY = 2, sequence = "4444";
			break;
		case RUNE_RHAA:
			iPosX = 2, iPosY = 0, sequence = "22222";
			break;
		case RUNE_SPACIUM:
			iPosX = 4, iPosY = 0, sequence = "44444222266688";
			break;
		case RUNE_STREGUM:
			iPosX = 0, iPosY = 4, sequence = "8888833338888";
			break;
		case RUNE_TAAR:
			iPosX = 0, iPosY = 1, sequence = "666222666";
			break;
		case RUNE_TEMPUS:
			iPosX = 0, iPosY = 4, sequence = "88886662226668866";
			break;
		case RUNE_TERA:
			iPosX = 0, iPosY = 3, sequence = "99922266";
			break;
		case RUNE_VISTA:
			iPosX = 1, iPosY = 0, sequence = "333111";
			break;
		case RUNE_VITAE:
			iPosX = 0, iPosY = 2, sequence = "66666888";
			break;
		case RUNE_YOK:
			iPosX = 0, iPosY = 0, sequence = "222226666888";
			break;
		default:
			return;
	}

	SYMBOL_DRAW * ptr;
	ptr = (SYMBOL_DRAW *)realloc(io->symboldraw, sizeof(SYMBOL_DRAW));

	if (!ptr) return;

	io->symboldraw = ptr;

	SYMBOL_DRAW *sd = io->symboldraw;
	sd->duration = duration < 1.0F ? 1 : (short)(long)duration;
	strcpy(sd->sequence, sequence);
	sd->starttime = (unsigned long)(arxtime);
	sd->lasttim = 0;
	
	sd->lastpos.x = io->pos.x - EEsin(radians(MAKEANGLE(io->angle.b - 45.0F + iPosX*2))) * 60.0F;
	sd->lastpos.y = io->pos.y - 120.0F - iPosY*5;
	sd->lastpos.z = io->pos.z + EEcos(radians(MAKEANGLE(io->angle.b - 45.0F + iPosX*2))) * 60.0F;
	
	sd->cPosStartX = checked_range_cast<char>(iPosX);
	sd->cPosStartY = checked_range_cast<char>(iPosY);

	io->GameFlags &= ~GFLAG_INVISIBILITY;

}

//-----------------------------------------------------------------------------
void ARX_SPELLS_RequestSymbolDraw3(const char *_pcName,char *_pcRes)
{
	if		(!strcmp(_pcName, "aam"))		strcpy(_pcRes, "6666");
	else if (!strcmp(_pcName, "cetrius"))	strcpy(_pcRes, "33388886666");
	else if (!strcmp(_pcName, "comunicatum")) 	strcpy(_pcRes, "6666622244442226666");
	else if (!strcmp(_pcName, "cosum"))     strcpy(_pcRes, "66666222244448888");
	else if (!strcmp(_pcName, "folgora"))   strcpy(_pcRes, "99993333");
	else if (!strcmp(_pcName, "fridd"))		strcpy(_pcRes, "888886662222");
	else if (!strcmp(_pcName, "kaom"))		strcpy(_pcRes, "44122366");
	else if (!strcmp(_pcName, "mega"))		strcpy(_pcRes, "88888");
	else if (!strcmp(_pcName, "morte"))		strcpy(_pcRes, "66666222");
	else if (!strcmp(_pcName, "movis"))		strcpy(_pcRes, "666611116666");
	else if (!strcmp(_pcName, "nhi"))		strcpy(_pcRes, "4444");
	else if (!strcmp(_pcName, "rhaa"))		strcpy(_pcRes, "22222");
	else if (!strcmp(_pcName, "spacium"))	strcpy(_pcRes, "44444222266688");
	else if (!strcmp(_pcName, "stregum"))	strcpy(_pcRes, "8888833338888");
	else if (!strcmp(_pcName, "taar"))		strcpy(_pcRes, "666222666");
	else if (!strcmp(_pcName, "tempus"))	strcpy(_pcRes, "88886662226668866");
	else if (!strcmp(_pcName, "tera"))		strcpy(_pcRes, "99922266");
	else if (!strcmp(_pcName, "vista"))		strcpy(_pcRes, "333111");
	else if (!strcmp(_pcName, "vitae"))		strcpy(_pcRes, "66666888");
	else if (!strcmp(_pcName, "yok"))		strcpy(_pcRes, "222226666888");
	else if (!strcmp(_pcName, "akbaa"))		strcpy(_pcRes, "22666772222");
}

#define OFFSET_X 8*2//0
#define OFFSET_Y 6*2//0

//-----------------------------------------------------------------------------
void GetSymbVector(char c,Vec2s * vec)
{
	switch (c)
	{
		case '1' :
			vec->x = -OFFSET_X, vec->y =  OFFSET_Y;
			break;
		case '2' :
			vec->x =         0, vec->y =  OFFSET_Y;
			break;
		case '3' :
			vec->x =  OFFSET_X, vec->y =  OFFSET_Y;
			break;
		case '4' :
			vec->x = -OFFSET_X, vec->y =         0;
			break;
		case '6' :
			vec->x =  OFFSET_X, vec->y =         0;
			break;
		case '7' :
			vec->x = -OFFSET_X, vec->y = -OFFSET_Y;
			break;
		case '8' :
			vec->x =         0, vec->y = -OFFSET_Y;
			break;
		case '9' :
			vec->x =  OFFSET_X, vec->y = -OFFSET_Y;
			break;
		default  :
			vec->x =         0, vec->y =         0;
			break;
	}
}

static bool MakeSpellName(char * spell, Spell num) {
	
	// TODO(spells) use map
	
	switch (num)
	{
		// Level 1
		case SPELL_MAGIC_SIGHT           :
			strcpy(spell, "magic_sight");
			break;
		case SPELL_MAGIC_MISSILE         :
			strcpy(spell, "magic_missile");
			break;
		case SPELL_IGNIT                 :
			strcpy(spell, "ignit");
			break;
		case SPELL_DOUSE                 :
			strcpy(spell, "douse");
			break;
		case SPELL_ACTIVATE_PORTAL       :
			strcpy(spell, "activate_portal");
			break;

		// Level 2
		case SPELL_HEAL                  :
			strcpy(spell, "heal");
			break;
		case SPELL_DETECT_TRAP           :
			strcpy(spell, "detect_trap");
			break;
		case SPELL_ARMOR                 :
			strcpy(spell, "armor");
			break;
		case SPELL_LOWER_ARMOR           :
			strcpy(spell, "lower_armor");
			break;
		case SPELL_HARM                  :
			strcpy(spell, "harm");
			break;

		// Level 3
		case SPELL_SPEED                 :
			strcpy(spell, "speed");
			break;
		case SPELL_DISPELL_ILLUSION      :
			strcpy(spell, "dispell_illusion");
			break;
		case SPELL_FIREBALL              :
			strcpy(spell, "fireball");
			break;
		case SPELL_CREATE_FOOD           :
			strcpy(spell, "create_food");
			break;
		case SPELL_ICE_PROJECTILE        :
			strcpy(spell, "ice_projectile");
			break;

		// Level 4 
		case SPELL_BLESS                 :
			strcpy(spell, "bless");
			break;
		case SPELL_DISPELL_FIELD         :
			strcpy(spell, "dispell_field");
			break;
		case SPELL_FIRE_PROTECTION       :
			strcpy(spell, "fire_protection");
			break;
		case SPELL_TELEKINESIS           :
			strcpy(spell, "telekinesis");
			break;
		case SPELL_CURSE                 :
			strcpy(spell, "curse");
			break;
		case SPELL_COLD_PROTECTION       :
			strcpy(spell, "cold_protection");
			break;

		// Level 5 
		case SPELL_RUNE_OF_GUARDING      :
			strcpy(spell, "rune_of_guarding");
			break;
		case SPELL_LEVITATE              :
			strcpy(spell, "levitate");
			break;
		case SPELL_CURE_POISON           :
			strcpy(spell, "cure_poison");
			break;
		case SPELL_REPEL_UNDEAD          :
			strcpy(spell, "repel_undead");
			break;
		case SPELL_POISON_PROJECTILE     :
			strcpy(spell, "poison_projectile");
			break;

		// Level 6 
		case SPELL_RISE_DEAD             :
			strcpy(spell, "raise_dead");
			break;
		case SPELL_PARALYSE              :
			strcpy(spell, "paralyse");
			break;
		case SPELL_CREATE_FIELD          :
			strcpy(spell, "create_field");
			break;
		case SPELL_DISARM_TRAP           :
			strcpy(spell, "disarm_trap");
			break;
		case SPELL_SLOW_DOWN             :
			strcpy(spell, "slowdown");
			break;

		// Level 7  
		case SPELL_FLYING_EYE            :
			strcpy(spell, "flying_eye");
			break;
		case SPELL_FIRE_FIELD            :
			strcpy(spell, "fire_field");
			break;
		case SPELL_ICE_FIELD             :
			strcpy(spell, "ice_field");
			break;
		case SPELL_LIGHTNING_STRIKE      :
			strcpy(spell, "lightning_strike");
			break;
		case SPELL_CONFUSE               :
			strcpy(spell, "confuse");
			break;

		// Level 8
		case SPELL_INVISIBILITY          :
			strcpy(spell, "invisibility");
			break;
		case SPELL_MANA_DRAIN            :
			strcpy(spell, "mana_drain");
			break;
		case SPELL_EXPLOSION             :
			strcpy(spell, "explosion");
			break;
		case SPELL_ENCHANT_WEAPON        :
			strcpy(spell, "enchant_weapon");
			break;
		case SPELL_LIFE_DRAIN            :
			strcpy(spell, "life_drain");
			break;

		// Level 9
		case SPELL_SUMMON_CREATURE       :
			strcpy(spell, "summon_creature");
			break;
		case SPELL_FAKE_SUMMON		     :
			strcpy(spell, "fake_summon");
			break;
		case SPELL_NEGATE_MAGIC          :
			strcpy(spell, "negate_magic");
			break;
		case SPELL_INCINERATE            :
			strcpy(spell, "incinerate");
			break;
		case SPELL_MASS_PARALYSE         :
			strcpy(spell, "mass_paralyse");
			break;

		// Level 10
		case SPELL_MASS_LIGHTNING_STRIKE :
			strcpy(spell, "mass_lightning_strike");
			break;
		case SPELL_CONTROL_TARGET        :
			strcpy(spell, "control");
			break;
		case SPELL_FREEZE_TIME           :
			strcpy(spell, "freeze_time");
			break;
		case SPELL_MASS_INCINERATE       :
			strcpy(spell, "mass_incinerate");
			break;
		default :
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
void SPELLCAST_Notify(long num)
{
	if (num < 0) return;

	if ((size_t)num >= MAX_SPELLS) return;

	char spell[128];
	long source = spells[num].caster;

	if (MakeSpellName(spell,spells[num].type))
	{
		for (long i=0;i<inter.nbmax;i++) 
		{
			if (inter.iobj[i]!=NULL) 
			{
				if (source >= 0) EVENT_SENDER = inter.iobj[source];
				else EVENT_SENDER = NULL;

				char param[256];
				sprintf(param,"%s %ld",spell,(long)spells[num].caster_level);
				SendIOScriptEvent(inter.iobj[i], SM_SPELLCAST, param);
			}
		}	
	}	
}

//-----------------------------------------------------------------------------
void SPELLCAST_NotifyOnlyTarget(long num)
{
	if (num < 0) return;

	if ((size_t)num >= MAX_SPELLS) return;

	if(spells[num].target<0) return;

	char spell[128];
	long source = spells[num].caster;

	if (MakeSpellName(spell,spells[num].type))
	{
		if (source >= 0) EVENT_SENDER = inter.iobj[source];
		else EVENT_SENDER = NULL;

		char param[256];
		sprintf(param,"%s %ld",spell,(long)spells[num].caster_level);
		SendIOScriptEvent(inter.iobj[spells[num].target], SM_SPELLCAST, param);
	}	
}

//-----------------------------------------------------------------------------
void SPELLEND_Notify(long num)
{
	if(num < 0 || (size_t)num >= MAX_SPELLS) {
		return;
	}

	char spell[128];
	long source=spells[num].caster;

	if (spells[num].type==SPELL_CONFUSE)
	{
		if (ValidIONum(source))
			EVENT_SENDER = inter.iobj[source];
		else 
			EVENT_SENDER = NULL;

		if (ValidIONum(spells[num].target))
		{			
			if (MakeSpellName(spell,spells[num].type))
			{
				char param[128];
				INTERACTIVE_OBJ * targ= inter.iobj[spells[num].target];
				sprintf(param,"%s %ld",spell,(long)spells[num].caster_level);
				SendIOScriptEvent(targ,SM_SPELLEND,param);
			}
		}

		return;
	}

	// we only notify player spells end.
	if (MakeSpellName(spell,spells[num].type))
		for (long i=0;i<inter.nbmax;i++)
			if (inter.iobj[i])
			{
				char param[128];

				if (ValidIONum(source))
					EVENT_SENDER = inter.iobj[source];
				else 
					EVENT_SENDER = NULL;

				sprintf(param,"%s %ld",spell,(long)spells[num].caster_level);
				SendIOScriptEvent(inter.iobj[i],SM_SPELLEND,param);
			}
}

//-----------------------------------------------------------------------------
void ReCenterSequence(char *_pcSequence,int &_iMinX,int &_iMinY,int &_iMaxX,int &_iMaxY)
{
	int iSizeX=0,iSizeY=0;
	_iMinX=_iMinY=0;
	_iMaxX=_iMaxY=0;
	int iLenght=strlen(_pcSequence);

	for(int iI=0;iI<iLenght;iI++)
	{
		Vec2s es2dVector;
		GetSymbVector(_pcSequence[iI],&es2dVector);
		iSizeX+=es2dVector.x;
		iSizeY+=es2dVector.y;
		_iMinX=std::min(_iMinX,iSizeX);
		_iMinY=std::min(_iMinY,iSizeY);
		_iMaxX=std::max(_iMaxX,iSizeX);
		_iMaxY=std::max(_iMaxY,iSizeY);
	}
}

//-----------------------------------------------------------------------------
void ARX_SPELLS_UpdateSymbolDraw() {
	unsigned long curtime = (unsigned long)(arxtime);

	//1
	for (long i=0;i<inter.nbmax;i++)
	{
		INTERACTIVE_OBJ * io=inter.iobj[i];

		if (io) 
		{
			if (io->spellcast_data.castingspell != SPELL_NONE)
			{
				if (io->symboldraw==NULL)
				{
					long tst=0;

					if (!(io->spellcast_data.spell_flags & SPELLCAST_FLAG_NOANIM) &&  (io->ioflags & IO_NPC))
					{
						ANIM_USE * ause1=&io->animlayer[1];

						if (ause1->cur_anim==io->anims[ANIM_CAST_START]  && (ause1->flags & EA_ANIMEND)) 
						{
							FinishAnim(io,ause1->cur_anim);
							ANIM_Set(ause1,io->anims[ANIM_CAST_CYCLE]);
							tst=1;
						}
						else if (ause1->cur_anim==io->anims[ANIM_CAST_CYCLE]) tst=1;
						else if (ause1->cur_anim!=io->anims[ANIM_CAST_START])
							io->spellcast_data.castingspell = SPELL_NONE;
					}
					else tst=1;

					if ((io->spellcast_data.symb[0] != RUNE_NONE)  && tst )
					{
						Rune symb = io->spellcast_data.symb[0];

						for (long j=0;j<3;j++)
							io->spellcast_data.symb[j]=io->spellcast_data.symb[j+1];

						io->spellcast_data.symb[3] = RUNE_NONE;
						ARX_SPELLS_RequestSymbolDraw2(io, symb, (1000-(io->spellcast_data.spell_level*60))*std::max(io->speed_modif+io->basespeed,0.01f));
						io->GameFlags &=~GFLAG_INVISIBILITY;
					}
					else if (tst)// cast spell !!!
					{					
						io->GameFlags &=~GFLAG_INVISIBILITY;
						ARX_SPELLS_Launch(io->spellcast_data.castingspell,i,io->spellcast_data.spell_flags,io->spellcast_data.spell_level,io->spellcast_data.target,io->spellcast_data.duration);

						if (!(io->spellcast_data.spell_flags & SPELLCAST_FLAG_NOANIM)
								&&  (io->ioflags & IO_NPC))
						{
							ANIM_USE * ause1=&io->animlayer[1];
							AcquireLastAnim(io);
							FinishAnim(io,ause1->cur_anim);
							ANIM_Set(ause1,io->anims[ANIM_CAST]);
						}

						io->spellcast_data.castingspell = SPELL_NONE;
					}
				}
			}

			float rr=rnd();

			if (io->flarecount)
			{
				if (io->dynlight==-1) io->dynlight=(short)GetFreeDynLight();

				if (io->dynlight!=-1)
				{
					DynLight[io->dynlight].pos.x=io->pos.x-EEsin(radians(MAKEANGLE(io->angle.b-45.f)))*60.f;
					DynLight[io->dynlight].pos.y=io->pos.y-120.f;
					DynLight[io->dynlight].pos.z=io->pos.z+EEcos(radians(MAKEANGLE(io->angle.b-45.f)))*60.f;
					DynLight[io->dynlight].fallstart=140.f+(float)io->flarecount*0.333333f+rr*5.f;
					DynLight[io->dynlight].fallend=220.f+(float)io->flarecount*0.5f+rr*5.f;
					DynLight[io->dynlight].intensity=1.6f;
					DynLight[io->dynlight].exist=1;	
					DynLight[io->dynlight].rgb.r=0.01f*io->flarecount*2;	
					DynLight[io->dynlight].rgb.g=0.009f*io->flarecount*2;
					DynLight[io->dynlight].rgb.b=0.008f*io->flarecount*2;
				}
			}
			else if (io->dynlight>-1) 
			{
				DynLight[io->dynlight].exist=0;
				io->dynlight=-1;
			}

			if(io->symboldraw) {
				SYMBOL_DRAW * sd = inter.iobj[i]->symboldraw;
				long tim=curtime-sd->starttime;
 


				if (tim>sd->duration)
				{
					if (io->dynlight!=-1)
					{
						DynLight[io->dynlight].time_creation = (unsigned long)(arxtime);
						DynLight[io->dynlight].duration = 600; 
						io->dynlight=-1;
					}			

					free(io->symboldraw);
					io->symboldraw=NULL;
					continue;
				}

				long nbcomponents=strlen(sd->sequence);

				if (nbcomponents<=0)
				{
					free(io->symboldraw);
					io->symboldraw=NULL;
					continue;
				}

				float ti=((float)sd->duration/(float)nbcomponents);

				if (ti<=0) ti=1;

				Vec2s pos1, vect, old_pos;
				long newtime=tim;
				long oldtime=sd->lasttim;

				if (oldtime>sd->duration) oldtime=sd->duration;

				if (newtime>sd->duration) newtime=sd->duration;

				sd->lasttim=(short)tim;

				pos1.x = (short)subj.centerx -OFFSET_X*2 + sd->cPosStartX*OFFSET_X;
				pos1.y = (short)subj.centery -OFFSET_Y*2 + sd->cPosStartY*OFFSET_Y;

				float div_ti=1.f/ti;

				if (io != inter.iobj[0])
				{
					old_pos.x=pos1.x;
					old_pos.y=pos1.y;

					for (long j=0;j<nbcomponents;j++)
					{
						GetSymbVector(sd->sequence[j],&vect);
						vect.x += vect.x >> 1;
						vect.y += vect.y >> 1;

						if (oldtime<=ti)
						{							
							float ratio=(float)(oldtime)*div_ti;
							old_pos.x+=(short)(float)(ratio*(float)vect.x);
							old_pos.y+=(short)(float)(ratio*(float)vect.y);
							break;
						}

						old_pos.x+=vect.x;
						old_pos.y+=vect.y;
						oldtime-=(long)ti;
					}						

					for (int j=0;j<nbcomponents;j++)
					{
						GetSymbVector(sd->sequence[j],&vect);
						vect.x += vect.x >> 1;
						vect.y += vect.y >> 1;

						if (newtime<=ti)
						{
							float ratio=(float)(newtime)*div_ti;
							pos1.x+=(short)(float)(ratio*(float)vect.x);
							pos1.y+=(short)(float)(ratio*(float)vect.y);
							AddFlare(&pos1,0.1f,1,inter.iobj[i]);
							FlareLine(&old_pos,&pos1,inter.iobj[i]);
							break;
						}

						pos1.x+=vect.x;
						pos1.y+=vect.y;
						newtime-=(long)ti;
					}
				}
				else 
				{
					int iMinX,iMinY,iMaxX,iMaxY;
					int iSizeX,iSizeY;
					ReCenterSequence(sd->sequence,iMinX,iMinY,iMaxX,iMaxY);
					iSizeX=iMaxX-iMinX;
					iSizeY=iMaxY-iMinY;
					pos1.x = 97;
					pos1.y = 64;


					long lPosX	= (((513>>1)-lMaxSymbolDrawSizeX)>>1);
					long lPosY	= (313-(((313*3/4)-lMaxSymbolDrawSizeY)>>1));

					pos1.x = checked_range_cast<short>(pos1.x + lPosX);
					pos1.y = checked_range_cast<short>(pos1.y + lPosY);



					lPosX =  ((lMaxSymbolDrawSizeX-iSizeX)>>1);
					lPosY =  ((lMaxSymbolDrawSizeY-iSizeY)>>1);

					pos1.x = checked_range_cast<short>(pos1.x + lPosX);
					pos1.y = checked_range_cast<short>(pos1.y + lPosY);



					int iX = pos1.x-iMinX;
					int iY = pos1.y-iMinY;

					pos1.x = checked_range_cast<short>(iX);
					pos1.y = checked_range_cast<short>(iY);


					for (long j=0;j<nbcomponents;j++)
					{

						GetSymbVector(sd->sequence[j],&vect);

						if (newtime<ti)
						{
							float ratio = (float)(newtime) * div_ti;
							

							float fX = pos1.x + (ratio*vect.x)*0.5f;
							float fY = pos1.y + (ratio*vect.y)*0.5f; 

							pos1.x = checked_range_cast<short>(fX);
							pos1.y = checked_range_cast<short>(fY);


							Vec2s pos;
							pos.x=(short)(pos1.x*Xratio);	
							pos.y=(short)(pos1.y*Yratio);

							if (io == inter.iobj[0])
								AddFlare2(&pos,0.1f,1,inter.iobj[i]);
							else
								AddFlare(&pos,0.1f,1,inter.iobj[i]);


							break;
						}

						pos1.x+=vect.x;
						pos1.y+=vect.y;

						newtime-=(long)ti;
					}
				}				
			}
		}
	}
}

//-----------------------------------------------------------------------------
void ARX_SPELLS_ClearAllSymbolDraw()
{
	for (long i(0); i < inter.nbmax; i++) 
		if (inter.iobj[i] && inter.iobj[i]->symboldraw)
			free(inter.iobj[i]->symboldraw), inter.iobj[i]->symboldraw = NULL;
}

static void ARX_SPELLS_AnalyseSYMBOL() {
	
	long sm = atoi(SpellMoves);
	switch(sm) {
		
		// COSUM
		case 62148  :
		case 632148 :
		case 62498  :
		case 62748  :
		case 6248   :
				SpellSymbol[CurrSpellSymbol++] = RUNE_COSUM;

				if((size_t)CurrSpellSymbol >= MAX_SPELL_SYMBOLS) {
					CurrSpellSymbol = MAX_SPELL_SYMBOLS - 1;
				}

				ARX_SOUND_PlaySFX(SND_SYMB_COSUM);
			break;

		// COMUNICATUM
		case 632426 :
		case 627426 :
		case 634236 :
		case 624326 :
		case 62426  :
				SpellSymbol[CurrSpellSymbol++] = RUNE_COMUNICATUM;

				if((size_t)CurrSpellSymbol >= MAX_SPELL_SYMBOLS) {
					CurrSpellSymbol = MAX_SPELL_SYMBOLS - 1;
				}

				ARX_SOUND_PlaySFX(SND_SYMB_COMUNICATUM);
			break;

		// FOLGORA
		case 9823   :
		case 9232   :
		case 983    :
		case 963    :
		case 923    :
		case 932    :
		case 93     :
				SpellSymbol[CurrSpellSymbol++] = RUNE_FOLGORA;

				if((size_t)CurrSpellSymbol >= MAX_SPELL_SYMBOLS) {
					CurrSpellSymbol = MAX_SPELL_SYMBOLS - 1;
				}

				ARX_SOUND_PlaySFX(SND_SYMB_FOLGORA);
			break;

		// SPACIUM
		case 42368  :
		case 42678  :
		case 42698  :
		case 4268   :
				SpellSymbol[CurrSpellSymbol++] = RUNE_SPACIUM;

				if((size_t)CurrSpellSymbol >= MAX_SPELL_SYMBOLS) {
					CurrSpellSymbol = MAX_SPELL_SYMBOLS - 1;
				}

				ARX_SOUND_PlaySFX(SND_SYMB_SPACIUM);
			break;

		// TERA
		case 9826   :
		case 92126  :
		case 9264   :
		case 9296   :
		case 926    :
				SpellSymbol[CurrSpellSymbol++] = RUNE_TERA;

				if((size_t)CurrSpellSymbol >= MAX_SPELL_SYMBOLS) {
					CurrSpellSymbol = MAX_SPELL_SYMBOLS - 1;
				}

				ARX_SOUND_PlaySFX(SND_SYMB_TERA);
			break;

		// CETRIUS
		case 286   :
		case 3286  :
		case 23836 :
		case 38636 :
		case 2986  :
		case 2386  :
		case 386   :
				SpellSymbol[CurrSpellSymbol++] = RUNE_CETRIUS;

				if((size_t)CurrSpellSymbol >= MAX_SPELL_SYMBOLS) {
					CurrSpellSymbol = MAX_SPELL_SYMBOLS - 1;
				}

				ARX_SOUND_PlaySFX(SND_SYMB_CETRIUS);
			break;

		// RHAA
		case 28    :
		case 2     :
				SpellSymbol[CurrSpellSymbol++] = RUNE_RHAA;

				if((size_t)CurrSpellSymbol >= MAX_SPELL_SYMBOLS) {
					CurrSpellSymbol = MAX_SPELL_SYMBOLS - 1;
				}

				ARX_SOUND_PlaySFX(SND_SYMB_RHAA);
			break;

		// FRIDD
		case 98362	:
		case 8362	:
		case 8632	:
		case 8962	:
		case 862	:
				SpellSymbol[CurrSpellSymbol++] = RUNE_FRIDD;

				if((size_t)CurrSpellSymbol >= MAX_SPELL_SYMBOLS) {
					CurrSpellSymbol = MAX_SPELL_SYMBOLS - 1;
				}

				ARX_SOUND_PlaySFX(SND_SYMB_FRIDD);
			break;

		// KAOM
		case 41236	:
		case 23		:
		case 236	:
		case 2369	:
		case 136	:
		case 12369	:
		case 1236	:

				if ((cur_arm>=0) && (cur_arm & 1) )
				{
					cur_arm++;					

					if (cur_arm>20)
						ApplySPArm();
				}
				else
					cur_arm=-1;

				SpellSymbol[CurrSpellSymbol++] = RUNE_KAOM;

				if((size_t)CurrSpellSymbol >= MAX_SPELL_SYMBOLS) {
					CurrSpellSymbol = MAX_SPELL_SYMBOLS - 1;
				}

				ARX_SOUND_PlaySFX(SND_SYMB_KAOM);
			break;

		// STREGUM
		case 82328 :
		case 8328  :
		case 2328  :
		case 8938  :
		case 8238  :
		case 838   :
				SpellSymbol[CurrSpellSymbol++] = RUNE_STREGUM;

				if (CurrSpellSymbol>=MAX_SPELL_SYMBOLS) CurrSpellSymbol=MAX_SPELL_SYMBOLS-1;

				ARX_SOUND_PlaySFX(SND_SYMB_STREGUM);
			break;

		// MORTE
		case 628   :
		case 621   :
		case 62    :
				SpellSymbol[CurrSpellSymbol++] = RUNE_MORTE;

				if (CurrSpellSymbol>=MAX_SPELL_SYMBOLS) CurrSpellSymbol=MAX_SPELL_SYMBOLS-1;

				ARX_SOUND_PlaySFX(SND_SYMB_MORTE);
			break;

		// TEMPUS
		case 962686  :
		case 862686  :
		case 8626862 : 
				SpellSymbol[CurrSpellSymbol++] = RUNE_TEMPUS;

				if (CurrSpellSymbol>=MAX_SPELL_SYMBOLS) CurrSpellSymbol=MAX_SPELL_SYMBOLS-1;

				ARX_SOUND_PlaySFX(SND_SYMB_TEMPUS);
			break;

		// MOVIS
		case 6316:
		case 61236:
		case 6146:
		case 61216:
		case 6216:
		case 6416:
		case 62126:
		case 61264:
		case 6126:
		case 6136:
		case 616: 
				SpellSymbol[CurrSpellSymbol++] = RUNE_MOVIS;

				if (CurrSpellSymbol>=MAX_SPELL_SYMBOLS) CurrSpellSymbol=MAX_SPELL_SYMBOLS-1;

				ARX_SOUND_PlaySFX(SND_SYMB_MOVIS);
			break;

		// NHI
		case 46:
		case 4:
				SpellSymbol[CurrSpellSymbol++] = RUNE_NHI;

				if (CurrSpellSymbol>=MAX_SPELL_SYMBOLS) CurrSpellSymbol=MAX_SPELL_SYMBOLS-1;

				ARX_SOUND_PlaySFX(SND_SYMB_NHI);
			break;

		// AAM
		case 64:
		case 6:
				SpellSymbol[CurrSpellSymbol++] = RUNE_AAM;

				if (CurrSpellSymbol>=MAX_SPELL_SYMBOLS) CurrSpellSymbol=MAX_SPELL_SYMBOLS-1;

				ARX_SOUND_PlaySFX(SND_SYMB_AAM);
			break;
																		
		// YOK
		case 412369:
		case 2687:
		case 2698:
		case 2638:
		case 26386:
		case 2368:
		case 2689:
		case 268:
				SpellSymbol[CurrSpellSymbol++] = RUNE_YOK;

				if (CurrSpellSymbol>=MAX_SPELL_SYMBOLS) CurrSpellSymbol=MAX_SPELL_SYMBOLS-1;

				ARX_SOUND_PlaySFX(SND_SYMB_YOK);
			break;

		// TAAR
		case 6236:
		case 6264:
		case 626:
				SpellSymbol[CurrSpellSymbol++] = RUNE_TAAR;

				if (CurrSpellSymbol>=MAX_SPELL_SYMBOLS) CurrSpellSymbol=MAX_SPELL_SYMBOLS-1;

				ARX_SOUND_PlaySFX(SND_SYMB_TAAR);
			break;

		// MEGA
		case 82:
		case 8:

				if ((cur_arm>=0) && !(cur_arm & 1) )
				{
					cur_arm++;					
				}
				else
					cur_arm=-1;

				SpellSymbol[CurrSpellSymbol++] = RUNE_MEGA;

				if (CurrSpellSymbol>=MAX_SPELL_SYMBOLS) CurrSpellSymbol=MAX_SPELL_SYMBOLS-1;

				ARX_SOUND_PlaySFX(SND_SYMB_MEGA);
			break;

		// VISTA
		case 3614:
		case 361:
		case 341:
		case 3212:
		case 3214:
		case 312:
		case 314:
		case 321:
		case 31:
				SpellSymbol[CurrSpellSymbol++] = RUNE_VISTA;

				if (CurrSpellSymbol>=MAX_SPELL_SYMBOLS) CurrSpellSymbol=MAX_SPELL_SYMBOLS-1;

				ARX_SOUND_PlaySFX(SND_SYMB_VISTA);
			break;

		// VITAE
		case 698:
		case 68:
			SpellSymbol[CurrSpellSymbol++] = RUNE_VITAE;

			if (CurrSpellSymbol>=MAX_SPELL_SYMBOLS) CurrSpellSymbol=MAX_SPELL_SYMBOLS-1;

			ARX_SOUND_PlaySFX(SND_SYMB_VITAE);
			break;

//--------------------------------------------------------------------------------------------------------------------

			// Special UW mode
		case 238:
		case 2398:
		case 23898:
		case 236987:
		case 23698:
				
			if (uw_mode_pos == 0) uw_mode_pos++;
		
			goto failed; 
		break;
		case 2382398:
		case 2829:
		case 23982398:
		case 39892398:
		case 2398938:
		case 28239898:
		case 238982398:
		case 238923898:
		case 28982398:
		case 3923989:
		case 292398:
		case 398329:
		case 38923898:
		case 2398289:
		case 289823898:
		case 2989238:
		case 29829:
		case 2389823898u:
		case 2393239:
		case 38239:
		case 2982323989u:
		case 3298232329u:
		case 239829:
		case 2898239:
		case 28982898:
		case 389389:
		case 3892389:
		case 289289:
		case 289239:
		case 239289:
		case 2989298:
		case 2392398:
		case 238929:
		case 28923898:
		case 2929:
		case 2398298:
		case 239823898:
		case 28238:
		case 2892398:
		case 28298:
		case 298289:
		case 38929:
		case 2389238989u:
		case 289298989:
		case 23892398:
		case 238239:
		case 29298:
		case 2329298:
		case 232389829:
		case 2389829:
		case 239239:
		case 282398:
		case 2398982398u:
		case 2389239:
		case 2929898:
		case 3292398:
		case 23923298:
		case 23898239:
		case 3232929:
		case 2982398:
		case 238298:
		case 3939:

			if (uw_mode_pos == 1)
			{
				ApplySPuw();
			}

			goto failed; 
		break;
		case 161:
		case 1621:
		case 1261:

			if (cur_sm==0) cur_sm++;

			if (cur_bh==0) cur_bh++;

			if (cur_bh==2) cur_bh++;

			if (cur_sos==0) cur_sos++;

			if (cur_sos == 2)
			{
				cur_sos = 0;
				ApplyCurSOS();
			}

			goto failed;
			break;
		case 83614:
		case 8361:
		case 8341:
		case 83212:
		case 83214:
		case 8312:
		case 8314:
		case 8321:
		case 831:
		case 82341:
		case 834:
		case 823:
		case 8234:
		case 8231:  

			if (cur_pom==0) cur_pom++; 

			if (cur_pnux==0) cur_pnux++;

			if (cur_pnux==2) cur_pnux++;			

			if (cur_bh == 1)
			{
				cur_bh++;
			}

			if (cur_bh == 3)
			{
				cur_bh = 0;
				EERIE_OBJECT_SetBHMode();
			}

			goto failed;				  
			break;
		break;

		case 83692:
		case 823982:
		case 83982:
		case 82369892:
		case 82392:
		case 83892:
		case 823282:
		case 8392:
		{
			if (cur_sm == 2)
			{
				cur_sm++;
				ApplySPBow();
			}

			if (cur_mx == 0)
			{
				cur_mx = 1;
			}

			if (cur_mr == 0)
			{
				cur_mr = 1;
			}

			if (cur_pom == 2)
			{
				cur_pom++;
				ApplySPWep();
			}

				goto failed;
			}
			break;			
		case 98324:
		case 92324:
		case 89324:
		case 9324:
		case 9892324:
		case 9234:
		case 934:
		{
			if (cur_mr == 1)
			{
				cur_mr = 2;
				MakeCoolFx(&player.pos);
			}

			if (cur_mx == 1)
			{
				cur_mx = 2;
				MakeCoolFx(&player.pos);
			}

			if (cur_rf == 1)
			{
				cur_rf = 2;
				MakeCoolFx(&player.pos);
			}

				if (cur_sm==1) cur_sm++;

				goto failed;
			}
			break;
		case 3249:
		case 2349:
		case 323489:
		case 23249:
		case 3489:
		case 32498:
		case 349:
		{
			if (cur_mx == 2)
			{
				cur_mx = 3;
				ApplySPMax();
			}

				goto failed;				
			}
		break;

		case 26:
		{
			if (cur_pnux == 1)
			{
				cur_pnux++;
				   }

			if (cur_pnux == 3)
		{
				cur_pnux++;
				ApplyCurPNux();
			}

			goto failed;
		}
		break;
		case 9232187:
		case 93187:
		case 9234187:
		case 831878:
		case 923187:
		case 932187:
		case 93217:
		case 9317:
		{
			if (cur_pom==1) cur_pom++; 

			if (cur_sos==1) cur_sos++;

			goto failed;
		}
		break;
		case 82313:
		case 8343:
		case 82343:
		case 83413:
		case 8313:
		{
			if (cur_mr == 2)
			{
				cur_mr = 3;
				MakeCoolFx(&player.pos);
				ApplyCurMr();
			}

			if (cur_rf == 0)
			{
				cur_rf = 1;
			}

			goto failed;
			break;
		}
		case 86:

			if (cur_rf == 2)
		{
				cur_rf = 3;
				MakeCoolFx(&player.pos);
				ApplySPRf();
		}

			goto failed;
			break;

		case 626262: 
		{
			passwall++;

			if (passwall==3)
			{
				passwall=0;
				ApplyPasswall(); 
			}
		}
		break;
		case 828282: 
		{
			player.skin++;

			if ((player.skin==4) && (rnd()<0.9f))
				player.skin++;

			if (player.skin>5)
				player.skin=0;

			ARX_EQUIPMENT_RecreatePlayerMesh();
			 goto failed; 
		}
		break;

		default:
		{
		failed:
			;
			std::string tex;

			if (SpellMoves.length()>=127)
				SpellMoves.resize(127);

			LAST_FAILED_SEQUENCE = SpellMoves;

			LogDebug("Unknown Symbol - " + SpellMoves);
		}
	}

	bPrecastSpell = false;

	// wanna precast?
	if (GInput->actionPressed(CONTROLS_CUST_STEALTHMODE))
	{
		bPrecastSpell = true;
	}
}

struct SpellDefinition {
	SpellDefinition * next[RUNE_COUNT];
	Spell spell;
	SpellDefinition() : spell(SPELL_NONE) {
		for(size_t i = 0; i < RUNE_COUNT; i++) {
			next[i] = NULL;
		}
	}
};

static SpellDefinition definedSpells;
typedef std::map<string, Spell> SpellNames;
static SpellNames spellNames;

static void addSpell(const Rune symbols[MAX_SPELL_SYMBOLS], Spell spell, const string & name) {
	
	typedef std::pair<SpellNames::const_iterator, bool> Res;
	Res res = spellNames.insert(std::make_pair(name, spell));
	if(!res.second) {
		LogWarning << "duplicate spell name: " + name;
	}
	
	if(symbols[0] == RUNE_NONE) {
		return;
	}
	
	SpellDefinition * def = &definedSpells;
	
	for(size_t i = 0; i < MAX_SPELL_SYMBOLS; i++) {
		if(symbols[i] == RUNE_NONE) {
			break;
		}
		arx_assert(symbols[i] >= 0 && (size_t)symbols[i] < RUNE_COUNT);
		if(def->next[symbols[i]] == NULL) {
			def->next[symbols[i]] = new SpellDefinition();
		}
		def = def->next[symbols[i]];
	}
	
	arx_assert(def->spell == SPELL_NONE);
	
	def->spell = spell;
}

static Spell getSpell(const Rune symbols[MAX_SPELL_SYMBOLS]) {
	
	const SpellDefinition * def = &definedSpells;
	
	for(size_t i = 0; i < MAX_SPELL_SYMBOLS; i++) {
		if(symbols[i] == RUNE_NONE) {
			break;
		}
		arx_assert(symbols[i] >= 0 && (size_t)symbols[i] < RUNE_COUNT);
		if(def->next[symbols[i]] == NULL) {
			return SPELL_NONE;
		}
		def = def->next[symbols[i]];
	}
	
	return def->spell;
}

Spell GetSpellId(const string & spell) {
	
	SpellNames::const_iterator it = spellNames.find(spell);
	
	return (it == spellNames.end()) ? SPELL_NONE : it->second;
}

struct RawSpellDefinition {
	Rune symbols[MAX_SPELL_SYMBOLS];
	Spell spell;
	std::string name;
};

// TODO move to external file
static const RawSpellDefinition allSpells[] = {
	{{RUNE_RHAA, RUNE_STREGUM, RUNE_VITAE, RUNE_NONE}, SPELL_CURSE, "curse"}, // level 4
	{{RUNE_RHAA, RUNE_TEMPUS, RUNE_NONE}, SPELL_FREEZE_TIME, "freeze_time"}, // level 10
	{{RUNE_RHAA, RUNE_KAOM, RUNE_NONE}, SPELL_LOWER_ARMOR, "lower_armor"}, // level 2
	{{RUNE_RHAA, RUNE_MOVIS, RUNE_NONE}, SPELL_SLOW_DOWN, "slowdown"}, // level 6
	{{RUNE_RHAA, RUNE_VITAE, RUNE_NONE}, SPELL_HARM, "harm"}, // level 2
	{{RUNE_RHAA, RUNE_VISTA, RUNE_NONE}, SPELL_CONFUSE, "confuse"}, // level 7
	{{RUNE_MEGA, RUNE_NHI, RUNE_MOVIS, RUNE_NONE}, SPELL_MASS_PARALYSE, "mass_paralyse"}, // level 9
	{{RUNE_MEGA, RUNE_KAOM, RUNE_NONE}, SPELL_ARMOR, "armor"}, // level 2
	{{RUNE_MEGA, RUNE_VISTA, RUNE_NONE}, SPELL_MAGIC_SIGHT, "magic_sight"}, // level 1
	{{RUNE_MEGA, RUNE_VITAE, RUNE_NONE}, SPELL_HEAL, "heal"}, // level 2
	{{RUNE_MEGA, RUNE_MOVIS, RUNE_NONE}, SPELL_SPEED, "speed"}, // level 3
	{{RUNE_MEGA, RUNE_STREGUM, RUNE_VITAE, RUNE_NONE}, SPELL_BLESS, "bless"}, // level 4
	{{RUNE_MEGA, RUNE_STREGUM, RUNE_COSUM, RUNE_NONE}, SPELL_ENCHANT_WEAPON, "enchant_weapon"}, // level 8
	{{RUNE_MEGA, RUNE_AAM, RUNE_MEGA, RUNE_YOK, RUNE_NONE}, SPELL_MASS_INCINERATE, "mass_incinerate"}, // level 10
	{{RUNE_MEGA, RUNE_SPACIUM, RUNE_NONE}, SPELL_ACTIVATE_PORTAL, "activate_portal"}, // level ?
	{{RUNE_MEGA, RUNE_SPACIUM, RUNE_MOVIS, RUNE_NONE}, SPELL_LEVITATE, "levitate"}, // level 5
	{{RUNE_NHI, RUNE_MOVIS, RUNE_NONE}, SPELL_PARALYSE, "paralyse"}, // level 6
	{{RUNE_NHI, RUNE_CETRIUS, RUNE_NONE}, SPELL_CURE_POISON, "cure_poison"}, // level 5
	{{RUNE_NHI, RUNE_YOK, RUNE_NONE}, SPELL_DOUSE, "douse"}, // level 1
	{{RUNE_NHI, RUNE_STREGUM, RUNE_VISTA, RUNE_NONE}, SPELL_DISPELL_ILLUSION, "dispell_illusion"}, // level 3
	{{RUNE_NHI, RUNE_STREGUM, RUNE_SPACIUM, RUNE_NONE}, SPELL_NEGATE_MAGIC, "negate_magic"}, // level 9
	{{RUNE_NHI, RUNE_SPACIUM, RUNE_NONE}, SPELL_DISPELL_FIELD, "dispell_field"}, // level 4
	{{RUNE_NHI, RUNE_MORTE, RUNE_COSUM, RUNE_NONE}, SPELL_DISARM_TRAP, "disarm_trap"}, // level 6
	{{RUNE_NHI, RUNE_VISTA, RUNE_NONE}, SPELL_INVISIBILITY, "invisibility"}, // level ?
	{{RUNE_VISTA, RUNE_MOVIS, RUNE_NONE}, SPELL_FLYING_EYE, "flying_eye"}, // level 7
	{{RUNE_MORTE, RUNE_KAOM, RUNE_NONE}, SPELL_REPEL_UNDEAD, "repel_undead"}, // level 5
	{{RUNE_MORTE, RUNE_COSUM, RUNE_VISTA, RUNE_NONE}, SPELL_DETECT_TRAP, "detect_trap"}, // level 2
	{{RUNE_MOVIS, RUNE_COMUNICATUM, RUNE_NONE}, SPELL_CONTROL_TARGET, "control"}, // level 10
	{{RUNE_STREGUM, RUNE_MOVIS, RUNE_NONE}, SPELL_MANA_DRAIN, "mana_drain"}, // level 8
	{{RUNE_AAM, RUNE_MEGA, RUNE_YOK, RUNE_NONE}, SPELL_INCINERATE, "incinerate"}, // level 9
	{{RUNE_AAM, RUNE_MEGA, RUNE_MORTE, RUNE_NONE}, SPELL_EXPLOSION, "explosion"}, // level 8
	{{RUNE_AAM, RUNE_KAOM, RUNE_SPACIUM, RUNE_NONE}, SPELL_CREATE_FIELD, "create_field"}, // level 6
	{{RUNE_AAM, RUNE_MORTE, RUNE_VITAE, RUNE_NONE}, SPELL_RISE_DEAD, "raise_dead"}, // level 6
	{{RUNE_AAM, RUNE_MORTE, RUNE_COSUM, RUNE_NONE}, SPELL_RUNE_OF_GUARDING, "rune_of_guarding"}, // level 5
	{{RUNE_AAM, RUNE_VITAE, RUNE_TERA, RUNE_NONE}, SPELL_SUMMON_CREATURE, "summon_creature"}, // level 9
	{{RUNE_AAM, RUNE_VITAE, RUNE_COSUM, RUNE_NONE}, SPELL_CREATE_FOOD, "create_food"}, // level 3
	{{RUNE_AAM, RUNE_FOLGORA, RUNE_TAAR, RUNE_NONE}, SPELL_LIGHTNING_STRIKE, "lightning_strike"}, // level 7
	{{RUNE_AAM, RUNE_FOLGORA, RUNE_SPACIUM, RUNE_NONE}, SPELL_MASS_LIGHTNING_STRIKE, "mass_lightning_strike"}, // level 10
	{{RUNE_AAM, RUNE_YOK, RUNE_NONE}, SPELL_IGNIT, "ignit"}, // level 1
	{{RUNE_AAM, RUNE_YOK, RUNE_SPACIUM, RUNE_NONE}, SPELL_FIRE_FIELD, "fire_field"}, // level 7
	{{RUNE_AAM, RUNE_YOK, RUNE_TAAR, RUNE_NONE}, SPELL_FIREBALL, "fireball"}, // level 3
	{{RUNE_AAM, RUNE_FRIDD, RUNE_SPACIUM, RUNE_NONE}, SPELL_ICE_FIELD, "ice_field"}, // level 7
	{{RUNE_AAM, RUNE_FRIDD, RUNE_TAAR, RUNE_NONE}, SPELL_ICE_PROJECTILE, "ice_projectile"}, // level 3
	{{RUNE_AAM, RUNE_CETRIUS, RUNE_TAAR, RUNE_NONE}, SPELL_POISON_PROJECTILE, "poison_projectile"}, // level 5
	{{RUNE_AAM, RUNE_TAAR, RUNE_NONE}, SPELL_MAGIC_MISSILE, "magic_missile"}, // level 1
	{{RUNE_YOK, RUNE_KAOM, RUNE_NONE}, SPELL_FIRE_PROTECTION, "fire_protection"}, // level 4
	{{RUNE_FRIDD, RUNE_KAOM, RUNE_NONE}, SPELL_COLD_PROTECTION, "cold_protection"}, // level 4
	{{RUNE_VITAE, RUNE_MOVIS, RUNE_NONE}, SPELL_LIFE_DRAIN, "life_drain"}, // level 8
	{{RUNE_SPACIUM, RUNE_COMUNICATUM, RUNE_NONE}, SPELL_TELEKINESIS, "telekinesis"}, // level 4
	{{RUNE_NONE}, SPELL_FAKE_SUMMON, "fake_summon"}
};

//! Plays the sound of Fizzling spell
static void ARX_SPELLS_Fizzle(long num) {
	if(num < 0) {
		ARX_SOUND_PlaySFX(SND_MAGIC_FIZZLE); // player fizzle
	} else {
		spells[num].tolive = 0;
		
		if(spells[num].caster >= 0) {
			ARX_SOUND_PlaySFX(SND_MAGIC_FIZZLE, &spells[num].caster_pos);
		}
	}
}

static bool ARX_SPELLS_AnalyseSPELL() {
	
	long caster = 0; // Local Player
	SpellcastFlags flags = 0;
	
	if(GInput->actionPressed(CONTROLS_CUST_STEALTHMODE) || bPrecastSpell) {
		flags |= SPELLCAST_FLAG_PRECAST;
	}
	
	bPrecastSpell = false;
	
	Spell spell;
	
	if(SpellSymbol[0] == RUNE_MEGA && SpellSymbol[1] == RUNE_MEGA
	   && SpellSymbol[2] == RUNE_MEGA && SpellSymbol[3] == RUNE_AAM
	   && SpellSymbol[4] == RUNE_VITAE && SpellSymbol[5] == RUNE_TERA) {
		cur_mega = 10;
		spell = SPELL_SUMMON_CREATURE;
	} else {
		spell = getSpell(SpellSymbol);
	}
	
	if(spell == SPELL_NONE) {
		
		ARX_SPELLS_Fizzle(-1);
		
		if(player.SpellToMemorize.bSpell) {
			CurrSpellSymbol = 0;
			player.SpellToMemorize.bSpell = false;
		}
		
		return false;
	}
	
	return ARX_SPELLS_Launch(spell, caster, flags);
	
}


bool No_MagicAllowed()
{
	ARX_SOUND_PlaySFX(SND_MAGIC_FIZZLE);
	return false;
}
extern long PLAYER_PARALYSED;


static void ARX_SPELLS_Analyse() {
	
	unsigned char dirs[MAX_POINTS];
	unsigned char lastdir = 255;
	long cdir = 0;

	for(long i = 1; i < CurrPoint ; i++) {
		
		Vec2s d = plist[i-1] - plist[i];
		
		if(d.lengthSqr() > 100) {
			
			float a = (float)abs(d.x);
			float b = (float)abs(d.y);
			
			if(b != 0.f && a / b > 0.4f && a / b < 2.5f) {
				// Diagonal movemement.
				
				if(d.x < 0 && d.y < 0) {
					if(lastdir != ADOWNRIGHT) {
						lastdir = dirs[cdir++] = ADOWNRIGHT;
					}
				} else if(d.x > 0 && d.y < 0) {
					if(lastdir != ADOWNLEFT) {
						lastdir = dirs[cdir++] = ADOWNLEFT;
					}
				} else if(d.x < 0 && d.y > 0) {
					if(lastdir != AUPRIGHT) {
						lastdir = dirs[cdir++] = AUPRIGHT;
					}
				} else if(d.x > 0 && d.y > 0) {
					if(lastdir != AUPLEFT) {
						lastdir = dirs[cdir++] = AUPLEFT;
					}
				}
				
			} else if(a > b) {
				// Horizontal movement.
				
				if(d.x < 0) {
					if(lastdir != ARIGHT) {
						lastdir = dirs[cdir++] = ARIGHT;
					}
				} else {
					if(lastdir != ALEFT) {
						lastdir = dirs[cdir++] = ALEFT;
					}
				}
				
			} else {
				// Vertical movement.
				
				if(d.y < 0) {
					if(lastdir != ADOWN) {
						lastdir = dirs[cdir++] = ADOWN;
					}
				} else {
					if(lastdir != AUP) {
						lastdir = dirs[cdir++] = AUP;
					}
				}
			}
		}
	}

	SpellMoves.clear();
	
	if ( cdir > 0 )
	{

		for (long i = 0 ; i < cdir ; i++ )
		{
			switch ( dirs[i] )
			{
				case AUP:
					spell[CurrSlot].SlotDir = 0;
					SpellMoves += "8"; //uses PAD values
					break;

				case ADOWN:
					spell[CurrSlot].SlotDir = 4;
					SpellMoves += "2";
					break;

				case ALEFT:
					spell[CurrSlot].SlotDir = 6;
					SpellMoves += "4";
					break;

				case ARIGHT:
					spell[CurrSlot].SlotDir = 2;
					SpellMoves += "6";
					break;

				case AUPRIGHT:
					spell[CurrSlot].SlotDir = 1;
					SpellMoves += "9";
					break;

				case ADOWNRIGHT:
					spell[CurrSlot].SlotDir = 3;
					SpellMoves += "3";
					break;

				case AUPLEFT:
					spell[CurrSlot].SlotDir = 7;
					SpellMoves += "7";
					break;

				case ADOWNLEFT:
					spell[CurrSlot].SlotDir = 5;
					SpellMoves += "1";
					break;
			}
		}
	}
}

void ARX_SPELLS_ManageMagic()
{
	if (ARXmenu.currentmode!=AMCM_OFF)
		return;

	INTERACTIVE_OBJ * io=inter.iobj[0];

	if (!io) return;

	if ((io->animlayer[1].cur_anim == io->anims[ANIM_BARE_UNREADY]) ||
		(io->animlayer[1].cur_anim == io->anims[ANIM_DAGGER_UNREADY_PART_1]) ||
		(io->animlayer[1].cur_anim == io->anims[ANIM_1H_UNREADY_PART_1]) ||
		(io->animlayer[1].cur_anim == io->anims[ANIM_2H_UNREADY_PART_1]) ||
		(io->animlayer[1].cur_anim == io->anims[ANIM_MISSILE_UNREADY_PART_1]) ||
		(io->animlayer[1].cur_anim == io->anims[ANIM_DAGGER_UNREADY_PART_2]) ||
		(io->animlayer[1].cur_anim == io->anims[ANIM_1H_UNREADY_PART_2]) ||
		(io->animlayer[1].cur_anim == io->anims[ANIM_2H_UNREADY_PART_2]) ||
		(io->animlayer[1].cur_anim == io->anims[ANIM_MISSILE_UNREADY_PART_2]))
		return;

	snip++;

	if ((!(player.Current_Movement & PLAYER_CROUCH)) && (!BLOCK_PLAYER_CONTROLS && 
		(GInput->actionPressed(CONTROLS_CUST_MAGICMODE))) && (!PLAYER_PARALYSED))
	{
		
		if (player.Interface & INTER_COMBATMODE)
		{
			WILLRETURNTOCOMBATMODE=1;

			ARX_INTERFACE_Combat_Mode(0);
			bGToggleCombatModeWithKey=false;

			
			ResetAnim(&inter.iobj[0]->animlayer[1]);
			inter.iobj[0]->animlayer[1].flags&=~EA_LOOP;
		}

		if ((TRUE_PLAYER_MOUSELOOK_ON))
		{
			WILLRETURNTOFREELOOK = 1;
			TRUE_PLAYER_MOUSELOOK_ON &= ~1;
		}

		if (player.doingmagic!=2)
		{
			player.doingmagic=2;

			if (inter.iobj[0]->anims[ANIM_CAST_START])
			{
				AcquireLastAnim(inter.iobj[0]);
				ANIM_Set(&inter.iobj[0]->animlayer[1],inter.iobj[0]->anims[ANIM_CAST_START]);
				MAGICMODE = 1;
			}
		}
		
		if (snip >= 2)
		{	
			if ((!EERIEMouseButton & 1) && (ARX_FLARES_broken==0)) // TODO should this be !(EERIEMouseButton & 1)?
			{
				ARX_FLARES_broken=2;
				PIPOrgb++;

				if (PIPOrgb>2) PIPOrgb=0;			
			}
			
			if (EERIEMouseButton & 1)
			{
				Vec2s pos,pos2;
				pos.x = DANAEMouse.x; 
				pos.y = DANAEMouse.y;
				extern long TRUE_PLAYER_MOUSELOOK_ON;
				
				if (TRUE_PLAYER_MOUSELOOK_ON)
				{
					pos.x = MemoMouse.x;
					pos.y = MemoMouse.y;
				}

				pos2.x=Lm.x;
				pos2.y=Lm.y;

				if (!ARX_FLARES_broken) FlareLine(&pos2,&pos);

				if (rnd()>0.6) AddFlare(&pos,1.f,-1);
				else AddFlare(&pos,1.f,3);

				OPIPOrgb=PIPOrgb;
				Lm.x = DANAEMouse.x; 
				Lm.y = DANAEMouse.y;
				
				if (TRUE_PLAYER_MOUSELOOK_ON)
				{
					Lm.x = MemoMouse.x;
					Lm.y = MemoMouse.y;
				}

				ARX_FLARES_broken=0;
				
				if (!ARX_SOUND_IsPlaying(SND_MAGIC_DRAW))
					ARX_SOUND_PlaySFX(SND_MAGIC_DRAW, NULL, 1.0F, ARX_SOUND_PLAY_LOOPED);
			}
			else
			{
				ARX_SOUND_Stop(SND_MAGIC_DRAW);				
			}
			
			snip=0;
		}
	}
	else
	{
		ARX_FLARES_broken=1;
		PIPOrgb++;

		if (PIPOrgb>2) PIPOrgb=0;

		if (player.doingmagic!=0)//==2) 
		{
			player.doingmagic=0;//1

			if (inter.iobj[0]->anims[ANIM_CAST_END])
			{
				AcquireLastAnim(inter.iobj[0]);
				ANIM_Set(&inter.iobj[0]->animlayer[1],inter.iobj[0]->anims[ANIM_CAST_END]);
			}
			
			ARX_FLARES_broken=3;
		}
	}
	

	if (ARX_FLARES_broken==3)
	{
		cur_arm=0;
		cur_mega=0;
		passwall=0;

		if (cur_mr!=3)
			cur_mr=0;

		if (cur_mx!=3)
			cur_mx=0;

		if (cur_rf!=3)
			cur_rf=0;

		if (cur_pom!=3)
			cur_pom=0;

		if (cur_pnux<3)
			cur_pnux=0;

		if (cur_sm<3)
			cur_sm=0;

		cur_bh=0;
		cur_sos=0;

		if (CurrSpellSymbol != 0)
		{
			if (!ARX_SPELLS_AnalyseSPELL())
			{
				if (inter.iobj[0]->anims[ANIM_CAST])
				{
					AcquireLastAnim(inter.iobj[0]);
					ANIM_Set(&inter.iobj[0]->animlayer[1],inter.iobj[0]->anims[ANIM_CAST]);
				}
			}
		}

		ARX_FLARES_broken=1;

		if (WILLRETURNTOCOMBATMODE)
		{
			player.Interface|=INTER_COMBATMODE;
			player.Interface|=INTER_NO_STRIKE;

			ARX_EQUIPMENT_LaunchPlayerReadyWeapon();
			player.doingmagic=0;
			WILLRETURNTOCOMBATMODE=0;

			if(config.misc.newControl) {
				TRUE_PLAYER_MOUSELOOK_ON|=1;
				bRenderInCursorMode=false;
			}
		}

		if (WILLRETURNTOFREELOOK)
		{
			TRUE_PLAYER_MOUSELOOK_ON |= 1;
			WILLRETURNTOFREELOOK = 0;
		}

		ARX_SPELLS_ResetRecognition();
	}
	else if (ARX_FLARES_broken==2)
	{
		ARX_SPELLS_Analyse();

		if (!SpellMoves.empty()) 
		 ARX_SPELLS_AnalyseSYMBOL();
	
		ARX_FLARES_broken = 1;
	}
}

/*!
 * Plays the sound of Fizzling spell plus "NO MANA" speech
 */
static void ARX_SPELLS_FizzleNoMana(long num) {
	if(num < 0) {
		return;
	}
	if(spells[num].caster >= 0) {
		spells[num].tolive = 0;
		ARX_SPELLS_Fizzle(num);
	}
}

long CanPayMana(long num, float cost, bool _bSound = true) {
	
	if (num<0) return 0;

	if (spells[num].flags & SPELLCAST_FLAG_NOMANA) return 1;

	if (spells[num].caster==0) 
	{
		if (player.mana<cost)
		{
			ARX_SPELLS_FizzleNoMana(num);

			if(_bSound) {
				ARX_SPEECH_Add(getLocalised("player_cantcast"));
				ARX_SPEECH_AddSpeech(inter.iobj[0], "player_cantcast", ANIM_TALK_NEUTRAL);
			}

			return 0;
		}

		player.mana -= cost;
		return 1;
	}
	else if (spells[num].caster<inter.nbmax)
	{
		if (inter.iobj[spells[num].caster]->ioflags & IO_NPC)
		{
			if (inter.iobj[spells[num].caster]->_npcdata->mana<cost)
			{
				ARX_SPELLS_FizzleNoMana(num);
				return 0;
			}

			inter.iobj[spells[num].caster]->_npcdata->mana-=cost;
			return 1;
		}		
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Resets Spell Recognition
void ARX_SPELLS_ResetRecognition() {
	
	for(size_t i = 0; i < MAX_SPELL_SYMBOLS; i++) {
		SpellSymbol[i] = RUNE_NONE;
	}
	
	for(size_t i = 0; i < 6; i++) {
		player.SpellToMemorize.iSpellSymbols[i] = RUNE_NONE;
	}
	
	CurrSpellSymbol = 0;
}

//-----------------------------------------------------------------------------
// Adds a 2D point to currently drawn spell symbol
void ARX_SPELLS_AddPoint(const Vec2s & pos) {
	plist[CurrPoint] = pos;
	CurrPoint++;
	if(CurrPoint >= MAX_POINTS) {
		CurrPoint = MAX_POINTS - 1;
	}
}

//-----------------------------------------------------------------------------
long TemporaryGetSpellTarget(const Vec3f *from)
{
	float mindist = std::numeric_limits<float>::max();
	long found(0);

	for (long i(1); i < inter.nbmax; i++)
		if (inter.iobj[i] && inter.iobj[i]->ioflags & IO_NPC)
		{
			float dist = distSqr(*from, inter.iobj[i]->pos);

			if (dist < mindist)
			{
				found = i;
				mindist = dist;
			}
		}

	return found;
}



//KNOWNSPELLS knownspells;

//-----------------------------------------------------------------------------
static void ARX_SPEELS_GetMaxRect(const char *_pcName)
{
	char tcTxt[32];
	int iMinX,iMinY,iMaxX,iMaxY;
	long iSizeX,iSizeY;

	ARX_SPELLS_RequestSymbolDraw3(_pcName,tcTxt);
	ReCenterSequence(tcTxt,iMinX,iMinY,iMaxX,iMaxY);
	iSizeX=iMaxX-iMinX;
	iSizeY=iMaxY-iMinY;
	lMaxSymbolDrawSizeX=std::max(iSizeX,lMaxSymbolDrawSizeX);
	lMaxSymbolDrawSizeY=std::max(iSizeY,lMaxSymbolDrawSizeY);
}
//-----------------------------------------------------------------------------
// Initializes Spell engine (Called once at DANAE startup)
void ARX_SPELLS_Init_Rects() {
	lMaxSymbolDrawSizeX = std::numeric_limits<long>::min();
	lMaxSymbolDrawSizeY = std::numeric_limits<long>::min();

	ARX_SPEELS_GetMaxRect("aam");
	ARX_SPEELS_GetMaxRect("cetrius");
	ARX_SPEELS_GetMaxRect("comunicatum");
	ARX_SPEELS_GetMaxRect("cosum");
	ARX_SPEELS_GetMaxRect("folgora");
	ARX_SPEELS_GetMaxRect("fridd");
	ARX_SPEELS_GetMaxRect("kaom");
	ARX_SPEELS_GetMaxRect("mega");
	ARX_SPEELS_GetMaxRect("morte");
	ARX_SPEELS_GetMaxRect("movis");
	ARX_SPEELS_GetMaxRect("nhi");
	ARX_SPEELS_GetMaxRect("rhaa");
	ARX_SPEELS_GetMaxRect("spacium");
	ARX_SPEELS_GetMaxRect("stregum");
	ARX_SPEELS_GetMaxRect("taar");
	ARX_SPEELS_GetMaxRect("tempus");
	ARX_SPEELS_GetMaxRect("tera");
	ARX_SPEELS_GetMaxRect("vista");
	ARX_SPEELS_GetMaxRect("vitae");
	ARX_SPEELS_GetMaxRect("yok");
	ARX_SPEELS_GetMaxRect("akbaa");
}

//-----------------------------------------------------------------------------
void ARX_SPELLS_Init() {
	
	for(size_t i = 0; i < MAX_SPELLS; i++) {
		spells[i].tolive = 0;
		spells[i].exist = false;
		spells[i].pSpellFx = NULL;
	}
	
	for(size_t i = 0; i < ARRAY_SIZE(allSpells); i++) {
		addSpell(allSpells[i].symbols, allSpells[i].spell, allSpells[i].name);
	}
	
}

// Clears All Spells.
void ARX_SPELLS_ClearAll() {
	
	for(size_t i = 0; i < MAX_SPELLS; i++) {
		if(spells[i].exist) {
			spells[i].tolive = 0;
			spells[i].exist = false;
			
			if(spells[i].pSpellFx) {
				delete spells[i].pSpellFx;
				spells[i].pSpellFx = NULL;
			}
		}
	}
	
	for(long i = 0; i < inter.nbmax; i++) {
		if(inter.iobj[i]) {
			ARX_SPELLS_RemoveAllSpellsOn(inter.iobj[i]);
		}
	}
}

// Obtains a Free Spell slot
static long ARX_SPELLS_GetFree() {
	
	for(size_t i = 0; i < MAX_SPELLS; i++) {
		if(!spells[i].exist) {
			spells[i].longinfo = spells[i].longinfo2 = -1;
			spells[i].misc = NULL;
			return i;
		}
	}
	
	return -1;
}

long ARX_SPELLS_GetInstance(Spell typ) {
	
	for(size_t i = 0; i < MAX_SPELLS; i++) {
		if(spells[i].exist && spells[i].type == typ) {
			return i;
		}
	}
	
	return -1;
}

// Checks for an existing instance of this spelltype
bool ARX_SPELLS_ExistAnyInstance(Spell typ) {
	return (ARX_SPELLS_GetInstance(typ) != -1);
}

long ARX_SPELLS_GetInstanceForThisCaster(Spell typ, long caster) {
	
	for(size_t i = 0; i < MAX_SPELLS; i++) {
		if(spells[i].exist && spells[i].type == typ && spells[i].caster == caster) {
			return i;
		}
	}
	
	return -1;
}

static bool ARX_SPELLS_ExistAnyInstanceForThisCaster(Spell typ, long caster) {
	return (ARX_SPELLS_GetInstanceForThisCaster(typ, caster) != -1);
}

// Plays the sound of aborted spell
void ARX_SPELLS_AbortSpellSound() {
	ARX_SOUND_PlaySFX(SND_MAGIC_FIZZLE);
}

void ARX_SPELLS_FizzleAllSpellsFromCaster(long num_caster) {
	
	for(size_t i = 0; i < MAX_SPELLS; i++) {
		if(spells[i].exist && spells[i].caster == num_caster) {
			spells[i].tolive = 0;
		}
	}
}

PRECAST_STRUCT Precast[MAX_PRECAST];

void ARX_SPELLS_Precast_Reset() {
	for(size_t i = 0; i < MAX_PRECAST; i++) {
		Precast[i].typ = SPELL_NONE;
	}
}

void ARX_SPELLS_Precast_Add(Spell typ, long _level, SpellcastFlags flags, long duration) {
	
	long found = -1;
	
	for(size_t i = 0; i < MAX_PRECAST; i++) {
		if(Precast[i].typ == SPELL_NONE) {
			found = i;
			break;
		}
	}
	
	if(found == -1) {
		for(size_t i = 1; i < MAX_PRECAST; i++) {
			memcpy(&Precast[i - 1], &Precast[i], sizeof(PRECAST_STRUCT));
		}
		
		found = MAX_PRECAST - 1;
	}
	
	Precast[found].typ = typ;
	Precast[found].level = _level;
	Precast[found].launch_time = 0;
	Precast[found].flags = flags;
	Precast[found].duration = duration;
}

unsigned long LAST_PRECAST_TIME=0;
long PrecastCheckCanPayMana(long num, float cost, bool _bSound = true)
{
	if (num<0) return 0;

	if (Precast[num].flags & SPELLCAST_FLAG_NOMANA) return 1;

		if (player.mana>=cost)
		{
			return 1;
		}
	
	ARX_SPELLS_FizzleNoMana(num);

	if(_bSound) {
		ARX_SPEECH_Add(getLocalised("player_cantcast"));
		ARX_SPEECH_AddSpeech(inter.iobj[0], "player_cantcast", ANIM_TALK_NEUTRAL);
	}

	return 0;
}

void ARX_SPELLS_Precast_Launch(long num) {
	
	if (float(arxtime) >= LAST_PRECAST_TIME+1000)
	{
		Spell iNumSpells=Precast[num].typ;
		float cost=ARX_SPELLS_GetManaCost(iNumSpells,-1);

		if(		(iNumSpells != SPELL_NONE)
			&&	(!PrecastCheckCanPayMana(num,cost)	)  )
			return;

		LAST_PRECAST_TIME = (unsigned long)(arxtime);

		if ((Precast[num].typ != SPELL_NONE) && (Precast[num].launch_time==0))
		{
			Precast[num].launch_time = (unsigned long)(arxtime);
			ARX_SOUND_PlaySFX(SND_SPELL_CREATE_FIELD);
		}
	}
}
void ARX_SPELLS_Precast_Check()
{
	for (size_t i = 0; i < MAX_PRECAST; i++)
	{
		if ((Precast[i].typ != SPELL_NONE) && (Precast[i].launch_time>0) &&(float(arxtime) >= Precast[i].launch_time))
		{
			ANIM_USE *ause1 = &inter.iobj[0]->animlayer[1];
			
			if (player.Interface & INTER_COMBATMODE)
			{
				WILLRETURNTOCOMBATMODE=1;
				ARX_INTERFACE_Combat_Mode(0);
				bGToggleCombatModeWithKey=false;
				ResetAnim(&inter.iobj[0]->animlayer[1]);
				inter.iobj[0]->animlayer[1].flags&=~EA_LOOP;
			}

			if ((ause1->cur_anim) && (ause1->cur_anim==inter.iobj[0]->anims[ANIM_CAST]))
			{
				if (ause1->ctime>ause1->cur_anim->anims[ause1->altidx_cur]->anim_time-550)
				{
					ARX_SPELLS_Launch(	Precast[i].typ,
										0,
										Precast[i].flags | SPELLCAST_FLAG_LAUNCHPRECAST, 
										Precast[i].level, 
										-1, 
										Precast[i].duration);
					Precast[i].typ = SPELL_NONE;

					for (size_t li=i; li < MAX_PRECAST - 1; li++)
					{
						if (Precast[li + 1].typ != SPELL_NONE)
						{
							memcpy(&Precast[li], &Precast[li + 1], sizeof(PRECAST_STRUCT));
							Precast[li + 1].typ = SPELL_NONE;
						}
					}
				}
			} else {
				ANIM_USE * ause1 = &inter.iobj[0]->animlayer[1];
				AcquireLastAnim(inter.iobj[0]);
				FinishAnim(inter.iobj[0], ause1->cur_anim);
				ANIM_Set(ause1, inter.iobj[0]->anims[ANIM_CAST]);	
			}
		}
	}
}
void ARX_SPELLS_Precast_Launch2() {
	

}
struct TARGETING_SPELL {
	Spell typ;
	long source;
	SpellcastFlags flags;
	long level;
	long target;
	long duration;
};
TARGETING_SPELL t_spell;

long LOOKING_FOR_SPELL_TARGET=0;
unsigned long LOOKING_FOR_SPELL_TARGET_TIME=0;
void ARX_SPELLS_CancelSpellTarget() {
	t_spell.typ = SPELL_NONE;
	LOOKING_FOR_SPELL_TARGET=0;
}

void ARX_SPELLS_LaunchSpellTarget(INTERACTIVE_OBJ * io) {
	long num=GetInterNum(io);
	if(num >= 0) {
		ARX_SPELLS_Launch(t_spell.typ, t_spell.source, t_spell.flags, t_spell.level, num, t_spell.duration);
	}
}
extern long FINAL_RELEASE;

static float ARX_SPELLS_GetManaCost(Spell _lNumSpell,long lNumSpellTab) {
	float Player_Magic_Level;
	Player_Magic_Level = (float) player.Full_Skill_Casting + player.Full_Attribute_Mind;
	Player_Magic_Level= std::max(1.0f,Player_Magic_Level*( 1.0f / 10 ));
	Player_Magic_Level= std::min(10.0f,Player_Magic_Level);

	switch (_lNumSpell) 
	{
	//-----------------------------
	case SPELL_MAGIC_SIGHT:
		return 0.3f;
		break;
	case SPELL_MAGIC_MISSILE:

		if (lNumSpellTab<0)
			return 	Player_Magic_Level;

		return spells[lNumSpellTab].caster_level;
		break;
	case SPELL_IGNIT:
		return 1.f;
		break;
	case SPELL_DOUSE:
		return 1.f;
		break;
	case SPELL_ACTIVATE_PORTAL:
		return 2.f;
		break;
	//-----------------------------
	case SPELL_HEAL:
		return 4.f;
		break;
	case SPELL_DETECT_TRAP:
		return 0.03f;
		break;
	case SPELL_ARMOR:
		return 0.01f;
		break;
	case SPELL_LOWER_ARMOR:
		return 0.01f;
		break;
	case SPELL_HARM:
		return 0.4f;
		break;
	//-----------------------------
	case SPELL_SPEED:            
		return 0.01f;
		break;
	case SPELL_DISPELL_ILLUSION: 
		return 7.f;
		break;
	case SPELL_FIREBALL:

		if (lNumSpellTab<0)
			return 	Player_Magic_Level*3.f;

		return 3.f*spells[lNumSpellTab].caster_level;
		break;
	case SPELL_CREATE_FOOD:      
		return 5.f;
		break;
	case SPELL_ICE_PROJECTILE:   

		if (lNumSpellTab<0)
			return 	Player_Magic_Level*1.5f;

		return 1.5f*spells[lNumSpellTab].caster_level;
		break;
	//----------------------------
	case SPELL_BLESS:            
		return 0.01f;
		break;
	case SPELL_DISPELL_FIELD:    
		return 7.f;
		break;
	case SPELL_FIRE_PROTECTION:  
		return 1.f;
		break;
	case SPELL_TELEKINESIS:      
		return 0.001f;
		break;
	case SPELL_CURSE:            
		return 0.001f;
		break;
	case SPELL_COLD_PROTECTION:  
		return 1.f;
		break;
	//-----------------------------
	case SPELL_RUNE_OF_GUARDING: 
		return 9.f;
		break;
	case SPELL_LEVITATE:         
		return 1.f;
		break;
	case SPELL_CURE_POISON:      
		return 10.f;
		break;
	case SPELL_REPEL_UNDEAD:     
		return 18.f;
		break;
	case SPELL_POISON_PROJECTILE:

		if (lNumSpellTab<0)
			return 	Player_Magic_Level*2.f;

		return 2.f*spells[lNumSpellTab].caster_level;
		break;
	//-----------------------------
	case SPELL_RISE_DEAD:        
		return 12.f;
		break;
	case SPELL_PARALYSE:         

		if (lNumSpellTab<0)
			return 	Player_Magic_Level*3.f;

		return 3.f*spells[lNumSpellTab].caster_level;
		break;
	case SPELL_CREATE_FIELD:     
		return 1.2f;
		break;
	case SPELL_DISARM_TRAP:      
		return 15.f;
		break;
	case SPELL_SLOW_DOWN:        
		return 1.2f;
		break;
	//-----------------------------
	case SPELL_FLYING_EYE:       
		return 4.f;
		break;
	case SPELL_FIRE_FIELD:       
		return 15.f;
		break;
	case SPELL_ICE_FIELD:        
		return 15.f;
		break;
	case SPELL_LIGHTNING_STRIKE: 

		if (lNumSpellTab<0)
			return 	Player_Magic_Level*6.f;

		return 6.f*spells[lNumSpellTab].caster_level;
		break;
	case SPELL_CONFUSE:          

		if (lNumSpellTab<0)
			return 	Player_Magic_Level*0.1f;

		return 0.1f*spells[lNumSpellTab].caster_level;
		break;
	//-----------------------------
	case SPELL_INVISIBILITY:     
		return 3.f;
		break;
	case SPELL_MANA_DRAIN:       
		return 0.4f;
		break;
	case SPELL_EXPLOSION:        
		return 45.f;
		break;
	case SPELL_ENCHANT_WEAPON:   
		return 35.f;
		break;
	case SPELL_LIFE_DRAIN:       
		return 3.f;//0.9f; //0.4f;
		break;
	//-----------------------------
	case SPELL_SUMMON_CREATURE:  

		if (lNumSpellTab<0)
		{
			if(Player_Magic_Level>=9)
			return 	80.f;
		}

		if(spells[lNumSpellTab].caster_level>=9)
			return 80.f;

		return 20.f;
		break;
	case SPELL_NEGATE_MAGIC:     
		return 2.f;
		break;
	case SPELL_INCINERATE:       
		return 40.f;
		break;
	case SPELL_MASS_PARALYSE:    

		if (lNumSpellTab<0)
			return 	Player_Magic_Level*3.f;

		return 3.f*spells[lNumSpellTab].caster_level;
		break;
	//----------------------------
	case SPELL_MASS_LIGHTNING_STRIKE:

		if (lNumSpellTab<0)
			return 	Player_Magic_Level*8.f;

		return 8.f*spells[lNumSpellTab].caster_level;
		break;
	case SPELL_CONTROL_TARGET:       
		return 40.f;
		break;
	case SPELL_FREEZE_TIME:          
		return 60.f;
		break;
	case SPELL_MASS_INCINERATE:      
		return 160.f;
		break;
	case SPELL_FAKE_SUMMON:			

		if (lNumSpellTab<0)
		{
			if(Player_Magic_Level>=9)
			return 	80.f;
		}

		if(spells[lNumSpellTab].caster_level>=9)
			return 80.f;

		return 20.f;
		break;
	//-----------------------------
	case SPELL_TELEPORT:	
		return 10.f;
		break;
	default:
		return 0.f;
	}
}

bool ARX_SPELLS_Launch(Spell typ, long source, SpellcastFlags flagss, long levell, long target, long duration) {
	
	SpellcastFlags flags = flagss;
	long level = levell;

	if ( cur_rf == 3 )
	{ 
		flags |= SPELLCAST_FLAG_NOCHECKCANCAST | SPELLCAST_FLAG_NOMANA;	
		level += 2;
	}

	if ( sp_max ) 
	{ 
		level = std::max( level, 15L );
	}
	
	if ( ( source == 0 ) && ( FINAL_RELEASE ) )
	if ( !( flags & SPELLCAST_FLAG_NOCHECKCANCAST ) )
	{
		for (size_t i = 0; i < MAX_SPELL_SYMBOLS; i++) {
			if ( SpellSymbol[i] != RUNE_NONE )
			{
				if ( !( player.rune_flags & (RuneFlag)( 1 << SpellSymbol[i] ) ) )
				{
					ARX_SOUND_PlaySpeech("player_cantcast");
					CurrSpellSymbol = 0;
					ARX_SPELLS_ResetRecognition();
					
					return false;
				}
			}
		}
	}

	float Player_Magic_Level = 0;

	if ( !source )
	{
		ARX_SPELLS_ResetRecognition();

		if ( player.SpellToMemorize.bSpell )
		{
			CurrSpellSymbol					= 0;
			player.SpellToMemorize.bSpell	= false;
		}

		ARX_PLAYER_ComputePlayerFullStats();

		if ( level == -1 )
		{
			Player_Magic_Level = (float) player.Full_Skill_Casting + player.Full_Attribute_Mind;
			Player_Magic_Level = std::max( 1.0f, Player_Magic_Level * ( 1.0f / 10 ) );
			Player_Magic_Level = std::min( 10.0f, Player_Magic_Level );
		}
		else 
		{
			Player_Magic_Level = static_cast<float>(level);
		}
	}

	arx_assert( !( source && (flags & SPELLCAST_FLAG_PRECAST) ) );


	if ( flags & SPELLCAST_FLAG_PRECAST )
	{
		int l = level;

		if(l <= 0) {
			l = checked_range_cast<int>(Player_Magic_Level);
		}

		SpellcastFlags flgs=flags;
		flgs&=~SPELLCAST_FLAG_PRECAST;
		ARX_SPELLS_Precast_Add( typ, l, flgs, duration);
		return true;
	}

	if ( flags & SPELLCAST_FLAG_NOMANA )
	{
		Player_Magic_Level = static_cast<float>(level);
	}

	static TextureContainer * tc4 = TextureContainer::Load("graph/particles/smoke");


	if ((target < 0)
		&&	( source == 0 )	)
	switch ( typ )
	{
		case SPELL_LOWER_ARMOR:
		case SPELL_CURSE:
		case SPELL_PARALYSE:				
		case SPELL_INCINERATE:			
		case SPELL_SLOW_DOWN:
		case SPELL_CONFUSE:
		{
				LOOKING_FOR_SPELL_TARGET_TIME	= (unsigned long)(arxtime);	
			LOOKING_FOR_SPELL_TARGET		= 1;
			t_spell.typ						= typ;
			t_spell.source					= source;
			t_spell.flags					= flags;
			t_spell.level					= level;
			t_spell.target					= target;
			t_spell.duration				= duration;
			return false;
		}			
		case SPELL_ENCHANT_WEAPON:		
		{
				LOOKING_FOR_SPELL_TARGET_TIME	= (unsigned long)(arxtime);
			LOOKING_FOR_SPELL_TARGET		= 2;
			t_spell.typ						= typ;
			t_spell.source					= source;
			t_spell.flags					= flags;
			t_spell.level					= level;
			t_spell.target					= target;
			t_spell.duration				= duration;
			return false;
		}	
		break;
		case SPELL_CONTROL_TARGET:
		{
			long		tcount = 0;

			if ( !ValidIONum( source ) )
				return false;

			Vec3f cpos = inter.iobj[source]->pos;

			for ( long ii = 1 ; ii < inter.nbmax ; ii++ )
			{
				INTERACTIVE_OBJ * ioo = inter.iobj[ii];

				if (	( ioo )
					&&	( ioo->ioflags & IO_NPC )
					&&	( ioo->_npcdata->life > 0.f )
					&&	( ioo->show == SHOW_FLAG_IN_SCENE )
					&&	( ioo->groups.find("demon") != ioo->groups.end())
					&&	( distSqr(ioo->pos, cpos) < square(900.f))
					)
				{
					tcount++;
				}
			}

			if ( tcount == 0 ) 
			{
				ARX_SOUND_PlaySFX( SND_MAGIC_FIZZLE, &cpos );
				return false;
			}

			ARX_SOUND_PlaySpeech("player_follower_attack");
				LOOKING_FOR_SPELL_TARGET_TIME	= (unsigned long)(arxtime);
			LOOKING_FOR_SPELL_TARGET		= 1;
			t_spell.typ						= typ;
			t_spell.source					= source;
			t_spell.flags					= flags;
			t_spell.level					= level;
			t_spell.target					= target;
			t_spell.duration				= duration;
			return false;
		}
		break;
		default: break;
	}

	if ( source == 0 )
	{
		ARX_SPELLS_CancelSpellTarget();
	}


	// Try to create a new spell instance
	long i = ARX_SPELLS_GetFree();

	if ( i < 0 )
	{
		return false;
	}

	if ( source >= 0 && source < inter.nbmax )
	{
		if ( spellicons[typ].bAudibleAtStart )
		{
			ARX_NPC_SpawnAudibleSound(&inter.iobj[source]->pos,inter.iobj[source]);
		}
	}

	spells[i].caster = source;	// Caster...
	spells[i].target = target;	// No target if <0

	if ( target < 0 )
		spells[i].target = TemporaryGetSpellTarget( &inter.iobj[spells[i].caster]->pos );

	// Create hand position if a hand is defined
	if ( spells[i].caster == 0 ) 
	{
		spells[i].hand_group = inter.iobj[spells[i].caster]->obj->fastaccess.primary_attach;
	}
	else 
	{
		spells[i].hand_group = inter.iobj[spells[i].caster]->obj->fastaccess.left_attach;
	}

	if(spells[i].hand_group != -1 ) {
		spells[i].hand_pos = inter.iobj[spells[i].caster]->obj->vertexlist3[spells[i].hand_group].v;
	}

	if(source == 0) {
		// Player source
		spells[i].caster_level = Player_Magic_Level; // Level of caster
		spells[i].caster_pos = player.pos;
	} else {
		// IO source
		spells[i].caster_level = level < 1 ? 1 : level > 10 ? 10 : static_cast<float>(level);
		spells[i].caster_pos = inter.iobj[source]->pos;
	}

	if(flags & SPELLCAST_FLAG_LAUNCHPRECAST) {
		spells[i].caster_level = static_cast<float>(level);
	}

	// Checks target
	if (target<0) // no target... targeted by sight
	{
		if (source==0) // no target... player spell targeted by sight
		{
			spells[i].target_pos.x=player.pos.x-EEsin(radians(player.angle.b))*60.f;
			spells[i].target_pos.y=player.pos.y+EEsin(radians(player.angle.a))*60.f;
			spells[i].target_pos.z=player.pos.z+EEcos(radians(player.angle.b))*60.f;
		}
		else
		{
			spells[i].target_pos.x=inter.iobj[target]->pos.x-EEsin(radians(inter.iobj[target]->angle.b))*60.f;
			spells[i].target_pos.y=inter.iobj[target]->pos.y-120.f;
			spells[i].target_pos.z=inter.iobj[target]->pos.z+EEcos(radians(inter.iobj[target]->angle.b))*60.f;
		}
	} else if (target==0) {
		// player target
		spells[i].target_pos = player.pos;
	} else {
		// IO target
		spells[i].target_pos = inter.iobj[target]->pos;
	}
	
	spells[i].flags=flags;
	spells[i].pSpellFx=NULL;
	spells[i].type = typ;
	spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);

	switch (typ)
	{
		case SPELL_NONE: break;
		//*********************************************************************************************
		// LEVEL 1 SPELLS -----------------------------------------------------------------------------
		case SPELL_MAGIC_SIGHT: // Launching MAGIC_SIGHT

			if (ARX_SPELLS_ExistAnyInstanceForThisCaster(typ,spells[i].caster)) return false;

			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			spells[i].exist=true;
			spells[i].fManaCostPerSecond=0.36f;
			spells[i].bDuration = true;

			if (duration>-1) spells[i].tolive=duration;
			else spells[i].tolive=6000000;

			SPELLCAST_Notify(i);
			ARX_SOUND_PlaySFX(SND_SPELL_VISION_START, &spells[i].caster_pos);

			if (spells[i].caster==0)
			{
				Project.improve = 1;
				spells[i].snd_loop = SND_SPELL_VISION_LOOP;
				ARX_SOUND_PlaySFX(spells[i].snd_loop, &spells[i].caster_pos, 1.0F, ARX_SOUND_PLAY_LOOPED);
			}

		break;
		//----------------------------------------------------------------------------------------------
		case SPELL_MAGIC_MISSILE: // Launching MAGIC_MISSILE
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			spells[i].exist = true;
			spells[i].tolive = 20000;
				
			CSpellFx *pCSpellFx = NULL;
			long number;

			if(sp_max || cur_rf == 3) {
				number = checked_range_cast<long>(spells[i].caster_level);
			} else {
						if ( spells[i].caster_level < 3 ) number = 1;
				else	if ( spells[i].caster_level < 5 ) number = 2;
				else	if ( spells[i].caster_level < 7 ) number = 3;
				else	if ( spells[i].caster_level < 9 ) number = 4;
				else	number=5;
			}

			pCSpellFx = new CMultiMagicMissile(number);			

			if(pCSpellFx != NULL) {
				pCSpellFx->spellinstance=i;
				pCSpellFx->SetDuration((unsigned long) (6000));
				static_cast<CMultiMagicMissile*>(pCSpellFx)->Create();
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
			}				

			SPELLCAST_Notify(i);
		}
		break;
		//----------------------------------------------------------------------------------------------
		case SPELL_IGNIT:// Launching IGNIT
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();
			
			spells[i].exist = true;
			spells[i].tolive = 20000;
					
			CSpellFx *pCSpellFx = NULL;
					
			pCSpellFx = new CIgnit();
			
			CIgnit *pIgnit=(CIgnit *)pCSpellFx;

			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				Vec3f target;

				if(spells[i].hand_group!=-1) {
					target = spells[i].hand_pos;
				} else {
					target = spells[i].caster_pos;
					target.y -= 50.f;
				}

				long id=GetFreeDynLight();

				if (id!=-1)
				{
					DynLight[id].exist		=	1;
					DynLight[id].intensity	=	1.8f;
					DynLight[id].fallend	=	450.f;
					DynLight[id].fallstart	=	380.f;
					DynLight[id].rgb.r		=	1.f;
					DynLight[id].rgb.g		=	0.75f;
					DynLight[id].rgb.b		=	0.5f;
					DynLight[id].pos.x		=	target.x;
					DynLight[id].pos.y		=	target.y;
					DynLight[id].pos.z		=	target.z;
					DynLight[id].duration	=	300;
				}

				
				float fPerimeter = 400.0f + spells[i].caster_level*30.0f;
				
				pIgnit->Create(&target, fPerimeter, 500);
				CheckForIgnition(&target,fPerimeter,1,1);

				for(size_t ii = 0; ii < MAX_LIGHTS; ii++) {
					if (GLight[ii]==NULL) continue;

					if ( (GLight[ii]->extras & EXTRAS_EXTINGUISHABLE)
						&& ((spells[i].caster!=0) || 
							((spells[i].caster==0) && !(GLight[ii]->extras & EXTRAS_NO_IGNIT)))
						&&
						((GLight[ii]->extras & EXTRAS_SEMIDYNAMIC) 
							|| (GLight[ii]->extras & EXTRAS_SPAWNFIRE)
							|| (GLight[ii]->extras & EXTRAS_SPAWNSMOKE) )
						&& (!GLight[ii]->status))
					{
						if(distSqr(target, GLight[ii]->pos) <= square(pIgnit->GetPerimetre())) {
							pIgnit->AddLight(ii);
						}
					}
				}

				for(size_t n = 0; n < MAX_SPELLS; n++) {
					if (!spells[n].exist) continue;

					if(spells[n].type == SPELL_FIREBALL) {
						CSpellFx *pCSpellFX = spells[n].pSpellFx;
						
						if(pCSpellFX) {
							CFireBall *pCF = (CFireBall*) pCSpellFX;
							
							EERIE_SPHERE sphere;
							sphere.origin.x=pCF->eCurPos.x;
							sphere.origin.y=pCF->eCurPos.y;
							sphere.origin.z=pCF->eCurPos.z;
							sphere.radius=std::max(spells[i].caster_level*2.f,12.f);

							if(distSqr(target, sphere.origin) < square(pIgnit->GetPerimetre() + sphere.radius)) {
								spells[n].caster_level += 1; 
							}
						}
					}
				}
			}

			if (pCSpellFx)
			{
				spells[i].pSpellFx = pCSpellFx;
				CIgnit *pIgnit=(CIgnit *)pCSpellFx;
				spells[i].tolive = pIgnit->GetDuration();
			}

			SPELLCAST_Notify(i);
		}
		break;
		//---------------------------------------------------------------------------------------------
		case SPELL_DOUSE:// Launching DOUSE
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();
			
			spells[i].exist = true;
			spells[i].tolive = 20000;
					
			CSpellFx *pCSpellFx = NULL;					
			pCSpellFx = new CDoze();
					
			CDoze *pDoze=(CDoze *)pCSpellFx;

			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				Vec3f target;

				if(spells[i].hand_group>=0) {
					target = spells[i].hand_pos;
				} else {
					target = spells[i].caster_pos;
					target.y -= 50.f;
				}
						
				float fPerimeter = 400.0f + spells[i].caster_level*30.0f;
				pDoze->CreateDoze(&target, fPerimeter, 500);						
				CheckForIgnition(&target,fPerimeter,0,1);		

				for(size_t ii = 0; ii < MAX_LIGHTS; ii++) {
					if (GLight[ii]==NULL) continue;

					if ( (GLight[ii]->extras & EXTRAS_EXTINGUISHABLE)
						&&
						((GLight[ii]->extras & EXTRAS_SEMIDYNAMIC) 
							|| (GLight[ii]->extras & EXTRAS_SPAWNFIRE)
							|| (GLight[ii]->extras & EXTRAS_SPAWNSMOKE) )
							&& (GLight[ii]->status))
					{
						if(distSqr(target, GLight[ii]->pos) <= square(pDoze->GetPerimetre())) {
							pDoze->AddLightDoze(ii);	
						}
					}
				}

				if((CURRENT_TORCH) && distSqr(target, player.pos) < square(pDoze->GetPerimetre())) {
					ARX_PLAYER_ClickedOnTorch(CURRENT_TORCH);
				}

				for(size_t n = 0; n < MAX_SPELLS; n++) {
					if (!spells[n].exist) continue;

					switch (spells[n].type)
					{
						case SPELL_FIREBALL:
						{							
							CSpellFx *pCSpellFX;
							pCSpellFX= spells[n].pSpellFx;

							if (pCSpellFX)
							{
								CFireBall *pCF = (CFireBall*) pCSpellFX;
						
							EERIE_SPHERE sphere;
							sphere.origin.x=pCF->eCurPos.x;
							sphere.origin.y=pCF->eCurPos.y;
							sphere.origin.z=pCF->eCurPos.z;
							sphere.radius=std::max(spells[i].caster_level*2.f,12.f);

							if(distSqr(target, sphere.origin) < square(pDoze->GetPerimetre() + sphere.radius)) {
								spells[n].caster_level-=spells[i].caster_level;

								if (spells[n].caster_level<1)
									spells[n].tolive=0;
							}
						}

						break;
						case SPELL_FIRE_FIELD:
						{					
							Vec3f pos;

							if (GetSpellPosition(&pos,n))
							{
								if(distSqr(target, pos) < square(pDoze->GetPerimetre() + 200)) {
									spells[n].caster_level-=spells[i].caster_level;
									if (spells[n].caster_level<1)
										spells[n].tolive=0;
								}
							}
						}
						break;
					}
						default: break;
				}

				}
			}

			if (pCSpellFx)
			{
				spells[i].pSpellFx = pCSpellFx;
				CDoze *pDoze=(CDoze *)pCSpellFx;
				spells[i].tolive = pDoze->GetDuration();
			}

			SPELLCAST_Notify(i);
		}
		break;
		//---------------------------------------------------------------------------------------------
		case SPELL_ACTIVATE_PORTAL:// Launching ACTIVATE_PORTAL
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			ARX_SOUND_PlayInterface(SND_SPELL_ACTIVATE_PORTAL);
			spells[i].exist = true;
			spells[i].tolive = 20;
			SPELLCAST_Notify(i);
		}
		break;			
		//*************************************************************************************************
		// LEVEL 2 SPELLS -----------------------------------------------------------------------------
		case SPELL_HEAL:// Launching HEAL
		{		
		//	return No_MagicAllowed();
			if (ARX_SPELLS_ExistAnyInstanceForThisCaster(typ,spells[i].caster)) return false;

			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			if (!(spells[i].flags & SPELLCAST_FLAG_NOSOUND))
				ARX_SOUND_PlaySFX(SND_SPELL_HEALING, &spells[i].caster_pos);

			spells[i].exist = true;
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 0.4f*spells[i].caster_level;

			if (duration>-1) spells[i].tolive=duration;
			else spells[i].tolive=3500;

			CSpellFx *pCSpellFx = NULL;
			pCSpellFx = new CHeal();

			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				static_cast<CHeal*>(pCSpellFx)->Create();
				pCSpellFx->SetDuration(spells[i].tolive);
				
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
			}

			SPELLCAST_Notify(i);
		}
		break;
		//---------------------------------------------------------------------------------------------
		case SPELL_DETECT_TRAP:// Launching DETECT_TRAP
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			long iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_DETECT_TRAP,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}

			if (spells[i].caster==0)
			if (!(spells[i].flags & SPELLCAST_FLAG_NOSOUND))
				ARX_SOUND_PlayInterface(SND_SPELL_DETECT_TRAP);

			spells[i].snd_loop = SND_SPELL_DETECT_TRAP_LOOP;

			if (spells[i].caster==0)
			if (!(spells[i].flags & SPELLCAST_FLAG_NOSOUND))
				ARX_SOUND_PlaySFX(spells[i].snd_loop, &spells[i].caster_pos, 1.0F, ARX_SOUND_PLAY_LOOPED);

			spells[i].exist = true;
			spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);
			spells[i].tolive = 60000;
			spells[i].fManaCostPerSecond=0.4f;
			spells[i].bDuration = true;
			SPELLCAST_Notify(i);
		}
		break;
		//---------------------------------------------------------------------------------------------
		case SPELL_ARMOR:// Launching ARMOR
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			long idx=ARX_SPELLS_GetSpellOn(inter.iobj[spells[i].target],SPELL_ARMOR);

			if (idx>=0)
			{
				spells[idx].tolive = 0;
			}
			
			long iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_LOWER_ARMOR,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;						
			}

			iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_FIRE_PROTECTION,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;						
			}

			iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_COLD_PROTECTION,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;						
			}

			if (spells[i].caster==0)
				spells[i].target=spells[i].caster;
			
			if (!(spells[i].flags & SPELLCAST_FLAG_NOSOUND))
				ARX_SOUND_PlaySFX(SND_SPELL_ARMOR_START, &spells[i].target_pos);

			spells[i].snd_loop = ARX_SOUND_PlaySFX(SND_SPELL_ARMOR_LOOP, &spells[i].caster_pos, 1.0F, ARX_SOUND_PLAY_LOOPED);
				
			spells[i].exist = true;

			if (spells[i].caster==0)
				spells[i].tolive = 20000000;
			else
				spells[i].tolive = 20000;

			if (duration>-1) spells[i].tolive=duration;
			
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 0.2f*spells[i].caster_level;
				
			CSpellFx *pCSpellFx = NULL;
			pCSpellFx = new CArmor();				

			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				CArmor *pArmor=(CArmor *)pCSpellFx;
				pArmor->Create(spells[i].tolive);
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pArmor->GetDuration();
			}

			ARX_SPELLS_AddSpellOn(spells[i].target,i);
			SPELLCAST_Notify(i);
		}
		break;
		//------------------------------------------------------------------------------------------------
		case SPELL_LOWER_ARMOR:// Launching LOWER_ARMOR
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			long idx=ARX_SPELLS_GetSpellOn(inter.iobj[spells[i].target],SPELL_LOWER_ARMOR);

			if (idx>=0)
			{
				spells[idx].tolive = 0;
			}

			if (spells[i].target<0) return false;

			long iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_ARMOR,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}

			iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_FIRE_PROTECTION,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}

			iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_COLD_PROTECTION,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}
				
			if (!(spells[i].flags & SPELLCAST_FLAG_NOSOUND))
				ARX_SOUND_PlaySFX(SND_SPELL_ARMOR_START, &spells[i].caster_pos);
				
			spells[i].exist = true;

			if (spells[i].caster==0)
				spells[i].tolive = 20000000;
			else
				spells[i].tolive = 20000;

			if (duration>-1) spells[i].tolive=duration;

			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 0.2f*spells[i].caster_level;
				
			CSpellFx *pCSpellFx = NULL;				
			pCSpellFx = new CLowerArmor();

			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				CLowerArmor *pLowerArmor=(CLowerArmor *)pCSpellFx;
				pLowerArmor->Create(spells[i].tolive);
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
			}

			ARX_SPELLS_AddSpellOn(spells[i].target,i);
			SPELLCAST_Notify(i);
		}
		break;
		//------------------------------------------------------------------------------------------------
		case SPELL_HARM:// Launching HARM
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			if (!(spells[i].flags & SPELLCAST_FLAG_NOSOUND))
				ARX_SOUND_PlaySFX(SND_SPELL_HARM, &spells[i].caster_pos);

			spells[i].snd_loop = ARX_SOUND_PlaySFX(SND_SPELL_MAGICAL_SHIELD, &spells[i].caster_pos, 1.0F, ARX_SOUND_PLAY_LOOPED);
			long iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_LIFE_DRAIN,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}

			iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_MANA_DRAIN,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}

			spells[i].exist=true;
			spells[i].tolive=6000000;
			
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 0.4f;

			spells[i].longinfo=ARX_DAMAGES_GetFree();

			if (spells[i].longinfo!=-1)
			{
				damages[spells[i].longinfo].radius=150.f;
				damages[spells[i].longinfo].damages=4.f;//2.f;
				damages[spells[i].longinfo].area=DAMAGE_FULL;
				damages[spells[i].longinfo].duration=100000000;
				damages[spells[i].longinfo].source=spells[i].caster;
				damages[spells[i].longinfo].flags=DAMAGE_FLAG_DONT_HURT_SOURCE | DAMAGE_FLAG_FOLLOW_SOURCE | DAMAGE_FLAG_ADD_VISUAL_FX;
				damages[spells[i].longinfo].type=DAMAGE_TYPE_FAKEFIRE | DAMAGE_TYPE_MAGICAL;
				damages[spells[i].longinfo].exist=true;
			}

			spells[i].longinfo2=GetFreeDynLight();

			if (spells[i].longinfo2 != -1)
			{
				long id = spells[i].longinfo2;
				DynLight[id].exist = 1;
				DynLight[id].intensity = 2.3f;
				DynLight[id].fallend = 700.f;
				DynLight[id].fallstart = 500.f;
				DynLight[id].rgb.r = 1.0f;
				DynLight[id].rgb.g = 0.0f;
				DynLight[id].rgb.b = 0.0f;
				DynLight[id].pos = spells[i].caster_pos;
			}

			if (duration>-1) spells[i].tolive=duration;

			SPELLCAST_Notify(i);				
		}
		break;			
		//**********************************************************************************************
		// LEVEL 3 SPELLS -----------------------------------------------------------------------------
		case SPELL_SPEED:// Launching SPEED
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 2.f;
			ARX_SOUND_PlaySFX(SND_SPELL_SPEED_START, &spells[i].caster_pos);

			if (spells[i].caster==0)
			{			
				spells[i].target = spells[i].caster;
				spells[i].snd_loop = ARX_SOUND_PlaySFX(SND_SPELL_SPEED_LOOP, &spells[i].caster_pos, 1.0F, ARX_SOUND_PLAY_LOOPED);
			}
				
			spells[i].exist = true;

			if (spells[i].caster==0)
				spells[i].tolive = 200000000;
			else
				spells[i].tolive = 20000;

			if (duration>-1) spells[i].tolive=duration;
			
			
			CSpellFx *pCSpellFx = NULL;
			
			pCSpellFx = new CSpeed();			
			CSpeed *pSpeed = (CSpeed *)pCSpellFx;

			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				pSpeed->Create(spells[i].target, spells[i].tolive);
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pSpeed->GetDuration();
			}

			SPELLCAST_Notify(i);
			ARX_SPELLS_AddSpellOn(spells[i].target,i);

			if ((spells[i].caster>=0) && (spells[i].target<inter.nbmax))
			{
				if (inter.iobj[spells[i].target])
					inter.iobj[spells[i].target]->speed_modif+=spells[i].caster_level*( 1.0f / 10 );
			}
		}
		break;
		
		//--------------------------------------------------------------------------------------------------
		case SPELL_DISPELL_ILLUSION:// Launching DISPELL_ILLUSION (REVEAL)
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();
			
			ARX_SOUND_PlaySFX(SND_SPELL_DISPELL_ILLUSION);
			spells[i].exist = true;
			spells[i].tolive = 1000;
			SPELLCAST_Notify(i);

			for(size_t n = 0; n < MAX_SPELLS; n++) {
				if (!spells[n].exist) continue;

				if (spells[n].target==spells[i].caster) continue;

				if (spells[n].caster_level>spells[i].caster_level) continue;

				if(spells[n].type == SPELL_INVISIBILITY) {
					if(ValidIONum(spells[n].target) && ValidIONum(spells[i].caster)) {
						if(distSqr(inter.iobj[spells[n].target]->pos, inter.iobj[spells[i].caster]->pos) < square(1000.f)) {
							spells[n].tolive=0;
						}
					}
				}
			}
		}
		break;
	
		//----------------------------------------------------------------------------------------------
		
		case SPELL_FIREBALL:// Launching FIREBALL
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			spells[i].exist = true;
			spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);
			spells[i].tolive = 20000;
			
			CFireBall * pCSpellFx = new CFireBall();

			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				Vec3f target;

				if (spells[i].caster!=0)
					spells[i].hand_group=-1;

				if(spells[i].hand_group >= 0) {
					target = spells[i].hand_pos;
				} else {
					target = spells[i].caster_pos;

					if ((ValidIONum(spells[i].caster))
						&& (inter.iobj[spells[i].caster]->ioflags & IO_NPC))
					{
						target.x-=EEsin(radians(inter.iobj[spells[i].caster]->angle.b))*30.f;
						target.y-=80.f;
						target.z+=EEcos(radians(inter.iobj[spells[i].caster]->angle.b))*30.f;
					}
				}
			
				pCSpellFx->SetDuration((unsigned long) (6000));

				if (spells[i].caster==0)
					pCSpellFx->Create(target,MAKEANGLE(player.angle.b),player.angle.a,spells[i].caster_level);
				else
				{
					float angle;

					if ((spells[i].target>=0) && (spells[i].target<inter.nbmax) && inter.iobj[spells[i].target])
					{
						angle = 0;
					}
					else angle=0;

					Vec3f eCurPos;

					eCurPos.x = inter.iobj[spells[i].caster]->pos.x ;
					eCurPos.y = inter.iobj[spells[i].caster]->pos.y;
					eCurPos.z = inter.iobj[spells[i].caster]->pos.z ;

					if ((ValidIONum(spells[i].caster))
						&& (inter.iobj[spells[i].caster]->ioflags & IO_NPC))
					{
						eCurPos.y-=80.f;
					}

					INTERACTIVE_OBJ * _io=inter.iobj[spells[i].caster];

					if (ValidIONum(_io->targetinfo))
					{
						Vec3f * p1=&eCurPos;
						Vec3f * p2=&inter.iobj[_io->targetinfo]->pos;
						angle = (degrees(getAngle(p1->y, p1->z, p2->y, p2->z + dist(Vec2f(p2->x, p2->z), Vec2f(p1->x, p1->z))))); //alpha entre orgn et dest;
					}

					pCSpellFx->Create(target,MAKEANGLE(inter.iobj[spells[i].caster]->angle.b),angle,spells[i].caster_level);
				}
			}

			if (pCSpellFx)
			{
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
			}

			ARX_SOUND_PlaySFX(SND_SPELL_FIRE_LAUNCH, &spells[i].caster_pos);
			spells[i].snd_loop = ARX_SOUND_PlaySFX(SND_SPELL_FIRE_WIND, &spells[i].caster_pos, 1.0F, ARX_SOUND_PLAY_LOOPED);
			SPELLCAST_Notify(i);
		}
		break;
		//-------------------------------------------------------------------------------------------------
		case SPELL_CREATE_FOOD:// Launching CREATE_FOOD
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();
			
			ARX_SOUND_PlaySFX(SND_SPELL_CREATE_FOOD, &spells[i].caster_pos);
			spells[i].exist = true;
			spells[i].tolive = 3500;

			if (duration>-1) spells[i].tolive=duration;
					
			CCreateFood * pCSpellFx = NULL;

			if ((spells[i].caster==0) || (spells[i].target==0))
				player.hunger=100;
		
			pCSpellFx = new CCreateFood();

			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				pCSpellFx->Create();
				pCSpellFx->SetDuration(spells[i].tolive);						
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
			}

			SPELLCAST_Notify(i);
		}
		break;
		//----------------------------------------------------------------------------------------------
		case SPELL_ICE_PROJECTILE:// Launching ICE_PROJECTILE
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			ARX_SOUND_PlaySFX(SND_SPELL_ICE_PROJECTILE_LAUNCH, &spells[i].caster_pos);
			spells[i].exist = true;
			spells[i].tolive = 4200;
				
			CSpellFx *pCSpellFx = NULL;
				
			pCSpellFx = new CIceProjectile();
				
			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				Vec3f target;

				if (spells[i].caster==0)
				{
					target.x = player.pos.x - EEsin(radians(MAKEANGLE(player.angle.b)))*150.0f;
					target.y = player.pos.y+160;
					target.z = player.pos.z + EEcos(radians(MAKEANGLE(player.angle.b)))*150.0f;
					CIceProjectile *pIP = (CIceProjectile*)pCSpellFx;
					pIP->Create(target, MAKEANGLE(player.angle.b), spells[i].caster_level);
				}
				else
				{
					target.x = inter.iobj[spells[i].caster]->pos.x - EEsin(radians(MAKEANGLE(inter.iobj[spells[i].caster]->angle.b)))*150.0f;
					target.y = inter.iobj[spells[i].caster]->pos.y;
					target.z = inter.iobj[spells[i].caster]->pos.z + EEcos(radians(MAKEANGLE(inter.iobj[spells[i].caster]->angle.b)))*150.0f;
					CIceProjectile *pIP = (CIceProjectile*)pCSpellFx;
					pIP->Create(target, MAKEANGLE(inter.iobj[spells[i].caster]->angle.b), spells[i].caster_level);
				}

				pCSpellFx->SetDuration(spells[i].tolive);
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
			}

			SPELLCAST_Notify(i);
		}
		break;
		//***********************************************************************************************	
		// LEVEL 4 SPELLS -----------------------------------------------------------------------------
		case SPELL_BLESS:// Launching BLESS
		{
			if (ARX_SPELLS_ExistAnyInstance(typ)) 
			{
				return false;
			}

			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			if (spells[i].caster==0)
				spells[i].target=0;

			long iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_BLESS,spells[i].target);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}

			ARX_SOUND_PlaySFX(SND_SPELL_BLESS);
			spells[i].exist = true;
			spells[i].tolive = 2000000;
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 0.5f*spells[i].caster_level*0.6666f;

			if (duration>-1) spells[i].tolive=duration;

			CBless * pCSpellFx = new CBless();
			
			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				Vec3f target;
				target.x = inter.iobj[spells[i].caster]->pos.x;
				target.y = inter.iobj[spells[i].caster]->pos.y;
				target.z = inter.iobj[spells[i].caster]->pos.z;
				pCSpellFx->Create(target, MAKEANGLE(player.angle.b));
				pCSpellFx->SetDuration(20000);
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
			}

			ARX_SPELLS_AddSpellOn(spells[i].target,i);
			SPELLCAST_Notify(i);
		}
		break;
		//-----------------------------------------------------------------------------------------------
		case SPELL_DISPELL_FIELD:// Launching DISPELL_FIELD
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;					

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();
			
			SPELLCAST_Notify(i);
			spells[i].tolive = 10;
			Vec3f target;
			target.x = inter.iobj[spells[i].caster]->pos.x;
			target.y = inter.iobj[spells[i].caster]->pos.y;
			target.z = inter.iobj[spells[i].caster]->pos.z;
			long valid=0;
			long dispelled=0;

			for(size_t n = 0; n < MAX_SPELLS; n++) {
				if (!spells[n].exist) continue;

				switch (spells[n].type)
				{					
					case SPELL_CREATE_FIELD:
					{							
						CSpellFx *pCSpellFX = spells[n].pSpellFx;

						if (pCSpellFX)
						{
							CCreateField *pCreateField = (CCreateField *) pCSpellFX;
					
							EERIE_SPHERE sphere;
							sphere.origin=pCreateField->eSrc;
							sphere.radius=400.f;

							if ((spells[i].caster!=0) || (spells[n].caster==0))
							{
								if(sphere.contains(target)) {
									valid++;

									if (spells[n].caster_level<=spells[i].caster_level)
									{
										spells[n].tolive=0;
										dispelled++;
									}
								}
							}
						}

						break;
					}
					case SPELL_FIRE_FIELD:
					{							
						CSpellFx *pCSpellFX = spells[n].pSpellFx;

						if (pCSpellFX)
						{
							CFireField *pFireField = (CFireField *) pCSpellFX;
					
							EERIE_SPHERE sphere;
							sphere.origin=pFireField->pos;
							sphere.radius=400.f;

							if (sphere.contains(target))
							{
								valid++;

								if (spells[n].caster_level<=spells[i].caster_level)
								{
									spells[n].tolive=0;
									dispelled++;
								}
							}
						}

						break;
					}
					case SPELL_ICE_FIELD:
					{							
						CSpellFx *pCSpellFX = spells[n].pSpellFx;

						if (pCSpellFX)
						{
							CIceField *pIceField = (CIceField *) pCSpellFX;
					
							EERIE_SPHERE sphere;
							sphere.origin.x=pIceField->eSrc.x;
							sphere.origin.y=pIceField->eSrc.y;
							sphere.origin.z=pIceField->eSrc.z;
							sphere.radius=400.f;

							if(sphere.contains(target)) {
								valid++;

								if (spells[n].caster_level<=spells[i].caster_level)
								{
									spells[n].tolive=0;
									dispelled++;
								}
							}
						}

						break;
					}
					default: break;
				}
			}

			if (valid>dispelled)
			{
				ARX_SPEECH_AddSpeech(inter.iobj[0], "player_not_skilled_enough", ANIM_TALK_NEUTRAL,ARX_SPEECH_FLAG_NOTEXT);

				if (dispelled>0)
				{
					ARX_SOUND_PlaySFX(SND_SPELL_DISPELL_FIELD);
				}
			}
			else if (valid>0)
			{
				ARX_SOUND_PlaySFX(SND_SPELL_DISPELL_FIELD);
			}
			else
			{
				ARX_SOUND_PlaySFX(SND_MAGIC_FIZZLE, &spells[i].caster_pos);
			}
		}
		break;
		//-----------------------------------------------------------------------------------------------
		case SPELL_FIRE_PROTECTION:// Launching FIRE_PROTECTION
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			long idx=ARX_SPELLS_GetSpellOn(inter.iobj[spells[i].target],SPELL_FIRE_PROTECTION);

			if (idx>=0)
			{
				spells[idx].tolive = 0;
			}
	
			ARX_SOUND_PlaySFX(SND_SPELL_FIRE_PROTECTION);
			
			if (duration>-1) spells[i].tolive=duration;

			long iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_ARMOR,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}

			iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_LOWER_ARMOR,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}

			iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_COLD_PROTECTION,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}
			
			spells[i].exist = true;
			spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);
			spells[i].tolive = 2000000;

			if (spells[i].caster==0)
				spells[i].target=0;
				
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 1.f;
			
			CFireProtection * pCSpellFx = new CFireProtection();
				
			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;		
				pCSpellFx->Create(spells[i].tolive);

				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
			}

			ARX_SPELLS_AddSpellOn(spells[i].target,i);
			SPELLCAST_Notify(i);
		}
		break;
		//----------------------------------------------------------------------------------------------
		case SPELL_COLD_PROTECTION:// Launching COLD_PROTECTION
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			long idx=ARX_SPELLS_GetSpellOn(inter.iobj[spells[i].target],SPELL_COLD_PROTECTION);

			if (idx>=0)
			{
				spells[idx].tolive = 0;
			}

			long iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_ARMOR,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}

			iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_LOWER_ARMOR,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}

			iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_FIRE_PROTECTION,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}

			ARX_SOUND_PlaySFX(SND_SPELL_COLD_PROTECTION_START);

			if (spells[i].caster==0)
				spells[i].target=0;

			spells[i].exist = true;
			spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);
			spells[i].tolive = 2000000;
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 1.f;

			if (duration>-1) spells[i].tolive=duration;

			CColdProtection * pCSpellFx =  new CColdProtection();

			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;		
				pCSpellFx->Create(spells[i].tolive, spells[i].target);
			
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
			}

			spells[i].snd_loop = ARX_SOUND_PlaySFX(SND_SPELL_COLD_PROTECTION_LOOP, &spells[i].caster_pos, 1.0F, ARX_SOUND_PLAY_LOOPED);
			ARX_SPELLS_AddSpellOn(spells[i].target,i);
			SPELLCAST_Notify(i);
		}
		break;
		//----------------------------------------------------------------------------------------------
		case SPELL_TELEKINESIS:// Launching TELEKINESIS
		{
			if (ARX_SPELLS_ExistAnyInstanceForThisCaster(SPELL_TELEKINESIS,spells[i].caster)) return false;			

			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			spells[i].exist=true;						
			spells[i].tolive=6000000;
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 0.9f;

			if (duration>-1) spells[i].tolive=duration;

			if (spells[i].caster==0)
				Project.telekinesis=1;

			ARX_SOUND_PlaySFX(SND_SPELL_TELEKINESIS_START, &spells[i].caster_pos);
			SPELLCAST_Notify(i);
		}
		break;
		//-----------------------------------------------------------------------------------------------
		case SPELL_CURSE:// Launching CURSE
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			long iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_CURSE,spells[i].target);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}
					
			ARX_SOUND_PlaySFX(SND_SPELL_CURSE, &spells[i].caster_pos);
			spells[i].exist = true;
			spells[i].tolive = 2000000;

			if (duration>-1) spells[i].tolive=duration;

			CCurse * pCSpellFx = NULL;
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 0.5f*spells[i].caster_level;
					
			pCSpellFx = new CCurse();
			
			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				Vec3f target = spells[i].target_pos;
				if ((spells[i].target>=0) && (inter.iobj[spells[i].target])) {
					if (spells[i].target==0) target.y-=200.f;
					else target.y+=inter.iobj[spells[i].target]->physics.cyl.height-50.f;
				}
				
				pCSpellFx->Create(target, MAKEANGLE(player.angle.b));
				pCSpellFx->SetDuration(spells[i].tolive);
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
			}

			ARX_SPELLS_AddSpellOn(spells[i].target,i);
			SPELLCAST_Notify(i);
		}
		break;
		//*********************************************************************
		// LEVEL 5 SPELLS -----------------------------------------------------------------------------
		case SPELL_RUNE_OF_GUARDING:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			long iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_RUNE_OF_GUARDING,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}
			
			ARX_SOUND_PlaySFX(SND_SPELL_RUNE_OF_GUARDING);
			spells[i].exist = true;
			spells[i].tolive = 99999999;

			if (duration>-1) spells[i].tolive=duration;

			CRuneOfGuarding * pCSpellFx  = new CRuneOfGuarding();
					
			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				Vec3f target;				
				target.x = inter.iobj[spells[i].caster]->pos.x;
				target.y = inter.iobj[spells[i].caster]->pos.y;
				target.z = inter.iobj[spells[i].caster]->pos.z;
				pCSpellFx->Create(target, 0);
				pCSpellFx->SetDuration(spells[i].tolive);

				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
			}

			SPELLCAST_Notify(i);
		}	
		break;
		//----------------------------------------------------------------------------
		case SPELL_LEVITATE:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			long iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_LEVITATE,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}
			
			ARX_SOUND_PlaySFX(SND_SPELL_LEVITATE_START);
			spells[i].exist = true;
			spells[i].tolive = 2000000000;
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 1.f;

			if (duration>-1) spells[i].tolive=duration;

			CSpellFx * pCSpellFx =  new CLevitate();
							
			if (pCSpellFx != NULL)
			{
				CLevitate *pLevitate=(CLevitate *)pCSpellFx;
				pCSpellFx->spellinstance=i;
				Vec3f target;

				if (	(spells[i].caster==0)
					||	(spells[i].target==0)	)
				{
					target.x=player.pos.x;
					target.y=player.pos.y+150.f;
					target.z=player.pos.z;
					spells[i].target = 0; 
					spells[i].tolive = 200000000;
					player.levitate=1;
				}
				else
				{
					target.x=inter.iobj[spells[i].target]->pos.x;
					target.y = inter.iobj[spells[i].target]->pos.y; 
					target.z=inter.iobj[spells[i].target]->pos.z;
				}

				pLevitate->Create(16,50.f,100.f,80.f,&target,spells[i].tolive);
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
			}

			ARX_SPELLS_AddSpellOn(spells[i].target,i);
			SPELLCAST_Notify(i);				
		}
		break;
		//----------------------------------------------------------------------------
		case SPELL_CURE_POISON:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			float cure=spells[i].caster_level*10;

			if (spells[i].caster==0) 
				spells[i].target=0; 

			if (spells[i].target==0) 
			{
				player.poison-=cure;

				if (player.poison<0.f) player.poison=0;

				ARX_SOUND_PlaySFX(SND_SPELL_CURE_POISON);
			}
			else
			{
				if (ValidIONum(spells[i].target))
				{
					INTERACTIVE_OBJ * io=inter.iobj[spells[i].target];

					if (io->ioflags & IO_NPC)
					{
						io->_npcdata->poisonned-=cure;

						if (io->_npcdata->poisonned<0) io->_npcdata->poisonned=0;
					}

					ARX_SOUND_PlaySFX(SND_SPELL_CURE_POISON,&io->pos);
				}
			}
			
			spells[i].exist = true;
			spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);
			spells[i].tolive = 3500;
					
			CCurePoison * pCSpellFx = new CCurePoison();
							
			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				pCSpellFx->Create();
				pCSpellFx->SetDuration(spells[i].tolive);

				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
				
			}

			SPELLCAST_Notify(i);				
		}
		break;
		//----------------------------------------------------------------------------
		case SPELL_REPEL_UNDEAD:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			long iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_REPEL_UNDEAD,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}
			
			ARX_SOUND_PlaySFX(SND_SPELL_REPEL_UNDEAD);

			if (spells[i].caster==0)
				spells[i].target=0;

			if (spells[i].target==0)
				spells[i].snd_loop = ARX_SOUND_PlaySFX(SND_SPELL_REPEL_UNDEAD_LOOP, &spells[i].caster_pos, 1.0F, ARX_SOUND_PLAY_LOOPED);

			spells[i].exist = true;
			spells[i].tolive = 20000000;
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 1.f;

			if (duration>-1) spells[i].tolive=duration;

			CRepelUndead *pCSpellFx =  new CRepelUndead();
			
			if (pCSpellFx != NULL)
			{
				Vec3f target;
				target.x=player.pos.x;
				target.y=player.pos.y;
				target.z=player.pos.z;
				pCSpellFx->spellinstance=i;
				pCSpellFx->Create(target, MAKEANGLE(player.angle.b));
				pCSpellFx->SetDuration(spells[i].tolive);
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
			}

			SPELLCAST_Notify(i);	
		}
		break;
		//----------------------------------------------------------------------------
		case SPELL_POISON_PROJECTILE:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();
			
			ARX_SOUND_PlaySFX(SND_SPELL_POISON_PROJECTILE_LAUNCH, &spells[i].caster_pos);
			spells[i].exist = true;
			spells[i].tolive = 900000000;
			
			CMultiPoisonProjectile * pCSpellFx;
			pCSpellFx = new CMultiPoisonProjectile(std::max(checked_range_cast<long>(spells[i].caster_level), 1L));


			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				Vec3f target(0, 0, 0);
				pCSpellFx->SetDuration((unsigned long) (8000));
				float ang;

				if (spells[i].caster==0) ang=player.angle.b;
				else ang=inter.iobj[spells[i].caster]->angle.b;

				pCSpellFx->Create(target, MAKEANGLE(ang));
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();				
			}

			SPELLCAST_Notify(i);			
		}
		break;
		//***************************************************************************
		// LEVEL 6 -----------------------------------------------------------------------------
		case SPELL_RISE_DEAD:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			long iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_RISE_DEAD,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}
			
			float beta;
			Vec3f target;

			if (spells[i].caster==0)
			{
				target.x=player.pos.x - EEsin(radians(MAKEANGLE(player.angle.b)))*300.f;
				target.y = player.pos.y + 170.f; 
				target.z=player.pos.z + EEcos(radians(MAKEANGLE(player.angle.b)))*300.f;
				beta=MAKEANGLE(player.angle.b);
				
			}
			else
			{
				if (inter.iobj[spells[i].caster]->ioflags & IO_NPC)
				{
					target.x=inter.iobj[spells[i].caster]->pos.x - EEsin(radians(MAKEANGLE(inter.iobj[spells[i].caster]->angle.b)))*300.f;
					target.y = inter.iobj[spells[i].caster]->pos.y; 
					target.z=inter.iobj[spells[i].caster]->pos.z + EEcos(radians(MAKEANGLE(inter.iobj[spells[i].caster]->angle.b)))*300.f;					
				}
				else
				{
					target.x=inter.iobj[spells[i].caster]->pos.x;
					target.y=inter.iobj[spells[i].caster]->pos.y;
					target.z=inter.iobj[spells[i].caster]->pos.z;					
				}

				beta=MAKEANGLE(inter.iobj[spells[i].caster]->angle.b);
			}

			if (!ARX_INTERACTIVE_ConvertToValidPosForIO(NULL, &target))
			{
				ARX_SOUND_PlaySFX(SND_MAGIC_FIZZLE);
				return false;
			}

			spells[i].target_pos = target;
			ARX_SOUND_PlaySFX(SND_SPELL_RAISE_DEAD, &spells[i].caster_pos);
			spells[i].exist = true;
			spells[i].tolive = 2000000;
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 1.2f;
			spells[i].longinfo=-1;

			if (duration>-1) spells[i].tolive=duration;

			CRiseDead * pCSpellFx = new CRiseDead();
					
			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;					
				{
					pCSpellFx->Create(target, beta);
					pCSpellFx->SetDuration(2000, 500, 1800);
					pCSpellFx->SetColorBorder(0.5, 0.5, 0.5);
					pCSpellFx->SetColorRays1(0.83f, 0.73f, 0.63f);
					pCSpellFx->SetColorRays2(0,0,0);
					pCSpellFx->SetColorRays1(0.5, 0.5, 0.5);
					pCSpellFx->SetColorRays2(1, 0, 0);

					if (pCSpellFx->lLightId == -1)
					{
						pCSpellFx->lLightId = GetFreeDynLight();
					}

					if (pCSpellFx->lLightId != -1)
					{
						long id=pCSpellFx->lLightId;
						DynLight[id].exist=1;
						DynLight[id].intensity = 1.3f;
						DynLight[id].fallend=450.f;
						DynLight[id].fallstart=380.f;
						DynLight[id].rgb.r=0.0f;
						DynLight[id].rgb.g=0.0f;
						DynLight[id].rgb.b=0.0f;
						DynLight[id].pos.x = target.x;
						DynLight[id].pos.y = target.y - 100;
						DynLight[id].pos.z = target.z;
						DynLight[id].duration=200;
						DynLight[id].time_creation = (unsigned long)(arxtime);
					}

					spells[i].pSpellFx = pCSpellFx;
					spells[i].tolive = pCSpellFx->GetDuration();
				}
			}

			SPELLCAST_Notify(i);
		}	
		break;
		//----------------------------------------------------------------------------
		case SPELL_PARALYSE:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;			

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			ARX_SOUND_PlaySFX(SND_SPELL_PARALYSE, &spells[i].caster_pos);
			spells[i].exist = true;
			spells[i].tolive = 5000;
			
			if (duration>-1) spells[i].tolive=duration;

			if (ValidIONum(spells[i].target))
			{
				if (	(spells[i].target==0)
					&& (spells[i].caster_level<=player.level)	)
				{
					float mul=player.resist_magic;

					if (rnd()*100.f<mul)
					{
						mul*=( 1.0f / 200 );
						mul=1.f-mul;

						if (mul<0.5f) mul=0.5f;

						spells[i].tolive=(long)(float)(spells[i].tolive*mul);
					}
				}
				else
				{
				INTERACTIVE_OBJ * ioo=inter.iobj[spells[i].target];

				if (ioo->ioflags & IO_NPC)
				{
					float mul=ioo->_npcdata->resist_magic;

					if (rnd()*100.f<mul)
					{
						mul*=( 1.0f / 200 );
						mul=1.f-mul;

						if (mul<0.5f) mul=0.5f;

						spells[i].tolive=(long)(float)(spells[i].tolive*mul);
					}
				}
				}
			}
					
			inter.iobj[spells[i].target]->ioflags |= IO_FREEZESCRIPT;
			ARX_SPELLS_AddSpellOn(spells[i].target,i);
			ARX_NPC_Kill_Spell_Launch(inter.iobj[spells[i].target]);
			SPELLCAST_Notify(i);
		}	
		break;
		//----------------------------------------------------------------------------
		case SPELL_CREATE_FIELD:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();
			
			spells[i].exist = true;

			if (flags & SPELLCAST_FLAG_RESTORE)
			{				
				if (float(arxtime)-4000>0)
					spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime) - 4000;
				else
					spells[i].lastupdate = spells[i].timcreation=0;
			}
			else
				spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);

			spells[i].tolive = 800000;
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 1.2f;

			if (duration>-1) spells[i].tolive=duration;


			Vec3f target;

			if (spells[i].caster==0)
			{
				target.x = inter.iobj[0]->pos.x - EEsin(radians(MAKEANGLE(player.angle.b)))*250.f;
				target.y = inter.iobj[0]->pos.y;
				target.z = inter.iobj[0]->pos.z + EEcos(radians(MAKEANGLE(player.angle.b)))*250.f;
			}
			else			
			{
				if (ValidIONum(spells[i].caster))
				{
					INTERACTIVE_OBJ * io=inter.iobj[spells[i].caster];

					if (io->ioflags & IO_NPC)
					{
						target.x = io->pos.x - EEsin(radians(MAKEANGLE(io->angle.b)))*250.f;
						target.y = io->pos.y;
						target.z = io->pos.z + EEcos(radians(MAKEANGLE(io->angle.b)))*250.f;
					}
					else
					{
						target.x=io->pos.x;					
						target.y=io->pos.y;
						target.z=io->pos.z;
					}
				}
			}

			ARX_SOUND_PlaySFX(SND_SPELL_CREATE_FIELD, &target);

			CCreateField * pCSpellFx  = new CCreateField();

			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				char tmptext[256];
				strcpy(tmptext,"graph/obj3d/interactive/fix_inter/blue_cube/blue_cube.asl");
				INTERACTIVE_OBJ * io;
				io=AddFix(tmptext,IO_IMMEDIATELOAD);

				if (io)
				{
					ARX_INTERACTIVE_HideGore(io);
					RestoreInitialIOStatusOfIO(io);
					spells[i].longinfo=GetInterNum(io);
					io->scriptload=1;
					io->ioflags|=IO_NOSAVE;
					io->ioflags|=IO_FIELD;

					io->initpos.x=io->pos.x=target.x;
					io->initpos.y=io->pos.y=target.y;
					io->initpos.z=io->pos.z=target.z;
					MakeTemporaryIOIdent(io);						
					SendInitScriptEvent(io);

					pCSpellFx->Create(target, 0);

					pCSpellFx->SetDuration(spells[i].tolive);
					pCSpellFx->lLightId = GetFreeDynLight();

					if (pCSpellFx->lLightId != -1)
					{
						long id=pCSpellFx->lLightId;
						DynLight[id].exist=1;
						DynLight[id].intensity = 0.7f + 2.3f;
						DynLight[id].fallend = 500.f;
						DynLight[id].fallstart = 400.f;
						DynLight[id].rgb.r = 0.8f;
						DynLight[id].rgb.g = 0.0f;
						DynLight[id].rgb.b = 1.0f;
						DynLight[id].pos.x = pCSpellFx->eSrc.x;
						DynLight[id].pos.y = pCSpellFx->eSrc.y-150;
						DynLight[id].pos.z = pCSpellFx->eSrc.z;
					}

					spells[i].pSpellFx = pCSpellFx;
					spells[i].tolive = pCSpellFx->GetDuration();

					if (flags & SPELLCAST_FLAG_RESTORE)
					{
						pCSpellFx->Update(4000);
					}
				}
				else spells[i].tolive=0;
			}

			SPELLCAST_Notify(i);
		}	
		break;
		//----------------------------------------------------------------------------
		case SPELL_DISARM_TRAP:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();
			
			ARX_SOUND_PlaySFX(SND_SPELL_DISARM_TRAP);
			spells[i].exist = true;
			spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);
			spells[i].tolive = 1;

			CDisarmTrap * pCSpellFx = new CDisarmTrap();
					
			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				Vec3f target;
				target.x=player.pos.x;
				target.y=player.pos.y;
				target.z=player.pos.z;
				pCSpellFx->Create(target, MAKEANGLE(player.angle.b));
				pCSpellFx->SetDuration(spells[i].tolive);
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
				
				SPELLCAST_Notify(i);

				for(size_t n = 0; n < MAX_SPELLS; n++) {
					if (!spells[n].exist) continue;

					if(spells[n].type == SPELL_RUNE_OF_GUARDING) {
						CSpellFx *pCSpellFX = spells[n].pSpellFx;
						
						if(pCSpellFX) {
							CRuneOfGuarding * crg=(CRuneOfGuarding *)pCSpellFX;
							EERIE_SPHERE sphere;
							sphere.origin=crg->eSrc;
							sphere.radius=400.f;
							
							if(sphere.contains(target)) {
								spells[n].caster_level-=spells[i].caster_level;
								if(spells[n].caster_level <= 0) {
									spells[n].tolive=0;
								}
							}
						}
					}
				}
			}
		}	
		break;
		//----------------------------------------------------------------------------
		case SPELL_SLOW_DOWN:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();
			
			bool bOk = true;

			INTERACTIVE_OBJ *io = inter.iobj[spells[i].target];

			for (int il=0; il<io->nb_spells_on; il++)
			{
				if (spells[io->spells_on[il]].type == SPELL_SLOW_DOWN)
				{
					spells[i].exist = false;
					return false;
				}
			}

			if (bOk)
			{
				ARX_SOUND_PlaySFX(SND_SPELL_SLOW_DOWN, &spells[i].caster_pos);
				spells[i].exist = true;

				if (spells[i].caster==0)
					spells[i].tolive = 10000000;
				else
					spells[i].tolive = 10000;

				if (duration>-1) spells[i].tolive=duration;

				spells[i].pSpellFx = NULL;
				spells[i].bDuration = true;
				spells[i].fManaCostPerSecond = 1.2f;
				
				CSlowDown * pCSpellFx = new CSlowDown();
				
				if (pCSpellFx != NULL)
				{
					pCSpellFx->spellinstance=i;
					Vec3f target;
					target = spells[i].target_pos;
					pCSpellFx->Create(target, MAKEANGLE(player.angle.b));
					pCSpellFx->SetDuration(spells[i].tolive);
					spells[i].pSpellFx = pCSpellFx;
					spells[i].tolive = pCSpellFx->GetDuration();
				}

				SPELLCAST_Notify(i);
				
				
				ARX_SPELLS_AddSpellOn(spells[i].target,i);

				if ((spells[i].target>=0) && (spells[i].target<inter.nbmax))
				{
					if (inter.iobj[spells[i].target])
					{
							inter.iobj[spells[i].target]->speed_modif -= spells[i].caster_level*( 1.0f / 20 );
					}
				}
			}
		}	
		break;
		//****************************************************************************************
		// LEVEL 7 SPELLS -----------------------------------------------------------------------------
		case SPELL_FLYING_EYE:
		{	
			if (eyeball.exist!=0) return false;

			if (ARX_SPELLS_ExistAnyInstanceForThisCaster(typ,spells[i].caster)) return false;

			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			if (spells[i].caster==0)
				spells[i].target=0;

			if (spells[i].target!=0)
				return false;

			ARX_SOUND_PlaySFX(SND_SPELL_EYEBALL_IN);
			spells[i].exist=true;
			spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);
			spells[i].tolive=1000000;
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 3.2f;
			eyeball.exist=1;
			eyeball.pos.x=player.pos.x-(float)EEsin(radians(MAKEANGLE(player.angle.b)))*200.f;
			eyeball.pos.y=player.pos.y+50.f;
			eyeball.pos.z=player.pos.z+(float)EEcos(radians(MAKEANGLE(player.angle.b)))*200.f;
			eyeball.angle = player.angle;
			long j;

			for (long n=0;n<12;n++)
			{
				j=ARX_PARTICLES_GetFree();

				if ((j!=-1) && (!arxtime.is_paused()))
				{
					ParticleCount++;
					PARTICLE_DEF * pd=&particle[j];
					pd->exist=true;
					pd->zdec=0;							
 
					pd->ov.x=eyeball.pos.x+5.f-rnd()*10.f;
					pd->ov.y=eyeball.pos.y+5.f-rnd()*10.f;
					pd->ov.z=eyeball.pos.z+5.f-rnd()*10.f;
					pd->move.x=2.f-4.f*rnd();
					pd->move.y=2.f-4.f*rnd();
					pd->move.z=2.f-4.f*rnd();
					pd->siz=28.f;
					pd->tolive=2000+(unsigned long)(float)(rnd()*4000.f);
					pd->scale.x=12.f;
					pd->scale.y=12.f;
					pd->scale.z=12.f;
					pd->timcreation=spells[i].lastupdate;
					pd->tc=tc4;
					pd->special=FADE_IN_AND_OUT | ROTATING | MODULATE_ROTATION | DISSIPATING;
					pd->fparam=0.0000001f;
					pd->rgb = Color3f(0.7f, 0.7f, 1.f);
				}
			}

			TRUE_PLAYER_MOUSELOOK_ON |= 1;	
			SLID_START=float(arxtime);
			bOldLookToggle=config.input.mouseLookToggle;
			config.input.mouseLookToggle=true;

			SPELLCAST_Notify(i);
		}	
		break;
		//----------------------------------------------------------------------------
		case SPELL_FIRE_FIELD:
		{
			if ( !CanPayMana( i, ARX_SPELLS_GetManaCost( typ, i ) ) )
			{
				return false;
			}

			if ( !GLOBAL_MAGIC_MODE )
			{
				return No_MagicAllowed();
			}
			
			long iCancel = ARX_SPELLS_GetInstanceForThisCaster( SPELL_FIRE_FIELD, spells[i].caster );
			
			if ( iCancel > -1 )
			{
				spells[iCancel].tolive = 0;
			}
					
			ARX_SOUND_PlaySFX( SND_SPELL_FIRE_FIELD_START );
			spells[i].exist					= true;
			spells[i].tolive				= 100000;
			spells[i].bDuration				= true;
			spells[i].fManaCostPerSecond	= 2.8f;
			spells[i].longinfo2				= -1;
			
			
			if ( duration > -1 ) spells[i].tolive = duration;
			
			CFireField * pCSpellFx = new CFireField();

			if ( pCSpellFx != NULL )
			{
				pCSpellFx->spellinstance	= i;

				Vec3f target;

				if ( spells[i].caster == 0 )
				{					
					target.x = player.pos.x - EEsin(radians(MAKEANGLE(player.angle.b)))*250.f;
					target.y = player.pos.y + 170;
					target.z = player.pos.z + EEcos(radians(MAKEANGLE(player.angle.b)))*250.f;
				}
				else			
				{
					if ( ValidIONum( spells[i].caster ) )
					{
						INTERACTIVE_OBJ * io = inter.iobj[spells[i].caster];

						if ( io->ioflags & IO_NPC )
						{
							target.x = io->pos.x - EEsin( radians( MAKEANGLE( io->angle.b ) ) ) * 250.f;
							target.y = io->pos.y;
							target.z = io->pos.z + EEcos( radians( MAKEANGLE( io->angle.b ) ) ) * 250.f;
						}
						else
						{
							target.x = io->pos.x;					
							target.y = io->pos.y;
							target.z = io->pos.z;
						}
					}

					else
					{
						ARX_DEAD_CODE();
						target.x = 0;					
						target.y = 0;
						target.z = 0;
					}


				}

				spells[i].longinfo = ARX_DAMAGES_GetFree();

				if ( spells[i].longinfo != -1 )
				{
					damages[spells[i].longinfo].radius  = 150.f;
					damages[spells[i].longinfo].damages = 10.f;
					damages[spells[i].longinfo].area	= DAMAGE_FULL;
					damages[spells[i].longinfo].duration= 100000000;
					damages[spells[i].longinfo].source	= spells[i].caster;
					damages[spells[i].longinfo].flags	= 0;
					damages[spells[i].longinfo].type	= DAMAGE_TYPE_MAGICAL | DAMAGE_TYPE_FIRE | DAMAGE_TYPE_FIELD;
					damages[spells[i].longinfo].exist	= true;
					damages[spells[i].longinfo].pos.x	= target.x;
					damages[spells[i].longinfo].pos.y	= target.y;
					damages[spells[i].longinfo].pos.z	= target.z;
				}

				pCSpellFx->Create( 200.f, &target, spells[i].tolive );

				spells[i].pSpellFx	= pCSpellFx;
				spells[i].tolive	= pCSpellFx->GetDuration();
				spells[i].snd_loop	= ARX_SOUND_PlaySFX( SND_SPELL_FIRE_FIELD_LOOP, &target, 1.0F, ARX_SOUND_PLAY_LOOPED );
			}

			SPELLCAST_Notify( i );			
		}
		break;
		//----------------------------------------------------------------------------
		case SPELL_ICE_FIELD:
		{
			if ( !CanPayMana( i, ARX_SPELLS_GetManaCost( typ, i ) ) )
			{
				return false;
			}

			if ( !GLOBAL_MAGIC_MODE )
			{
				return No_MagicAllowed();
			}
			
			long iCancel = ARX_SPELLS_GetInstanceForThisCaster( SPELL_ICE_FIELD, spells[i].caster );

			if ( iCancel > -1 )
			{
				spells[iCancel].tolive = 0;
			}
				
			ARX_SOUND_PlaySFX(SND_SPELL_ICE_FIELD);
			spells[i].exist			= true;
			spells[i].lastupdate	= spells[i].timcreation = (unsigned long)(arxtime);
			spells[i].tolive		= 100000;
			spells[i].bDuration		= true;
			spells[i].fManaCostPerSecond = 2.8f;
			spells[i].longinfo2		=-1;

			if ( duration > -1 ) 
			{
				spells[i].tolive = duration;
			}

			CIceField * pCSpellFx = new CIceField();
					
			if ( pCSpellFx != NULL )
			{
				pCSpellFx->spellinstance = i;
				Vec3f target;

				if ( spells[i].caster == 0 )						
				{
					target.x = player.pos.x - EEsin( radians( MAKEANGLE( player.angle.b ) ) ) * 250.f;
					target.y = player.pos.y + 170;
					target.z = player.pos.z + EEcos( radians( MAKEANGLE( player.angle.b ) ) ) * 250.f;
				}
				else			
				{
					if ( ValidIONum( spells[i].caster ) )
					{
						INTERACTIVE_OBJ * io = inter.iobj[spells[i].caster];

						if ( io->ioflags & IO_NPC )
						{
							target.x = io->pos.x - EEsin( radians( MAKEANGLE( io->angle.b ) ) ) * 250.f;
							target.y = io->pos.y;
							target.z = io->pos.z + EEcos( radians( MAKEANGLE( io->angle.b ) ) ) * 250.f;
						}
						else
						{
							target.x = io->pos.x;					
							target.y = io->pos.y;
							target.z = io->pos.z;
						}
					}

					else
					{
						ARX_DEAD_CODE();
						target.x = 0;					
						target.y = 0;
						target.z = 0;
					}


				}

				spells[i].longinfo = ARX_DAMAGES_GetFree();

				if ( spells[i].longinfo != -1 )
				{
					damages[spells[i].longinfo].radius  = 150.f;
					damages[spells[i].longinfo].damages = 10.f;
					damages[spells[i].longinfo].area	= DAMAGE_FULL;
					damages[spells[i].longinfo].duration= 100000000;
					damages[spells[i].longinfo].source	= spells[i].caster;
					damages[spells[i].longinfo].flags	= 0;
					damages[spells[i].longinfo].type	= DAMAGE_TYPE_MAGICAL | DAMAGE_TYPE_COLD | DAMAGE_TYPE_FIELD;
					damages[spells[i].longinfo].exist	= true;
					damages[spells[i].longinfo].pos.x	= target.x;
					damages[spells[i].longinfo].pos.y	= target.y;
					damages[spells[i].longinfo].pos.z	= target.z;
				}

				pCSpellFx->Create( target, MAKEANGLE( player.angle.b ) );
				pCSpellFx->SetDuration( spells[i].tolive );
				spells[i].pSpellFx	= pCSpellFx;
				spells[i].tolive	= pCSpellFx->GetDuration();
			}

			SPELLCAST_Notify( i );
		}	
		break;
		//----------------------------------------------------------------------------
		case SPELL_LIGHTNING_STRIKE:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			spells[i].exist = true;
				
			CLightning * pCSpellFx = new CLightning();
				
			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				Vec3f source, target;
				source.x = 0;
				source.y = 0;
				source.z = 0;
				target.x = 0;
				target.y = 0;
				target.z = -500;
				pCSpellFx->Create(source, target, MAKEANGLE(player.angle.b));
				pCSpellFx->SetDuration((long)(500*spells[i].caster_level));
				pCSpellFx->lSrc = 0;
					
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
			}

			ARX_SOUND_PlaySFX(SND_SPELL_LIGHTNING_START, &spells[i].caster_pos);
			spells[i].snd_loop = ARX_SOUND_PlaySFX(SND_SPELL_LIGHTNING_LOOP, &spells[i].caster_pos, 1.0F, ARX_SOUND_PLAY_LOOPED);
			SPELLCAST_Notify(i);
		}	
		break;
		//----------------------------------------------------------------------------
		case SPELL_CONFUSE:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();
			
			ARX_SOUND_PlaySFX(SND_SPELL_CONFUSE);
			spells[i].exist = true;
			spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 1.5f;

			if (duration>-1) spells[i].tolive=duration;	

			CConfuse * pCSpellFx = new CConfuse();

			if (pCSpellFx)
			{
				pCSpellFx->spellinstance=i;
				Vec3f target;
				target.x=player.pos.x;
				target.y=player.pos.y;
				target.z=player.pos.z;
				pCSpellFx->Create(target, MAKEANGLE(player.angle.b));
				pCSpellFx->SetDuration(spells[i].tolive);
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
			}

		
			ARX_SPELLS_AddSpellOn(spells[i].target,i);
			long source = spells[i].caster;
			char spell[128];

			if (MakeSpellName(spell,spells[i].type))
			{
				if (ValidIONum(spells[i].target))				
				{
					if (source >= 0) 
						EVENT_SENDER = inter.iobj[source];
					else 
						EVENT_SENDER = NULL;

					char param[256];
					sprintf(param,"%s %ld",spell,(long)spells[i].caster_level);
					SendIOScriptEvent(inter.iobj[spells[i].target], SM_SPELLCAST, param);
				}
			}	
		
		}	
		break;
		//*********************************************************************************
		// LEVEL 8 SPELLS -----------------------------------------------------------------------------
		case SPELL_INVISIBILITY:
		{
			if (ARX_SPELLS_ExistAnyInstanceForThisCaster(typ,spells[i].caster)) return false;

			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			spells[i].exist=true;
			spells[i].tolive=6000000;
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 3.f;

			if (duration>-1) spells[i].tolive=duration;

			if (spells[i].caster==0)
				spells[i].target=0;

			inter.iobj[spells[i].target]->GameFlags|=GFLAG_INVISIBILITY;
			inter.iobj[spells[i].target]->invisibility=0.f;
			ARX_SOUND_PlaySFX(SND_SPELL_INVISIBILITY_START, &spells[i].caster_pos);
			SPELLCAST_Notify(i);				
			ARX_SPELLS_AddSpellOn(spells[i].target,i);
		}
		break;
		//----------------------------------------------------------------------------
		case SPELL_MANA_DRAIN:
		{
			if (ARX_SPELLS_ExistAnyInstanceForThisCaster(typ,spells[i].caster)) return false;
			
			long iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_LIFE_DRAIN,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}

			iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_HARM,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}

			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			spells[i].exist=true;
			spells[i].tolive=6000000;					
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 2.f;
			spells[i].snd_loop = ARX_SOUND_PlaySFX(SND_SPELL_MAGICAL_SHIELD, &spells[i].caster_pos, 1.2F, ARX_SOUND_PLAY_LOOPED);

			if (duration>-1) spells[i].tolive=duration;

			spells[i].longinfo=ARX_DAMAGES_GetFree();

			if (spells[i].longinfo!=-1)
			{
				damages[spells[i].longinfo].radius=150.f;
				damages[spells[i].longinfo].damages = 8.f;
				damages[spells[i].longinfo].area=DAMAGE_FULL;
				damages[spells[i].longinfo].duration=100000000;
				damages[spells[i].longinfo].source=spells[i].caster;
				damages[spells[i].longinfo].flags=DAMAGE_FLAG_DONT_HURT_SOURCE | DAMAGE_FLAG_FOLLOW_SOURCE | DAMAGE_FLAG_ADD_VISUAL_FX;
				damages[spells[i].longinfo].type=DAMAGE_TYPE_FAKEFIRE | DAMAGE_TYPE_MAGICAL | DAMAGE_TYPE_DRAIN_MANA;
				damages[spells[i].longinfo].exist=true;
			}

			spells[i].longinfo2=GetFreeDynLight();

			if (spells[i].longinfo2 != -1)
			{
				long id = spells[i].longinfo2;
				DynLight[id].exist = 1;
				DynLight[id].intensity = 2.3f;
				DynLight[id].fallend = 700.f;
				DynLight[id].fallstart = 500.f;
				DynLight[id].rgb.r = 0.0f;
				DynLight[id].rgb.g = 0.0f;
				DynLight[id].rgb.b = 1.0f;
				DynLight[id].pos = spells[i].caster_pos;
				DynLight[id].duration=900;
			}

			SPELLCAST_Notify(i);				
		}
		break;
		//----------------------------------------------------------------------------
		case SPELL_EXPLOSION:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();
			
			ARX_SOUND_PlaySFX(SND_SPELL_EXPLOSION);
					
			spells[i].exist = true;
			spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);
			spells[i].tolive = 2000;
			
			spells[i].longinfo=ARX_DAMAGES_GetFree();
			Vec3f target;
			target.x=inter.iobj[spells[i].caster]->pos.x;
			target.y=inter.iobj[spells[i].caster]->pos.y-60.f;

			if (spells[i].caster==0)
					target.y+=120.f;

			target.z=inter.iobj[spells[i].caster]->pos.z;

			if (spells[i].longinfo!=-1)
			{
				damages[spells[i].longinfo].radius=350.f;
				damages[spells[i].longinfo].damages=10.f;
				damages[spells[i].longinfo].area = DAMAGE_AREA; 
				damages[spells[i].longinfo].duration=spells[i].tolive;
				damages[spells[i].longinfo].source=spells[i].caster;
				damages[spells[i].longinfo].flags=DAMAGE_FLAG_DONT_HURT_SOURCE | DAMAGE_FLAG_FOLLOW_SOURCE | DAMAGE_FLAG_ADD_VISUAL_FX;
				damages[spells[i].longinfo].type=DAMAGE_TYPE_FAKEFIRE | DAMAGE_TYPE_MAGICAL;
				damages[spells[i].longinfo].exist=true;
				damages[spells[i].longinfo].pos.x=target.x;
				damages[spells[i].longinfo].pos.y=target.y;
				damages[spells[i].longinfo].pos.z=target.z;
			}

			spells[i].longinfo2=GetFreeDynLight();

			if (spells[i].longinfo2 != -1)
			{
				long id = spells[i].longinfo2;
				DynLight[id].exist = 1;
				DynLight[id].intensity = 2.3f;
				DynLight[id].fallend = 700.f;
				DynLight[id].fallstart = 500.f;
				DynLight[id].rgb.r = 0.1f+rnd()*( 1.0f / 3 );
				DynLight[id].rgb.g = 0.1f+rnd()*( 1.0f / 3 );
				DynLight[id].rgb.b = 0.8f+rnd()*( 1.0f / 5 );
				DynLight[id].pos.x = target.x;
				DynLight[id].pos.y = target.y;
				DynLight[id].pos.z = target.z;
				DynLight[id].duration=200;
			}

			AddQuakeFX(300,2000,400,1);

			for(long i_angle = 0 ; i_angle < 360 ; i_angle += 12) {
				for(long j = -100 ; j < 100 ; j += 50) {
					
					float rr = radians((float) i_angle);
					Vec3f pos(target.x - EEsin(rr) * 360.f, target.y, target.z + EEcos(rr) * 360.f);
					Vec3f dir = Vec3f(pos.x - target.x, 0.f, pos.z - target.z).getNormalized() * 60.f;
					
					Color3f rgb(0.1f + rnd() * (1.f/3), 0.1f + rnd() * (1.f/3), 0.8f + rnd() * (1.f/5));
					
					Vec3f posi = target + Vec3f(0.f, j * 2, 0.f);
					
					LaunchFireballBoom(&posi, 16, &dir, &rgb);
				}
			}
			
			ARX_SOUND_PlaySFX(SND_SPELL_FIRE_WIND);
			SPELLCAST_Notify(i);
		}	
		break;
		//----------------------------------------------------------------------------
		case SPELL_ENCHANT_WEAPON:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			spells[i].exist = true;
			spells[i].tolive = 20;
				
			SPELLCAST_NotifyOnlyTarget(i);
		}
		break;			
		//----------------------------------------------------------------------------
		case SPELL_LIFE_DRAIN:
		{
			if (ARX_SPELLS_ExistAnyInstanceForThisCaster(typ,spells[i].caster)) return false;

			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			long iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_HARM,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}

			iCancel = ARX_SPELLS_GetInstanceForThisCaster(SPELL_MANA_DRAIN,spells[i].caster);

			if (iCancel > -1)
			{
				spells[iCancel].tolive = 0;
			}				

			spells[i].exist=true;
			spells[i].tolive=6000000;
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 12.f;

			if (duration>-1) spells[i].tolive=duration;

			spells[i].snd_loop = ARX_SOUND_PlaySFX(SND_SPELL_MAGICAL_SHIELD, &spells[i].caster_pos, 0.8F, ARX_SOUND_PLAY_LOOPED);
			spells[i].longinfo=ARX_DAMAGES_GetFree();

			if (spells[i].longinfo!=-1)
			{
				long id=spells[i].longinfo;
				damages[id].radius=150.f;
				damages[id].damages=spells[i].caster_level*( 1.0f / 10 )*.8f;
				damages[id].area = DAMAGE_AREA; 
				damages[id].duration=100000000;
				damages[id].source=spells[i].caster;
				damages[id].flags=DAMAGE_FLAG_DONT_HURT_SOURCE | DAMAGE_FLAG_FOLLOW_SOURCE | DAMAGE_FLAG_ADD_VISUAL_FX;
				damages[id].type=DAMAGE_TYPE_FAKEFIRE | DAMAGE_TYPE_MAGICAL | DAMAGE_TYPE_DRAIN_LIFE;
				damages[id].exist=true;
			}

			spells[i].longinfo2=GetFreeDynLight();

			if (spells[i].longinfo2 != -1)
			{
				long id = spells[i].longinfo2;
				DynLight[id].exist = 1;
				DynLight[id].intensity = 2.3f;
				DynLight[id].fallend = 700.f;
				DynLight[id].fallstart = 500.f;
				DynLight[id].rgb.r = 1.0f;
				DynLight[id].rgb.g = 0.0f;
				DynLight[id].rgb.b = 0.0f;
				DynLight[id].pos = spells[i].caster_pos;
				DynLight[id].duration=900;
			}

			SPELLCAST_Notify(i);				
		}
		break;
		//*****************************************************************************************
		// LEVEL 9 SPELLS -----------------------------------------------------------------------------
		case SPELL_SUMMON_CREATURE:
		{
			if (spells[i].caster_level>=9)
			{
				if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;
			}
			else if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();
			
			ARX_SOUND_PlaySFX(SND_SPELL_SUMMON_CREATURE);
			spells[i].exist = true;
			spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 1.9f;
			spells[i].longinfo=0;
			spells[i].longinfo2=0;

			if (duration>-1) 
				spells[i].tolive=duration;	
			else
				spells[i].tolive = 2000000;
			
			Vec3f target;

			if (spells[i].caster==0)
			{
				target.x=player.pos.x - EEsin(radians(MAKEANGLE(player.angle.b)))*300.f;
				target.y = player.pos.y + 170.f; 
				target.z=player.pos.z + EEcos(radians(MAKEANGLE(player.angle.b)))*300.f;
			}
			else
			{
				if (inter.iobj[spells[i].caster]->ioflags & IO_NPC)
				{
					target.x=inter.iobj[spells[i].caster]->pos.x - EEsin(radians(MAKEANGLE(inter.iobj[spells[i].caster]->angle.b)))*300.f;
					target.y = inter.iobj[spells[i].caster]->pos.y;
					target.z=inter.iobj[spells[i].caster]->pos.z + EEcos(radians(MAKEANGLE(inter.iobj[spells[i].caster]->angle.b)))*300.f;					
				}
				else
				{
					target.x=inter.iobj[spells[i].caster]->pos.x;
					target.y=inter.iobj[spells[i].caster]->pos.y;
					target.z=inter.iobj[spells[i].caster]->pos.z;					
				}
			}

			if (!ARX_INTERACTIVE_ConvertToValidPosForIO(NULL, &target))
			{
				spells[i].exist = false;
				ARX_SOUND_PlaySFX(SND_MAGIC_FIZZLE);
				return false;
			}

			if ((spells[i].caster==0) && (cur_mega==10))
				spells[i].fdata=1.f;
			else
				spells[i].fdata=0.f;

			spells[i].target_pos = target;
			CSummonCreature * pCSpellFx = new CSummonCreature();

			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
					
				pCSpellFx->Create(target, MAKEANGLE(player.angle.b));
					pCSpellFx->SetDuration(2000, 500, 1500);
					pCSpellFx->SetColorBorder(Color3f::red);
					pCSpellFx->SetColorRays1(Color3f::red);
					pCSpellFx->SetColorRays2(Color3f::yellow * .5f);
					
					pCSpellFx->lLightId = GetFreeDynLight();

					if (pCSpellFx->lLightId > -1)
					{
						long id = pCSpellFx->lLightId;
						DynLight[id].exist = 1;
						DynLight[id].intensity = 0.3f;
						DynLight[id].fallend = 500.f;
						DynLight[id].fallstart = 400.f;
						DynLight[id].rgb = Color3f::red;
						DynLight[id].pos = pCSpellFx->eSrc;
					}

				spells[i].pSpellFx = pCSpellFx;
			}

			SPELLCAST_Notify(i);
		}	
		break;
		case SPELL_FAKE_SUMMON:
		{
			if (spells[i].caster_level>=9)
			{
				if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;
			}
			else if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (	(spells[i].caster<=0)
				||	(!ValidIONum(spells[i].target))		)
				return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			ARX_SOUND_PlaySFX(SND_SPELL_SUMMON_CREATURE);
			spells[i].exist = true;
			spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 1.9f;

			if (duration>-1) 
				spells[i].tolive = 4000; 
			else
				spells[i].tolive = 4000;
			
			Vec3f target = inter.iobj[spells[i].target]->pos;			
			if(spells[i].target != 0) {
				target.y -= 170.f;
			}
			
			spells[i].target_pos = target;
			CSummonCreature * pCSpellFx = new CSummonCreature();

			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
					
				pCSpellFx->Create(target, MAKEANGLE(player.angle.b));
				pCSpellFx->SetDuration(2000, 500, 1500);
				pCSpellFx->SetColorBorder(Color3f::red);
				pCSpellFx->SetColorRays1(Color3f::red);
				pCSpellFx->SetColorRays2(Color3f::yellow * .5f);
					
				pCSpellFx->lLightId = GetFreeDynLight();

				if (pCSpellFx->lLightId > -1)
				{
					long id = pCSpellFx->lLightId;
					DynLight[id].exist = 1;
					DynLight[id].intensity = 0.3f;
					DynLight[id].fallend = 500.f;
					DynLight[id].fallstart = 400.f;
					DynLight[id].rgb = Color3f::red;
					DynLight[id].pos = pCSpellFx->eSrc;
					
				}

				spells[i].pSpellFx = pCSpellFx;
			}

			SPELLCAST_Notify(i);
		}	
		break;
		//----------------------------------------------------------------------------
		case SPELL_NEGATE_MAGIC:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();
			
			ARX_SOUND_PlaySFX(SND_SPELL_NEGATE_MAGIC);
			spells[i].exist = true;
			spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);
			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 2.f;

			if (duration>-1) 
				spells[i].tolive=duration;	
			else
				spells[i].tolive = 1000000;

			CNegateMagic * pCSpellFx = new CNegateMagic();
			
			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				Vec3f target;
				target.x=player.pos.x;
				target.y=player.pos.y;
				target.z=player.pos.z;
				pCSpellFx->Create(target, MAKEANGLE(player.angle.b));
				pCSpellFx->SetDuration(spells[i].tolive);
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
			}

			if (spells[i].caster==0)
				spells[i].target=0;

			if (ValidIONum(spells[i].target))
				LaunchAntiMagicField(i);

			SPELLCAST_Notify(i);
		}	
		break;
		//----------------------------------------------------------------------------
		case SPELL_INCINERATE:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			INTERACTIVE_OBJ * tio=inter.iobj[spells[i].target];

			if	((tio->ioflags & IO_NPC) && (tio->_npcdata->life<=0.f))
				return false;
			
			ARX_SOUND_PlaySFX(SND_SPELL_INCINERATE);
			spells[i].exist = true;
			spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);
			spells[i].tolive = 20000;
			
			tio->sfx_flag|=SFX_TYPE_YLSIDE_DEATH;
			tio->sfx_flag|=SFX_TYPE_INCINERATE;
			tio->sfx_time = (unsigned long)(arxtime);
			ARX_SPELLS_AddSpellOn(spells[i].target,i);
			SPELLCAST_Notify(i);
			spells[i].snd_loop = ARX_SOUND_PlaySFX(SND_FIREPLACE, &spells[i].caster_pos, 1.0F, ARX_SOUND_PLAY_LOOPED);
		}	
		break;
		//----------------------------------------------------------------------------
		case SPELL_MASS_PARALYSE:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();
			
			ARX_SOUND_PlaySFX(SND_SPELL_MASS_PARALYSE);
			spells[i].exist = true;

			if (duration>-1) 
				spells[i].tolive=duration;
			else
				spells[i].tolive = 10000;

			spells[i].longinfo2=0;

			for (long ii=0;ii<inter.nbmax;ii++)
			{
				INTERACTIVE_OBJ * tio=inter.iobj[ii];

				if (	(ii==spells[i].caster)
					||	(!tio)
					||	(!(tio->ioflags & IO_NPC))
					||	(tio->show!=SHOW_FLAG_IN_SCENE)
					||	(tio->ioflags & IO_FREEZESCRIPT)
					||	(distSqr(tio->pos, inter.iobj[spells[i].caster]->pos) > square(500.f))	
					)
					continue;

				tio->ioflags |= IO_FREEZESCRIPT;
				ARX_NPC_Kill_Spell_Launch(tio);
				ARX_SPELLS_AddSpellOn(ii,i);
				spells[i].longinfo2++;
				spells[i].misc=realloc(spells[i].misc,sizeof(long)*spells[i].longinfo2);
				long * ptr=(long *)spells[i].misc;
				ptr[spells[i].longinfo2-1]=ii;
			}			

					SPELLCAST_Notify(i);
		}	
		break;
		//********************************************************************************************
		// LEVEL 10 SPELLS -----------------------------------------------------------------------------
		case SPELL_MASS_LIGHTNING_STRIKE:
		{	
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();
			
			for(size_t ii = 0; ii < MAX_SPELLS; ii++) {
				if ((spells[ii].exist) && (spells[ii].type==typ))
				{
					if (spells[ii].longinfo!=-1) 
						DynLight[spells[ii].longinfo].exist=0;

					spells[ii].longinfo=-1;
					spells[ii].tolive=0;
				}
			}
					
			spells[i].exist=true;
			spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);
			spells[i].tolive=5000;			
			spells[i].siz=0;
			spells[i].longinfo=GetFreeDynLight();

			if (spells[i].longinfo!=-1)
			{
				long id=spells[i].longinfo;
				DynLight[id].exist=1;
				DynLight[id].intensity=1.8f;
				DynLight[id].fallend=450.f;
				DynLight[id].fallstart=380.f;
				DynLight[id].rgb.r=1.f;
				DynLight[id].rgb.g=0.75f;
				DynLight[id].rgb.b=0.75f;
				DynLight[id].pos = spells[i].vsource;
			}

			CMassLightning * pCSpellFx;
			pCSpellFx = new CMassLightning(std::max(checked_range_cast<long>(spells[i].caster_level), 1L));

		
			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				Vec3f target;

				if (spells[i].caster==0)
				{
					target.x=player.pos.x  - EEsin(radians(MAKEANGLE(player.angle.b)))*500.f;
					target.y=player.pos.y  + 150.f;
					target.z=player.pos.z  + EEcos(radians(MAKEANGLE(player.angle.b)))*500.f;
				}
				else
				{
					INTERACTIVE_OBJ * io=inter.iobj[spells[i].caster];
					target.x=io->pos.x  - EEsin(radians(MAKEANGLE(io->angle.b)))*500.f;
					target.y=io->pos.y  - 20.f;
					target.z=io->pos.z  + EEcos(radians(MAKEANGLE(io->angle.b)))*500.f;
				}

				pCSpellFx->SetDuration((long)(500*spells[i].caster_level));
				pCSpellFx->Create(target, MAKEANGLE(player.angle.b));
			
				spells[i].pSpellFx = pCSpellFx;
				spells[i].tolive = pCSpellFx->GetDuration();
				spells[i].snd_loop = ARX_SOUND_PlaySFX(SND_SPELL_LIGHTNING_LOOP, &target, 1.0F, ARX_SOUND_PLAY_LOOPED);
			}
					
			ARX_SOUND_PlaySFX(SND_SPELL_LIGHTNING_START);
			SPELLCAST_Notify(i);
			// Draws White Flash on Screen
			GRenderer->SetBlendFunc(Renderer::BlendOne, Renderer::BlendOne);												
			GRenderer->SetRenderState(Renderer::AlphaBlending, true);
			float val = 1.f; 

			EERIEDrawBitmap(0.f, 0.f, (float)DANAESIZX, (float)DANAESIZY, 0.00009f, NULL, Color3f(0.5f + val * (1.0f/2), val, val).to<u8>());
			GRenderer->SetRenderState(Renderer::AlphaBlending, false);
		}	
		break;
		//----------------------------------------------------------------------------
		case SPELL_CONTROL_TARGET:
		{
			if (ARX_SPELLS_ExistAnyInstanceForThisCaster(typ,spells[i].caster)) return false;

			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!ValidIONum(spells[i].target))
				return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			long tcount=0;
			INTERACTIVE_OBJ * tmp_io=inter.iobj[spells[i].target];

			for (long ii=1;ii<inter.nbmax;ii++)
			{
				INTERACTIVE_OBJ * ioo=inter.iobj[ii];

				if (	(ioo)
					&&	(ioo->ioflags & IO_NPC)
					&&	(ioo->_npcdata->life>0.f)
					&&	(ioo->show==SHOW_FLAG_IN_SCENE)
					&&	(ioo->groups.find("demon") != ioo->groups.end())
					&&	(distSqr(ioo->pos, spells[i].caster_pos) < square(900.f))
					)
				{
					tcount++;
					long n = spells[i].caster_level;
					std::string str = tmp_io->long_name() + ' ' + itoa(n);
					SendIOScriptEvent( ioo, SM_NULL, str, "npc_control" );
				}
			}

			if (tcount==0) return false;
			
			ARX_SOUND_PlaySFX(SND_SPELL_CONTROL_TARGET);
			spells[i].exist = true;
			spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);
			spells[i].tolive = 1000;
			
			CControlTarget * pCSpellFx = new CControlTarget();

			if (pCSpellFx != NULL)
			{
				pCSpellFx->spellinstance=i;
				Vec3f target;
				target.x=player.pos.x;
				target.y=player.pos.y;
				target.z=player.pos.z;
				pCSpellFx->Create(target, MAKEANGLE(player.angle.b));
				pCSpellFx->SetDuration(spells[i].tolive);
				spells[i].pSpellFx = pCSpellFx;
			}

			SPELLCAST_Notify(i);
		}	
		break;

		//----------------------------------------------------------------------------	
		case SPELL_FREEZE_TIME:
		{	
			if (ARX_SPELLS_ExistAnyInstanceForThisCaster(typ,spells[i].caster)) return false;

			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			ARX_SOUND_PlaySFX(SND_SPELL_FREEZETIME);
			spells[i].siz=spells[i].caster_level*0.08f;
			GLOBAL_SLOWDOWN -= spells[i].siz;
			
			spells[i].exist=true;						
			spells[i].tolive=200000;		

			if (duration>-1) spells[i].tolive=duration;

			spells[i].bDuration = true;
			spells[i].fManaCostPerSecond = 30.f*spells[i].siz;
			spells[i].longinfo=(long)arxtime.get_updated();
			SPELLCAST_Notify(i);
		}	
		break;
		//----------------------------------------------------------------------------
		case SPELL_MASS_INCINERATE:
		{
			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();
			
			ARX_SOUND_PlaySFX(SND_SPELL_MASS_INCINERATE);
			spells[i].exist = true;
			spells[i].lastupdate = spells[i].timcreation = (unsigned long)(arxtime);
			spells[i].tolive = 20000;
			long nb_targets=0;

			for (long ii=0;ii<inter.nbmax;ii++)
			{
				INTERACTIVE_OBJ * tio=inter.iobj[ii];

				if (	(ii==spells[i].caster)
					||	(!tio)
					||	(!(tio->ioflags & IO_NPC))
					||	((tio->ioflags & IO_NPC) && (tio->_npcdata->life<=0.f))
					||	(tio->show!=SHOW_FLAG_IN_SCENE)
					||	(distSqr(tio->pos, inter.iobj[spells[i].caster]->pos) > square(500.f))	)
					continue;

				tio->sfx_flag|=SFX_TYPE_YLSIDE_DEATH;
				tio->sfx_flag|=SFX_TYPE_INCINERATE;
				tio->sfx_time = (unsigned long)(arxtime);
				nb_targets++;
				ARX_SPELLS_AddSpellOn(ii,i);
			}			

			SPELLCAST_Notify(i);

			if (nb_targets)
				spells[i].snd_loop = ARX_SOUND_PlaySFX(SND_FIREPLACE, &spells[i].caster_pos, 1.0F, ARX_SOUND_PLAY_LOOPED);
			else
				spells[i].snd_loop = -1;
		}	
		break;			
		//----------------------------------------------------------------------------
		case SPELL_TELEPORT:
		{
			if (ARX_SPELLS_ExistAnyInstanceForThisCaster(typ,spells[i].caster)) return false;

			if (!CanPayMana(i,ARX_SPELLS_GetManaCost(typ,i))) return false;

			if (!GLOBAL_MAGIC_MODE)
				return No_MagicAllowed();

			spells[i].exist=true;
			spells[i].tolive=7000;
			ARX_SOUND_PlaySFX(SND_SPELL_TELEPORT, &spells[i].caster_pos);

			if (spells[i].caster==0) LASTTELEPORT = 0.0F;

			SPELLCAST_Notify(i);
		}
		break;
	}

	return true;
}

//*************************************************************************************
// Used for specific Spell-End FX
//*************************************************************************************
void ARX_SPELLS_Kill(long i) {
	
	static TextureContainer * tc4=TextureContainer::Load("graph/particles/smoke");

	if (!spells[i].exist) return;

	spells[i].exist=false;

	// All Levels - Kill Light
	if (spells[i].pSpellFx && spells[i].pSpellFx->lLightId != -1)
	{
		DynLight[spells[i].pSpellFx->lLightId].duration = 500; 
		DynLight[spells[i].pSpellFx->lLightId].time_creation = (unsigned long)(arxtime);
	}

	switch(spells[i].type)
	{
		//----------------------------------------------------------------------------
		case SPELL_FIREBALL :

			if (spells[i].longinfo!=-1) 
			{
				DynLight[spells[i].longinfo].duration = 500; 
				DynLight[spells[i].longinfo].time_creation = (unsigned long)(arxtime);
			}

			spells[i].longinfo=-1;

			if (spells[i].pSpellFx)
				delete spells[i].pSpellFx;

			spells[i].pSpellFx=NULL;
		break;
		//----------------------------------------------------------------------------
		case SPELL_LIGHTNING_STRIKE :

			if (spells[i].longinfo!=-1) 
			{
				DynLight[spells[i].longinfo].duration = 200; 
				DynLight[spells[i].longinfo].time_creation = (unsigned long)(arxtime);
			}

			spells[i].longinfo=-1;
			ARX_SOUND_Stop(spells[i].snd_loop);
			ARX_SOUND_PlaySFX(SND_SPELL_LIGHTNING_END);

			if (spells[i].pSpellFx)
				delete spells[i].pSpellFx;

			spells[i].pSpellFx=NULL;
		break;
		//----------------------------------------------------------------------------
		case SPELL_MASS_LIGHTNING_STRIKE :

			if (spells[i].longinfo!=-1) 
			{
				DynLight[spells[i].longinfo].duration = 200; 
				DynLight[spells[i].longinfo].time_creation = (unsigned long)(arxtime);
			}

			spells[i].longinfo=-1;
			ARX_SOUND_Stop(spells[i].snd_loop);
			ARX_SOUND_PlaySFX(SND_SPELL_LIGHTNING_END);

			if (spells[i].pSpellFx)
				delete spells[i].pSpellFx;

			spells[i].pSpellFx=NULL;
		break;
		case SPELL_REPEL_UNDEAD:

			if (spells[i].longinfo!=-1) 
			{
				DynLight[spells[i].longinfo].duration = 200; 
				DynLight[spells[i].longinfo].time_creation = (unsigned long)(arxtime);
			}

			spells[i].longinfo=-1;
			ARX_SOUND_Stop(spells[i].snd_loop);

			if (spells[i].pSpellFx)
				delete spells[i].pSpellFx;

			spells[i].pSpellFx=NULL;
		break;
		//----------------------------------------------------------------------------
		case SPELL_HARM :
		case SPELL_LIFE_DRAIN :
		case SPELL_MANA_DRAIN :

			if (spells[i].longinfo!=-1) damages[spells[i].longinfo].exist=false;	

			if (spells[i].longinfo2!=-1) 
			{
				DynLight[spells[i].longinfo2].time_creation = (unsigned long)(arxtime);
				DynLight[spells[i].longinfo2].duration = 600; 
			}

			ARX_SOUND_Stop(spells[i].snd_loop);
			break;
		//----------------------------------------------------------------------------
		case SPELL_FLYING_EYE :
		{
			ARX_SOUND_PlaySFX(SND_SPELL_EYEBALL_OUT);
			eyeball.exist = -100;

			for (long n=0;n<12;n++)
			{
				long j = ARX_PARTICLES_GetFree();

				if ((j!=-1) && (!arxtime.is_paused()))
				{
					ParticleCount++;
					PARTICLE_DEF * pd=&particle[j];
					pd->exist=true;
					pd->zdec=0;					
 
					pd->ov.x=eyeball.pos.x+5.f-rnd()*10.f;
					pd->ov.y=eyeball.pos.y+5.f-rnd()*10.f;
					pd->ov.z=eyeball.pos.z+5.f-rnd()*10.f;
					pd->move.x=2.f-4.f*rnd();
					pd->move.y=2.f-4.f*rnd();
					pd->move.z=2.f-4.f*rnd();
					pd->siz=28.f;
					pd->tolive=2000+(unsigned long)(float)(rnd()*4000.f);
					pd->scale = Vec3f(12.f, 12.f, 12.f);
					pd->timcreation=spells[i].lastupdate;
					pd->tc=tc4;
					pd->special=FADE_IN_AND_OUT | ROTATING | MODULATE_ROTATION | DISSIPATING;
					pd->fparam=0.0000001f;
					pd->rgb = Color3f(0.7f, 0.7f, 1.f);
				}
			}

			config.input.mouseLookToggle = bOldLookToggle; 
			break;
		}
		//----------------------------------------------------------------------------
		// Level 06	
		//---------------------------------------LEVEL1
		case SPELL_IGNIT:

			if (spells[i].pSpellFx)
				delete spells[i].pSpellFx;

			spells[i].pSpellFx=NULL;
		break;
		case SPELL_DOUSE:

			if (spells[i].pSpellFx)
				delete spells[i].pSpellFx;

			spells[i].pSpellFx=NULL;
		break;
		//----------------------------------------------------------------------------
		case SPELL_PARALYSE:
			ARX_SOUND_PlaySFX(SND_SPELL_PARALYSE_END);
			break;

		//---------------------------------------------------------------------
		// Level 7
		case SPELL_FIRE_FIELD:
			{
				ARX_SOUND_Stop(spells[i].snd_loop);
				ARX_SOUND_PlaySFX(SND_SPELL_FIRE_FIELD_END);
		}
		break;
		case SPELL_LOWER_ARMOR:
		break;

		//----------------------------------------------------------------------------
		case SPELL_EXPLOSION:
		{
			if (spells[i].pSpellFx)
				delete spells[i].pSpellFx;

			spells[i].pSpellFx=NULL;
			break;
		}

		//----------------------------------------------------------------------------
		case SPELL_MASS_PARALYSE:

			ARX_SOUND_PlaySFX(SND_SPELL_PARALYSE_END);
			break;
		//----------------------------------------------------------------------------
		case SPELL_SUMMON_CREATURE:

			if (spells[i].pSpellFx->lLightId > -1)
			{
				long id = spells[i].pSpellFx->lLightId;
				DynLight[id].exist = 0;
				spells[i].pSpellFx->lLightId=-1;
			}

			if (ValidIONum(spells[i].longinfo2) && (spells[i].longinfo2!=0))
			{
				if (	(inter.iobj[spells[i].longinfo2]->scriptload)
					&&	(inter.iobj[spells[i].longinfo2]->ioflags & IO_NOSAVE)	)
				{
					AddRandomSmoke(inter.iobj[spells[i].longinfo2],100);
					Vec3f posi;
					posi = inter.iobj[spells[i].longinfo2]->pos;
					posi.y-=100.f;
					MakeCoolFx(&posi);
					long nn=GetFreeDynLight();

					if (nn>=0)
					{
						DynLight[nn].exist=1;
						DynLight[nn].intensity = 0.7f + 2.f*rnd();
						DynLight[nn].fallend = 600.f;
						DynLight[nn].fallstart = 400.f;
						DynLight[nn].rgb.r = 1.0f;
						DynLight[nn].rgb.g = 0.8f;
						DynLight[nn].rgb.b = .0f;
						DynLight[nn].pos.x = posi.x;
						DynLight[nn].pos.y = posi.y;
						DynLight[nn].pos.z = posi.z;
						DynLight[nn].duration=600;
					}

					ARX_INTERACTIVE_DestroyIO(inter.iobj[spells[i].longinfo2]);
				}
			}

			spells[i].longinfo2=0;
		break;
		case SPELL_FAKE_SUMMON:

			if (spells[i].pSpellFx->lLightId > -1)
			{
				long id = spells[i].pSpellFx->lLightId;
				DynLight[id].exist = 0;
				spells[i].pSpellFx->lLightId=-1;
			}

		break;
		default:

			if (spells[i].pSpellFx)
				delete spells[i].pSpellFx;

			spells[i].pSpellFx=NULL;
			break;
	}

	if (spells[i].pSpellFx)
		delete spells[i].pSpellFx;

	spells[i].pSpellFx=NULL;
}


EYEBALL_DEF eyeball;

Anglef cabalangle;
Vec3f cabalpos;
Vec3f cabalscale;
Color3f cabalcolor;


float ARX_SPELLS_ApplyFireProtection(INTERACTIVE_OBJ * io,float damages)
{
	if (io)
	{
		long idx=ARX_SPELLS_GetSpellOn(io,SPELL_FIRE_PROTECTION);

		if (idx>=0)
		{
			float modif=1.f-((float)spells[idx].caster_level*( 1.0f / 10 ));

			if (modif>1.f) modif=1.f;
			else if (modif<0.f) modif=0.f;

			damages*=modif;
		}

		if (io->ioflags & IO_NPC)
		{
			damages-=io->_npcdata->resist_fire*( 1.0f / 100 )*damages;

			if (damages<0.f) damages=0.f;
		}
	}

	return damages;
}
float ARX_SPELLS_ApplyColdProtection(INTERACTIVE_OBJ * io,float damages)
{
	long idx=ARX_SPELLS_GetSpellOn(io,SPELL_COLD_PROTECTION);

	if (idx>=0)
	{
		float modif=1.f-((float)spells[idx].caster_level*( 1.0f / 10 ));

		if (modif>1.f) modif=1.f;
		else if (modif<0.f) modif=0.f;

		damages*=modif;
	}

	return damages;
}

//*************************************************************************************
// Updates all currently working spells.
//*************************************************************************************
void ARX_SPELLS_Update()
{
	
	unsigned long tim;
	long framediff,framediff3;

	ucFlick++;

	tim = (unsigned long)(arxtime);

	for(size_t i = 0; i < MAX_SPELLS; i++) {

		if (!GLOBAL_MAGIC_MODE) spells[i].tolive=0;

		if (spells[i].exist) 
		{
			if (spells[i].bDuration && !CanPayMana(i,spells[i].fManaCostPerSecond * (float)FrameDiff * ( 1.0f / 1000 ), false))
				ARX_SPELLS_Fizzle(i);

			framediff=spells[i].timcreation+spells[i].tolive-tim;
			framediff3=tim-spells[i].lastupdate;

			if (framediff<0) 
			{
				SPELLEND_Notify(i);

				switch (spells[i].type)
				{
				//----------------------------------------------------------------------------
				case SPELL_TELEPORT:
					ARX_SOUND_PlaySFX(SND_MAGIC_FIZZLE, &spells[i].caster_pos);
				break;
				//----------------------------------------------------------------------------
				
				//****************************************************************************
				// LEVEL 1 SPELLS -----------------------------------------------------------------------------
				//----------------------------------------------------------------------------
				case SPELL_MAGIC_SIGHT:

					if (spells[i].caster==0)
					{
						Project.improve=0;
						ARX_SOUND_Stop(spells[i].snd_loop);
					}

					ARX_SOUND_PlaySFX(SND_SPELL_VISION_START, &spells[i].caster_pos);					
				break;
				//-----------------------------------------------------------------------------
				case SPELL_MAGIC_MISSILE:
				{
					if (spells[i].longinfo!=-1) 		
						DynLight[spells[i].longinfo].exist=0;							
				}
				break;
				//----------------------------------------------------------------------------
				case SPELL_IGNIT:
				{
					CIgnit *pIgnit;
					pIgnit=(CIgnit *)spells[i].pSpellFx;
					pIgnit->Action(1);					
				}
				break;
				//----------------------------------------------------------------------------
				case SPELL_DOUSE:
				{
					CDoze *pDoze;
					pDoze=(CDoze *)spells[i].pSpellFx;
					pDoze->Action(0);					
				}
				break;
				//----------------------------------------------------------------------------------
				//**********************************************************************************
				// LEVEL 2 -------------------------------------------------------------------------
				//----------------------------------------------------------------------------------				
				case SPELL_DETECT_TRAP:

					if (spells[i].caster==0)
					{
						Project.improve=0;
						ARX_SOUND_Stop(spells[i].snd_loop);
					}					

				break;					
				//----------------------------------------------------------------------------
				case SPELL_ARMOR:
				{
					ARX_SOUND_Stop(spells[i].snd_loop);
					ARX_SOUND_PlaySFX(SND_SPELL_ARMOR_END, &spells[i].caster_pos);					
					INTERACTIVE_OBJ * io=inter.iobj[spells[i].target];

					if (spells[i].longinfo)
					{
						io->halo.flags&=~HALO_ACTIVE;
						ARX_HALO_SetToNative(io);
					}

					ARX_SPELLS_RemoveSpellOn(spells[i].target,i);
				}
				break;
				//----------------------------------------------------------------------------
				case SPELL_LOWER_ARMOR:
				{
					INTERACTIVE_OBJ * io=inter.iobj[spells[i].target];

					if (spells[i].longinfo)
					{
						io->halo.flags&=~HALO_ACTIVE;
						ARX_HALO_SetToNative(io);
					}

					ARX_SPELLS_RemoveSpellOn(spells[i].target,i);					
				}
				break;					
				//----------------------------------------------------------------------------------
				//**********************************************************************************
				// LEVEL 3 -------------------------------------------------------------------------
				//----------------------------------------------------------------------------------
				case SPELL_SPEED:						

					ARX_SPELLS_RemoveSpellOn(spells[i].target,i);

					if ((spells[i].target>=0) && (spells[i].target<inter.nbmax))
					{
						if (inter.iobj[spells[i].target])
							inter.iobj[spells[i].target]->speed_modif-=spells[i].caster_level*( 1.0f / 10 );
					}

					if (spells[i].caster == 0) ARX_SOUND_Stop(spells[i].snd_loop);

					ARX_SOUND_PlaySFX(SND_SPELL_INVISIBILITY_END, &spells[i].caster_pos);
				break;
				//----------------------------------------------------------------------------------
				case SPELL_FIREBALL:
					ARX_SOUND_Stop(spells[i].snd_loop);					
				break;
				//----------------------------------------------------------------------------------
				//**********************************************************************************
				// LEVEL 4 -------------------------------------------------------------------------
				//----------------------------------------------------------------------------------
				case SPELL_BLESS:
					ARX_SPELLS_RemoveSpellOn(spells[i].target,i);
				break;
				case SPELL_CURSE:
					ARX_SPELLS_RemoveSpellOn(spells[i].target,i);
				break;
				//----------------------------------------------------------------------------
				case SPELL_TELEKINESIS:						

					if (spells[i].caster==0)
						Project.telekinesis=0;

					ARX_SOUND_PlaySFX(SND_SPELL_TELEKINESIS_END, &spells[i].caster_pos);					
				break;
				case SPELL_FIRE_PROTECTION:
					ARX_SPELLS_RemoveSpellOn(spells[i].target,i);;

					if (ValidIONum(spells[i].target))
						ARX_HALO_SetToNative(inter.iobj[spells[i].target]);

				break;
				case SPELL_COLD_PROTECTION:
					ARX_SOUND_Stop(spells[i].snd_loop);
					ARX_SOUND_PlaySFX(SND_SPELL_COLD_PROTECTION_END);
					ARX_SPELLS_RemoveSpellOn(spells[i].target,i);;

					if (ValidIONum(spells[i].target))
						ARX_HALO_SetToNative(inter.iobj[spells[i].target]);

				break;
				//----------------------------------------------------------------------------------
				//**********************************************************************************
				// LEVEL 5 -------------------------------------------------------------------------
				//----------------------------------------------------------------------------------
				//----------------------------------------------------------------------------
				case SPELL_LEVITATE:
				{
 
					ARX_SPELLS_RemoveSpellOn(spells[i].target,i);					

					if (spells[i].target==0)
						player.levitate=0;
				}
				break;
				//----------------------------------------------------------------------------------
				//**********************************************************************************
				// LEVEL 6 SPELLS ------------------------------------------------------------------
				//----------------------------------------------------------------------------------
				case SPELL_PARALYSE:
					ARX_SPELLS_RemoveSpellOn(spells[i].target,i);
					inter.iobj[spells[i].target]->ioflags&=~IO_FREEZESCRIPT;											
				break;
				//----------------------------------------------------------------------------------
				case SPELL_RISE_DEAD:
					ARX_SOUND_PlaySFX(SND_SPELL_ELECTRIC, &spells[i].caster_pos);

					if (ValidIONum(spells[i].longinfo) && spells[i].longinfo!=0)
					{				
						if (	(inter.iobj[spells[i].longinfo]->scriptload)
							&&	(inter.iobj[spells[i].longinfo]->ioflags & IO_NOSAVE)	)
						{
							AddRandomSmoke(inter.iobj[spells[i].longinfo],100);
							Vec3f posi = inter.iobj[spells[i].longinfo]->pos;
							posi.y-=100.f;
							MakeCoolFx(&posi);
							long nn=GetFreeDynLight();

							if (nn>=0)
							{
								DynLight[nn].exist=1;
								DynLight[nn].intensity = 0.7f + 2.f*rnd();
								DynLight[nn].fallend = 600.f;
								DynLight[nn].fallstart = 400.f;
								DynLight[nn].rgb.r = 1.0f;
								DynLight[nn].rgb.g = 0.8f;
								DynLight[nn].rgb.b = .0f;
								DynLight[nn].pos.x = posi.x;
								DynLight[nn].pos.y = posi.y;
								DynLight[nn].pos.z = posi.z;
								DynLight[nn].duration=600;
							}

							ARX_INTERACTIVE_DestroyIO(inter.iobj[spells[i].longinfo]);
						}
					}					

				break;
				case SPELL_CREATE_FIELD:
					CCreateField * pCreateField;
					pCreateField = (CCreateField *) spells[i].pSpellFx;

					if (	(pCreateField)
						&&	(pCreateField->lLightId != -1)	)
					{
						long id=pCreateField->lLightId;
						DynLight[id].duration=800;
					}

					if (ValidIONum(spells[i].longinfo))
					{				
						ReleaseInter(inter.iobj[spells[i].longinfo]);
					}					

				break;
				//----------------------------------------------------------------------------
				case SPELL_SLOW_DOWN:
					ARX_SPELLS_RemoveSpellOn(spells[i].target,i);

					if ((spells[i].target>=0) && (spells[i].target<inter.nbmax))
					{
						if (inter.iobj[spells[i].target])
							inter.iobj[spells[i].target]->speed_modif+=spells[i].caster_level*( 1.0f / 20 );
					}

				break;				
				//----------------------------------------------------------------------------------
				//**********************************************************************************
				// LEVEL 7 -------------------------------------------------------------------------
				//----------------------------------------------------------------------------------				
				case SPELL_ICE_FIELD:

					if (spells[i].longinfo!=-1)
						damages[spells[i].longinfo].exist=false;					

				break;
				case SPELL_FIRE_FIELD:

					if (spells[i].longinfo!=-1)
						damages[spells[i].longinfo].exist=false;					

				break;
				//----------------------------------------------------------------------------
				case SPELL_LIGHTNING_STRIKE:					
					ARX_SOUND_PlaySFX(SND_SPELL_ELECTRIC, &spells[i].caster_pos);					
				break;
				//----------------------------------------------------------------------------
				case SPELL_FLYING_EYE:					
					ARX_SOUND_PlaySFX(SND_MAGIC_FIZZLE, &inter.iobj[spells[i].caster]->pos);
				break;
				case SPELL_CONFUSE:
					ARX_SPELLS_RemoveSpellOn(spells[i].target,i);
				break;
				//----------------------------------------------------------------------------------
				//**********************************************************************************
				// LEVEL 8 -------------------------------------------------------------------------
				//----------------------------------------------------------------------------------
				case SPELL_EXPLOSION:					
				break;
				//----------------------------------------------------------------------------
				case SPELL_INVISIBILITY:	
					{
						if (ValidIONum(spells[i].target))
						{
							inter.iobj[spells[i].target]->GameFlags&=~GFLAG_INVISIBILITY;											
							ARX_SOUND_PlaySFX(SND_SPELL_INVISIBILITY_END, &inter.iobj[spells[i].target]->pos);					
							ARX_SPELLS_RemoveSpellOn(spells[i].target,i);
						}
					}
				break;
				//----------------------------------------------------------------------------------
				//**********************************************************************************
				// LEVEL 9 -------------------------------------------------------------------------
				//----------------------------------------------------------------------------------
				//----------------------------------------------------------------------------
				case SPELL_MASS_PARALYSE:
					{
						long * ptr=(long *)spells[i].misc;

						for (long in=0;in<spells[i].longinfo2;in++)
						{
							
							if (ValidIONum(ptr[in]))
							{
								ARX_SPELLS_RemoveSpellOn(ptr[in],i);
								inter.iobj[ptr[in]]->ioflags&=~IO_FREEZESCRIPT;											
							}
						}

						if (ptr) free(spells[i].misc);

						spells[i].misc=NULL;
					}
				break;
				case SPELL_SUMMON_CREATURE :
						ARX_SOUND_PlaySFX(SND_SPELL_ELECTRIC, &spells[i].caster_pos);						

						if (spells[i].pSpellFx->lLightId > -1)
					{
						long id = spells[i].pSpellFx->lLightId;
						DynLight[id].exist = 0;
						spells[i].pSpellFx->lLightId=-1;
					}

						// need to killio
				break;
				case SPELL_FAKE_SUMMON :
					ARX_SOUND_PlaySFX(SND_SPELL_ELECTRIC, &spells[i].caster_pos);						

					if (spells[i].pSpellFx->lLightId > -1)
					{
						long id = spells[i].pSpellFx->lLightId;
						DynLight[id].exist = 0;
						spells[i].pSpellFx->lLightId=-1;
					}

				break;
				case SPELL_INCINERATE:
					ARX_SPELLS_RemoveSpellOn(spells[i].target,i);					
					ARX_SOUND_Stop(spells[i].snd_loop);
				break;
				//----------------------------------------------------------------------------------
				//**********************************************************************************
				// LEVEL 10 ------------------------------------------------------------------------
				//----------------------------------------------------------------------------------
				case SPELL_FREEZE_TIME:
						GLOBAL_SLOWDOWN += spells[i].siz;
					ARX_SOUND_PlaySFX(SND_SPELL_TELEKINESIS_END, &spells[i].caster_pos);					
				break;
				case SPELL_MASS_INCINERATE:
					ARX_SPELLS_RemoveMultiSpellOn(i);
					ARX_SOUND_Stop(spells[i].snd_loop);
				break;
				default: break;
				//----------------------------------------------------------------------------------
			}				

			ARX_SPELLS_Kill(i);
			continue;			
		}
			
		//******************************************************************************************
		//******************************************************************************************
		//******************************************************************************************
		//******************************************************************************************
		//******************************************************************************************
		//******************************************************************************************

	if (spells[i].exist) 
		switch (spells[i].type)
		{
			case SPELL_DISPELL_FIELD: break;
			case SPELL_NONE: break;
			//**************************************************************************************
			// LEVEL 1 -----------------------------------------------------------------------------
			case SPELL_MAGIC_MISSILE:
				{
					CSpellFx *pCSpellFX = spells[i].pSpellFx;

					if (pCSpellFX)
					{
						CMultiMagicMissile *pMMM = (CMultiMagicMissile *) pCSpellFX;
							pMMM->CheckCollision();
						
						// Update
						pCSpellFX->Update(FrameDiff);

						if (pCSpellFX->Render()==-1)
							spells[i].tolive=0;
					}
				}
			break;
			//---------------------------------------------------------------------------------------
			case SPELL_IGNIT:
			case SPELL_DOUSE:
			{
				CSpellFx *pCSpellFX = spells[i].pSpellFx;

				if (pCSpellFX)
				{
					pCSpellFX->Update(FrameDiff);
				}
			} 
			break;
			//---------------------------------------------------------------------------------------
			case SPELL_ACTIVATE_PORTAL:
			{
			} 
			break;
			//---------------------------------------------------------------------------------------
			//***************************************************************************************	
			// LEVEL 2 -----------------------------------------------------------------------------
			case SPELL_HEAL: // guérit les ennemis collés
			{
				CSpellFx *pCSpellFX = spells[i].pSpellFx;

				if (pCSpellFX)
				{
					pCSpellFX->Update(FrameDiff);
					pCSpellFX->Render();
				}

				CHeal * ch=(CHeal *)pCSpellFX;

				if (ch)
				for (long ii=0;ii<inter.nbmax;ii++)
				{
					if ((inter.iobj[ii]) 						
						&& (inter.iobj[ii]->show==SHOW_FLAG_IN_SCENE) 
						&& (inter.iobj[ii]->GameFlags & GFLAG_ISINTREATZONE)
								        && (inter.iobj[ii]->ioflags & IO_NPC)
						&& (inter.iobj[ii]->_npcdata->life>0.f)
						)
					{
						float dist;

						if (ii==spells[i].caster) dist=0;
						else dist=fdist(ch->eSrc, inter.iobj[ii]->pos);

						if (dist<300.f)
						{
							float gain=((rnd()*1.6f+0.8f)*spells[i].caster_level)*(300.f-dist)*( 1.0f / 300 )*_framedelay*( 1.0f / 1000 );

							if (ii==0) 
							{
								if (!BLOCK_PLAYER_CONTROLS)
									player.life=std::min(player.life+gain,player.Full_maxlife);									
							}
							else inter.iobj[ii]->_npcdata->life=std::min(inter.iobj[ii]->_npcdata->life+gain,inter.iobj[ii]->_npcdata->maxlife);
						}
					}
				}
			}
			break;
			//------------------------------------------------------------------------------------
			case SPELL_DETECT_TRAP:				
			{
				if (spells[i].caster == 0)
				{
					ARX_SOUND_RefreshPosition(spells[i].snd_loop);						
				}

				CSpellFx *pCSpellFX = spells[i].pSpellFx;

				if (pCSpellFX)
				{
					pCSpellFX->Update(FrameDiff);
					pCSpellFX->Render();
				}
			} 
			break;
			//------------------------------------------------------------------------------------
			case SPELL_ARMOR:
			case SPELL_LOWER_ARMOR:
			{
				CSpellFx *pCSpellFX = spells[i].pSpellFx;

				if (pCSpellFX)
				{
					pCSpellFX->Update(FrameDiff);
					pCSpellFX->Render();
				}
			} 
			break;
			//--------------------------------------------------------------------------------------
			case SPELL_HARM:
			{						
				if ( (cabal!=NULL) )
				{
					float refpos;
					float scaley;

					if (spells[i].caster==0) scaley=90.f;
					else scaley=EEfabs(inter.iobj[spells[i].caster]->physics.cyl.height*( 1.0f / 2 ))+30.f;

 
					float mov=EEsin((float)arxtime.get_frame_time()*( 1.0f / 800 ))*scaley;

					if (spells[i].caster==0)
					{
								cabalpos.x = player.pos.x; 
						cabalpos.y=player.pos.y+60.f-mov;
								cabalpos.z = player.pos.z; 
						refpos=player.pos.y+60.f;							
					}
					else
					{							
								cabalpos.x = inter.iobj[spells[i].caster]->pos.x; 
						cabalpos.y=inter.iobj[spells[i].caster]->pos.y-scaley-mov;
								cabalpos.z = inter.iobj[spells[i].caster]->pos.z; 
						refpos=inter.iobj[spells[i].caster]->pos.y-scaley;							
					}

					float Es=EEsin((float)arxtime.get_frame_time()*( 1.0f / 800 ) + radians(scaley));

					if (spells[i].longinfo2!=-1)
					{
						DynLight[spells[i].longinfo2].pos.x = cabalpos.x;
						DynLight[spells[i].longinfo2].pos.y = refpos;
						DynLight[spells[i].longinfo2].pos.z = cabalpos.z; 
						DynLight[spells[i].longinfo2].rgb.r=rnd()*0.2f+0.8f;
						DynLight[spells[i].longinfo2].rgb.g=rnd()*0.2f+0.6f;
						DynLight[spells[i].longinfo2].fallstart=Es*1.5f;
					}

					GRenderer->SetBlendFunc(Renderer::BlendOne, Renderer::BlendOne);
					GRenderer->SetRenderState(Renderer::AlphaBlending, true);
					GRenderer->SetRenderState(Renderer::DepthWrite, false);
					cabalangle.b=spells[i].fdata+(float)FrameDiff*0.1f;
					spells[i].fdata=cabalangle.b;

							cabalangle.g = 0.f; 
							cabalcolor.r = 0.8f;
							cabalcolor.g = 0.4f;
							cabalcolor.b = 0.f;
					cabalscale.z=cabalscale.y=cabalscale.x=Es;				
					DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
					mov=EEsin((float)(arxtime.get_frame_time()-30.f)*( 1.0f / 800 ))*scaley;
					cabalpos.y=refpos-mov;						
							cabalcolor.b = 0.f;
							cabalcolor.g = 3.f;
							cabalcolor.r = 0.5f;
					DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
					mov=EEsin((float)(arxtime.get_frame_time()-60.f)*( 1.0f / 800 ))*scaley;
					cabalpos.y=refpos-mov;
							cabalcolor.b = 0.f;
							cabalcolor.g = 0.1f;
							cabalcolor.r = 0.25f;
					DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
					mov=EEsin((float)(arxtime.get_frame_time()-120.f)*( 1.0f / 800 ))*scaley;
					cabalpos.y=refpos-mov;
							cabalcolor.b = 0.f;
							cabalcolor.g = 0.1f;
							cabalcolor.r = 0.15f;
					DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
					GRenderer->SetRenderState(Renderer::AlphaBlending, false);		
					GRenderer->SetRenderState(Renderer::DepthWrite, true);	
				}
			}
			break;				
			//--------------------------------------------------------------------------------------
			//**************************************************************************************
			// LEVEL 3 SPELLS -----------------------------------------------------------------------------
			case SPELL_FIREBALL:
			{
				CSpellFx *pCSpellFX = spells[i].pSpellFx;

				if (pCSpellFX)
				{
					CFireBall *pCF = (CFireBall*) pCSpellFX;
						
					if (spells[i].longinfo==-1) spells[i].longinfo=GetFreeDynLight();

					if (spells[i].longinfo!=-1)
					{
						long id=spells[i].longinfo;
						EERIE_LIGHT * el=&DynLight[id];
						el->exist=1;
						el->pos = pCF->eCurPos;
						el->intensity = 2.2f;
						el->fallend = 500.f;
						el->fallstart = 400.f;
						el->rgb.r = 1.0f-rnd()*0.3f;
						el->rgb.g = 0.6f-rnd()*0.1f;;
						el->rgb.b = 0.3f-rnd()*0.1f;;
					}

					EERIE_SPHERE sphere;
					sphere.origin.x=pCF->eCurPos.x;
					sphere.origin.y=pCF->eCurPos.y;
					sphere.origin.z=pCF->eCurPos.z;
					sphere.radius=std::max(spells[i].caster_level*2.f,12.f);
					#define MIN_TIME_FIREBALL 2000 

					if (pCF->pPSFire.iParticleNbMax)
					{
						if (pCF->ulCurrentTime > MIN_TIME_FIREBALL)
						{
							SpawnFireballTail(&pCF->eCurPos,&pCF->eMove,(float)spells[i].caster_level,0);
						}
						else
						{
							if (rnd()<0.9f)
							{
								Vec3f move(0, 0, 0);
								float dd=(float)pCF->ulCurrentTime / (float)MIN_TIME_FIREBALL*10;

								if (dd>spells[i].caster_level) dd=spells[i].caster_level;

								if (dd<1) dd=1;

								SpawnFireballTail(&pCF->eCurPos,&move,(float)dd,1);
							}
						}
					}

					if (pCF->bExplo == false)
					if (CheckAnythingInSphere(&sphere,spells[i].caster,CAS_NO_SAME_GROUP))
					{
						ARX_BOOMS_Add(&pCF->eCurPos);
						LaunchFireballBoom(&pCF->eCurPos,(float)spells[i].caster_level);
						pCF->pPSFire.iParticleNbMax = 0;
						pCF->pPSFire2.iParticleNbMax = 0;
						pCF->eMove.x *= 0.5f;
						pCF->eMove.y *= 0.5f;
						pCF->eMove.z *= 0.5f;
						pCF->pPSSmoke.iParticleNbMax = 0;
						pCF->SetTTL(1500);
						pCF->bExplo = true;
						
						DoSphericDamage(&pCF->eCurPos,3.f*spells[i].caster_level,30.f*spells[i].caster_level,DAMAGE_AREA,DAMAGE_TYPE_FIRE | DAMAGE_TYPE_MAGICAL,spells[i].caster);
						spells[i].tolive=0;
						ARX_SOUND_PlaySFX(SND_SPELL_FIRE_HIT, &sphere.origin);
						ARX_NPC_SpawnAudibleSound(&sphere.origin, inter.iobj[spells[i].caster]);
								}

					pCSpellFX->Update(FrameDiff);
					ARX_SOUND_RefreshPosition(spells[i].snd_loop, &pCF->eCurPos);
				}
			}
			break;
			//-----------------------------------------------------------------------------------------
			case SPELL_SPEED:

			if (spells[i].pSpellFx)
			{
				spells[i].pSpellFx->Update(FrameDiff);
				spells[i].pSpellFx->Render();

				if (spells[i].caster == 0) ARX_SOUND_RefreshPosition(spells[i].snd_loop);
			}

			break;
			//-----------------------------------------------------------------------------------------
			case SPELL_CREATE_FOOD:
			case SPELL_ICE_PROJECTILE:
			case SPELL_DISPELL_ILLUSION:

			if (spells[i].pSpellFx)
			{
				spells[i].pSpellFx->Update(FrameDiff);
				spells[i].pSpellFx->Render();
			}

			break;
			//-----------------------------------------------------------------------------------------
			//*****************************************************************************************
			// LEVEL 4 SPELLS -----------------------------------------------------------------------------
			case SPELL_BLESS:
			{
				if (spells[i].pSpellFx)
				{
					CBless * pBless=(CBless *)spells[i].pSpellFx;

					if (pBless)
					{
						if (ValidIONum(spells[i].target))
						{
							pBless->eSrc = inter.iobj[spells[i].target]->pos;
							Anglef angle = Anglef::ZERO;

							if (spells[i].target==0)
								angle.b=player.angle.b;	
							else 
								angle.b=inter.iobj[spells[i].target]->angle.b;

							pBless->Set_Angle(angle);
						}
					}

					spells[i].pSpellFx->Update(FrameDiff);
					spells[i].pSpellFx->Render();
				}
			}
			break;
			//-----------------------------------------------------------------------------------------
			case SPELL_CURSE:

			if (spells[i].pSpellFx)
			{
				CCurse * curse=(CCurse *)spells[i].pSpellFx;
				Vec3f target(0, 0, 0);
					
				if ((spells[i].target>=0) && (inter.iobj[spells[i].target]))
				{
					target = inter.iobj[spells[i].target]->pos;

					if (spells[i].target==0) target.y-=200.f;
					else target.y+=inter.iobj[spells[i].target]->physics.cyl.height-30.f;
				}
				
				curse->Update(checked_range_cast<unsigned long>(FrameDiff));
				
				curse->eTarget = target;
				curse->Render();
				GRenderer->SetCulling(Renderer::CullNone);
			}

			break;
			//-----------------------------------------------------------------------------------------
			case SPELL_FIRE_PROTECTION:
				spells[i].pSpellFx->Update(FrameDiff);
				spells[i].pSpellFx->Render();
			break;
			//-----------------------------------------------------------------------------------------
			case SPELL_COLD_PROTECTION:
				spells[i].pSpellFx->Update(FrameDiff);
				spells[i].pSpellFx->Render();
			break;
			//-----------------------------------------------------------------------------------------
			//*****************************************************************************************				
			// LEVEL 5 SPELLS -----------------------------------------------------------------------------
			//-----------------------------------------------------------------------------------------
			case SPELL_CURE_POISON:
			{
				if (spells[i].pSpellFx)
				{
					spells[i].pSpellFx->Update(FrameDiff);
					spells[i].pSpellFx->Render();					
				}
			}
			break;
			//-----------------------------------------------------------------------------------------
			case SPELL_RUNE_OF_GUARDING:
			{
				if (spells[i].pSpellFx)
				{
					spells[i].pSpellFx->Update(FrameDiff);
					spells[i].pSpellFx->Render();
					CRuneOfGuarding * pCRG=(CRuneOfGuarding *)spells[i].pSpellFx;

					if (pCRG)
					{
						EERIE_SPHERE sphere;
						sphere.origin.x=pCRG->eSrc.x;
						sphere.origin.y=pCRG->eSrc.y;
						sphere.origin.z=pCRG->eSrc.z;
						sphere.radius=std::max(spells[i].caster_level*15.f,50.f);

						if (CheckAnythingInSphere(&sphere,spells[i].caster,CAS_NO_SAME_GROUP | CAS_NO_BACKGROUND_COL | CAS_NO_ITEM_COL| CAS_NO_FIX_COL | CAS_NO_DEAD_COL))
						{
							ARX_BOOMS_Add(&pCRG->eSrc);
							LaunchFireballBoom(&pCRG->eSrc,(float)spells[i].caster_level);
							DoSphericDamage(&pCRG->eSrc,4.f*spells[i].caster_level,30.f*spells[i].caster_level,DAMAGE_AREA,DAMAGE_TYPE_FIRE | DAMAGE_TYPE_MAGICAL,spells[i].caster);
							spells[i].tolive=0;
							ARX_SOUND_PlaySFX(SND_SPELL_FIRE_HIT, &sphere.origin);
						}
					}
				}
			}
			break;
			case SPELL_REPEL_UNDEAD:
			{
				if (spells[i].pSpellFx)
				{
					spells[i].pSpellFx->Update(FrameDiff);
					spells[i].pSpellFx->Render();					

					if (spells[i].target == 0)
						ARX_SOUND_RefreshPosition(spells[i].snd_loop);
				}
			}
			break;
			//-----------------------------------------------------------------------------------------
			case SPELL_POISON_PROJECTILE:

			if (spells[i].pSpellFx)
			{
				spells[i].pSpellFx->Update(FrameDiff);
				spells[i].pSpellFx->Render();
						}

			break;
			//-----------------------------------------------------------------------------------------	
			case SPELL_LEVITATE:
			{
				CLevitate *pLevitate=(CLevitate *)spells[i].pSpellFx;
				Vec3f target;

				if (spells[i].target==0)
				{
					target.x=player.pos.x;
					target.y=player.pos.y+150.f;
					target.z=player.pos.z;
					player.levitate=1;
				}
				else
				{
					target.x=inter.iobj[spells[i].caster]->pos.x;
							target.y = inter.iobj[spells[i].caster]->pos.y; 
					target.z=inter.iobj[spells[i].caster]->pos.z;
				}

				pLevitate->ChangePos(&target);
					
				CSpellFx *pCSpellFX = spells[i].pSpellFx;

				if (pCSpellFX)
				{
					pCSpellFX->Update(FrameDiff);
					pCSpellFX->Render();
					GRenderer->SetCulling(Renderer::CullNone);
				}
			}
			break;
			//-----------------------------------------------------------------------------------------
			//*****************************************************************************************
			// LEVEL 6 SPELLS -----------------------------------------------------------------------------
			case SPELL_RISE_DEAD:
			{
				CSpellFx *pCSpellFX = spells[i].pSpellFx;

				if (pCSpellFX)
				{				
					if (spells[i].longinfo==-2) 
					{
						pCSpellFX->lLightId=-1;
						break;
					}

					spells[i].tolive+=200;
				
					pCSpellFX->Update(FrameDiff);
							pCSpellFX->Render();

					if (pCSpellFX->lLightId > -1)
					{
						long id=pCSpellFX->lLightId;
						DynLight[id].exist=1;
								DynLight[id].intensity = 0.7f + 2.3f;
						DynLight[id].fallend = 500.f;
						DynLight[id].fallstart = 400.f;
						DynLight[id].rgb.r = 0.8f;
						DynLight[id].rgb.g = 0.2f;
						DynLight[id].rgb.b = 0.2f;
						DynLight[id].duration=800;
								DynLight[id].time_creation = (unsigned long)(arxtime);
					}

					unsigned long tim=pCSpellFX->getCurrentTime();

					if ((tim>3000) && (spells[i].longinfo==-1))
					{
						ARX_SOUND_PlaySFX(SND_SPELL_ELECTRIC, &spells[i].caster_pos);
						char tmptext[256];						
						CRiseDead *prise;
						prise= (CRiseDead *)spells[i].pSpellFx;

						if (prise)
						{	
							EERIE_CYLINDER phys;
							phys.height=-200;
							phys.radius=50;
							phys.origin=spells[i].target_pos;
	
									float anything = CheckAnythingInCylinder(&phys, NULL, CFLAG_JUST_TEST);

							if (EEfabs(anything)<30)
							{
								strcpy(tmptext,"graph/obj3d/interactive/npc/undead_base/undead_base.asl");
								INTERACTIVE_OBJ * io;
								io=AddNPC(tmptext,IO_IMMEDIATELOAD);

								if (io)
								{
									ARX_INTERACTIVE_HideGore(io);
									RestoreInitialIOStatusOfIO(io);
											

									long lSpellsCaster = spells[i].caster ; 
									io->summoner = checked_range_cast<short>(lSpellsCaster);

										
									io->ioflags|=IO_NOSAVE;
									spells[i].longinfo=GetInterNum(io);
									io->scriptload=1;
											
									ARX_INTERACTIVE_Teleport(io,&phys.origin,0);
									MakeTemporaryIOIdent(io);						
									SendInitScriptEvent(io);

									if ((spells[i].caster>=0) && (spells[i].caster<inter.nbmax))
										EVENT_SENDER=inter.iobj[spells[i].caster];
									else EVENT_SENDER=NULL;

									SendIOScriptEvent(io,SM_SUMMONED);
										
									Vec3f pos;
										{
											pos.x=prise->eSrc.x+rnd()*100.f-50.f;
											pos.y=prise->eSrc.y+100+rnd()*100.f-50.f;
											pos.z=prise->eSrc.z+rnd()*100.f-50.f;
											MakeCoolFx(&pos);
										}
									}

									pCSpellFX->lLightId=-1;
								}
								else
								{
									ARX_SOUND_PlaySFX(SND_MAGIC_FIZZLE);
									spells[i].longinfo=-2;
									spells[i].tolive=0;
								}
							}
							
						}
						else if ((!arxtime.is_paused()) && (tim<4000))
						{
						  if (rnd()>0.95f) 
							{
								CRiseDead *pRD = (CRiseDead*)pCSpellFX;
								Vec3f pos;
								pos.x = pRD->eSrc.x;
								pos.y = pRD->eSrc.y;
								pos.z = pRD->eSrc.z;
								MakeCoolFx(&pos);
							}
						}
						
					}
			}
			break;
								
			//-----------------------------------------------------------------------------------------
			case SPELL_SLOW_DOWN:
			{
				CSpellFx *pCSpellFX = spells[i].pSpellFx;

				if (pCSpellFX)
				{
					pCSpellFX->Update(FrameDiff);
					pCSpellFX->Render();
				}					
			}
			break;
			case SPELL_DISARM_TRAP:
			{
			}
			break;
			case SPELL_PARALYSE:
			break;
			case SPELL_CREATE_FIELD:
			{
				CSpellFx *pCSpellFX = spells[i].pSpellFx;

				if (pCSpellFX)
				{				
					if (ValidIONum(spells[i].longinfo))
					{
						INTERACTIVE_OBJ * io=inter.iobj[spells[i].longinfo];
						CCreateField * ccf=(CCreateField *)pCSpellFX;
						io->pos.x = ccf->eSrc.x;
						io->pos.y = ccf->eSrc.y;
						io->pos.z = ccf->eSrc.z;

						if (IsAnyNPCInPlatform(io))
						{
							spells[i].tolive=0;
						}
					
						pCSpellFX->Update(FrameDiff);

						if (VisibleSphere(ccf->eSrc.x,ccf->eSrc.y-120.f,ccf->eSrc.z,400.f))					
							pCSpellFX->Render();
					}
				}					
			}
			break;

			//-----------------------------------------------------------------------------------------
			//*****************************************************************************************
			// LEVEL 7 SPELLS -----------------------------------------------------------------------------
			case SPELL_CONFUSE:
			{
				CSpellFx *pCSpellFX = spells[i].pSpellFx;

				if (pCSpellFX)
				{
					pCSpellFX->Update(FrameDiff);
					pCSpellFX->Render();
				}
			}
			break;
			case SPELL_FIRE_FIELD:
			{
				CSpellFx *pCSpellFX = spells[i].pSpellFx;

				if (pCSpellFX)
				{
					CFireField *pf = (CFireField *) pCSpellFX;
					pCSpellFX->Update(FrameDiff);
					
					if (spells[i].longinfo2==-1)
						spells[i].longinfo2=GetFreeDynLight();

					if (spells[i].longinfo2!=-1)
					{
						EERIE_LIGHT * el=&DynLight[spells[i].longinfo2];						
						
						el->pos.x = pf->pos.x;
						el->pos.y = pf->pos.y-120.f;
						el->pos.z = pf->pos.z;
						el->exist = 1;
						el->intensity = 4.6f;
						el->fallstart = 150.f+rnd()*30.f;
						el->fallend   = 290.f+rnd()*30.f;
						el->rgb.r = 1.f-rnd()*( 1.0f / 10 );
						el->rgb.g = 0.8f;
						el->rgb.b = 0.6f;
						el->duration = 600;
						el->extras=0;						
					}

 

					if (VisibleSphere(pf->pos.x,pf->pos.y-120.f,pf->pos.z,350.f))					
					{
						pCSpellFX->Render();


						float fDiff = FrameDiff / 8.f;
						int nTime = checked_range_cast<int>(fDiff);

						
						for (long nn=0;nn<=nTime+1;nn++)
						{
							long j=ARX_PARTICLES_GetFree();

							if ((j!=-1) && (!arxtime.is_paused()) )
							{
								ParticleCount++;
								PARTICLE_DEF * pd=&particle[j];
								pd->exist=true;
								pd->zdec=0;
								float sy=rnd()*3.14159f*2.f-3.14159f;
								float sx=EEsin(sy);
								float sz=EEcos(sy);
								sy=EEsin(sy); 
								pd->ov.x=pf->pos.x+120.f*sx*rnd();
								pd->ov.y=pf->pos.y+15.f*sy*rnd();
								pd->ov.z=pf->pos.z+120.f*sz*rnd();
								
								pd->move.x=(2.f-4.f*rnd());
								pd->move.y=(1.f-8.f*rnd());
								pd->move.z=(2.f-4.f*rnd());
								
								pd->siz			=	7.f;
								pd->tolive		=	500+(unsigned long)(rnd()*1000.f);
								pd->special		=	0;
								pd->tc			=	fire2;						
										pd->special		|=	ROTATING | MODULATE_ROTATION | FIRE_TO_SMOKE;
								pd->fparam		=	0.1f-rnd()*0.2f;
								pd->scale.x		=	-8.f;
								pd->scale.y		=	-8.f;
								pd->scale.z		=	-8.f;
										pd->timcreation	=	(long)arxtime;
								pd->rgb = Color3f::white;
								long j2			=	ARX_PARTICLES_GetFree();

								if (j2!=-1)
								{
									ParticleCount++;
									PARTICLE_DEF * pd2=&particle[j2];
									memcpy(pd2,pd,sizeof(PARTICLE_DEF));
									pd2->delay=(long)(float)(rnd()*150.f+60.f);
								}
							}
						}
					}
				}
			}
			break;
			case SPELL_ICE_FIELD:
			{
				CSpellFx *pCSpellFX = spells[i].pSpellFx;

				if (pCSpellFX)
				{
					pCSpellFX->Update(FrameDiff);
					
					CIceField *pf = (CIceField *) pCSpellFX;

					if (spells[i].longinfo2==-1)
						spells[i].longinfo2=GetFreeDynLight();

					if (spells[i].longinfo2!=-1)
					{
						EERIE_LIGHT * el=&DynLight[spells[i].longinfo2];						
						
						el->pos.x = pf->eSrc.x;
						el->pos.y = pf->eSrc.y-120.f;
						el->pos.z = pf->eSrc.z;
						el->exist = 1;
						el->intensity = 4.6f;
						el->fallstart = 150.f+rnd()*30.f;
						el->fallend   = 290.f+rnd()*30.f;
						el->rgb.r = 0.76f;
						el->rgb.g = 0.76f;
						el->rgb.b = 1.0f-rnd()*( 1.0f / 10 );
						el->duration = 600;
						el->extras=0;						
					}

 

					if (VisibleSphere(pf->eSrc.x,pf->eSrc.y-120.f,pf->eSrc.z,350.f))
					{
						pCSpellFX->Render();
					}
				}

				GRenderer->SetCulling(Renderer::CullNone);
			}
			break;
			//-----------------------------------------------------------------------------------------
			case SPELL_LIGHTNING_STRIKE:
				{
					CSpellFx *pCSpellFX = spells[i].pSpellFx;

					if (pCSpellFX)
					{
						pCSpellFX->Update(FrameDiff);
						pCSpellFX->Render();
					}
						}
			break;
			//-----------------------------------------------------------------------------------------
			//*****************************************************************************************
			// LEVEL 8 SPELLS -----------------------------------------------------------------------------
			case SPELL_ENCHANT_WEAPON:
			{
				CSpellFx *pCSpellFX = spells[i].pSpellFx;

				if (pCSpellFX)
				{
					pCSpellFX->Update(FrameDiff);
					pCSpellFX->Render();
				}
			}
			case SPELL_EXPLOSION:
			{
				if (spells[i].longinfo2 == -1)
					spells[i].longinfo2=GetFreeDynLight();

				if (spells[i].longinfo2 != -1)
				{
					long id = spells[i].longinfo2;
					DynLight[id].rgb.r = 0.1f+rnd()*( 1.0f / 3 );;
					DynLight[id].rgb.g = 0.1f+rnd()*( 1.0f / 3 );;
					DynLight[id].rgb.b = 0.8f+rnd()*( 1.0f / 5 );;
					DynLight[id].duration=200;
				
					float rr,r2;
					Vec3f pos;
					
					float choice = rnd();
					if(choice > .8f) {
						long lvl = rnd() * 9.f + 4.f;
						rr=radians(rnd()*360.f);
						r2=radians(rnd()*360.f);
						pos.x=DynLight[id].pos.x-EEsin(rr)*260;
						pos.y=DynLight[id].pos.y-EEsin(r2)*260;
						pos.z=DynLight[id].pos.z+EEcos(rr)*260;
						Color3f rgb(0.1f + rnd()*(1.f/3), 0.1f + rnd()*(1.0f/3), 0.8f + rnd()*(1.0f/5));
						LaunchFireballBoom(&pos, static_cast<float>(lvl), NULL, &rgb);
					} else if(choice > .6f) {
						rr=radians(rnd()*360.f);
						r2=radians(rnd()*360.f);
						pos.x=DynLight[id].pos.x-EEsin(rr)*260;
						pos.y=DynLight[id].pos.y-EEsin(r2)*260;
						pos.z=DynLight[id].pos.z+EEcos(rr)*260;
						MakeCoolFx(&pos);
					} else if(choice > 0.4f) {
						rr=radians(rnd()*360.f);
						r2=radians(rnd()*360.f);
						pos.x=DynLight[id].pos.x-EEsin(rr)*160;
						pos.y=DynLight[id].pos.y-EEsin(r2)*160;
						pos.z=DynLight[id].pos.z+EEcos(rr)*160;
						ARX_PARTICLES_Add_Smoke(&pos, 2, 20); // flag 1 = randomize pos
					}
				}
			}
			break;
			//-----------------------------------------------------------------------------------------
			//*****************************************************************************************	
			// LEVEL 9 SPELLS -------------------------------------------------------------------------
			//-----------------------------------------------------------------------------------------
			case SPELL_SUMMON_CREATURE:
			{
 

				if (!arxtime.is_paused())
				{

					if (float(arxtime)-(float)spells[i].timcreation<=4000)
					{
						if (rnd()>0.7f) 
						{
							Vec3f pos;
							CSummonCreature *pSummon;
							pSummon= (CSummonCreature *)spells[i].pSpellFx;

							if (pSummon)
							{
								pos.x=pSummon->eSrc.x;
								pos.y=pSummon->eSrc.y;
								pos.z=pSummon->eSrc.z;
								MakeCoolFx(&pos);
							}
						}

						CSpellFx *pCSpellFX = spells[i].pSpellFx;

						if (pCSpellFX)
						{
							pCSpellFX->Update(FrameDiff);
							pCSpellFX->Render();
						}	

						spells[i].longinfo=1;
						spells[i].longinfo2=-1;
					}
					else if (spells[i].longinfo)
					{
						if (spells[i].pSpellFx->lLightId > -1)
						{
							long id = spells[i].pSpellFx->lLightId;
							DynLight[id].exist = 0;
							spells[i].pSpellFx->lLightId=-1;
						}

						spells[i].longinfo=0;
						ARX_SOUND_PlaySFX(SND_SPELL_ELECTRIC, &spells[i].caster_pos);
						char tmptext[256];						
						CSummonCreature *pSummon;
						pSummon= (CSummonCreature *)spells[i].pSpellFx;

						if (pSummon)
						{			
							EERIE_CYLINDER phys;
							phys.height=-200;
							phys.radius=50;
							phys.origin=spells[i].target_pos;
									float anything = CheckAnythingInCylinder(&phys, NULL, CFLAG_JUST_TEST);

							if (EEfabs(anything)<30)
							{
 
							long tokeep=0;
							

							if (spells[i].caster_level>=9)
							{
								tokeep=1;
								strcpy(tmptext,"graph/obj3d/interactive/npc/demon/demon.asl");
							}
							else 
							{
								tokeep=0;

								if (rnd()>0.98f)
								{
									strcpy(tmptext,"graph/obj3d/interactive/npc/wrat_base/wrat_base.asl");
									tokeep=-1;
								}
								else
									strcpy(tmptext,"graph/obj3d/interactive/npc/chicken_base/chicken_base.asl");
							}

							if ((rnd()>0.997f) || ((cur_rf>=3) && (rnd()>0.8f)) || ((cur_mr>=3) && (rnd()>0.3f)))
							{
								strcpy(tmptext,"graph/obj3d/interactive/npc/wrat_base/wrat_base.asl");
								tokeep=-1;
							}

							if ((rnd()>0.997f) || (sp_max && (rnd()>0.8f)) || ((cur_mr>=3) && (rnd()>0.3f)))
							{
								strcpy(tmptext,"graph/obj3d/interactive/npc/y_mx/y_mx.asl");
								tokeep=0;
							}

							if (spells[i].fdata==1.f)
							{
								if (rnd()>0.5) 
								{
									strcpy(tmptext,"graph/obj3d/interactive/npc/wrat_base/wrat_base.asl");
									tokeep=-1;
								}
								else
								{
									strcpy(tmptext,"graph/obj3d/interactive/npc/y_mx/y_mx.asl");
									tokeep=0;
								}
							}

							INTERACTIVE_OBJ * io;
							io=AddNPC(tmptext,IO_IMMEDIATELOAD);

							if (!io)
							{
								strcpy(tmptext,"graph/obj3d/interactive/npc/chicken_base/chicken_base.asl");
								tokeep=0;
								io=AddNPC(tmptext,IO_IMMEDIATELOAD);
							}

							if (io)
							{
								RestoreInitialIOStatusOfIO(io);
								

								long lSpellsCaster = spells[i].caster ; 
								io->summoner = checked_range_cast<short>(lSpellsCaster);


								io->scriptload=1;

								if (tokeep==1)
									io->ioflags|=IO_NOSAVE;

											io->pos.x = phys.origin.x; 
											io->pos.y = phys.origin.y;
											io->pos.z = phys.origin.z;
								MakeTemporaryIOIdent(io);
								SendInitScriptEvent(io);

								if (tokeep<0)
								{
									io->scale=1.65f;
									io->physics.cyl.radius=25;
									io->physics.cyl.height=-43;
									io->speed_modif=1.f;
								}

								if ((spells[i].caster>=0) && (spells[i].caster<inter.nbmax))
									EVENT_SENDER=inter.iobj[spells[i].caster];
								else EVENT_SENDER=NULL;

								SendIOScriptEvent(io,SM_SUMMONED);
								
											Vec3f pos;
								
								for (long j=0;j<3;j++)
								{
									pos.x=pSummon->eSrc.x+rnd()*100.f-50.f;
									pos.y=pSummon->eSrc.y+100+rnd()*100.f-50.f;
									pos.z=pSummon->eSrc.z+rnd()*100.f-50.f;
									MakeCoolFx(&pos);
								}

								if (tokeep==1)	spells[i].longinfo2=GetInterNum(io);
								else spells[i].longinfo2=-1;
							}
							}
					}
					}
					else if (spells[i].longinfo2<=0)
					{
						spells[i].tolive=0;
					}
				}
			}
			break;
			case SPELL_FAKE_SUMMON:
			{
 

					if (!arxtime.is_paused())
						if (rnd()>0.7f) 
						{
							Vec3f pos;
							CSummonCreature *pSummon;
							pSummon= (CSummonCreature *)spells[i].pSpellFx;

							if (pSummon)
							{
								pos.x=pSummon->eSrc.x;
								pos.y=pSummon->eSrc.y;
								pos.z=pSummon->eSrc.z;
								MakeCoolFx(&pos);
							}
						}

				CSpellFx *pCSpellFX = spells[i].pSpellFx;

				if (pCSpellFX)
				{
					pCSpellFX->Update(FrameDiff);
					pCSpellFX->Render();
				}					
			}
			break;
			//-----------------------------------------------------------------------------------------	
			
			case SPELL_INCINERATE:
			{
				if (ValidIONum(spells[i].caster))
				{
					ARX_SOUND_RefreshPosition(spells[i].snd_loop, &inter.iobj[spells[i].caster]->pos);
				}
			}
			break;
			case SPELL_NEGATE_MAGIC:
			{
				if (ValidIONum(spells[i].target))
					LaunchAntiMagicField(i);

				CSpellFx *pCSpellFX = spells[i].pSpellFx;

				if (pCSpellFX)
				{
					pCSpellFX->Update(FrameDiff);
					pCSpellFX->Render();
				}
			}
			break;
			case SPELL_MASS_PARALYSE:
			break;
			//*******************************************************************************************	
			// LEVEL 10 SPELLS -----------------------------------------------------------------------------
			//-----------------------------------------------------------------------------------------
			case SPELL_FREEZE_TIME:
			{
			}
			break;
			//-----------------------------------------------------------------------------------------				
			case SPELL_CONTROL_TARGET:
			{
				CSpellFx *pCSpellFX = spells[i].pSpellFx;

				if (pCSpellFX)
				{
					pCSpellFX->Update(FrameDiff);
					pCSpellFX->Render();
				}
			}
			break;
			case SPELL_MASS_INCINERATE:
			{
				if (ValidIONum(spells[i].caster))
				{
					ARX_SOUND_RefreshPosition(spells[i].snd_loop, &inter.iobj[spells[i].caster]->pos);
				}
			}
			break;
			//-----------------------------------------------------------------------------------------
			case SPELL_MASS_LIGHTNING_STRIKE:
			{
				CSpellFx *pCSpellFX = spells[i].pSpellFx;

				if (pCSpellFX)
				{
					pCSpellFX->Update(FrameDiff);
					pCSpellFX->Render();
				}
				
						Vec3f _source = spells[i].vsource;
						float _fx;
						_fx = 0.5f;
						unsigned long _gct;
						_gct = 0;

				Vec3f position;

				spells[i].lastupdate=tim;

				position.x=_source.x+rnd()*500.f-250.f;
				position.y=_source.y+rnd()*500.f-250.f;
				position.z=_source.z+rnd()*500.f-250.f;
				ARX_SOUND_RefreshPosition(spells[i].snd_loop, &position);
				ARX_SOUND_RefreshVolume(spells[i].snd_loop, _fx + 0.5F);
				ARX_SOUND_RefreshPitch(spells[i].snd_loop, 0.8F + 0.4F * rnd());

				if (rnd()>0.62f) 
				{
					position.x=_source.x+rnd()*500.f-250.f;
					position.y=_source.y+rnd()*500.f-250.f;
					position.z=_source.z+rnd()*500.f-250.f;
					ARX_SOUND_PlaySFX(SND_SPELL_SPARK, &position, 0.8F + 0.4F * rnd());
				} 

				if (rnd()>0.82f) 
				{
					position.x=_source.x+rnd()*500.f-250.f;
					position.y=_source.y+rnd()*500.f-250.f;
					position.z=_source.z+rnd()*500.f-250.f;
					ARX_SOUND_PlaySFX(SND_SPELL_ELECTRIC, &position, 0.8F + 0.4F * rnd());
				}

				if ((_gct>spells[i].tolive-1800) && (spells[i].siz==0))
				{
					spells[i].siz=1;
					ARX_SOUND_PlaySFX(SND_SPELL_ELECTRIC, NULL, 0.8F + 0.4F * rnd());
				}

				if (spells[i].longinfo!=-1)
				{
					float fxx;

					if (_fx>0.2f)  fxx=1.f;
					else fxx=_fx*5.f;

					DynLight[spells[i].longinfo].intensity=1.3f+rnd()*1.f;
					DynLight[spells[i].longinfo].fallend=850.f;
					DynLight[spells[i].longinfo].fallstart=500.f;
					DynLight[spells[i].longinfo].rgb.r=1.f*fxx;					
					DynLight[spells[i].longinfo].rgb.g=0.f*fxx;
					DynLight[spells[i].longinfo].rgb.b=0.f*fxx;
				}
			}
		break;
		//-----------------------------------------------------------------------------------------				
		case SPELL_TELEPORT:
				{
					float TELEPORT = (float)(((float)tim-(float)spells[i].timcreation)/(float)spells[i].tolive);

					if(LASTTELEPORT < 0.5f && TELEPORT >= 0.5f) {
						Vec3f pos = lastteleport;
						lastteleport = player.pos;
						player.pos = pos;
						LASTTELEPORT = 32.f;
						ARX_SOUND_PlaySFX(SND_SPELL_TELEPORTED, &player.pos);
					} else {
						LASTTELEPORT = TELEPORT;
					}
				}
				break;				
				//-----------------------------------------------------------------------------------------
				case SPELL_MAGIC_SIGHT:

					if (spells[i].caster == 0)
					{
						ARX_SOUND_RefreshPosition(spells[i].snd_loop);

						if (subj.focal>IMPROVED_FOCAL) subj.focal-=DEC_FOCAL;
					}

				break;
				//-----------------------------------------------------------------------------------------
				case SPELL_TELEKINESIS:
				break;
				//-----------------------------------------------------------------------------------------
				case SPELL_INVISIBILITY:

					if (spells[i].target!=0)
					{
						if (!(inter.iobj[spells[i].target]->GameFlags & GFLAG_INVISIBILITY))
						{
							ARX_SPELLS_RemoveSpellOn(spells[i].target,i);
							ARX_SPELLS_Fizzle(i);
						}
					}
				break;				
				//-----------------------------------------------------------------------------------------
				case SPELL_MANA_DRAIN:
					{
						
					if ( (cabal!=NULL) )
					{
						float refpos;
						float scaley;

						if (spells[i].caster==0) scaley=90.f;
						else scaley=EEfabs(inter.iobj[spells[i].caster]->physics.cyl.height*( 1.0f / 2 ))+30.f;

						float mov1=EEsin((float)arxtime.get_last_frame_time()*( 1.0f / 800 ))*scaley;
						float mov=EEsin((float)arxtime.get_frame_time()*( 1.0f / 800 ))*scaley;

						if ((mov1<scaley-10.f) && (mov>scaley-10.f)) ARX_SOUND_PlaySFX(SND_SPELL_MAGICAL_SHIELD, &spells[i].caster_pos, 0.4F);

						if ((mov1>-scaley+10.f) && (mov<-scaley+10.f)) ARX_SOUND_PlaySFX(SND_SPELL_MAGICAL_SHIELD, &spells[i].caster_pos, 0.4F);

						if (spells[i].caster==0)
						{
								cabalpos.x = player.pos.x; 
							cabalpos.y=player.pos.y+60.f-mov;
								cabalpos.z = player.pos.z; 
							refpos=player.pos.y+60.f;							
						}
						else
						{							
								cabalpos.x = inter.iobj[spells[i].caster]->pos.x; 
							cabalpos.y=inter.iobj[spells[i].caster]->pos.y-scaley-mov;
								cabalpos.z = inter.iobj[spells[i].caster]->pos.z; 
							refpos=inter.iobj[spells[i].caster]->pos.y-scaley;							
						}

						float Es=EEsin((float)arxtime.get_frame_time()*( 1.0f / 800 ) + radians(scaley));

						if (spells[i].longinfo2!=-1)
						{
							DynLight[spells[i].longinfo2].pos.x = cabalpos.x;
							DynLight[spells[i].longinfo2].pos.y = refpos;
							DynLight[spells[i].longinfo2].pos.z = cabalpos.z;
							DynLight[spells[i].longinfo2].rgb.b=rnd()*0.2f+0.8f;
							DynLight[spells[i].longinfo2].fallstart=Es*1.5f;
						}

						GRenderer->SetBlendFunc(Renderer::BlendOne, Renderer::BlendOne);
						GRenderer->SetRenderState(Renderer::AlphaBlending, true);
						GRenderer->SetRenderState(Renderer::DepthWrite, false);
						cabalangle.b=spells[i].fdata+(float)FrameDiff*0.1f;
						spells[i].fdata=cabalangle.b;

							cabalangle.g = 0.f; 
						
							cabalcolor.r = cabalcolor.g = 0.4f;
							cabalcolor.b = 0.8f;
						
						cabalscale.z=cabalscale.y=cabalscale.x=Es;				
						DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
						mov=EEsin((float)(arxtime.get_frame_time()-30.f)*( 1.0f / 800 ))*scaley;
						cabalpos.y=refpos-mov;						
							cabalcolor.r = cabalcolor.g = 0.2f;
							cabalcolor.b = 0.5f;
						DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
						mov=EEsin((float)(arxtime.get_frame_time()-60.f)*( 1.0f / 800 ))*scaley;
						cabalpos.y=refpos-mov;
							cabalcolor.r = cabalcolor.g = 0.1f;
							cabalcolor.b = 0.25f;
						DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
						mov=EEsin((float)(arxtime.get_frame_time()-120.f)*( 1.0f / 800 ))*scaley;
						cabalpos.y=refpos-mov;
							cabalcolor.r = cabalcolor.g = 0.f;
							cabalcolor.b = 0.15f;
						DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
						cabalangle.b=-cabalangle.b;
						cabalpos.y=refpos-mov;
						cabalscale.x=cabalscale.y=cabalscale.z=Es;
							cabalcolor.r = cabalcolor.g = 0.f;
							cabalcolor.b = 0.15f;
						DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
						mov=EEsin((float)(arxtime.get_frame_time()+30.f)*( 1.0f / 800 ))*scaley;
						cabalpos.y=refpos+mov;
							cabalcolor.r = cabalcolor.g = 0.1f;
							cabalcolor.b = 0.25f;
						DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
						mov=EEsin((float)(arxtime.get_frame_time()+60.f)*( 1.0f / 800 ))*scaley;
						cabalpos.y=refpos+mov;
							cabalcolor.r = cabalcolor.g = 0.2f;
							cabalcolor.b = 0.5f;
						DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
						mov=EEsin((float)(arxtime.get_frame_time()+120.f)*( 1.0f / 800 ))*scaley;
						cabalpos.y=refpos+mov;
							cabalcolor.r = cabalcolor.g = 0.4f;
							cabalcolor.b = 0.8f;
						DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
						cabalangle.b=-cabalangle.b;
						GRenderer->SetRenderState(Renderer::AlphaBlending, false);		
						GRenderer->SetRenderState(Renderer::DepthWrite, true);	

						ARX_SOUND_RefreshPosition(spells[i].snd_loop, &cabalpos);
					}
					}
				break;				
				//-----------------------------------------------------------------------------------------
				case SPELL_LIFE_DRAIN:

					{
					if ( (cabal!=NULL) )
					{
						float refpos;
						float scaley;

						if (spells[i].caster==0) scaley=90.f;
						else scaley=EEfabs(inter.iobj[spells[i].caster]->physics.cyl.height*( 1.0f / 2 ))+30.f;

 
						float mov=EEsin((float)arxtime.get_frame_time()*( 1.0f / 800 ))*scaley;

						if (spells[i].caster==0)
						{
								cabalpos.x = player.pos.x; 
							cabalpos.y=player.pos.y+60.f-mov;
								cabalpos.z = player.pos.z; 
							refpos=player.pos.y+60.f;							
						}
						else
						{							
								cabalpos.x = inter.iobj[spells[i].caster]->pos.x; 
							cabalpos.y=inter.iobj[spells[i].caster]->pos.y-scaley-mov;
								cabalpos.z = inter.iobj[spells[i].caster]->pos.z; 
							refpos=inter.iobj[spells[i].caster]->pos.y-scaley;							
						}

						float Es=EEsin((float)arxtime.get_frame_time()*( 1.0f / 800 ) + radians(scaley));

						if (spells[i].longinfo2!=-1)
						{
							DynLight[spells[i].longinfo2].pos.x = cabalpos.x;
							DynLight[spells[i].longinfo2].pos.y = refpos;
							DynLight[spells[i].longinfo2].pos.z = cabalpos.z; 
							DynLight[spells[i].longinfo2].rgb.r=rnd()*0.2f+0.8f;
							DynLight[spells[i].longinfo2].fallstart=Es*1.5f;
						}

						GRenderer->SetCulling(Renderer::CullNone);
						GRenderer->SetBlendFunc(Renderer::BlendOne, Renderer::BlendOne);
						GRenderer->SetRenderState(Renderer::AlphaBlending, true);
						GRenderer->SetRenderState(Renderer::DepthWrite, false);
						cabalangle.b=spells[i].fdata+(float)FrameDiff*0.1f;
						spells[i].fdata=cabalangle.b;
							cabalangle.g = 0.f;
							cabalcolor.r = 0.8f;
							cabalcolor.g = 0.f;
							cabalcolor.b = 0.f;
						cabalscale.z=cabalscale.y=cabalscale.x=Es;				
						DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
						mov=EEsin((float)(arxtime.get_frame_time()-30.f)*( 1.0f / 800 ))*scaley;
						cabalpos.y=refpos-mov;						
							cabalcolor.b = cabalcolor.g = 0.f;
							cabalcolor.r = 0.5f;
						DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
						mov=EEsin((float)(arxtime.get_frame_time()-60.f)*( 1.0f / 800 ))*scaley;
						cabalpos.y=refpos-mov;
							cabalcolor.b = cabalcolor.g = 0.f;
							cabalcolor.r = 0.25f;
						DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
						mov=EEsin((float)(arxtime.get_frame_time()-120.f)*( 1.0f / 800 ))*scaley;
						cabalpos.y=refpos-mov;
							cabalcolor.b = cabalcolor.g = 0.f;
							cabalcolor.r = 0.15f;
						DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
						cabalangle.b=-cabalangle.b;
						cabalpos.y=refpos-mov;
						cabalscale.x=cabalscale.y=cabalscale.z=Es;
							cabalcolor.b = cabalcolor.g = 0.f;
							cabalcolor.r = 0.15f;
						DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
						mov=EEsin((float)(arxtime.get_frame_time()+30.f)*( 1.0f / 800 ))*scaley;
						cabalpos.y=refpos+mov;
							cabalcolor.b = cabalcolor.g = 0.f;
							cabalcolor.r = 0.25f;
						DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
						mov=EEsin((float)(arxtime.get_frame_time()+60.f)*( 1.0f / 800 ))*scaley;
						cabalpos.y=refpos+mov;
							cabalcolor.b = cabalcolor.g = 0.f;
							cabalcolor.r = 0.5f;
						DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
						mov=EEsin((float)(arxtime.get_frame_time()+120.f)*( 1.0f / 800 ))*scaley;
						cabalpos.y=refpos+mov;
							cabalcolor.b = cabalcolor.g = 0.f;
							cabalcolor.r = 0.8f;
						DrawEERIEObjEx(cabal,&cabalangle,&cabalpos,&cabalscale,&cabalcolor);	
						cabalangle.b=-cabalangle.b;
						GRenderer->SetRenderState(Renderer::AlphaBlending, false);		
						GRenderer->SetRenderState(Renderer::DepthWrite, true);	

						ARX_SOUND_RefreshPosition(spells[i].snd_loop, &cabalpos);
						}
					}
				break;				
				//-----------------------------------------------------------------------------------------
				case SPELL_FLYING_EYE:
					{
						eyeball.floating=EEsin((spells[i].lastupdate-spells[i].timcreation)*( 1.0f / 1000 ))*10.f;					
						
							if (spells[i].lastupdate-spells[i].timcreation<=3000)
							{
								eyeball.exist = spells[i].lastupdate-spells[i].timcreation*( 1.0f / 30 );
								float d=(float)eyeball.exist*( 1.0f / 100 );
							
							eyeball.size.x = 1.f - d; 
							eyeball.size.y = 1.f - d; 
							eyeball.size.z = 1.f - d; 
								
								eyeball.angle.b+=framediff3*( 1.0f / 10 )*6.f;
							}
							else 
							{
								eyeball.exist=2;
							}
						
						spells[i].lastupdate=tim;
					}
				break;
			}		
		}
	}
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void TryToCastSpell(INTERACTIVE_OBJ * io, Spell spellid, long level, long target, SpellcastFlags flags, long duration)
{
	if (!io || io->spellcast_data.castingspell != SPELL_NONE) return;

	if (!(flags & SPELLCAST_FLAG_NOMANA)
			&& (io->ioflags & IO_NPC) && (io->_npcdata->mana<=0.f))
		return;

	unsigned long i(0);

	for (; i < SPELL_COUNT; i++)
		if (spellicons[i].spellid == spellid) break;

	if ( i >= SPELL_COUNT) return; // not an existing spell...

	for (unsigned long j(0); j < 4; j++) io->spellcast_data.symb[j] = RUNE_NONE;

	// checks for symbol drawing...
	if (!(flags & SPELLCAST_FLAG_NOANIM) && io->ioflags & IO_NPC)
	{
		ANIM_USE *ause1 = &io->animlayer[1];

		AcquireLastAnim(io);
		FinishAnim(io, ause1->cur_anim);
		ANIM_Set(ause1, io->anims[ANIM_CAST_START]);

		for (unsigned long j(0); j < 4; j++)
			io->spellcast_data.symb[j] = spellicons[i].symbols[j];
	}

	io->spellcast_data.castingspell = spellid;
	

	io->spellcast_data.spell_flags = flags;
	io->spellcast_data.spell_level = checked_range_cast<short>(level);


	io->spellcast_data.duration = duration;
	io->spellcast_data.target = target;
	
	io->GameFlags &=~GFLAG_INVISIBILITY;
	
	if (	((io->spellcast_data.spell_flags & SPELLCAST_FLAG_NOANIM)
		&&	(io->spellcast_data.spell_flags & SPELLCAST_FLAG_NODRAW) )
		||	(io->spellcast_data.spell_flags & SPELLCAST_FLAG_PRECAST))	
	{
		
		ARX_SPELLS_Launch(io->spellcast_data.castingspell,GetInterNum(io),io->spellcast_data.spell_flags,io->spellcast_data.spell_level,io->spellcast_data.target,io->spellcast_data.duration);
		io->spellcast_data.castingspell = SPELL_NONE;
	}

	io->spellcast_data.spell_flags &=~SPELLCAST_FLAG_NODRAW; // temporary, removes colored flares
}

static void ApplySPWep() {
	if (!sp_wep)
	{		
		ARX_SPSound();
		
		res::path file = "graph/obj3d/interactive/items/weapons/sword_mx/sword_mx.teo";
		
		INTERACTIVE_OBJ * ioo = AddItem(file,IO_IMMEDIATELOAD);

		if (ioo!=NULL)
		{			
			sp_wep=1;
			MakeCoolFx(&player.pos);
			MakeCoolFx(&player.pos);
			ioo->scriptload=1;
			MakeTemporaryIOIdent(ioo);
			SendInitScriptEvent(ioo);
			ioo->show = SHOW_FLAG_IN_INVENTORY;

			if (!CanBePutInInventory(ioo))
				PutInFrontOfPlayer(ioo);

			MakeSpCol();
			strcpy(sp_max_ch,"!!!_Grosbillite_!!!");
			sp_max_nb=strlen(sp_max_ch);
			sp_max_start=arxtime.get_updated();
		}
	}
}
void MakeSpCol()
{
	ARX_SPSound();

	for (long i=0;i<64;i++)
	{
		sp_max_y[i]=0;
	}

	sp_max_col[0] = Color::fromRGBA(0x00FF0000);
	sp_max_col[1] = Color::fromRGBA(0x0000FF00);
	sp_max_col[2] = Color::fromRGBA(0x000000FF);
	
	sp_max_col[3] = Color::fromRGBA(0x00FFFF00);
	sp_max_col[4] = Color::fromRGBA(0x00FF00FF);
	sp_max_col[5] = Color::fromRGBA(0x0000FFFF);
	
	for(size_t i = 6; i < 24; i++) {
		sp_max_col[i] = sp_max_col[i - 6];
	}
	
	for(size_t i = 24; i < 27; i++) {
		sp_max_col[i] = sp_max_col[i - 3];
	}
	
	for(size_t i = 27; i < 33; i++) {
		sp_max_col[i] = sp_max_col[i - 9];
	}
	
}

static void ApplyCurSOS() {
	MakeSpCol();
	ARX_MINIMAP_Reveal();
	strcpy(sp_max_ch,"!!!_Temple of Elemental Lavis_!!!");
	sp_max_nb=strlen(sp_max_ch);	
	sp_max_start=arxtime.get_updated();
}

static void ApplySPBow() {
	
	ARX_SPSound();
	const char OBJ_BOW[] = "graph/obj3d/interactive/items/weapons/bow_mx/bow_mx.teo";
	INTERACTIVE_OBJ * ioo = AddItem(OBJ_BOW, IO_IMMEDIATELOAD);
	
	if(ioo!=NULL) {
		MakeCoolFx(&player.pos);
		MakeCoolFx(&player.pos);
		ioo->scriptload=1;
		MakeTemporaryIOIdent(ioo);
		SendInitScriptEvent(ioo);
		ioo->show=SHOW_FLAG_IN_INVENTORY;
		
		if (!CanBePutInInventory(ioo))
			PutInFrontOfPlayer(ioo);
		
		MakeSpCol();
		strcpy(sp_max_ch,"!!!_Bow to Samy & Anne_!!!");
		sp_max_nb=strlen(sp_max_ch);
		sp_max_start=arxtime.get_updated();
	}
}

static void ApplySPArm() {
	ARX_SPSound();
	
	res::path file;
	switch (sp_arm) {
		case 0:
			file = "graph/obj3d/interactive/items/armor/helmet_plate_cm/helmet_plate_cm.teo";
		break;
		case 1:
			file = "graph/obj3d/interactive/items/armor/legging_plate_cm/legging_plate_cm.teo";
		break;
		case 2:
			file = "graph/obj3d/interactive/items/armor/chest_plate_cm/chest_plate_cm.teo";
		break;
		default:
			return;
		break;
	}

	INTERACTIVE_OBJ * ioo = AddItem(file, IO_IMMEDIATELOAD);

	if (ioo!=NULL)
	{			
		sp_wep=1;
		MakeCoolFx(&player.pos);
		MakeCoolFx(&player.pos);
		ioo->scriptload=1;
		MakeTemporaryIOIdent(ioo);
		SendInitScriptEvent(ioo);
		ioo->show=SHOW_FLAG_IN_INVENTORY;

		if (!CanBePutInInventory(ioo))
			PutInFrontOfPlayer(ioo);

		MakeSpCol();
		strcpy(sp_max_ch,"!! Toi aussi cherches les Cheats !!");

		switch (sp_arm)
		{
		case 0:
			strcpy(sp_max_ch,"------ZoliChapo------");
		break;
		case 1:
			strcpy(sp_max_ch,"-----TiteBottine-----");
		break;
		case 2:
			strcpy(sp_max_ch,"-----Roooo-La-La-----");
		break;
		default:
			return;
		break;
		}

		sp_max_nb=strlen(sp_max_ch);
		sp_max_start=arxtime.get_updated();
	}

	sp_arm++;
}

long SPECIAL_PNUX;
static void ApplyCurPNux() {
	
	MakeSpCol();
	strcpy(sp_max_ch,"! PhilNux & Gluonne !");
	sp_max_nb=strlen(sp_max_ch);
	
	SPECIAL_PNUX = (SPECIAL_PNUX + 1) % 3;
	
	// TODO-RENDERING: Create a post-processing effect for that cheat... see original source...
	
	cur_pnux=0;
	sp_max_start=arxtime.get_updated();
}

static void ApplyPasswall() {
	MakeSpCol();
	strcpy(sp_max_ch,"!!! PassWall !!!");
	sp_max_nb=strlen(sp_max_ch);
	sp_max_start=arxtime.get_updated();

	if (USE_PLAYERCOLLISIONS)
		USE_PLAYERCOLLISIONS=0;
	else
		USE_PLAYERCOLLISIONS=1;
}

static void ApplySPRf() {
	if(cur_rf == 3) {
		MakeSpCol();
		strcpy(sp_max_ch,"!!! RaFMode !!!");
		sp_max_nb=strlen(sp_max_ch);
		sp_max_start=arxtime.get_updated();
	}
}

static void ApplyCurMr() {
	if(cur_mr == 3) {
		MakeSpCol();
		strcpy(sp_max_ch,"!!! Marianna !!!");
		sp_max_nb=strlen(sp_max_ch);
		sp_max_start=arxtime.get_updated();
	}
}

static void ApplySPuw() {
	uw_mode_pos=0;
	uw_mode=~uw_mode;
	ARX_SOUND_PlayCinematic("menestrel_uw2.wav");
	MakeCoolFx(&player.pos);
	if(uw_mode) {
		MakeSpCol();
		strcpy(sp_max_ch,"~-__-~~-__.U.W.__-~~-__-~");
		sp_max_nb=strlen(sp_max_ch);
		sp_max_start=arxtime.get_updated();
	}
}

static void ApplySPMax() {
	
	MakeCoolFx(&player.pos);
	sp_max=~sp_max;

	if (sp_max)
	{
		MakeSpCol();
		strcpy(sp_max_ch,"!!!_FaNt0mAc1e_!!!");
		sp_max_nb=strlen(sp_max_ch);
		sp_max_start=arxtime.get_updated();

			player.skin=4;

			ARX_EQUIPMENT_RecreatePlayerMesh();
		
		ARX_PLAYER_Rune_Add_All();
		std::string text = "!!!!!!! FanTomAciE !!!!!!!";
		ARX_SPEECH_Add(text);
		player.Attribute_Redistribute+=10;
		player.Skill_Redistribute+=50;
		player.level=std::max((int)player.level,10);
		player.xp=GetXPforLevel(10);
	}
	else
	{
		TextureContainer * tcm=TextureContainer::Load("graph/obj3d/textures/npc_human_cm_hero_head");

		if (tcm)
		{
			delete tcm;
			player.heads[0]=TextureContainer::Load("graph/obj3d/textures/npc_human_base_hero_head");
			player.heads[1]=TextureContainer::Load("graph/obj3d/textures/npc_human_base_hero2_head");
			player.heads[2]=TextureContainer::Load("graph/obj3d/textures/npc_human_base_hero3_head");
			ARX_EQUIPMENT_RecreatePlayerMesh();
		}
	}	
}
