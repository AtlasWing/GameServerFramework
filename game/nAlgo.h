#ifndef _NALGO_H_
#define _NALGO_H_

#include "game/nPos.h"
#include "game/nRect.h"
#include "base/nBaseServer.h"

namespace Game
{
	namespace nGraphAlgo
	{
		/// @brief	获取距离
		/// @param	begin 起点坐标
		/// @param	pos 到该点的距离
		/// @return	距离
		inline WORD getDistance(const nPos& begin, const nPos& pos)
		{
			return (WORD)sqrt( ((begin.x - pos.x) * (begin.x - pos.x)) + ((begin.y - pos.y) * (begin.y - pos.y)) );
		}
		inline float getDistance(const nPos2D& begin, const nPos2D& pos)
		{
			return (float)sqrt(( begin.x - pos.x) * (begin.x - pos.x) + (begin.z - pos.z) * (begin.z - pos.z) );
		}
		inline float getDistance(const nPos3D& begin, const nPos3D& pos)
		{
			return (float)sqrt( ((begin.x - pos.x) * (begin.x - pos.x)) + ((begin.z - pos.z) * ((begin.z - pos.z))) + ((begin.y - pos.y) * (begin.y - pos.y)) );
		}
		/// @brief	获取两点之间的近似距离
		/// @param	begin 起点坐标
		/// @param	end 终点坐标
		/// @return	两点近似距离
		inline WORD getSimilarDistance(const nPos& begin, const nPos& end)
		{
			return std::max(abs(end.x - begin.x), abs(end.y - begin.y));
		} 
		/// @brief	以自身为中心点，获取到另外一个坐标的方向
		/// @param	begin 起点坐标
		/// @param	pos 另外一个坐标点
		/// @return	方向
		inline const int getDirect(const nPos2D& begin, const nPos2D &pos)
		{
			SWORD dX = (SWORD)(pos.x - begin.x);
			SWORD dY = (SWORD)(pos.z - begin.z);
			if(dX > 0)
			{
				dX = 2;
			}
			else if(dX < 0)
			{
				dX = 1;
			}
			if(dY > 0)
			{
				dY = 2;
			}
			else if(dY < 0)
			{
				dY = 1;
			}
			static BYTE direct[3][3] = {{_DIR_WRONG, _DIR_UP, _DIR_DOWN}, {_DIR_LEFT, _DIR_LEFTUP, _DIR_DOWNLEFT}, {_DIR_RIGHT, _DIR_UPRIGHT, _DIR_RIGHTDOWN}};
			return direct[dX][dY];
		}

