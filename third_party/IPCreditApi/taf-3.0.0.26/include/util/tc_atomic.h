#ifndef __TC_ATOMIC_H
#define __TC_ATOMIC_H

#include <stdint.h>

namespace taf
{

#include "util/atomic_asm.h"

/////////////////////////////////////////////////
/** 
 * @file  tc_atomic.h 
 * @brief  ԭ�Ӽ�����. 
 *  
 * @author  jarodruan@tencent.com 
 */           
/////////////////////////////////////////////////

/**
 * @brief ԭ�Ӳ�����,��int��ԭ�Ӳ���
 */
class TC_Atomic
{
public:

    /**
     * ԭ������
     */
    typedef int atomic_type;

    /**
	 * @brief ���캯��,��ʼ��Ϊ0 
     */
    TC_Atomic(atomic_type at = 0)
    {
        set(at);
    }

    TC_Atomic& operator++()
    {
        inc();
        return *this;
    }

    TC_Atomic& operator--()
    {
        dec();
        return *this;
    }

    operator atomic_type() const
    {
        return get();
    }

    TC_Atomic& operator+=(atomic_type n)
    {
        add(n);
        return *this;
    }

    TC_Atomic& operator-=(atomic_type n)
    {
        sub(n);
        return *this;
    }

    TC_Atomic& operator=(atomic_type n)
    {
        set(n);
        return *this;
    }

    /**
     * @brief ��ȡֵ
     *
     * @return int
     */
    atomic_type get() const         { return taf_atomic_read(&_value); }

    /**
     * @brief ���
     * @param i
     *
     * @return int
     */
    atomic_type add(atomic_type i)  { return taf_atomic_add_return(i, &_value); }

    /**
     * @brief ����
     * @param i
     *
     * @return int
     */
    atomic_type sub(atomic_type i)  { return taf_atomic_sub_return(i, &_value); }

    /**
     * @brief �Լ�1
     *
     * @return int
     */
    atomic_type inc()               { return add(1); }

    /**
     * @brief �Լ�1
     */
    atomic_type dec()               { return sub(1); }

    /**
     * @brief �Լ�1
     *
     * @return void
     */
    void inc_fast()               { return taf_atomic_inc(&_value); }

    /**
     * @brief �Լ�1
     * Atomically decrements @_value by 1 and returns true if the
     * result is 0, or false for all other
     */
    bool dec_and_test()               { return taf_atomic_dec_and_test(&_value); }

    /**
     * @brief ����ֵ
     */
    atomic_type set(atomic_type i)  { taf_atomic_set(&_value, i); return i; }

protected:

    /**
     * ֵ
     */
    taf_atomic_t    _value;
};

}

#endif
