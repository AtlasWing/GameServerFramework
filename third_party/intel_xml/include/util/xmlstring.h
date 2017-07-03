/*
 
 *---------------------------------------------------------------------------
 *
 *     Copyright 2007 Intel Corporation All Rights Reserved.
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

/** @file xmlstring.h The %String interface
 *
 * The XMLString class is the primary datatype for
 * representing native character strings and handling common string operations.
 *
 */

#ifndef __COM_INTEL_XML_UTIL_XMLSTRING_H__
#define __COM_INTEL_XML_UTIL_XMLSTRING_H__

#include "include/common/types.h"
#include "include/common/defines.h"

INTEL_XML_UTIL_NAMESPACE_BEGIN

/**
 * Contains a sequence of ixml::XMLChar and its length.
 *
 * This class implements different kinds of constructors and inter-operators between
 * XMLString and ixml::XMLChar. 
 * 
 * @note When copying an XMLString object, the underlying buffer is also copied. 
 */

class UTIL_EXPORT XMLString
{
public:
    /** 
     * Default constructor 
     */
    XMLString();

    /**
     * Destructor
     */
    virtual    ~XMLString();

    /** 
     * Character constructor
     *
     * @param data - the pointer to a sequence of ixml::XMLChar
     *               ending with @c \\0
     *
     */
    XMLString(const ixml::XMLChar* data);    

    /** 
     *
     * Char constructor with length
     *
     * @param data   - the pointer to a sequence of ixml::XMLChar, 
     *                 ending with @c \\0
     * @param length - the length of the ixml::XMLChar sequence
     */
    XMLString(const ixml::XMLChar* data, int length);

    /** 
     *
     * Copy constructor 
     *
     * @param other - the reference of the XMLString object
     *                that needs to be copied
     * 
     */
    XMLString(const XMLString& other);

    /** 
     * Assigns the operator.
     *
     * @param other - the reference of another XMLString object
     *                that is assigned to this object
     *
     */
    XMLString& operator = (const XMLString& other);

    /** 
     *
     * Appends the operator with character data.
     *
     * @param data - the pointer of a sequence of ixml::XMLChar which should be appended, 
     * the end of this sequence should be '0x00'.
     */
    XMLString& operator += (const ixml::XMLChar* data);
    
    /** 
     *
     * Appends the operator with the XMLString object.
     *
     * @param other - the reference of another XMLString object
     *                that needs to be appended
     *
     */
    XMLString& operator += (const XMLString& other);

    /*
     * Append a sequence of ixml::XMLChar to the end of this String.
     * 
     * @param data - the pointer to a sequence of ixml::XMLChar     
     *               ending with @c \\0
     */
    XMLString operator+(const ixml::XMLChar* data) const;

    /*
     * Append a string to the end of this String.
     * 
     * @param data -  the reference of a XMLString object
     *                that needs to be appended
     */
    XMLString operator+(const XMLString& data) const;
            
    /** 
     *
     * Appends a sequence of ixml::XMLChar.
     *
     * @param data - the pointer to a sequence of ixml::XMLChar     
     *               ending with @c \\0
     *
     */
    void append(const ixml::XMLChar* data);
    
    /**
     * Gets the size of this string.
     * 
     * @return The number of characters contained in the string.
     * 
     */
    int size() const    {return _size;}

    /**
     * Gets the pointer to the character sequence that this string contains.
     * 
     * @return The pointer to the first character in the sequence.
     * 
     */
    const ixml::XMLChar* c_str() const {return _data;}

    /**
     * Empties this string.
     * 
     * @return TRUE for a successful operation; otherwise,
     *         FALSE.
     * 
     */
    bool empty() const {return _size==0;}

protected:
    /**
     *
     * The pointer to the sequence of ixml::XMLChar that this object holds.
     */
    ixml::XMLChar*    _data;

    /**
     *
     * The length of the sequence of ixml::XMLChar that this object holds.
     */
    
    int             _size;

    /**
     *
     * The actual memory allocated for ixml::XMLChar sequence of this object.
     * This value is always the minimal multiplier of 1024 that is larger than 
     * _size. 
     *
     */    
    int            _maxSize;
};

INTEL_XML_UTIL_NAMESPACE_END

#endif