		inline const int getDirect(const nPos& begin, const nPos &pos)
		{
			SWORD dX = pos.x - begin.x;
			SWORD dY = pos.y - begin.y;
			if(dX > 0)
			{
				dX = 2;
			}
			else if(dX < 0)
			{
				dX = 1;
			}
			if(dY > 0)
			{
				dY = 2;
			}
			else if(dY < 0)
			{
				dY = 1;
			}
			static BYTE direct[3][3] = {{_DIR_WRONG, _DIR_UP, _DIR_DOWN}, {_DIR_LEFT, _DIR_LEFTUP, _DIR_DOWNLEFT}, {_DIR_RIGHT, _DIR_UPRIGHT, _DIR_RIGHTDOWN}};
			return direct[dX][dY];
#if 0
			if(begin.x == pos.x && begin.y > pos.y)
			{
				return _DIR_UP;
			}
			else if(begin.x < pos.x && begin.y > pos.y)
			{
				return _DIR_UPRIGHT;
			}
			else if(begin.x < pos.x && begin.y == pos.y)
			{
				return _DIR_RIGHT;
			}
			else if(begin.x < pos.x && begin.y < pos.y)
			{
				return _DIR_RIGHTDOWN;
			}
			else if(begin.x == pos.x && begin.y < pos.y)
			{
				return _DIR_DOWN;
			}
			else if(begin.x > pos.x && begin.y < pos.y)
			{
				return _DIR_DOWNLEFT;
			}
			else if(begin.x > pos.x && begin.y == pos.y)
			{
				return _DIR_LEFT;
			}
			else if(begin.x > pos.x && begin.y > pos.y)
			{
				return _DIR_LEFTUP;
			}
			return _DIR_WRONG;
#endif
		}
		/// @brief	以自身为中心点，获取到另一个坐标的视线方向 
		/// @param	begin 起点坐标
		/// @param	pos 另外一个坐标点
		/// @return	方向
		inline const int getLineDirect(const nPos& begin, const nPos& pos)
		{
			//Bresenham算法，只用了第一步
			int deltaX = pos.x - begin.x;
			int deltaY = pos.y - begin.y;

			int stepX = deltaX < 0 ? -1:1;
			int stepY = deltaY < 0 ? -1:1;

			deltaX = abs(deltaX);
			deltaY = abs(deltaY);

			if(deltaY > deltaX)		//斜率>1
			{
				if(deltaX*2 - deltaY >= 0)		//决策参数
				{
					return getDirect(begin, nPos(begin.x + stepX, begin.y + stepY));
				}
				else
				{
					return getDirect(begin, nPos(begin.x, begin.y + stepY));
				}
			}
			else						//斜率<=1
			{
				if(deltaY*2 - deltaX >= 0)		//决策参数
				{
					return getDirect(begin, nPos(begin.x + stepX, begin.y + stepY));
				}
				else
				{
					return getDirect(begin, nPos(begin.x + stepX, begin.y));
				}
			}
			return	_DIR_WRONG; 
		} 
		/// @brief	Bresenham线段算法
		/// @param	begin 起点坐标
		/// @param	end 终点坐标
		/// @param	len 线段最长长度
		/// @return	posVec 线段点集
		inline void bresenhamLine(const nPos& begin, const nPos& end, const BYTE& len, nPosVector& posVec)
		{
			bool interchange = false;
			WORD deltaX = abs(end.x - begin.x);
			WORD deltaY = abs(end.y - begin.y);
			if(deltaY > deltaX)
			{
				WORD temp = deltaX;
				deltaX = deltaY;
				deltaY = temp;
				interchange = true;
			}
			SWORD sx = end.x > begin.x ? 1 : -1;
			SWORD sy = end.y > begin.y ? 1 : -1;

			SWORD p = (deltaY<<1) - deltaX;
			WORD x = begin.x, y = begin.y;
			for(WORD i=0; i<=len; ++i)
			{
				nPos pos(x, y);
				posVec.push_back(pos);
				if(p >= 0)
				{
					if(! interchange)
						y += sy;
					else
						x += sx;
					p -= deltaX<<1;
				}
				if(! interchange)
					x += sx;
				else
					y += sy;
				p += deltaY<<1;
			}
		} 

