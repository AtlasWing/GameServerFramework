#ifndef __TC_FUNCTOR_H
#define __TC_FUNCTOR_H

#include <memory>
#include <iostream>
#include "util/tc_loki.h"

using namespace std;

namespace taf
{
///////////////////////////////////////////////////////////////////////////////////////////////////
/** 
 * @file tc_functor.h
 * @brief ͨ�÷º�����
 
 * �ο�C++�����˼ά, loki������ 
 * Functor�Ե��õķ�װ,���Զ����¼��ֵ��÷�װ:
 * C���ĺ�������
 * C���ĺ���ָ��
 * C++���ĺ�������
 * ָ�����Ա������ָ�����
 * ��װ�Ժ�ĵ�����ʽ���Ա������:
 * A �º���������÷�ʽ, ���������ļ��ַ�ʽ���������Ҳ����һ��Բ����,���������ڲ���һ����ʵĲ���������
 * B ���������ù��̷�װһ����������, ��ʱ����, ���õ�ʱ���ô������, �����ڶ�����ʱ�ʹ����˲���
 * C��������:
 *  void TestFunction3(const string &s, int i)
 *
 *  {
 *
 *       cout << "TestFunction3('" << s << "', '" << i << "')" << endl;
 *
 *  }
 *
 * TC_Functor<void, TL::TLMaker<const string&, int>::Result> cmd3(TestFunction3);
 * string s3("s3");
 * cmd3(s3, 10);
 * C����������TC_FunctorWrapper��װ:*
 *  TC_FunctorWrapper<TC_Functor<void, TL::TLMaker<const string&, int>::Result> > fwrapper3(cmd3, s3, 10);
 *  fwrapper3();
 * ˵��:
 * void : �����ķ���ֵ
 * C++�����������:
 *  struct TestFunctor
 *  {
 *
 *      void operator()(const string &s, int i)
 *
 *      {
 *
 *          cout << "TestFunctor::operator(" << s << "," << i << ") called" << endl;
 *
 *      }
 *
 *  }
 *
 *  TestFunctor f;
 *
 *  TC_Functor<void, TL::TLMaker<const string&, int>::Result> cmd3(f);
 *
 *  string s3("s3");
 *
 *  cmd3(s3, 10);
 *
 * C++�������������TC_FunctorWrapper��װ:
 *
 *  TC_FunctorWrapper<TC_Functor<void, TL::TLMaker<const string&, int>::Result> > fwrapper3(cmd3, s3, 10);
 *
 *  fwrapper3();
 *
 * ָ�����Ա�����ĵ���
 *  struct TestMember
 * {
 *     void mem3(const string &s, int i)
 *     {
 *          cout << "TestMember::mem3(" << s << "," << i << ") called" << endl;
 *     }
 *  }
 *  TC_Functor<void, TL::TLMaker<const string&, int>::Result> cmd3(&tm, &TestMember::mem3);
 * cmd3("a", 33);
 * ָ�����Ա�����ĵ�����TC_FunctorWrapper��װ:
 *  TC_FunctorWrapper<TC_Functor<void, TL::TLMaker<const string&, int>::Result> > fwrapper3(cmd3, "a", 10);
 *  fwrapper3();
 * ע�����е�TC_FunctorWrapper�ĵ���, �ھ�����õ�ʱ���ǲ���������, �����ڹ����ʱ���Ѿ�����ȥ��
 ///////////////////////////////////////////////////////////////////////////////////////////////////
 *
 * ����C���ĺ�������, ͬ���Ƶĺ���ͨ��ֻ��һ��, ����ж����ͬ���Ƶĺ���
 *��ͬ�������͵ĺ���, ���ڹ���TC_Functor��ʱ����Ҫǿ������ת��, ����:
 *  void TestFunction1()
 *  {
 *      cout << "TestFunction1()" << endl;
 * }
 *  void TestFunction1(char c)
 *  {
 *      cout << "TestFunction1(" << c << ")" << endl;
 * }
 * typedef void (*TpFunc)();
 * TC_Functor<void> cmd1(static_cast<TpFunc>(TestFunction1));
 * cmd1();
 * typedef void (*TpFunc1)(char);
 * TC_Functor<void, TL::TLMaker<char>::Result>
 * cmd11(static_cast<TpFunc1>(TestFunction1)); cmd11('c');
 ///////////////////////////////////////////////////////////////////////////////////////////////////
 *
 * �������Ա������ָ�����, ͬ���Ƶĺ���ͨ��ֻ��һ��, ����ж����ͬ���Ƶĺ���
 * ��ͬ�������͵ĺ���, ���ڹ���TC_Functor��ʱ����Ҫǿ������ת��, ����:
 *  class TestMember
 *  {
 *  public:
 *      void mem1()
 *      {
 *          cout << "TestMember::mem1() called" << endl;
 *      }
 *      void mem1(char c)
 *      {
 *          cout << "TestMember::mem1('" << c << "') called" << endl;
 *      }
 *  };
 
 *  TestMember tm;
 
 *  typedef void (TestMember::*TpMem)();
 
 *  typedef void (TestMember::*TpMem1)(char);
 
 *  TC_Functor<void> cmd1(&tm, static_cast<TpMem>(&TestMember::mem1));
 *  cmd1();
 *
 *  typedef void (TestMember::*TpMem1)(char);
 
 * TC_Functor<void, TL::TLMaker<char>::Result> cmd11(&tm,
 
 * static_cast<TpMem1>(&TestMember::mem1)); cmd11('c');
 *
 * ����ʾ����μ�test/test_tc_functor.cpp.
 
 * @author jarodruan@tencent.com 
 */

template<typename R, class TList>
class TC_FunctorImp
{
public:
    virtual R operator()() = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R>
class TC_FunctorImp<R, TL::EmptyType>
{
public:
    virtual R operator()() = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1>
class TC_FunctorImp<R, TL::TYPELIST_1(P1)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2>
class TC_FunctorImp<R, TL::TYPELIST_2(P1, P2)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3>
class TC_FunctorImp<R, TL::TYPELIST_3(P1, P2, P3)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4>
class TC_FunctorImp<R, TL::TYPELIST_4(P1, P2, P3, P4)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
class TC_FunctorImp<R, TL::TYPELIST_5(P1, P2, P3, P4, P5)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
class TC_FunctorImp<R, TL::TYPELIST_6(P1, P2, P3, P4, P5, P6)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5,
                         typename TL::TypeTraits<P6>::ReferenceType p6) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
class TC_FunctorImp<R, TL::TYPELIST_7(P1, P2, P3, P4, P5, P6, P7)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5,
                         typename TL::TypeTraits<P6>::ReferenceType p6,
                         typename TL::TypeTraits<P7>::ReferenceType p7) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
class TC_FunctorImp<R, TL::TYPELIST_8(P1, P2, P3, P4, P5, P6, P7, P8)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5,
                         typename TL::TypeTraits<P6>::ReferenceType p6,
                         typename TL::TypeTraits<P7>::ReferenceType p7,
                         typename TL::TypeTraits<P8>::ReferenceType p8) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
