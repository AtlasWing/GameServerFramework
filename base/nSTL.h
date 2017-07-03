#ifndef _NSTL_H
#define _NSTL_H

#include "base/nAssert.h"
#include "base/nSys.h"
#include "base/nMisc.h"
#include "base/nType.h"

namespace Tidy
{
#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
	  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
		                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif
	inline uint32_t SuperFastHash (const char * data, int len) {
		uint32_t hash = len, tmp;
		int rem;

		if (len <= 0 || data == NULL) return 0;

		rem = len & 3;
		len >>= 2;

		/* Main loop */
		for (;len > 0; len--) {
			hash  += get16bits (data);
			tmp    = (get16bits (data+2) << 11) ^ hash;
			hash   = (hash << 16) ^ tmp;
			data  += 2*sizeof (uint16_t);
			hash  += hash >> 11;
		}

		/* Handle end cases */
		switch (rem) {
			case 3: hash += get16bits (data);
					hash ^= hash << 16;
					hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
					hash += hash >> 11;
					break;
			case 2: hash += get16bits (data);
					hash ^= hash << 11;
					hash += hash >> 17;
					break;
			case 1: hash += (signed char)*data;
					hash ^= hash << 10;
					hash += hash >> 1;
		}

		/* Force "avalanching" of final 127 bits */
		hash ^= hash << 3;
		hash += hash >> 5;
		hash ^= hash << 4;
		hash += hash >> 17;
		hash ^= hash << 25;
		hash += hash >> 6;

		return hash;
	}
    typedef std::string string;
    template<typename T>
        struct basic_ostream: public std::basic_ostream<T>
    {
    };
    template<typename T>
        struct basic_istream: public std::basic_istream<T>
    {
    };

    template<typename T>
        struct vector: public std::vector<T>
    {
        typedef std::vector<T> ImpType;
        vector()
        {
        }
        vector(int size):std::vector<T>(size)
        {
        }
        vector(const Tidy::vector<T>& v):std::vector<T>(v)
        {
        }
        vector(const size_t size, const T &v) : std::vector<T>(size, v)
        {
        }
        template <class InputIterator>
            vector(InputIterator first, InputIterator last) : std::vector<T>(first, last)
            {
            }
        size_t datasize() const
        {
            return this->size() * sizeof(T);
        }
        size_t remove(const T& value)
        {
            for(typename vector<T>::iterator iter = this->begin() ; iter != this->end(); ++iter) {
                if(*iter == value) {
                    *iter = this->back();
                    erase(this->end()-1);
                    return 1;
                }
            }
            return 0;
        }
        void removeByIndex(const size_t index)
        {
            assert_debug(index < this->size() && this->size() > 0, "");
            if((index + 1) < this->size()) {
                (*this)[index] = this->back();
            }
            erase(this->end() - 1);
        }
        T& add(const T &v)
        {
            this->push_back(v);
            return this->back();
        }
        T& add()
        {
            this->resize(this->size() + 1);
            return this->back();
        }
        void freeAll()
        {
            vector<T> tempVec;
            swap(tempVec);
        }
		const T* data() const
		{
			if(this->empty() == true){
				return NULL;
			}
			return &this->at(0);
		}
    };
    template<typename T>
        struct deque: public std::deque<T>
    {
        T& add(const T &value)
        {
            this->push_back(value);
            return this->back();
        }
    };
    template<typename T>
        struct list: public std::list<T>
    {
    };
    template<typename T>
        struct slist: public std::list<T>
    {
    };
    template<typename T, typename ContainerT=list<T> >
        struct queue: public std::queue<T ,ContainerT>
    {
    };
    template<typename T, typename ContainerT=list<T> >
        struct stack: public std::stack<T ,ContainerT>
    {
    };


    struct key_equal : public std::binary_function<const char*, const char*, bool>
    {
        /// @brief 字串的等值比较
        /// @param s1	比较参数1
        /// @param s2	比较参数2
        /// @return 是否相等
        bool operator()(const char * s1, const char * s2) const
        {
            return strcmp(s1, s2) == 0;
        }
        bool operator()(const std::string &s1, const std::string &s2) const
        {
            return s1.length() == s2.length() && strcmp(s1.c_str(), s2.c_str()) == 0;
        }
        /// @brief 数值的等值比较
        /// @param s1	比较参数1
        /// @param s2	比较参数2
        /// @return 是否相等
#if 0
        bool operator()(const DWORD s1, const DWORD s2) const
        {
            return s1 == s2;
        }
#endif
        bool operator()(const size_t s1, const size_t s2) const
        {
            return s1 == s2;
        }
        /// @brief 指针的等值比较
        /// @param s1	比较参数1
        /// @param s2	比较参数2
        /// @return 是否相等
        template<typename K>
            bool operator()(const K* s1, const K* s2) const
            {
                return s1 == s2;
            }
    };

