#ifndef _N_CLASS_MANAGER_H_
#define _N_CLASS_MANAGER_H_

#include "base/nSTL.h" 

namespace Tidy
{
	/// @brief	使用id存储class的容器,方便根据id创建对象.
	// 		要求类型提供无参构造函数,addType的模板类型是BaseType的子类.使用vector容器,必须执行resize()
	template<typename BaseType>
		class ClassManager
		{
			private:
				/// @brief	存储类型的基类
				class ClassTypeBase
				{
					public:
						virtual ~ClassTypeBase(){}
						virtual BaseType* clone() const= 0;
				};
				/// @brief	存储具体类型.SubType是BaseType的子类
				template<typename SubType>
					class  ClassType : public ClassTypeBase
				{
					public:
						virtual BaseType* clone() const
						{
							Tidy::check_subtype<SubType, BaseType>();
							return new SubType();
						}
				};

			public:
				/// @brief	构造函数
				ClassManager() : _size(0)
			{
			}
				/// @brief	构造函数
				ClassManager(int conSize) : _size(conSize)
			{
				_id_class_map.reserve(_size);
			}
				/// @brief	析构函数
				~ClassManager()
				{
					for(typename IDClassMap::iterator iter = _id_class_map.begin(); iter != _id_class_map.end(); ++ iter)
					{
						delete(*iter);
					}
					_id_class_map.clear();
				}
			public:
				/// @brief	添加新的类型
				/// @param	id 用于标识该类的ID,一个类型可以有多个id.SubType是BaseType的子类
				template<typename SubType>
					bool addClass(const int &id)
					{
						Tidy::check_subtype<SubType, BaseType>();
						_id_class_map[id] = new ClassType<SubType>();
						return true;
					}
				/// @brief	根据addType时的id创建存储的类型的对象,使用无参构造函数.需要调用者负责对象删除
				BaseType* createObject(const int &id) 
				{
					return (id < _size) ? (_id_class_map[id]->clone()) : NULL;
				}
				/// @brief	手动分配空间
				void resize(int size)
				{
					_id_class_map.resize(size);
					_size = size;
				}
			private:
				/// @brief	类型对象容器
				typedef Tidy::vector<ClassTypeBase*> IDClassMap;
				IDClassMap _id_class_map;
				/// @brief	容量
				int _size;
		};
};
#endif
