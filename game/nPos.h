#ifndef _NPOS_H_
#define _NPOS_H_

#include "protocol/commonType.h"

#include "base/Tidy.h"
#include "base/nSTL.h"

namespace Game
{
	extern DWORD SCREEN_GRID_WIDTH;	///< 单屏宽 
	extern DWORD SCREEN_GRID_HEIGHT;///< 单屏高 

	/// @brief 常量定义
#define PI    (3.14159265358979323846f)        
#define _2PI   (2.0 * 3.14159265358979323846f)
#define PIP2    (3.14159265358979323846f/2)  
#define PIP4   (2.0 * 3.14159265358979323846f/4)  
#define PIP8   (2.0 * 3.14159265358979323846f/8) 
#define PIP16   (2.0 * 3.14159265358979323846f/16)  
#define PIMUL	1024

	/// @brief 定义时针方向
	enum ClockWiseType
	{
		ClockWise 		= 0,			///< 顺时针方向 
		AntiClockWise	= 1				///< 逆时针方向 
	};

	/// @brief 场景屏坐标
	typedef WORD nPosI;

	/// @brief 场景坐标
#pragma pack(1)
	struct nPos
	{
		public:
			nPos()
			{
				hashValue = 0;
			}
			nPos(const WORD x, const WORD y)
			{
				this->x = x;
				this->y = y;
			}
			nPos( const DWORD hash )
			{
				this->hashValue = hash;
			}

			nPos(const nPos &pos)
			{
				hashValue = pos.hashValue;
			}

		public:
			/// @brief	检查两点是否在范围内.计算的不是绝对距离
			/// @param	pos 另外一点
			/// @param	range 范围半径
			/// @return	是否在范围内
			bool checkInRange(const nPos& pos, const WORD& range) const;
			/// @brief 获得屏
			/// @return 屏值
			nPosI getScreenIndex() const;
			/// @brief 获得DWORD hash值
			DWORD hash() const;
			/// @brief	根据中心点以及方向获取该坐标的绝对坐标
			/// @param	center 中心点
			/// @param	dir 方向
			/// @return	绝对坐标
			nPos getAbsPos(const nPos &center, const BYTE &dir);
			/// @brief	根据方向半径为distance获取坐标点
			/// @param	dir 方向
			/// @param	distance 距离
			/// @return	获取的坐标点
			nPos getPosByDistance(const BYTE& dir, const WORD& distance) const; 
			/// @brief	设置坐标点
			void set(const WORD& x, const WORD& y);
			/// @brief	互换纵横坐标
			void swap(void);
			/// @brief	内容清零
			void clear(void);
			const bool isValid() const;
			const bool isZero() const;
		public:
			/// @brief 赋值操作符号
			nPos & operator= (const nPos &pos);
			/// @brief 重载+运算符号
			const nPos & operator+ (const nPos &pos);
			/// @brief 重载+=运算符号
			const nPos & operator+= (const nPos &pos);
			/// @brief 重载-运算符号
			const nPos & operator- (const nPos &pos);
			/// @brief 重载-=运算符号
			const nPos & operator-= (const nPos &pos);
			/// @brief 重载==逻辑运算符号
			const bool operator== (const nPos &pos) const;
			/// @brief 重载>逻辑运算符号
			const bool operator> (const nPos &pos) const;
			/// @brief 重载>=逻辑运算符号
			const bool operator>= (const nPos &pos) const;
			/// @brief 重载<逻辑运算符号
			const bool operator< (const nPos &pos) const;
			/// @brief 重载<=逻辑运算符号
			const bool operator<= (const nPos &pos) const;
			/// @brief 重载!=逻辑运算符号
			const bool operator!= (const nPos &pos) const;

		public:
			union
			{
				struct
				{
					/// @brief 横坐标
					WORD x;
					/// @brief 纵坐标
					WORD y;
				};
				DWORD hashValue;
				WORD  value[2];
			};
	};
#pragma pack()
	/// @brief 存放屏编号的向量
	typedef Tidy::vector<nPosI> nPosIVector;
	/// @brief 存放坐标的向量
	typedef Tidy::vector<nPos> nPosVector;
	/// @brief	非法屏索引
	const nPosI nInvalidPosI = (const nPosI)-1;
	/// @brief	非法坐标
	const nPos  nInvalidPos((WORD)-1, (WORD)-1);

	///< ----------------------------------------------------------------------------------
	inline bool nPos::checkInRange(const nPos& pos, const WORD& range) const
	{
		return (abs(x - pos.x) <= range) && (abs(y - pos.y) <= range);
	}

	extern WORD MAP_SCREEN_X;

	inline nPosI nPos::getScreenIndex() const
	{
		return ((*this) != nInvalidPos) ? (MAP_SCREEN_X*(this->y/SCREEN_GRID_HEIGHT) + (this->x/SCREEN_GRID_WIDTH)) : nInvalidPosI;
	}