		/* 返回顶角在o点，起始边为os，终止边为oe的夹角(单位：弧度) 
		   角度小于pi，返回正值 
		   角度大于pi，返回负值 
		   可以用于求线段之间的夹角 
		   */ 
		inline SWORD getAngle(const nPos2D &o, const nPos2D &s, const nPos2D &e) 
		{
			double dsx = s.x - o.x; 
			double dsy = s.z - o.z; 
			double dex = e.x - o.x; 
			double dey = e.z - o.z;

			double cosfi=dsx*dex+dsy*dey; 
			double norm=(dsx*dsx+dsy*dsy)*(dex*dex+dey*dey); 
			cosfi /= sqrt( norm );

			double fi = 0;
			if (cosfi >= 1.0 )
			{
				fi = 0;
			}
			else if (cosfi <= -1.0 ) 
			{
				fi = -PI;
			}
			else
			{
				fi=acos(cosfi); 
				if ((dsx*dey - dsy*dex) <= 0)
				{
					fi = -fi;
				}
			}
			SWORD angle = static_cast<SWORD>(fi * 180 /PI);
			return angle;
		}
		/// 获取两点之间的向量的相对坐标系的弧度
		inline float getAngle(float srcX, float srcY, float destX, float destY)
		{
			float disX = destX - srcX;
			float disY = destY - srcY;
			float len = sqrt(disX * disX + disY * disY);
			if(len == 0) return 0;
			float normalX = disX/len;
			float normalY = disY/len;
			return acos(normalY) * (normalX > 0 ? 1 : -1);
		}
		inline bool pointInTriangle(const nPos2D &point, const nPos2D &a, const nPos2D &b, const nPos2D &c)
		{
			// Algorithm from http://www.blackpawn.com/texts/pointinpoly/default.html
			float x0 = c.x - a.x;
			float z0 = c.z - a.z;
			float x1 = b.x - a.x;
			float z1 = b.z - a.z;
			float x2 = point.x - a.x;
			float z2 = point.z - a.z;

			float dot00 = x0 * x0 + z0 * z0;
			float dot01 = x0 * x1 + z0 * z1;
			float dot02 = x0 * x2 + z0 * z2;
			float dot11 = x1 * x1 + z1 * z1;
			float dot12 = x1 * x2 + z1 * z2;
			float denominator = dot00 * dot11 - dot01 * dot01;
			if (!denominator)
				// Triangle is zero-area. Treat querz point as not being inside.
				return false;
			// Compute
			float inverseDenominator = 1.0f / denominator;
			float u = (dot11 * dot02 - dot01 * dot12) * inverseDenominator;
			float v = (dot00 * dot12 - dot01 * dot02) * inverseDenominator;
			// 允许的误差
			const float tolerance = 0.0000;

			return (u >= 0.0f - tolerance) && (v >= 0.0f - tolerance) && (u + v <= 1.0f + tolerance);
		}

		inline bool pointInRect(const nPos2D &pos, const nRect rect)
		{
			return  pos.x >= rect.pos.x && pos.x <= rect.pos.x + rect.size.x &&
				pos.z >= rect.pos.z && pos.z <= rect.pos.z + rect.size.z;
		}

		enum eSide
		{
			eSide_None,
			eSide_Left,
			eSide_Right,
		};
		inline BYTE classifyPoint(const nPos2D &pos, const nPos2D &lineStart, const nPos2D &lineEnd)
		{
			float v = (pos.x - lineStart.x) * (lineEnd.z - lineStart.z) - (lineEnd.x - lineStart.x) * (pos.z - lineStart.z);
			if(v  > 0){
				return eSide_Right;
			}
			else if(v < 0){
				return eSide_Left;
			}
			return eSide_None;
		}

