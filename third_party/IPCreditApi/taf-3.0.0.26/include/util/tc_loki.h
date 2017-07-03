#ifndef __TC_TYPETRAITS_H
#define __TC_TYPETRAITS_H

#include <memory>

namespace taf
{
/////////////////////////////////////////////////
// ˵��: loki(copy��loki��)
// Author : jarodruan@tencent.com              
/////////////////////////////////////////////////

namespace TL
{
    //ֻ����, ���������, ��ΪTYPELIST��ĩ������
    class NullType;

    //������
    struct EmptyType { };

    /**
     * ��ֵ�����͵�ӳ��
     */
    template<int v>
    struct Int2Type
    {
       enum { value = v };
    };

    /**
     * ���͵����͵�ӳ��
     */
    template<typename T>
    struct Type2Type
    {
        typedef T OriginalType;
    };

    ///////////////////////////////////////////////////////////////////////////
    // ������TypeList�Ķ���(Ŀǰֻ֧��10������)
    /**
     * ������������
    */
    template<typename Head, typename Tail>
    struct TypeList
    {
        typedef Head H;
        typedef Tail T;
    };

    #define TYPELIST_1(T1) TypeList<T1, TL::NullType>
    #define TYPELIST_2(T1, T2) TypeList<T1, TL::TYPELIST_1(T2)>
    #define TYPELIST_3(T1, T2, T3) TypeList<T1, TL::TYPELIST_2(T2, T3)>
    #define TYPELIST_4(T1, T2, T3, T4) TypeList<T1, TL::TYPELIST_3(T2, T3, T4)>
    #define TYPELIST_5(T1, T2, T3, T4, T5) TypeList<T1, TL::TYPELIST_4(T2, T3, T4, T5)>
    #define TYPELIST_6(T1, T2, T3, T4, T5, T6) TypeList<T1, TL::TYPELIST_5(T2, T3, T4, T5, T6)>
    #define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) TypeList<T1, TL::TYPELIST_6(T2, T3, T4, T5, T6, T7)>
    #define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) TypeList<T1, TL::TYPELIST_7(T2, T3, T4, T5, T6, T7, T8)>
    #define TYPELIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9) TypeList<T1, TL::TYPELIST_8(T2, T3, T4, T5, T6, T7, T8, T9)>
    #define TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) TypeList<T1, TL::TYPELIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10)>
    #define TYPELIST_11(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) TypeList<T1, TL::TYPELIST_10(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)>
    #define TYPELIST_12(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) TypeList<T1, TL::TYPELIST_11(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)>
    #define TYPELIST_13(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) TypeList<T1, TL::TYPELIST_12(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)>
    #define TYPELIST_14(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14) TypeList<T1, TL::TYPELIST_13(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)>
    #define TYPELIST_15(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15) TypeList<T1, TL::TYPELIST_14(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)>
    #define TYPELIST_16(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16) TypeList<T1, TL::TYPELIST_15(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)>
    #define TYPELIST_17(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17) TypeList<T1, TL::TYPELIST_16(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17)>
    #define TYPELIST_18(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18) TypeList<T1, TL::TYPELIST_17(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18)>
    #define TYPELIST_19(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19) TypeList<T1, TL::TYPELIST_18(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19)>
    #define TYPELIST_20(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) TypeList<T1, TL::TYPELIST_19(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20)>


    //////////////////////////////////////////////////////////////////////////////
    // ���¶���TypeList�ı����ڵĲ�������(ͨ��ƫ�ػ�ʵ��)
    /**
     * Length: ȡTypeList�ĳ���
     */
    template<class TList> struct Length;
    template<> struct Length<NullType>
    {
        enum { value = 0 };
    };
    template<class Head, class Tail> struct Length<TypeList<Head, Tail> >
    {
        enum { value = 1 + Length<Tail>::value };
    };

    /**
     * TypeAt, ȡ������iλ���ϵ�����
     */
    template<class TList, unsigned int i> struct TypeAt;
    template<class Head, class Tail> struct TypeAt<TypeList<Head, Tail>, 0>
    {
        typedef Head Result;
    };
    template<class Head, class Tail, unsigned int i> struct TypeAt<TypeList<Head, Tail>, i>
    {
        typedef typename TypeAt<Tail, i-1>::Result Result;
    };