    /// @brief	hash_map使用的key类型的hash的定义
    struct hash
    {
        inline size_t operator()(const char* __s) const { return __gnu_cxx::__stl_hash_string(__s); }
        inline size_t operator()(const std::string &__s) const { return SuperFastHash(__s.c_str(), __s.size()); }
        inline size_t operator()(unsigned char __x) const { return __x; }
        inline size_t operator()(short __x) const { return __x; }
        inline size_t operator()(unsigned short __x) const { return __x; }
        inline size_t operator()(int __x) const { return __x; }
        inline size_t operator()(unsigned int __x) const { return __x; }
        inline size_t operator()(long __x) const { return __x; }
        inline size_t operator()(unsigned long __x) const { return __x; }
        inline size_t operator()(void *__x) const { return (size_t)__x; } 
        inline size_t operator()(unsigned long long __x) const { return __x; } 
    };

    /// @brief	比较函数.string做了偏特化
    template <class KeyT>
        struct less : public std::less<KeyT>
    {
    };
    /// @brief	特化pair模板
    template <>
        struct less<std::pair<DWORD, WORD> > : public std::less<std::pair<DWORD, WORD> >
        {
            bool operator()(const std::pair<DWORD, WORD>& p1, const std::pair<DWORD, WORD>& p2) const
            {
                if(p1.first<p2.first && p1.second<p2.second)
                    return true;
                return false;
            }
        };
    template <>
        struct less<const char*> : public std::less<const char*>
        {
            bool operator()(const char *__x, const char *__y) const
            {
                return strcmp(__x, __y) < 0;
            }
        };
    template <>
        struct less<std::string> : public std::less<std::string>
        {
            bool operator()(const std::string& __x, const std::string& __y) const
            {
                if(__x.length() < __y.length())
                {
                    return true;
                }
                else if(__x.length() > __y.length())
                {
                    return false;
                }
                return memcmp(__x.c_str(), __y.c_str(), __y.length()) < 0;
            }
        };

    /// @brief	hash_map.目前支持整形,const char*,普通指针
    template<typename K, typename V>
        struct hash_map : public __gnu_cxx::hash_map<K, V, hash, key_equal>
	{
		typedef hash_map<K, V> MapT;
		const V* get(const K &key) const{
			typename MapT::const_iterator iter = find(key);
			if(iter == this->end()){
				return NULL;
			}
			return &iter->second;
		}
		V* get(const K &key){
			typename MapT::iterator iter = find(key);
			if(iter == this->end()){
				return NULL;
			}
			return &iter->second;
		}
		V* fetch(const K &key){
			typename MapT::iterator iter = find(key);
			if(iter == this->end()){
				return &((*insert(typename MapT::value_type(key, V())).first).second);
			}
			else{
				return &iter->second;
			}
		}
		V* fetch(const K &key, const V &defaultValue){
			typename MapT::iterator iter = find(key);
			if(iter == this->end()){
				return &((*insert(typename MapT::value_type(key, defaultValue)).first).second);
			}
			else{
				return &iter->second;
			}
		}
    };
    /// @brief	hash_set.目前支持整形,const char*,普通指针
    template<typename V>
        struct hash_set : public __gnu_cxx::hash_set<V, hash, key_equal>
    {
    };
    /// @brief	std::map使用内存池.对string的less做了优化
    template<typename K, typename V>
        struct map : public std::map<K, V, less<K> >
    {
		typedef map<K, V> MapT;
        template<typename ListT>
        void getAllKey(ListT &list) const
        {
            for(typename map<K, V>::const_iterator iter = this->begin(); iter != this->end(); ++ iter){
                list.push_back(iter->first);
            }
        }
		const V* get(const K &key) const{
			typename MapT::const_iterator iter = find(key);
			if(iter == this->end()){
				return NULL;
			}
			return &iter->second;
		}
		V* get(const K &key){
			typename MapT::iterator iter = find(key);
			if(iter == this->end()){
				return NULL;
			}
			return &iter->second;
		}
		V* fetch(const K &key){
			typename MapT::iterator iter = lower_bound(key);
			if(iter == this->end() || iter->first != key){
				return &((*(insert(iter, typename MapT::value_type(key, V())))).second);
			}
			else{
				return &iter->second;
			}
		}
    };
    /// @brief	std::multimap使用内存池
    template<typename K, typename V>
        struct multimap : public std::multimap<K, V, less<K> >
    {
    };