class TC_FunctorImp<R, TL::TYPELIST_9(P1, P2, P3, P4, P5, P6, P7, P8, P9)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5,
                         typename TL::TypeTraits<P6>::ReferenceType p6,
                         typename TL::TypeTraits<P7>::ReferenceType p7,
                         typename TL::TypeTraits<P8>::ReferenceType p8,
                         typename TL::TypeTraits<P9>::ReferenceType p9) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
class TC_FunctorImp<R, TL::TYPELIST_10(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5,
                         typename TL::TypeTraits<P6>::ReferenceType p6,
                         typename TL::TypeTraits<P7>::ReferenceType p7,
                         typename TL::TypeTraits<P8>::ReferenceType p8,
                         typename TL::TypeTraits<P9>::ReferenceType p9,
                         typename TL::TypeTraits<P10>::ReferenceType p10) = 0;
    virtual TC_FunctorImp* clone() const = 0;
    virtual ~TC_FunctorImp() {};
};

/**
 * ��װ�Ժ��������Լ������ĵ���
 */
template<class ParentFunctor, typename Fun>
class TC_FunctorHandler : public TC_FunctorImp<typename ParentFunctor::ResultType, typename ParentFunctor::ParamList>
{
public:
    typedef typename ParentFunctor::ResultType ResultType;