	inline DWORD nPos::hash() const
	{
		return hashValue;
	}

	inline nPos nPos::getAbsPos(const nPos &center, const BYTE &dir)
	{
		nPos absPos;
		switch(dir)
		{
			case _DIR_UP:
			case _DIR_UPRIGHT:
				{
					absPos.x = x;
					absPos.y = y;
				}
				break;
			case _DIR_RIGHT:
			case _DIR_RIGHTDOWN:
				{
					absPos.x = -y;
					absPos.y = x;
				}
				break;
			case _DIR_DOWN:
			case _DIR_DOWNLEFT:
				{
					absPos.x = -x;
					absPos.y = -y;
				}
				break;
			case _DIR_LEFT:
			case _DIR_LEFTUP:
				{
					absPos.x = y;
					absPos.y = -x;
				}
				break; 
			default:
				{
					absPos.x = x;
					absPos.y = y;
				}
				break;
		}
		absPos.x += center.x;
		absPos.y += center.y;
		return absPos;
	}

	inline nPos nPos::getPosByDistance(const BYTE& dir, const WORD& distance) const
	{
		nPos desPos;
		switch(dir)
		{
			case _DIR_UP:
				{
					desPos.x = x;
					desPos.y = (y > distance) ? (y - distance) : 0;
				}
				break;
			case _DIR_DOWN:
				{
					desPos.x = x;
					desPos.y = y + distance;
				}
				break;
			case _DIR_RIGHT:
				{
					desPos.x = x + distance;
					desPos.y = y;
				}
				break;
			case _DIR_LEFT:
				{
					desPos.x = (x > distance) ? (x - distance) : 0;
					desPos.y = y;
				}
				break;
			case _DIR_RIGHTUP:
				{
					desPos.x = x + distance;
					desPos.y = (y > distance) ? (y - distance) : 0;
				}
				break;
			case _DIR_RIGHTDOWN:
				{
					desPos.x = x + distance;
					desPos.y = y + distance;
				}
				break;
			case _DIR_DOWNLEFT:
				{
					desPos.x = (x > distance) ? (x - distance) : 0;
					desPos.y = y + distance;
				}
				break;
			case _DIR_LEFTUP:
				{
					desPos.x = x - distance;
					desPos.y = y - distance;
				}
				break; 
			default:
				{
					desPos.x = x + distance;
					desPos.y = y + distance;
				}
				break;
		} 
		return desPos;
	}

	inline nPos& nPos::operator= (const nPos &pos)
	{
		hashValue = pos.hashValue;
		return *this;
	}

	inline const nPos& nPos::operator+ (const nPos &pos)
	{
		x += pos.x;
		y += pos.y;
		return *this;
	}

	inline const nPos& nPos::operator+= (const nPos &pos)
	{
		x += pos.x;
		y += pos.y;
		return *this;
	}

	inline const nPos& nPos::operator- (const nPos &pos)
	{
		x -= pos.x;
		y -= pos.y;
		return *this;
	}

	inline const nPos& nPos::operator-= (const nPos &pos)
	{
		x -= pos.x;
		y -= pos.y;
		return *this;
	}

	inline const bool nPos::operator== (const nPos &pos) const
	{
		return hashValue == pos.hashValue;
	}

	inline const bool nPos::operator> (const nPos &pos) const
	{
		return hashValue > pos.hashValue;
	}

	inline const bool nPos::operator>= (const nPos &pos) const
	{
		return hashValue >= pos.hashValue;
	}

	inline const bool nPos::operator< (const nPos &pos) const
	{
		return hashValue < pos.hashValue;
	}

	inline const bool nPos::operator<= (const nPos &pos) const
	{
		return hashValue <= pos.hashValue;
	}

	inline const bool nPos::operator!= (const nPos &pos) const
	{
		return hashValue != pos.hashValue;
	}

	inline void nPos::set(const WORD& x, const WORD& y)
	{
		this->x = x;
		this->y = y;
	}

	inline void nPos::swap(void)
	{
		WORD temp = x;
		x = y;
		y = temp;
	}
	inline void nPos::clear(void)
	{
		x = 0;
		y = 0;
	}

	inline const bool nPos::isValid() const
	{
		return (*this) != nInvalidPos;
	}

	inline const bool nPos::isZero() const
	{
		return x == 0 && y == 0;
	}

#pragma pack(1)
	struct nPos2D
	{
		float x;
		float z;
		nPos2D(const nPos &pos) : x(pos.x), z(pos.y)
		{
		}
		nPos2D() : x(0), z(0)
		{
		}
		nPos2D(const float xPos, const float zPos) : x(xPos), z(zPos)
		{
		}
		nPos2D(const nPos2D& other) : x(other.x), z(other.z)
		{
		}
		const nPos toPos() const
		{
			return nPos((WORD)x, (WORD)z);
		}

