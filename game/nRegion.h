#ifndef _N_REGION_H_
#define _N_REGION_H_

#include "base/nRandom.h"

#include "game/nPos.h"

namespace Game
{
	struct nRegion
	{
		nPos leftTopPos;
		nPos rightDownPos;
		nRegion()
		{
		}
		nRegion(const nPos& leftTopPos, const nPos& rightDownPos) : leftTopPos(leftTopPos), rightDownPos(rightDownPos)
		{
			assert_debug(leftTopPos.x <= rightDownPos.x && leftTopPos.y <= rightDownPos.y, "");
			if(leftTopPos.x > rightDownPos.x)
			{
				WORD temp = leftTopPos.x;
				this->leftTopPos.x = rightDownPos.x;
				this->rightDownPos.x = temp;
			}
			if(leftTopPos.y > rightDownPos.y)
			{
				WORD temp = leftTopPos.y;
				this->leftTopPos.y = rightDownPos.y;
				this->rightDownPos.y = temp;
			}
		}
		nRegion(const nPos& centerPos, const WORD width, const WORD height)
		{
			if(centerPos.x < (width/2))
			{
				leftTopPos.x = 0;
			}
			else
			{
				leftTopPos.x = centerPos.x - (width/2);
			}
			if(centerPos.y < (height/2))
			{
				leftTopPos.y = 0;
			}
			else
			{
				leftTopPos.y = centerPos.y - (height/2);
			}

			rightDownPos.x = leftTopPos.x + width;
			rightDownPos.y = leftTopPos.y + height;
		}

		nRegion(const nRegion& other) : leftTopPos(other.leftTopPos), rightDownPos(other.rightDownPos)
		{
		}

		nPos randPos() const
		{
			return nPos(Tidy::randBetween(leftTopPos.x, rightDownPos.x), Tidy::randBetween(leftTopPos.y, rightDownPos.y));
		}

		void getAllPos(Tidy::vector<nPos>& posList)
		{
			posList.reserve((rightDownPos.x - leftTopPos.x + 1) * (rightDownPos.y - leftTopPos.y + 1));
			for(WORD x = leftTopPos.x; x <= rightDownPos.x; ++x)
			{
				for(WORD y = leftTopPos.y; y <= rightDownPos.y; ++y)
				{
					posList.push_back(nPos(x, y));
				}
			}
		}
	};

	struct nRegion2D
	{
		nPos2D leftTopPos;
		nPos2D rightDownPos2D;
		nRegion2D()
		{
		}
		nRegion2D(const nPos2D& leftTopPos, const nPos2D& rightDownPos2D) : leftTopPos(leftTopPos), rightDownPos2D(rightDownPos2D)
		{
			assert_debug(leftTopPos.x <= rightDownPos2D.x && leftTopPos.z <= rightDownPos2D.z, "");
			if(leftTopPos.x > rightDownPos2D.x)
			{
				float temp = leftTopPos.x;
				this->leftTopPos.x = rightDownPos2D.x;
				this->rightDownPos2D.x = temp;
			}
			if(leftTopPos.z > rightDownPos2D.z)
			{
				float temp = leftTopPos.z;
				this->leftTopPos.z = rightDownPos2D.z;
				this->rightDownPos2D.z = temp;
			}
		}
		nRegion2D(const nPos2D& centerPos, const WORD width, const WORD height)
		{
			if(centerPos.x < (width/2))
			{
				leftTopPos.x = 0;
			}
			else
			{
				leftTopPos.x = centerPos.x - (width/2);
			}
			if(centerPos.z < (height/2))
			{
				leftTopPos.z = 0;
			}
			else
			{
				leftTopPos.z = centerPos.z - (height/2);
			}

			rightDownPos2D.x = leftTopPos.x + width;
			rightDownPos2D.z = leftTopPos.z + height;
		}

		nRegion2D(const nRegion2D& other) : leftTopPos(other.leftTopPos), rightDownPos2D(other.rightDownPos2D)
		{
		}

		nPos2D randPos() const
		{
			return nPos2D(Tidy::randBetween(leftTopPos.x, rightDownPos2D.x), Tidy::randBetween(leftTopPos.z, rightDownPos2D.z));
		}
	};
};
#endif