    TC_FunctorHandler(Fun fun) : _fun(fun)
    {

    }

    TC_FunctorHandler *clone() const
    {
        return new TC_FunctorHandler(*this);
    }

    ResultType operator()()
    { return _fun(); }

    ResultType operator()(typename ParentFunctor::Reference1 p1)
	{ return _fun(p1); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2)
	{ return _fun(p1, p2); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3)
	{ return _fun(p1, p2, p3); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4)
	{ return _fun(p1, p2, p3, p4); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5)
	{ return _fun(p1, p2, p3, p4, p5); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5,
                        typename ParentFunctor::Reference6 p6)
	{ return _fun(p1, p2, p3, p4, p5, p6); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5,
                        typename ParentFunctor::Reference6 p6,
                        typename ParentFunctor::Reference7 p7)
	{ return _fun(p1, p2, p3, p4, p5, p6, p7); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5,
						typename ParentFunctor::Reference6 p6,
						typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8)
	{ return _fun(p1, p2, p3, p4, p5, p6, p7, p8); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5,
						typename ParentFunctor::Reference6 p6,
						typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8,
                        typename ParentFunctor::Reference9 p9)
	{ return _fun(p1, p2, p3, p4, p5, p6, p7, p8, p9); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5,
						typename ParentFunctor::Reference6 p6,
						typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8,
                        typename ParentFunctor::Reference9 p9,
                        typename ParentFunctor::Reference10 p10)
	{ return _fun(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }

private:
    Fun _fun;
};

template<class ParentFunctor, typename PointerToObj, typename PointerToMemFun>
/**
 * @brief ��װ�Գ�Ա�����ĵ���
 */
class TC_MemFunHandler
	: public TC_FunctorImp<typename ParentFunctor::ResultType, typename ParentFunctor::ParamList>
{
public:
	typedef typename ParentFunctor::ResultType ResultType;

	TC_MemFunHandler(const PointerToObj &pObj, PointerToMemFun pMemFn)
		: _pObj(pObj), _pMemFn(pMemFn)
	{
	}

	TC_MemFunHandler* clone() const
	{
		return new TC_MemFunHandler(*this);
	}

	ResultType operator()()
	{ return (_pObj->*_pMemFn)(); }

	ResultType operator()(typename ParentFunctor::Reference1 p1)
	{ return (_pObj->*_pMemFn)(p1); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2)
	{ return (_pObj->*_pMemFn)(p1, p2); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3)
	{ return (_pObj->*_pMemFn)(p1, p2, p3); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4)
	{ return (_pObj->*_pMemFn)(p1, p2, p3, p4); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5)
	{ return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5,
						typename ParentFunctor::Reference6 p6)
	{ return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5, p6); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5,
						typename ParentFunctor::Reference6 p6,
						typename ParentFunctor::Reference7 p7)
	{ return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5, p6, p7); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5,
						typename ParentFunctor::Reference6 p6,
						typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8)
	{ return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5, p6, p7, p8); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5,
						typename ParentFunctor::Reference6 p6,
						typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8,
                        typename ParentFunctor::Reference9 p9)
	{ return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5, p6, p7, p8, p9); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5,
						typename ParentFunctor::Reference6 p6,
						typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8,
                        typename ParentFunctor::Reference9 p9,
                        typename ParentFunctor::Reference10 p10)
	{ return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }

private:
	PointerToObj	_pObj;
	PointerToMemFun _pMemFn;
};

template<class ParentFunctor>
class TC_FunctorWrapper;


template<typename R, class TList = TL::NullType>
/**
 * @brief ����������, ���Է�װ��: ��������, ����, ��Ա���� 
 *  	  �ĵ���
 */
class TC_Functor
{
public:
    typedef R     ResultType;
    typedef TList ParamList;

