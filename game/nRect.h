#ifndef _NRECT_H_
#define _NRECT_H_

#include "game/nPos.h"

namespace Game
{
	struct nRect
	{
		nPos2D pos;
		nPos2D size;
		nRect(const nPos2D &posData, const nPos2D &sizeData) :
			pos(posData),
			size(sizeData)
		{
		}
		nRect()
		{
		}

		void getTopLine(nPos2D *start, nPos2D *end) const
		{
			*start = pos;
			end->x = pos.x + size.x;
			end->z = pos.z;
		}
		void getBottomLine(nPos2D *start, nPos2D *end) const
		{
			start->x = pos.x;
			start->z = pos.z + size.z;
			end->x = pos.x + size.x;
			end->z = pos.z + size.z;
		}
		void getLeftLine(nPos2D *start, nPos2D *end) const
		{
			*start = pos;
			end->x = pos.x;
			end->z = pos.z + size.z;
		}
		void getRightLine(nPos2D *start, nPos2D *end) const
		{
			start->x = pos.x + size.x;
			start->z = pos.z;
			end->x = pos.x + size.x;
			end->z = pos.z + size.z;
		}
	};
};
#endif