    /**
     * TypeAt, ȡ������iλ���ϵ�����, i�����˷���, �򷵻�DefaultType
     */
    template<class TList, unsigned int index, typename DefaultType = NullType> struct TypeAtNonStrict
    {
        typedef DefaultType Result;
    };
    template <class Head, class Tail, typename DefaultType> struct TypeAtNonStrict<TypeList<Head, Tail>, 0, DefaultType>
    {
        typedef Head Result;
    };
    template <class Head, class Tail, unsigned int i, typename DefaultType> struct TypeAtNonStrict<TypeList<Head, Tail>, i, DefaultType>
    {
        typedef typename TypeAtNonStrict<Tail, i - 1, DefaultType>::Result Result;
    };

    /**
     * ȡ����������ΪT�����, û���򷵻�-1
     */
    template<class TList, class T> struct IndexOf;
    template<class T> struct IndexOf<NullType, T>
    {
        enum { value = -1 };
    };
    template<class Tail, class T> struct IndexOf<TypeList<T,Tail>, T>
    {
        enum { value = 0 };
    };
    template<class Head, class Tail, class T> struct IndexOf<TypeList<Head, Tail>, T>
    {
    private:
        enum { temp = IndexOf<Tail, T>::value };
    public:
        enum { value = temp == -1 ? -1 : 1 + temp };
    };

    /**
     * Append, ��ӵ�����β��
     */
    template<class TList, class T> struct Append;
    template<> struct Append<NullType, NullType>
    {
        typedef NullType Result;
    };
    template<class T> struct Append<NullType,T>
    {
        typedef TYPELIST_1(T) Result;
    };
    template<class Head, class Tail> struct Append<NullType, TypeList<Head, Tail> >
    {
        typedef TypeList<Head, Tail> Result;
    };
    template<class Head, class Tail, class T> struct Append<TypeList<Head, Tail>, T>
    {
        typedef TypeList<Head, typename Append<Tail, T>::Result> Result;
    };

    /**
     * Erase ɾ��
     */
    template<class TList, class T> struct Erase;
    template<class T> struct Erase<NullType, T>
    {
        typedef NullType Result;
    };
    template<class T, class Tail> struct Erase<TypeList<T, Tail>, T>
    {
        typedef Tail Result;
    };
    template<class Head, class Tail, class T> struct Erase<TypeList<Head, Tail>, T>
    {
        typedef TypeList<Head, typename Erase<Tail, T>::Result> Result;
    };

    /**
     * EraseAll ɾ��
     */
    template<class TList, class T> struct EraseAll;
    template<class T> struct EraseAll<NullType, T>
    {
        typedef NullType Result;
    };
    template<class T, class Tail> struct EraseAll<TypeList<T, Tail>, T>
    {
        typedef typename EraseAll<Tail, T>::Result Result;
    };
    template<class Head, class Tail, class T> struct EraseAll<TypeList<Head, Tail>, T>
    {
        typedef TypeList<Head, typename EraseAll<Tail, T>::Result> Result;
    };

    /**
     * ����TypeList����
     */
    template<class T1=NullType,  class T2=NullType,  class T3=NullType,  class T4=NullType,  class T5=NullType,
             class T6=NullType,  class T7=NullType,  class T8=NullType,  class T9=NullType,  class T10=NullType,
             class T11=NullType, class T12=NullType, class T13=NullType, class T14=NullType, class T15=NullType,
             class T16=NullType, class T17=NullType, class T18=NullType, class T19=NullType, class T20=NullType>
    struct TLMaker
    {
    private:
        typedef TYPELIST_20(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
                            T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) TmpList;
    public:
        typedef typename EraseAll<TmpList, NullType>::Result Result;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //�ж�����T�Ƿ����ת��������U(�ο���wbl��, ֱ�Ӳ���loki, ����ʱ���о���
    //�ؼ�:����ܹ�ת��, �����U�ĺ���, Ҳ�ܹ�����T
    template<class T, class U>
    class Conversion
    {
    protected:
		typedef char YES;
		struct NO {char dummy[2];};
		struct any_conversion
		{
			template <typename P> any_conversion(const volatile P&);
			template <typename P> any_conversion(P&);
		};

		template <typename P> struct conversion_checker
		{
			static NO  _m_check(any_conversion ...);
			static YES _m_check(P, int);
		};	

		static T _m_from;
    public:
        enum
        {
            //�Ƿ����ת��(���Test(makeT())ƥ�䵽��static Small Test(U), �����ת��)
            exists      = (sizeof(conversion_checker<U>::_m_check(_m_from, 0)) == sizeof(YES)),
            //�Ƿ����˫��ת��
            exists2Way  = exists && Conversion<U, T>::exists,
            //�Ƿ���ͬ����
            sameType    = false
        };
    };