    /**�����װ����*/
    typedef TC_FunctorWrapper<TC_Functor<R, TList> > wrapper_type;

    /**���������б���ÿ��������ԭ����*/
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 0, TL::EmptyType>::Result>::ParameterType Param1;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 1, TL::EmptyType>::Result>::ParameterType Param2;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 2, TL::EmptyType>::Result>::ParameterType Param3;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 3, TL::EmptyType>::Result>::ParameterType Param4;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 4, TL::EmptyType>::Result>::ParameterType Param5;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 5, TL::EmptyType>::Result>::ParameterType Param6;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 6, TL::EmptyType>::Result>::ParameterType Param7;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 7, TL::EmptyType>::Result>::ParameterType Param8;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 8, TL::EmptyType>::Result>::ParameterType Param9;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 9, TL::EmptyType>::Result>::ParameterType Param10;

    /**���������б���ÿ����������������*/
    typedef typename TL::TypeTraits<Param1>::ReferenceType Reference1;
    typedef typename TL::TypeTraits<Param2>::ReferenceType Reference2;
    typedef typename TL::TypeTraits<Param3>::ReferenceType Reference3;
    typedef typename TL::TypeTraits<Param4>::ReferenceType Reference4;
    typedef typename TL::TypeTraits<Param5>::ReferenceType Reference5;
    typedef typename TL::TypeTraits<Param6>::ReferenceType Reference6;
    typedef typename TL::TypeTraits<Param7>::ReferenceType Reference7;
    typedef typename TL::TypeTraits<Param8>::ReferenceType Reference8;
    typedef typename TL::TypeTraits<Param9>::ReferenceType Reference9;
    typedef typename TL::TypeTraits<Param10>::ReferenceType Reference10;

public:
    TC_Functor()
    {
    }

    TC_Functor(const TC_Functor &functor)
    : _spImpl(functor._spImpl->clone())
    {
    }

    TC_Functor& operator=(const TC_Functor &functor)
    {
        if(this != &functor)
        {
            _spImpl = std::auto_ptr<Impl>(functor._spImpl->clone());
        }

        return (*this);
    }

    template<class Fun>
    TC_Functor(Fun fun)
	: _spImpl(new TC_FunctorHandler<TC_Functor, Fun>(fun))
    {
    }

	template<typename PointerToObj, typename PointerToMemFun>
	TC_Functor(const PointerToObj &pObj, PointerToMemFun pMemFn)
	: _spImpl(new TC_MemFunHandler<TC_Functor, PointerToObj, PointerToMemFun>(pObj, pMemFn))
	{
	}

	// �ж��Ƿ��Ѿ�������functor
	// ��������ʹ��if(functor)
	operator bool() const
	{
		return _spImpl.get() ? true : false;
	}

    R operator()()
	{
		return (*_spImpl)();
	}

    R operator()(Reference1 p1)
	{
		return (*_spImpl)(p1);
	}

    R operator()(Reference1 p1, Reference2 p2)
	{
		return (*_spImpl)(p1, p2);
	}

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3)
	{
		return (*_spImpl)(p1, p2, p3);
	}

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4)
	{
		return (*_spImpl)(p1, p2, p3, p4);
	}

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5)
	{
		return (*_spImpl)(p1, p2, p3, p4, p5);
	}

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5, Reference6 p6)
	{
		return (*_spImpl)(p1, p2, p3, p4, p5, p6);
	}

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5, Reference6 p6, Reference7 p7)
	{
		return (*_spImpl)(p1, p2, p3, p4, p5, p6, p7);
	}

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5, Reference6 p6, Reference7 p7, Reference8 p8)
	{
		return (*_spImpl)(p1, p2, p3, p4, p5, p6, p7, p8);
	}

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5, Reference6 p6, Reference7 p7, Reference8 p8, Reference9 p9)
	{
		return (*_spImpl)(p1, p2, p3, p4, p5, p6, p7, p8, p9);
	}

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5, Reference6 p6, Reference7 p7, Reference8 p8, Reference9 p9, Reference10 p10)
	{
		return (*_spImpl)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
	}