		bool checkInRange(const nPos2D& pos, const float range) const;
		float normalise();
		const bool operator== (const nPos2D &pos) const
		{
			return x == pos.x && z == pos.z;
		}
		nPos2D operator+ (const nPos2D &pos) const{
			return nPos2D(x + pos.x, z + pos.z);
		}
		nPos2D operator- (const nPos2D &pos) const{
			return nPos2D(x - pos.x, z - pos.z);
		}
		bool operator!= (const nPos2D &pos) const{
			return x != pos.x || z != pos.z;
		}
		void operator= (const nPos2D &pos) {
			x = pos.x;
			z = pos.z;
		}
        bool operator > (const nPos2D &other) const{
            if(x > other.x){
                return true;
            }
            else if(x == other.x){
                if(z > other.z){
                    return true;
                }
                else{
                    return false;
                }
            }
            return false;
        }   
        bool operator < (const nPos2D &other) const{
            if(x < other.x){
                return true;
            }
            else if(x == other.x){
                if(z < other.z){
                    return true;
                }
                else{
                    return false;
                }
            }
            return false;
        }
	};
	struct nPos3D : public nPos2D
	{
		float y;
		nPos3D(const float xPos, const float zPos, const float yPos) : nPos2D(xPos, zPos), y(yPos)
		{
		}
		nPos3D() : y(0.0)
		{
		}
		nPos3D(const nPos3D &other) : nPos2D(other), y(other.y)
		{
		}
		float normalise();
		nPos3D operator+ (const nPos3D &pos) const{
			return nPos3D(x + pos.x, z + pos.z, y + pos.y);
		}
		nPos3D operator- (const nPos3D &pos) const{
			return nPos3D(x - pos.x, z - pos.z, y - pos.y);
		}
		void operator= (const nPos3D &pos){
			x = pos.x;
			y = pos.y;
			z = pos.z;
		}
	};

	struct nGridPos
	{
		nGridPos(const DWORD xIndex, const DWORD zIndex) : x(xIndex), z(zIndex){
		}
		nGridPos():
			x(0),
			z(0)
		{
		}
		bool operator == (const nGridPos &other) const{
			return x == other.x && z == other.z;
		}
        bool operator > (const nGridPos &other) const{
            if(x > other.x){
                return true;
            }
            else if(x == other.x){
                if(z > other.z){
                    return true;
                }
                else{
                    return false;
                }
            }
            return false;
        }   
        bool operator < (const nGridPos &other) const{
            if(x < other.x){
                return true;
            }
            else if(x == other.x){
                if(z < other.z){
                    return true;
                }
                else{
                    return false;
                }
            }
            return false;
        }
		DWORD x;
		DWORD z;
	};
#pragma pack()
	const nGridPos nInvalidGridPos(MAX_DWORD, MAX_DWORD);
	const nPos2D nInvalidPos2D((float)-1, (float)-1);
	inline nPosI getScreenIndex(const nGridPos &pos)
	{
		DWORD z = static_cast<DWORD>(pos.z);
		DWORD x = static_cast<DWORD>(pos.x);
		return (pos == nInvalidGridPos) ? nInvalidPosI : static_cast<nPosI>(MAP_SCREEN_X*(z/SCREEN_GRID_HEIGHT) + (x/SCREEN_GRID_WIDTH));
		//return (pos == nInvalidPos2D) ? nInvalidPosI : static_cast<nPosI>(MAP_SCREEN_X*(static_cast<DWORD>(pos.z)/SCREEN_GRID_HEIGHT) + (static_cast<DWORD>(pos.x)/SCREEN_GRID_WIDTH));
	}

	inline bool nPos2D::checkInRange(const nPos2D& pos, const float range) const
	{
		return (std::abs(x - pos.x) <= range) && (std::abs(z - pos.z) <= range);
	}

	inline float nPos2D::normalise()
	{
		// 向量单位化
		float fLength = sqrt( x * x + z * z );
		if(fLength > 1e-08) {
			float fInvLength = (float)1.0f / fLength;
			x *= fInvLength;
			z *= fInvLength;
		}
		return fLength;
	}

	inline float nPos3D::normalise()
	{
		// 向量单位化
		float fLength = sqrt( x * x + y * y + z * z );
		if(fLength > 1e-08) {
			float fInvLength = (float)1.0f / fLength;
			x *= fInvLength;
			y *= fInvLength;
			z *= fInvLength;
		}
		return fLength;
	}

	struct nPathNode{
		nPathNode(const nPos2D &pathPos, const DWORD pathFace): pos(pathPos), face(pathFace){
		}
		nPos2D pos;
		DWORD  face;
	};
};
#endif