    //ƫ�ػ���ȷ��sameType
    template<class T>
    class Conversion<T, T>
    {
    public:
        enum
        {
            exists      = true,
            exists2Way  = true,
            sameType    = true
        };
    };

    //�ж��������Ƿ���Լ̳�
    //�ؼ�:����ָ�����ת���ɸ���ָ��, �Ҳ���void*����
    //��ͬ����, SUPERSUBCLASS�ж�Ϊtrue
    #define SUPERSUBCLASS(T, U) (TL::Conversion<const U*, const T*>::exists && !TL::Conversion<const T*, const void*>::sameType)
    //��ͬ����, SUPERSUBCLASS_STRICT�ж�Ϊfalse
    #define SUPERSUBCLASS_STRICT(T, U) (SUPERSUBCLASS(T, U) && !TL::Conversion<const T, const U>::sameType)

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // ����ѡ����
    template<bool flag, typename U, typename V>
    struct TypeSelect
    {
    	typedef U Result;
    };

    template<typename U, typename V>
    struct TypeSelect<false, U, V>
    {
    	typedef V Result;
    };

    ///////////////////////////////////////////////////////////////////////////////////////
    /**
     * ������ȡ��, copy��Loki��
     */
    template<typename T>
    class TypeTraits
    {
    private:

        ///////////////////////////////////////////////////////
        //��ȡ���õ�ԭʼ����(��ȥ����������)
        template<class U>
        struct ReferenceTraits
        {
            enum { result = false };
            typedef U Result;
        };

        template<class U>
        struct ReferenceTraits<U&>
        {
            enum { result = true };
            typedef U Result;
        };

    	///////////////////////////////////////////////////////
    	//ָ������
    	template<class U>
    	struct PointerTraits
    	{
    		enum { result = false };
    		typedef TL::NullType Result;
    	};

    	template<class U>
    	struct PointerTraits<U*>
    	{
    		enum { result = true };
    		typedef U Result;
    	};

    	template<class U>
    	struct PointerTraits<U*&>
    	{
    		enum { result = true };
    		typedef U Result;
    	};

    	///////////////////////////////////////////////////////
    	//��Ա����ָ��, gcc����֧��������, ����֮
    	template<typename U>
    	struct PointerToMemberTraits
    	{
    		enum { result = false };
    	};

    	template<class U, class V>
    	struct PointerToMemberTraits<U V::*>
    	{
    		enum { result = true };
    	};

    	template<class U, class V>
    	struct PointerToMemberTraits<U V::*&>
    	{
    		enum { result = true };
    	};

        ///////////////////////////////////////////////////////
        // const
        template<typename U>
        struct UnConstTraits
        {
            enum { result = false };
            typedef U Result;
        };
        template<typename U>
        struct UnConstTraits<const U>
        {
            enum { result = true };
            typedef U Result;
        };
        template<typename U>
        struct UnConstTraits<const U&>
        {
            enum { result = true };
            typedef U& Result;
        };

        ///////////////////////////////////////////////////////
        // volatile
        template<typename U>
        struct UnVolatileTraits
        {
            enum { result = false };
            typedef U Result;
        };
        template<typename U>
        struct UnVolatileTraits<volatile U>
        {
            enum { result = true };
            typedef U Result;
        };
        template<typename U>
        struct UnVolatileTraits<volatile U&>
        {
            enum { result = true };
            typedef U& Result;
        };
    public:
        //T�Ƿ���ָ������
    	enum { isPointer        = PointerTraits<T>::result };
        //T�Ƿ�����������
        enum { isReference 		= ReferenceTraits<T>::result };
        //T�Ƿ�ָ���Ա������ָ��
        enum { isMemberPointer 	= PointerToMemberTraits<T>::result };

        //T�Ƿ���Const����
        enum { isConst          = UnConstTraits<T>::result };
        //T�Ƿ���Volatile����
        enum { isVolatile       = UnVolatileTraits<T>::result };