private:
    template<class ParentFunctor>
    friend class TC_FunctorWrapper;

    typedef TC_FunctorImp<R, TList> Impl;

    std::auto_ptr<Impl>     _spImpl;
};

/**
 * @brief wapper����
 */
class TC_FunctorWrapperInterface
{
public:
    virtual void operator()() = 0;
    virtual ~TC_FunctorWrapperInterface(){}
};

template<class ParentFunctor>

/**
 * @brief ��TC_Functor���з�װ, ������TC_Functor�����º󱻵���
 */
class TC_FunctorWrapper : public TC_FunctorWrapperInterface
{
public:
    typedef typename ParentFunctor::ResultType R;
    typedef typename ParentFunctor::ParamList  TList;

    /**
     * @brief ��������
     * @param fw
     */
    TC_FunctorWrapper(const TC_FunctorWrapper &fw)
    : _spImpl(fw._spImpl->clone())
    , _p1(fw._p1)
    , _p2(fw._p2)
    , _p3(fw._p3)
    , _p4(fw._p4)
    , _p5(fw._p5)
    , _p6(fw._p6)
    , _p7(fw._p7)
    , _p8(fw._p8)
    , _p9(fw._p9)
    , _p10(fw._p10)
    {
    }

    /**
     * @brief ���캯��
     * @param tf
     */
    TC_FunctorWrapper(ParentFunctor &tf)
    : _spImpl(tf._spImpl->clone())
    {
    }

    /**
     * @brief һ���������õĹ��캯��
     * @param tf
     * @param p1
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                  typename ParentFunctor::Reference1 p1)
    : _spImpl(tf._spImpl->clone()), _p1(p1)
    {
    }

    /**
     * @brief �����������õĹ��캯��
     * @param tf
     * @param p1
     * @param p2
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                  typename ParentFunctor::Reference1 p1,
                  typename ParentFunctor::Reference2 p2)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2)
    {
    }

    /**
     * @brief �����������õĹ��캯��
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                  typename ParentFunctor::Reference1 p1,
                  typename ParentFunctor::Reference2 p2,
                  typename ParentFunctor::Reference3 p3)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3)
    {
    }

    /**
     * @brief �ĸ��������õĹ��캯��
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4)
    {
    }

    /**
     * @brief ����������õĹ��캯��
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5)
    {
    }

    /**
     * @brief �����������õĹ��캯��
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5,
                                            typename ParentFunctor::Reference6 p6)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5), _p6(p6)
    {
    }

    /**
     * @brief �߸��������õĹ��캯��
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     * @param p7
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5,
                                            typename ParentFunctor::Reference6 p6,
                                            typename ParentFunctor::Reference7 p7)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5), _p6(p6), _p7(p7)
    {
    }

    /**
     * @brief �˸��������õĹ��캯��
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     * @param p7
     * @param p8
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5,
                                            typename ParentFunctor::Reference6 p6,
                                            typename ParentFunctor::Reference7 p7,
                                            typename ParentFunctor::Reference8 p8)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5), _p6(p6), _p7(p7), _p8(p8)
    {
    }

    /**
     * @brief �Ÿ��������õĹ��캯��
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     * @param p7
     * @param p8
     * @param p9
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5,
                                            typename ParentFunctor::Reference6 p6,
                                            typename ParentFunctor::Reference7 p7,
                                            typename ParentFunctor::Reference8 p8,
                                            typename ParentFunctor::Reference9 p9)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5), _p6(p6), _p7(p7), _p8(p8), _p9(p9)
    {
    }

    /**
     * @brief ʮ���������õĹ��캯��
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     * @param p7
     * @param p8
     * @param p9
     * @param p10
     */
    TC_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5,
                                            typename ParentFunctor::Reference6 p6,
                                            typename ParentFunctor::Reference7 p7,
                                            typename ParentFunctor::Reference8 p8,
                                            typename ParentFunctor::Reference9 p9,
                                            typename ParentFunctor::Reference10 p10)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5), _p6(p6), _p7(p7), _p8(p8), _p9(p9), _p10(p10)
    {
    }

    /**
     * @brief �������
     * @param tf
     * @param p1
     */
	virtual R operator()()
	{
		return todo(TL::Int2Type<TL::Length<TList>::value>());
	}

	virtual ~TC_FunctorWrapper(){ }