    /// @brief	std::set使用内存池.对string的less做了优化
    template<typename K>
        struct set : public std::set<K, less<K> >
    {
    };
    /// @brief	添加父子类型编译时期检查,对运行效率没影响
    template <typename To, typename From>
        inline To implicit_cast(const From& from)
        {
            return from;
        }
    template <typename SubType, typename BaseType>
        inline void check_subtype(void)
        {
            if(false)
            {
                implicit_cast<BaseType*, SubType*>(0);
            }
        }

    template<typename ContainerType>
        inline typename ContainerType::const_iterator find(const ContainerType& container, const typename ContainerType::value_type& value)
        {
            return std::find(container.begin(), container.end(), value);
        }

    template <class T>
        class share_ptr : public std::tr1::shared_ptr<T>
    {
        public:
            typedef class std::tr1::shared_ptr<T> ImpType;
        public:
            explicit share_ptr(T* p) : ImpType(p)
        {
        }
            share_ptr(const share_ptr& other) : ImpType(other)
        {
        }

            const bool valid() const
            {
                return static_cast<bool>(*this);
            }
    };

    //class JsonWritter : public rapidjson::Writer< rapidjson::GenericStringBuffer< rapidjson::UTF8<>, rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator>

    typedef rapidjson::GenericStringBuffer< rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<> > JsonStringBuffer;
    typedef rapidjson::Writer< JsonStringBuffer, rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<> > BaseWriter;
    class StackAlloc
    {
        public:
            StackAlloc():
                _mem_alloctor(_stack_buffer, sizeof(_stack_buffer)),
                _str_buffer(&_mem_alloctor, sizeof(_stack_buffer))
        {
        }

        public:
                void clear(){
                    _str_buffer.Clear();
                }
                const char* getCString() const{
                    return _str_buffer.GetString();
                }
                const DWORD getSize() const{
                    return _str_buffer.Size();
                }

        protected:
            rapidjson::MemoryPoolAllocator<> _mem_alloctor;
            JsonStringBuffer _str_buffer;

        private:
            char _stack_buffer[64 * 1024];
    };
    class JsonWritter : public StackAlloc, public BaseWriter
    {
        public:
            JsonWritter() : 
                StackAlloc(),
                BaseWriter(_str_buffer, &_mem_alloctor)
            {
            }
    };
    template<size_t N>
    class BitSet{
        public:
            BitSet(){
                reset();
            }
            BitSet(const BitSet<N> &other){
                *this = other;
            }

        public:
            bool any() const {
                DWORD spaceSize = getSpaceSize();
                for(DWORD i = 0; i < spaceSize; ++ i){
                    if(_data[i] > 0){
                        return true;
                    }
                }
                return false;
            }
            bool none() const{
                return any() == false;
            }
            BitSet<N>& reset(){
                DWORD spaceSize = getSpaceSize();
                for(DWORD i = 0; i < spaceSize; ++ i){
                    _data[i] = 0;
                }
                return *this;
            }
            BitSet<N>& set( size_t pos, bool val = true ){
                if(val == true){
                    _data[pos/8] |= (1 << (pos % 8));
                }
                else{
                    reset(pos);
                }
                return *this;
            }
            BitSet<N>& set(){
                DWORD spaceSize = getSpaceSize();
                for(DWORD i = 0; i < spaceSize; ++ i){
                    _data[i] = ((BYTE)-1);
                }
                return *this;
            }
            bool test(size_t pos) const{
                return (_data[pos/8] & (1 << (pos % 8))) > 0;
            }
            BitSet<N>& reset(size_t pos) {
                _data[pos/8] &= (~(1 << (pos % 8)));
                return *this;
            }
            size_t size() const {
                return N;
            }
            bool operator== (const BitSet<N> &other) const{
                DWORD spaceSize = getSpaceSize();
                for(DWORD i = 0; i < spaceSize; ++ i){
                    if(_data[i] != other._data[i]){
                        return false;
                    }
                }
                return true;
            }
            bool operator!= (const BitSet<N> &other) const{
                return !((*this) == other);
            }
            BitSet<N>& operator= (const BitSet<N> &other){
                DWORD spaceSize = getSpaceSize();
                for(DWORD i = 0; i < spaceSize; ++ i){
                    _data[i] = other._data[i];
                }
                return *this;
            }
        private:
            DWORD getSpaceSize() const{
                return (N + 8)/8;
            }
        private:
            BYTE _data[(N + 8)/8];
    };
}; 
#endif
