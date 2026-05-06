#ifndef PGXP_DEFS_H
#define PGXP_DEFS_H

#include "PsyX/PsyX_config.h"

#if USE_PGXP
#include "PsyX/common/half_float.h"

// Helpful macro
#define PGXP_LOOKUP_VALUE(x, y) (*(u_short*)&(x) | (*(u_short*)&(y) << 16))

//-------------------------------------

// PC port (Silent Hill): VERTTYPE is `short` in BOTH C and C++ even when USE_PGXP=1.
// Originally upstream used `half` in C++, but the game writes prim XY fields as raw
// shorts via setXY0() — a bit-pattern mismatch with half-floats produces NaN vertices
// at the boundary. The high-precision PGXP data flows through PGXPVData via the
// PGXP cache lookup + the `a_zw` vertex attribute, NOT through the prim XY shorts,
// so keeping VERTTYPE=short here costs us nothing.
//
// `_HF()` is therefore a no-op everywhere (kept as a symbol so legacy call sites
// like libgpu.c text-rendering still compile).
typedef short VERTTYPE;
#define _HF(x) (x)

typedef struct
{
	float px, py, pz;	// 32 bit values
	VERTTYPE x, y, z;	// 16 bit values (for lookup and backwards compat if not found in cache)
} PGXPVector3D;

typedef struct
{
	uint lookup;
	float px, py, pz;
	float scr_h, ofx, ofy;
} PGXPVData;

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

extern PGXPVector3D g_FP_SXYZ0; // direct access PGXP without table lookup
extern PGXPVector3D g_FP_SXYZ1;
extern PGXPVector3D g_FP_SXYZ2;

/* clears PGXP vertex buffer */
void	PGXP_ClearCache();

/* emits new PGXP vertex */
ushort	PGXP_EmitCacheData(PGXPVData* newData);

/* sets Z offset (works like Z bias) */
void	PGXP_SetZOffsetScale(float offset, float scale);

/* searches for vertex with given lookup value */
int		PGXP_GetCacheData(PGXPVData* out, uint lookup, ushort indexhint /* = 0xFFFF */);

/* used by primitive setup */
ushort	PGXP_GetIndex(int checkTransform);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

// special PGXP type
// PC port: vx/vy were `half` upstream — switched to `short` to match VERTTYPE
// (see note above). The pgxp_index carries the cache identity separately.
typedef struct {		/* 2D short vector */
	short vx, vy;
	ushort pgxp_index;
} DVECTORF;

#else
typedef short VERTTYPE;

#define _HF(x) x

#endif


#endif // PGXP_DEFS