		/*计算客户端需要的向量
		  X和Z轴的mol*/   
		inline const nPos2D getDir(const nPos2D& begin, const nPos2D& pos)
		{
			nPos2D dir;
			dir.x = pos.x - begin.x;
			dir.z = pos.z - begin.z;
			float absDirX = fabs(dir.x);
			float absDirZ = fabs(dir.z);
			float max;
			if(absDirX > absDirZ){
				max = absDirX;
			}
			else{
				max = absDirZ;
			}
			dir.x = dir.x/ceil(max);
			dir.z = dir.z/ceil(max);
			return dir;
		}
		inline bool getDir(const nPos3D& start, const nPos3D& end, nPos3D *direct)
		{
			float distance = getDistance(start, end);
			if(distance == 0){
				return false;
			}
			direct->x = (end.x - start.x) / distance;
			direct->y = (end.y - start.y) / distance;
			direct->z = (end.z - start.z) / distance;
			return true;
		}
		inline bool isRectIntersect(const nRect &a, const nRect &b)
		{
			float minx = std::max(a.pos.x, b.pos.x);
			float minz = std::max(a.pos.z, b.pos.z);
			float maxx = std::min(a.pos.x + a.size.x, b.pos.x + b.size.x);
			float maxz = std::min(a.pos.z + a.size.z, b.pos.z + b.size.z);
			if(minx >= maxx || minz >= maxz) {
				return false;
			}
			else {
				return true;
			}
		}
		inline void getRectByLine(const nPos2D &lineStart, const nPos2D &lineEnd, nRect *dest)
		{
			float minx = std::min(lineStart.x, lineEnd.x);
			float minz = std::min(lineStart.z, lineEnd.z);

			float maxx = std::max(lineStart.x, lineEnd.x);
			float maxz = std::max(lineStart.z, lineEnd.z);

			dest->pos.x = minx;
			dest->pos.z = minz;
			dest->size.x = maxx - minx;
			dest->size.z = maxz - minz;
			/// 加一个修正，防止完全水平或者垂直的线的宽度是0，造成判断错误
			if(dest->size.x == 0) {
				dest->size.x = 0.0001;
			}
			if(dest->size.z == 0) {
				dest->size.z = 0.0001;
			}
			return;
		}
		inline double mul(nPos2D a, nPos2D b, nPos2D c)
		{
			nPos2D AB, AC;
			AB.x = b.x - a.x;
			AB.z = b.z - a.z;
			AC.x = c.x - a.x;
			AC.z = c.z - a.z;
			return (AB.x * AC.z - AB.z * AC.x);
		}
		inline bool isLineIntersect(const nPos2D &line1Start, const nPos2D &line1End, const nPos2D &line2Start, const nPos2D &line2End)
		{
			// 快速排斥判断
			nRect line1Rect;
			getRectByLine(line1Start, line1End, &line1Rect);
			nRect line2Rect;
			getRectByLine(line2Start, line2End, &line2Rect);
			if(isRectIntersect(line1Rect, line2Rect) == false) {
				return false;
			}
			// 跨立判断
			double a = mul(line2Start, line1Start, line2End) * mul(line2Start, line1End, line2End);
			double b = mul(line1Start, line2Start, line1End) * mul(line1Start, line2End, line1End);
			if(	a <= 0.1 && b <= 0.1) {
				return true;
			}
			else {
				return false;
			}
		}
		/// brief 取得两线段交点，需要先确认两条线段相交，否则取到的点就是错误的
		inline void getLineIntersectPos(const nPos2D &aStart, const nPos2D &aEnd, const nPos2D &bStart, const nPos2D &bEnd, nPos2D *dest)
		{
			*dest = aStart;
			double t = ((aStart.x-bStart.x)*(bStart.z-bEnd.z)-(aStart.z-bStart.z)*(bStart.x-bEnd.x)) / ((aStart.x-aEnd.x)*(bStart.z-bEnd.z)-(aStart.z-aEnd.z)*(bStart.x-bEnd.x));
			dest->x += (aEnd.x - aStart.x) * t;
			dest->z += (aEnd.z - aStart.z) * t;
		}
		inline bool isLineIntersectRect(const nPos2D &startPos, const nPos2D &endPos, const nRect &rect)
		{
			if(pointInRect(startPos, rect) == true || pointInRect(endPos, rect) == true) {
				return true;
			}
			nPos2D rectSideStartPos, rectSideEndPos;
			rect.getTopLine(&rectSideStartPos, &rectSideEndPos);
			if(isLineIntersect(startPos, endPos, rectSideStartPos, rectSideEndPos) == true) {
				return true;
			}
			rect.getBottomLine(&rectSideStartPos, &rectSideEndPos);
			if(isLineIntersect(startPos, endPos, rectSideStartPos, rectSideEndPos) == true) {
				return true;
			}
			rect.getLeftLine(&rectSideStartPos, &rectSideEndPos);
			if(isLineIntersect(startPos, endPos, rectSideStartPos, rectSideEndPos) == true) {
				return true;
			}
			rect.getRightLine(&rectSideStartPos, &rectSideEndPos);
			if(isLineIntersect(startPos, endPos, rectSideStartPos, rectSideEndPos) == true) {
				return true;
			}
			return false;
		}
		inline bool isLineIntersectTriangle(const nPos2D &startPos, const nPos2D &endPos, const nPos2D &vertexA, const nPos2D &vertexB, const nPos2D &vertexC) {
			if(pointInTriangle(startPos, vertexA, vertexB, vertexC) == true) {
				return true;
			}
			if(pointInTriangle(endPos, vertexA, vertexB, vertexC) == true) {
				return true;
			}
			if(isLineIntersect(startPos, endPos, vertexA, vertexB) == true) {
				return true;
			}
			if(isLineIntersect(startPos, endPos, vertexB, vertexC) == true) {
				return true;
			}
			if(isLineIntersect(startPos, endPos, vertexC, vertexA) == true) {
				return true;
			}
			return true;
		}
#if 0
		/// @ brief 返回以pos为中心，dstPos的角度(0 - 360)
		inline const double getAngle(const nPos2D &pos, const nPos2D &dstPos)
		{
			double dRotateAngle = atan2(fabs(pos.x-dstPos.x),fabs(pos.z-dstPos.z));
			if (dstPos.x >= pos.x)
			{
				if (dstPos.z >= pos.z)
				{
				}
				else
				{
					dRotateAngle = PI - dRotateAngle;
				}
			}
			else
			{
				if (dstPos.z >= pos.z)
				{
					dRotateAngle = 2 * PI - dRotateAngle;
				}
				else
				{
					dRotateAngle = PI + dRotateAngle;
				}
			}
			dRotateAngle = dRotateAngle * 180 / PI;
			return dRotateAngle;
		}
		/// @ brief 以pos为中心，取得dstPos的朝向，分别有8个朝向
		inline const BYTE getDirectArea(const nPos2D &pos, const nPos2D &dstPos)
		{
			double angle = getAngle(pos, dstPos);
			return static_cast<BYTE>(angle / 45);
		}
		/// @ brief 以pos为中心，取得与dst朝向相反的朝向
		inline const BYTE getAntiDirectArea(const nPos2D &pos, const nPos2D &dstPos)
		{
			BYTE directArea = getDirectArea(pos, dstPos);
			directArea += 4;
			directArea %= 8;
			return directArea;
		}
		enum eDirect {
			eDirect_Min = 0,
			eDirect_Max = 7,
			eDirect_Count
		};
		inline void getDirectVector(const BYTE area, nPos2D *vector) 
		{
			assert_debug(area <= eDirect_Max, "错误的朝向区域");
			static float directVector[eDirect_Count][2] = {
				{0.5, 1.0},
				{1.0, 0.5},
				{1.0, -0.5},
				{0.5, -1.0},
				{-0.5, -1.0},
				{-1.0, -0.5},
				{-1.0, 0.5},
				{-0.5, 1.0}
			};
			vector->x = directVector[area][0];
			vector->z = directVector[area][1];
			vector->normalise();
		}
		/// @ brief 取得以pos为中心，area方向，len距离的一个点
		inline void getDirectPos(const nPos2D &pos, const float len, const BYTE area, nPos2D *dst)
		{
			getDirectVector(area, dst);
			dst->x *= len;
			dst->z *= len;
			dst->x += pos.x;
			dst->z += pos.z;
		}
		/// @ brief 取得以pos为中心，与dstPos朝向相反的(按areaRemote修正朝向)，len距离的点
		inline void getAntiDirectPos(const nPos2D &pos, const nPos2D &dstPos, const float len, const int areaRemote, nPos2D *dst)
		{
			int antiDirectArea = getAntiDirectArea(pos, dstPos) + areaRemote;
			if(antiDirectArea >= 0) {
				antiDirectArea %= eDirect_Count;
			}
			else {
				antiDirectArea = eDirect_Count - (abs(antiDirectArea) % eDirect_Count);
			}
			getDirectPos(pos, len, antiDirectArea, dst);
		}
#endif
		enum eDirect
		{
			eDirect_Min = 0,
			eDirect_Right = 0,
			eDirect_RightDown = 1,
			eDirect_Down = 2,
			eDirect_LeftDown = 3,
			eDirect_Left = 4,
			eDirect_LeftUp = 5,
			eDirect_Up = 6,
			eDirect_RightUp = 7,
			eDirect_Max = 7,
			eDirect_Count = 8
		};
		inline const float getAngle(const nPos2D& start, const nPos2D& end)
		{
			//const float PI = 3.141592653;
			float x = end.x - start.x;
			float y = end.z - start.z;

			float hypotenuse = sqrt(x * x + y * y);
			float cos = x / hypotenuse;
			float radian = acos(cos);

			float angle = 180 / (PI / radian);
			if(y < 0)
			{
				angle = 360 - angle;
			}
			else if(y == 0 && x < 0)
			{
				angle = 180;
			}
			return angle;
		}
		inline const DWORD getDirectInEight(const nPos2D& start, const nPos2D& end)
		{
			float angle = static_cast<DWORD>(getAngle(start, end));
			angle += 22.5;
			return static_cast<DWORD>((angle > 359 ? angle - 359 : angle) / 45);
		}
		inline const DWORD getAntiDirectInEight(const nPos2D& start, const nPos2D& end)
		{
			return (getDirectInEight(start, end) + 4) % 8;
		}