        //���T��ָ������,���ȡT��ԭ����, ��ȥ��ָ������
    	typedef typename PointerTraits<T>::Result 		    PointeeType;
        //���T����������,���ȡT��ԭ����, ��ȥ����������
    	typedef typename ReferenceTraits<T>::Result         ReferencedType;
        //���T��Const����,���ȡT��ԭ����, ��ȥ��Const����
        typedef typename UnConstTraits<T>::Result           NonConstType;
        //���T��volatile����,���ȡT��ԭ����, ��ȥ��volatile����
        typedef typename UnVolatileTraits<T>::Result        NonVolatileType;
        //ȥ��const volatile����
        typedef typename UnVolatileTraits<typename UnConstTraits<T>::Result>::Result UnqualifiedType;

    public:

    	//////////////////////////////////////////////////////
    	//
    	typedef TL::TLMaker<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>::Result UnsignedInts;
    	typedef TL::TLMaker<signed char, short, int, long, long long>::Result SignedInts;
    	typedef TL::TLMaker<bool, char, wchar_t>::Result OtherInts;
    	typedef TL::TLMaker<float, double, long double>::Result Floats;
        typedef TL::TYPELIST_2(TL::EmptyType, TL::NullType) NullTypes;

        //�޷�������
    	enum { isStdUnsignedInt = TL::IndexOf<UnsignedInts, T>::value >= 0 };
        //�з�������
    	enum { isStdSignedInt 	= TL::IndexOf<SignedInts, T>::value >= 0 };
        //����
    	enum { isStdInt 		= isStdUnsignedInt || isStdSignedInt || TL::IndexOf<OtherInts, T>::value >= 0 };
        //��������
    	enum { isStdFloat	 	= TL::IndexOf<Floats, T>::value >= 0 };
        //��ֵ����
    	enum { isStdArith	 	= isStdInt || isStdFloat };
        //��������(����void)
    	enum { isStdFundamental	= isStdArith || TL::IndexOf<TL::TYPELIST_1(void), T>::value >= 0};
        //������
        enum { isNullType       = TL::IndexOf<NullTypes, T>::value >= 0 };
        //������
        enum { isBaseType       = isStdArith || isPointer || isMemberPointer };

    	//���ڸ�������, ��ȡ���ݵ���������, ��������������
    	typedef typename TypeSelect<isBaseType, T, ReferencedType&>::Result ReferenceType;

        //���ڸ��������ҷǿ�����, ��ȡ���ݵ���������, ��������������
        //typedef typename TypeSelect<isBaseType || isNullType, T, ReferencedType&>::Result ReferenceTypeEx;

        //��ȡ���ݵ�ԭ����, �������õ������������
    	typedef typename TypeSelect<!isReference, T, ReferencedType&>::Result ParameterType;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //�����ʹ�ÿ�ʼչʾTypeList������, �����Զ�����class

    //ɢ�ҵļ̳���ϵ
    template<class TList, template <class> class Unit>
    class ScatterHierarchy;

    /*
    namespace p
    {
        //ע��copy��loki��
        // The following type helps to overcome subtle flaw in the original
        // implementation of GenScatterHierarchy.
        // The flaw is revealed when the input type list of GenScatterHierarchy
        // contains more then taf element of the same type (e.g. LOKI_TYPELIST_2(int, int)).
        // In this case GenScatterHierarchy will contain multiple bases of the same
        // type and some of them will not be reachable (per 10.3).
        // For example before the fix the first element of Tuple<LOKI_TYPELIST_2(int, int)>
        // is not reachable in any way!
        template<class, class>
        struct ScatterHierarchyTag;
    }

    template<class T1, class T2, template <class> class Unit>
    class ScatterHierarchy<TypeList<T1, T2>, Unit> : public ScatterHierarchy<p::ScatterHierarchyTag<T1, T2>, Unit>
                                                   , public ScatterHierarchy<T2, Unit>
    {
    public:
        typedef TypeList<T1, T2> TList;
        typedef ScatterHierarchy<p::ScatterHierarchyTag<T1, T2>, Unit> LeftBase;
        typedef ScatterHierarchy<T2, Unit> RightBase;

        template<typename T> struct Rebind
        {
            typedef Unit<T> Result;
        };
    };

    // In the middle *unique* class that resolve possible ambiguity
    template <class T1, class T2, template <class> class Unit>
    class ScatterHierarchy<p::ScatterHierarchyTag<T1, T2>, Unit>
        : public ScatterHierarchy<T1, Unit>
    {
    };
    */

