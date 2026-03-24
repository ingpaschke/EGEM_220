
#define __RC_OBJ_C__
#include "e_gem.h"

#if defined(__GNUC__) || (SHRT_MAX != INT_MAX)
int rc_inside(int x, int y, GRECT *rec)
{
	return (x >= rec->g_x && x < rec->g_x + rec->g_w &&
			y >= rec->g_y && y < rec->g_y + rec->g_h);
}
#endif	/* __GNUC__ || 32-bit int */

#if SHRT_MAX != INT_MAX
/* 32-bit int: C implementations replace assembly RC_OB.S */

int rc_copy(GRECT *src, GRECT *dst)
{
	*dst = *src;
	return 0;
}

int rc_equal(GRECT *a, GRECT *b)
{
	return (a->g_x == b->g_x && a->g_y == b->g_y &&
			a->g_w == b->g_w && a->g_h == b->g_h);
}

int rc_intersect(GRECT *r1, GRECT *r2)
{
	int x1 = r1->g_x > r2->g_x ? r1->g_x : r2->g_x;
	int y1 = r1->g_y > r2->g_y ? r1->g_y : r2->g_y;
	int x2 = r1->g_x + r1->g_w < r2->g_x + r2->g_w ? r1->g_x + r1->g_w : r2->g_x + r2->g_w;
	int y2 = r1->g_y + r1->g_h < r2->g_y + r2->g_h ? r1->g_y + r1->g_h : r2->g_y + r2->g_h;

	if (x2 <= x1 || y2 <= y1)
		return 0;

	r2->g_x = x1;
	r2->g_y = y1;
	r2->g_w = x2 - x1;
	r2->g_h = y2 - y1;
	return 1;
}
#endif	/* 32-bit int */

#if defined(LATTICE) || (SHRT_MAX != INT_MAX)
void rc_grect_to_array(GRECT *rec,INT16 *pxy)
{
	pxy[0] = rec->g_x;
	pxy[1] = rec->g_y;
	pxy[2] = rec->g_x + rec->g_w - 1;
	pxy[3] = rec->g_y + rec->g_h - 1;
}
#endif	/* LATTICE || 32-bit int */

void rc_array_to_grect(INT16 *pxy,GRECT *rec)
{
	rec->g_x = pxy[0];
	rec->g_y = pxy[1];
	rec->g_w = pxy[2] - pxy[0] + 1;
	rec->g_h = pxy[3] - pxy[1] + 1;
}

#if defined(__MINT_LIB__) || (SHRT_MAX != INT_MAX)
int EvntMulti(EVENT *evnt_struct)
{
	return (evnt_struct->ev_mwich = evnt_multi(evnt_struct->ev_mflags,
		evnt_struct->ev_mbclicks,
		evnt_struct->ev_bmask,
		evnt_struct->ev_mbstate,
		evnt_struct->ev_mm1flags,
		evnt_struct->ev_mm1x,
		evnt_struct->ev_mm1y,
		evnt_struct->ev_mm1width,
		evnt_struct->ev_mm1height,
		evnt_struct->ev_mm2flags,
		evnt_struct->ev_mm2x,
		evnt_struct->ev_mm2y,
		evnt_struct->ev_mm2width,
		evnt_struct->ev_mm2height,
		evnt_struct->ev_mmgpbuf,
		((unsigned long)evnt_struct->ev_mthicount << 16) + evnt_struct->ev_mtlocount,
		&evnt_struct->ev_mmox,
		&evnt_struct->ev_mmoy,
		&evnt_struct->ev_mmobutton,
		&evnt_struct->ev_mmokstate,
		&evnt_struct->ev_mkreturn,
		&evnt_struct->ev_mbreturn));
}
#endif	/* __MINT_LIB__ || 32-bit int */