		inline void getDirectPos(const nPos2D& start, const DWORD direct, const float len, nPos2D *dest)
		{
			nPos2D vector;
			switch(direct)
			{
				case eDirect_Right:
					{
						vector.x = 1;
						vector.z = 0;
					}
					break;
				case eDirect_RightDown:
					{
						vector.x = 1;
						vector.z = 1;
					}
					break;
				case eDirect_Down:
					{
						vector.x = 0;
						vector.z = 1;
					}
					break;
				case eDirect_LeftDown:
					{
						vector.x = -1;
						vector.z = 1;
					}
					break;
				case eDirect_Left:
					{
						vector.x = -1;
						vector.z = 0;
					}
					break;
				case eDirect_LeftUp:
					{
						vector.x = -1;
						vector.z = -1;
					}
					break;
				case eDirect_Up:
					{
						vector.x = 0;
						vector.z = -1;
					}
					break;
				case eDirect_RightUp:
					{
						vector.x = 1;
						vector.z = -1;
					}
					break;
			}
			vector.normalise();
			dest->x = start.x + vector.x * len;
			dest->z = start.z + vector.z * len;
		}

		inline void getAntiDirectPos(const nPos2D& start, const nPos2D& end, const float len, const int off, nPos2D *dest)
		{
			int AntiDirect = getAntiDirectInEight(start, end);
			AntiDirect = (AntiDirect + off) % 8;
			getDirectPos(start, AntiDirect, len, dest);
		}