    //���ֻ��̳���ϵ
    template <class T1, class T2, template <class> class Unit>
    class ScatterHierarchy<TypeList<T1, T2>, Unit>
        : public ScatterHierarchy<T1, Unit>
        , public ScatterHierarchy<T2, Unit>
    {
    public:
        typedef TypeList<T1, T2> TList;
        typedef ScatterHierarchy<T1, Unit> LeftBase;
        typedef ScatterHierarchy<T2, Unit> RightBase;
        template <typename T> struct Rebind
        {
            typedef Unit<T> Result;
        };
    };

    template<class AtomicType, template <class> class Unit>
    class ScatterHierarchy : public Unit<AtomicType>
    {
    public:
        typedef Unit<AtomicType> LeftBase;

        template<typename T> struct Rebind
        {
            typedef Unit<T> Result;
        };
    };

    template<template <class> class Unit>
    class ScatterHierarchy<NullType, Unit>
    {
    public:
        template<typename T> struct Rebind
        {
            typedef Unit<T> Result;
        };
    };

    /////////////////////////////////////////////////////////////////////
    //�����̳���ϵ��, ��������ĺ�����ȡ�̳���ϵ��ĳ����

    template<class T, class H>
    struct FieldHelperByType
    {
        typedef typename H::template Rebind<T>::Result ResultType;
        static ResultType& dochange(H &obj)
        {
            return static_cast<ResultType&>(obj);
        }
    };
    template<class T, class H>
    struct FieldHelperByType<T, const H>
    {
        typedef const typename H::template Rebind<T>::Result ResultType;
        static ResultType& dochange(const H &obj)
        {
            return (ResultType&)obj;
        }
    };

    //ֱ�Ӱ������ͻ�ȡ, �����������ͬ������, ����벻��
    template<class T, class H>
    typename FieldHelperByType<T, H>::ResultType& field(H &obj)
    {
        return FieldHelperByType<T, H>::dochange(obj);
    }

    /////////////////////////////////////////////////////////////////////
    // ����������ȡ�ֶ�
    //����Tuple, Ĭ�ϵ����ݲ�����(Unit)
    template<typename T>
    struct TupleUnit
    {
        T _value;
        operator T&()               { return _value; }
        operator const T&() const   { return _value; }
    };

    template<class TList>
    struct Tuple : public ScatterHierarchy<TList, TupleUnit>
    {
    };

    //����FieldHelperByIndex
    template<class H, unsigned int i> struct FieldHelperByIndex;

    //�ػ��汾��FieldHelperByIndex, �Ƶ������һ��Ԫ��
    template<class H>
    struct FieldHelperByIndex<H, 0>
    {
        typedef typename H::TList::H ElementType;
        typedef typename H::template Rebind<ElementType>::Result UnitType;

        enum
        {
            isTuple = Conversion<UnitType, TupleUnit<ElementType> >::sameType,
            isConst = TypeTraits<H>::isConst
        };

        typedef const typename H::LeftBase ConstLeftBase;
        typedef typename TypeSelect<isConst, ConstLeftBase, typename H::LeftBase>::Result LeftBase;
        typedef typename TypeSelect<isTuple, ElementType, UnitType>::Result UnqualifiedResultType;
        typedef typename TypeSelect<isConst, const UnqualifiedResultType, UnqualifiedResultType>::Result ResultType;

        static ResultType &dochange(H& obj)
        {
            LeftBase &leftBase = obj;
            return (ResultType&)leftBase;
        }
    };

    //����������ȡFieldHelper
    template<class H, unsigned int i>
    struct FieldHelperByIndex
    {
        typedef typename TypeAt<typename H::TList, i>::Result ElementType;
        typedef typename H::template Rebind<ElementType>::Result UnitType;

        enum
        {
            isTuple = Conversion<UnitType, TupleUnit<ElementType> >::sameType,
            isConst = TypeTraits<H>::isConst
        };

        typedef const typename H::RightBase ConstRightBase;
        typedef typename TypeSelect<isConst, ConstRightBase, typename H::RightBase>::Result RightBase;
        typedef typename TypeSelect<isTuple, ElementType, UnitType>::Result UnqualifiedResultType;
        typedef typename TypeSelect<isConst, const UnqualifiedResultType, UnqualifiedResultType>::Result ResultType;

        static ResultType &dochange(H& obj)
        {
            RightBase &rightBase = obj;
            return FieldHelperByIndex<RightBase, i-1>::dochange(rightBase);
        }
    };

    //���尴��������ȡ
    template<unsigned int i, class H>
    typename FieldHelperByIndex<H, i>::ResultType &field(H& obj)
    {
        return FieldHelperByIndex<H, i>::dochange(obj);
    }
}

}
#endif

