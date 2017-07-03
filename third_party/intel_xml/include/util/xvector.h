/*
 
 *---------------------------------------------------------------------------
 *
 *     Copyright 2007~2008 Intel Corporation All Rights Reserved.
 *     The source code contained or described herein and all documents 
 *     related to the source code ("Material") are owned by Intel 
 *     Corporation or its suppliers or licensors. Title to the Material
 *     remains with Intel Corporation or its suppliers and licensors. 
 *     The Material may contain trade secrets and proprietary and 
 *     confidential information of Intel Corporation and its suppliers
 *     and licensors, and is protected by worldwide copyright and trade
 *     secret laws and treaty provisions. No part of the Material may 
 *     be used, copied, reproduced, modified, published, uploaded, 
 *     posted, transmitted, distributed, or disclosed in any way without
 *     Intel's prior express written permission. 
 *
 *     No license under any patent, copyright, trade secret or other 
 *     intellectual property right is granted to or conferred upon you 
 *     by disclosure or delivery of the Materials, either expressly, by
 *     implication, inducement, estoppel or otherwise. Any license under
 *     such intellectual property rights must be express and approved by 
 *     Intel in writing.
 *
 *---------------------------------------------------------------------------

*/

/** @file xvector.h The %Vector interface
 *
 * A vector for storing a variable number of elements of some
 * type. 
 *
 */

#ifndef __COM_INTEL_XML_UTIL_VECTOR_H__
#define __COM_INTEL_XML_UTIL_VECTOR_H__

#include "../common/constants.h"
#include <stdlib.h>
#include <new>

INTEL_XML_UTIL_NAMESPACE_BEGIN

/**  
 * Wraps XSLT and XPath variable-size element array.
 *
 * XVector is a C++ container similar to @c std::vector. 
 * It is used by Intel(R) XPath Accelerator and Intel(R) XSLT Accelerator
 * to pass info between user code and XML processing engine.
 * */
template <class T>
class XVector {
public:
    
    /** Default constructor.  */
    XVector()
        : _data(NULL), _size(0), _used(0) {}

    /** 
     * Copy constructor.
     *
     * @param copy - the vector to be copied
     * */
    XVector(const XVector& copy)
        : _data(NULL), _size(0), _used(0) {
        if (copy._used>_size) reserve(copy._used);
        _used=copy._used;
        for (unsigned int i=0; i<_used; i++)
            _data[i]=copy[i];
    }

    /** Destructor.  */
    virtual ~XVector(){
        for (unsigned int i=0; i<_size; i++)
            _data[i].~T();
        if (_data) free(_data);
    }

    /** 
     * The assignment operator.
     *
     * @param copy - the reference of another XVector object
     *               that is assigned to this object
     * @return  This vector object.
     */
    const XVector& operator=(const XVector& copy){
        for (unsigned int i=0; i<_size; i++)
            _data[i].~T();
        if (_data) free(_data);
        _size = _used = copy.size();
        if (_size>0) {
            _data = static_cast<T*>(malloc(sizeof(T)*_size));
            // _size same as _used
            for (unsigned int i=0; i<_used; i++) {
                new (&_data[i]) T();
                _data[i]=copy[i];
            }
        } else {
            _data=NULL;
        }
        return *this;
    }

    /** 
     * Tests the equality of two vectors.
     *
     * @param rhs - the other XVector object
     *
     * @return TRUE if two vectors are equal; otherwise,
     *           FALSE.
     */
    bool operator==(const XVector<T>& rhs) {
        if (size()!=rhs.size())
            return false;
        for(unsigned int i=0; i<size(); i++) {
            if (_data[i]!=rhs[i]) return false;
        }
        return true;
    }

    /** 
     * Increases the capacity of the vector so that it can
     * contain at lease the number of elements specified in @e size.
     *
     * @param size - the size to increase
     */
    void reserve(unsigned int size){
        size=8>size?8:size;
        if (_size>size) return;
        T* olddata=_data;
        _data = static_cast<T*>(malloc(sizeof(T)*size));
        for (unsigned int i=0; i<size; i++) {
            new (&_data[i]) T();
            if (i<_used) _data[i]=olddata[i];
            if (i<_size) olddata[i].~T();
        }
        if (olddata) free(olddata);
        _size=size;
    }

    /** 
     * Adds an element to the end of the vector.
     *
     * @param data - one element of the T type
     */
    void push_back(const T& data){
        if (_used+1>_size) reserve(_size*2);
        _data[_used]=data;
        _used++;
    }

    /** 
     * Removes the last element in the vector.
     */
    void pop_back(){
        _used--;
    }

    /** 
     * Removes all the elements in the vector.
     */
    void clear(){
        _used=0;
    }

    /** 
     * Gets the element at position specified in @e pos (starting from 0). 
     * 
     * @descr @n @note In this call, bounds-checking is not performed.
     *
     * @param pos - the position of the element
     *
     * @return The reference of the element at the specified position.
     */
    T& operator[](unsigned int pos) {
        return _data[pos];
    }

    /** 
     * Gets the element at position specified in @e pos (starting from 0).
     * 
     * @descr @n @note In this call, bounds-checking is not performed.
     *
     * @param pos - the position of the element
     *
     * @return The reference of the element at the specified position.
     */
    const T& operator[](unsigned int pos) const {
        return _data[pos];
    }

    /** 
     * Tests whether the vector is empty.
     *
     * @return TRUE if the vector has no elements; otherwise,
     *           FALSE.
     */
    bool empty() const {return _used==0;}

    /** 
     * Gets a counter of the number of elements in the vector.
     *
     * @return The number of elements in the vector.
     */
    unsigned int size() const {return _used;}

    /** 
     * Gets the current capacity, that is the number of elements
     * that the array can hold before it needs to grow again.
     *
     * @return The capacity of the array.
     */
    unsigned int capacity() const { return _size; }  
           
protected:
    T* _data;
    unsigned int _size;
    unsigned int _used;    
};


INTEL_XML_UTIL_NAMESPACE_END

#endif