		inline float checkProjectInLine(const nPos2D &startPos, const nPos2D &endPos, const nPos2D &checkPos )
		{
			nPos2D startLine((checkPos.x - startPos.x), (checkPos.z - startPos.z));
			nPos2D endLine((endPos.x - startPos.x), (endPos.z - startPos.z));
			float v = (startLine.x * endLine.x + startLine.z * endLine.z)/(endLine.x * endLine.x + endLine.z * endLine.z);
			if(v >= 0 && v <= 1){
				nPos2D startToProjectLine(endLine.x * v, endLine.z * v);
				v = startToProjectLine.normalise()/endLine.normalise();
			}
			return v;
		}

        inline void rotationToVector(float x, float y, float z, float w, float& outx, float& outy, float& outz)
        {
            float unitZ_x = 0.0f;
            float unitZ_y = 0.0f;
            float unitZ_z = 1.0f;
            float uv_x = y * unitZ_z - z * unitZ_y;
            float uv_y = z * unitZ_x - x * unitZ_z;
            float uv_z = x * unitZ_y - y * unitZ_x;

            float uuv_x = y * uv_z - z * uv_y;
            float uuv_y = z * uv_x - x * uv_z;
            float uuv_z = x * uv_y - y * uv_x;

            uv_x *= 2.0f * w;
            uv_y *= 2.0f * w;
            uv_z *= 2.0f * w;

            uuv_x *= 2.0f;
            uuv_y *= 2.0f;
            uuv_z *= 2.0f;

            outx = unitZ_x + uv_x + uuv_x;
            outy = unitZ_y + uv_y + uuv_y;
            outz = unitZ_z + uv_z + uuv_z;
        }

        /// 正角度为顺时针旋转
        inline void rotateVector2D(const nPos2D& src, const float angle, nPos2D* dest)
        {
            float radian = angle * PI / 180.0f;
            nPos2D srcVector = src; // 这么做是为了防止src和dest是一个对象
            dest->x = srcVector.x * cos(radian) - srcVector.z * sin(radian);
            dest->z = srcVector.z * cos(radian) + srcVector.x * sin(radian);
        }
	};
};
#endif
