
#ifndef ARX_GRAPHICS_DATA_FTLFORMAT_H
#define ARX_GRAPHICS_DATA_FTLFORMAT_H

#include "core/Common.h"
#include "graphics/GraphicsFormat.h"

#define CURRENT_FTL_VERSION  0.83257f

// FTL File Structures Definitions

// FTL FILE Structure:
//
// ARX_FTL_PRIMARY_HEADER
// Checksum
// --> All the following data is then compressed and must be expanded
// ARX_FTL_SECONDARY_HEADER;
// -> Then depending on offsets just read data directly.


#pragma pack(push,1)


struct ARX_FTL_PRIMARY_HEADER {
	char ident[4]; // FTL
	f32 version; // starting with version 1.0f
};

struct ARX_FTL_SECONDARY_HEADER {
	s32 offset_3Ddata; // -1 = no
	s32 offset_cylinder; // -1 = no
	s32 offset_progressive_data; // -1 = no
	s32 offset_clothes_data; // -1 = no
	s32 offset_collision_spheres; // -1 = no
	s32 offset_physics_box; // -1 = no
};

struct ARX_FTL_PROGRESSIVE_DATA_HEADER {
	s32 nb_vertex;
};

struct ARX_FTL_CLOTHES_DATA_HEADER {
	s32 nb_cvert;
	s32 nb_springs;
};

struct ARX_FTL_COLLISION_SPHERES_DATA_HEADER {
	s32 nb_spheres;
};

struct ARX_FTL_3D_DATA_HEADER {
	s32 nb_vertex;
	s32 nb_faces;
	s32 nb_maps;
	s32 nb_groups;
	s32 nb_action;
	s32 nb_selections; // data will follow this order
	s32 origin;
	char name[256];
};

struct Texture_Container_FTL {
	char name[256];
};

const size_t IOPOLYVERT_FTL = 3;

struct EERIE_FACE_FTL {
	
	s32 facetype; // 0 = flat, 1 = text, 2 = Double-Side
	
	u32 rgb[IOPOLYVERT_FTL];
	u16 vid[IOPOLYVERT_FTL];
	s16 texid;
	f32 u[IOPOLYVERT_FTL];
	f32 v[IOPOLYVERT_FTL];
	s16 ou[IOPOLYVERT_FTL];
	s16 ov[IOPOLYVERT_FTL];
	
	f32 transval;
	SavedVec3 norm;
	SavedVec3 nrmls[IOPOLYVERT_FTL];
	f32 temp;
	
};

struct EERIE_GROUPLIST_FTL {
	
	char name[256];
	s32 origin;
	s32 nb_index;
	s32 indexes;
	f32 siz;
	
	EERIE_GROUPLIST_FTL & operator=(const EERIE_GROUPLIST & b) {
		strcpy(name, b.name.c_str());
		origin = b.origin;
		nb_index = b.indexes.size();
		indexes = 0;
		siz = b.siz;
		return *this;
	}
	
};

struct EERIE_ACTIONLIST_FTL {
	
	char name[256];
	s32 idx; // index vertex;
	s32 action;
	s32 sfx;
	
	inline operator EERIE_ACTIONLIST() {
		EERIE_ACTIONLIST a;
		a.name = name;
		a.idx = idx;
		a.act = action;
		a.sfx = sfx;
		return a;
	}
	
	inline EERIE_ACTIONLIST_FTL & operator=(const EERIE_ACTIONLIST & b) {
		strcpy(name, b.name.c_str());
		idx = b.idx;
		action = b.act;
		sfx = b.sfx;
		return *this;
	}
	
};

struct EERIE_SELECTIONS_FTL {
	
	char name[64];
	s32 nb_selected;
	s32 selected;
	
	inline EERIE_SELECTIONS_FTL & operator=(const EERIE_SELECTIONS & b) {
		strcpy(name, b.name.c_str());
		nb_selected = b.selected.size();
		selected = 0;
		return *this;
	}
	
};

struct COLLISION_SPHERE_FTL {
	
	s16 idx;
	s16 flags;
	f32 radius;
	
	inline operator COLLISION_SPHERE() {
		COLLISION_SPHERE a;
		a.idx = idx;
		a.flags = flags;
		a.radius = radius;
		return a;
	}
	
	inline COLLISION_SPHERE_FTL & operator=(const COLLISION_SPHERE & b) {
		idx = b.idx;
		flags = b.flags;
		radius = b.radius;
		return *this;
	}
	
};

struct EERIE_SPRINGS_FTL {
	
	s16 startidx;
	s16 endidx;
	f32 restlength;
	f32 constant; // spring constant
	f32 damping; // spring damping
	s32 type;
	
	inline operator EERIE_SPRINGS() {
		EERIE_SPRINGS a;
		a.startidx = startidx;
		a.endidx = endidx;
		a.restlength = restlength;
		a.constant = constant;
		a.damping = damping;
		a.type = type;
		return a;
	}
	
	inline EERIE_SPRINGS_FTL & operator=(const EERIE_SPRINGS & b) {
		startidx = b.startidx;
		endidx = b.endidx;
		restlength = b.restlength;
		constant = b.constant;
		damping = b.damping;
		type = b.type;
		return *this;
	}
	
};

struct EERIE_OLD_VERTEX {
	
	SavedD3DTLVertex vert;
	SavedVec3 v;
	SavedVec3 norm;
	
	inline operator EERIE_VERTEX() {
		EERIE_VERTEX a;
		a.vert = vert, a.v = v, a.norm = norm;
		return a;
	}
	
	inline EERIE_OLD_VERTEX & operator=(const EERIE_VERTEX & b) {
		vert = b.vert, v = b.v, norm = b.norm;
		return *this;
	}
	
};

struct CLOTHESVERTEX_FTL {
	
	s16 idx;
	u8 flags;
	s8 coll;
	SavedVec3 pos;
	SavedVec3 velocity;
	SavedVec3 force;
	f32 mass;
	
	SavedVec3 t_pos;
	SavedVec3 t_velocity;
	SavedVec3 t_force;
	
	SavedVec3 lastpos;
	
	inline operator CLOTHESVERTEX() {
		CLOTHESVERTEX a;
		a.idx = idx;
		a.flags = flags;
		a.coll = coll;
		a.pos = pos;
		a.velocity = velocity;
		a.force = force;
		a.mass = mass;
		a.t_pos = t_pos;
		a.t_velocity = t_velocity;
		a.t_force = t_force;
		a.lastpos = lastpos;
		return a;
	}
	
	inline CLOTHESVERTEX_FTL & operator=(const CLOTHESVERTEX & b) {
		idx = b.idx;
		flags = b.flags;
		coll = b.coll;
		pos = b.pos;
		velocity = b.velocity;
		force = b.force;
		mass = b.mass;
		t_pos = b.t_pos;
		t_velocity = b.t_velocity;
		t_force = b.t_force;
		lastpos = b.lastpos;
		return *this;
	}
	
};


#pragma pack(pop)


#endif // ARX_GRAPHICS_DATA_FTLFORMAT_H