protected:
    /**
     * @brief ��ֵ����������, ֻ����
     * @param fw
     *
     * @return TC_FunctorWrapper&
     */
    TC_FunctorWrapper& operator=(const TC_FunctorWrapper &fw);

	R todo(TL::Int2Type<0>)
    {
		return (*_spImpl)();
	}

	R todo(TL::Int2Type<1>)
	{
        return (*_spImpl)(_p1);
	}

	R todo(TL::Int2Type<2>)
	{
        return (*_spImpl)(_p1, _p2);
	}

	R todo(TL::Int2Type<3>)
	{
        return (*_spImpl)(_p1, _p2, _p3);
	}

	R todo(TL::Int2Type<4>)
	{
        return (*_spImpl)(_p1, _p2, _p3, _p4);
	}

	R todo(TL::Int2Type<5>)
	{
        return (*_spImpl)(_p1, _p2, _p3, _p4, _p5);
	}

	R todo(TL::Int2Type<6>)
	{
        return (*_spImpl)(_p1, _p2, _p3, _p4, _p5, _p6);
	}

	R todo(TL::Int2Type<7>)
	{
        return (*_spImpl)(_p1, _p2, _p3, _p4, _p5, _p6, _p7);
	}

	R todo(TL::Int2Type<8>)
	{
        return (*_spImpl)(_p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8);
	}

	R todo(TL::Int2Type<9>)
	{
        return (*_spImpl)(_p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8, _p9);
	}

	R todo(TL::Int2Type<10>)
	{
		return (*_spImpl)(_p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8, _p9, _p10);
	}

protected:
    typedef TC_FunctorImp<R, TList> Impl;

    std::auto_ptr<Impl>             _spImpl;

    /*
    typename TL::TypeTraits<typename ParentFunctor::Param1>::ReferenceTypeEx   _p1;
    typename TL::TypeTraits<typename ParentFunctor::Param2>::ReferenceTypeEx   _p2;
    typename TL::TypeTraits<typename ParentFunctor::Param3>::ReferenceTypeEx   _p3;
    typename TL::TypeTraits<typename ParentFunctor::Param4>::ReferenceTypeEx   _p4;
    typename TL::TypeTraits<typename ParentFunctor::Param5>::ReferenceTypeEx   _p5;
    typename TL::TypeTraits<typename ParentFunctor::Param6>::ReferenceTypeEx   _p6;
    typename TL::TypeTraits<typename ParentFunctor::Param7>::ReferenceTypeEx   _p7;
    typename TL::TypeTraits<typename ParentFunctor::Param8>::ReferenceTypeEx   _p8;
    typename TL::TypeTraits<typename ParentFunctor::Param9>::ReferenceTypeEx   _p9;
    typename TL::TypeTraits<typename ParentFunctor::Param10>::ReferenceTypeEx  _p10;
    */

    typename TL::TypeTraits<typename ParentFunctor::Param1>::ParameterType   _p1;
    typename TL::TypeTraits<typename ParentFunctor::Param2>::ParameterType   _p2;
    typename TL::TypeTraits<typename ParentFunctor::Param3>::ParameterType   _p3;
    typename TL::TypeTraits<typename ParentFunctor::Param4>::ParameterType   _p4;
    typename TL::TypeTraits<typename ParentFunctor::Param5>::ParameterType   _p5;
    typename TL::TypeTraits<typename ParentFunctor::Param6>::ParameterType   _p6;
    typename TL::TypeTraits<typename ParentFunctor::Param7>::ParameterType   _p7;
    typename TL::TypeTraits<typename ParentFunctor::Param8>::ParameterType   _p8;
    typename TL::TypeTraits<typename ParentFunctor::Param9>::ParameterType   _p9;
    typename TL::TypeTraits<typename ParentFunctor::Param10>::ParameterType  _p10;
};

}

#endif

