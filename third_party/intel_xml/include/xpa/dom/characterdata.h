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

/** @file characterdata.h @brief Definition of character data in DOM 
 *
 * This file defines the methods and attributes for accessing character data in
 * DOM. This set is defined in one structure rather than in each object that
 * uses these attributes and methods. 
 *
 * As defined by the <a href='http://www.w3.org/TR/2000/REC-DOM-Level-2-Core-20001113'>
 * Document Object Model (DOM) Level 2 Core Specification</a>, text strings in
 * DOM are represented in UTF-8, that is, as sequences of 8-bit units. In this document,
 * the term @e 8-bit units is used to indicate that indexing on CharacterData
 * is done in 8-bit units.
 */



#ifndef __COM_INTEL_XML_PARSE_DOM_CHARACTERDATA_H__
#define __COM_INTEL_XML_PARSE_DOM_CHARACTERDATA_H__

#include "include/xpa/dom/node.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

/**
 * Extends Node with a set of
 * attributes and methods for accessing character data. No DOM objects correspond
 * directly to CharacterData, though Text and others
 * inherit the interface from it. All @c offsets in this interface
 * start with @c 0.
 *
 * Since DOM Level 1
 */
class DOM_EXPORT CharacterData : public Node
{
public:
    /** 
     * Destructor 
     */
    virtual ~CharacterData() {}

public:    
    /**
     * Returns the character data of the node that implements this interface.
     *
     * @descr @n The DOM implementation can set limit to the amount of data that can be
     * stored in a CharacterData node. As a result, some data might
     * not fit into a single @c XMLChar* String object. To resolve this, call
     * substringData() to retrieve the data in appropriately sized pieces.
     * 
     * Since DOM Level 1
     *
     * @return In case of an error, produces DOMException:
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised when the node is read-only.
     */
     virtual const ixml::XMLChar* getData() const = 0;

    /**
     * Returns the number of characters that are available through getData() methods.
     * @descr @n Since DOM Level 1
     *
     * @return The character data length or 0 when the CharacterData node
     * is empty.
     */
    virtual unsigned int getLength() const = 0;

    /**
     * Extracts the range of data from the node. 
     * 
     * @descr @n If the sum of @e offset and @e count exceeds the sum of 8-bit
     * units this node contained, all characters to the end of the data are returned.
     * 
     * Since DOM Level 1
     *
     * @param offset - the start offset of the substring to extract
     * @param count  - the number of characters to extract
     * 
     * @return The specified substring. In case of an error, produces DOMException:
     * - @c INDEX_SIZE_ERR: Raised if the specified offset is negative or
     *         greater than the number of characters in @c data,
     *         or if the specified @e count is negative.
     */
    virtual iutil::XMLString substringData(int offset,int count) = 0;

    /**
     * Appends the string to the end of the character data of the node.
     *
     * @descr @n Since DOM Level 1
     * @param arg - the string to append
     *
     * @return In case of an error, produces DOMException:
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node is read-only.
     */
    virtual void appendData(const XMLChar* arg) = 0;

    /**
     * Inserts a string at the specified character offset.
     *     
     * @descr @n Since DOM Level 1
     * @param offset - the character offset at which to insert the string
     * @param arg    - the string to insert

     *
     * @return In case of an error, produces DOMException:
     * - @c INDEX_SIZE_ERR: Raised if the specified offset is negative
     *            or greater than the number of characters in @c data.
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node is read-only.
     */
    virtual void insertData(int offset,const XMLChar* arg) = 0;

    /**
     * Removes a range of characters from the node.
     *
     * @descr @n Upon successful data deletion, @c data and @c length
     * reflect the change. If the sum of @e offset and @e count
     *  exceeds @c length, all characters from @c offset
     *  to the end of the data are deleted. 
     *
     * Since DOM Level 1
     *
     * @param offset - the offset, from which to remove characters
     * @param count  - the number of characters to delete
     *
     * @return In case of an error, produces DOMException:
     * - @c INDEX_SIZE_ERR: Raised if the specified offset is negative or
     *            greater than the number of characters in @c data,
     *            or if the specified @e count is negative.
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node is read-only.
     */
    virtual void deleteData(int offset, int count) = 0;
    
    /**
     * Replaces the characters starting at the specified character offset with
     * the specified string.
     *
     * @descr @n If the sum of @e offset and @e count exceeds
     *  @c length, all characters to the end of the data are replaced. With
     * this, the effect is the same as a deleteData() method call
     * with the same range, followed by the insertData() method
     * invocation.
     *
     * Since DOM Level 1
     *
     * @param offset - the offset, from which to start replacing
     * @param count  - the number of characters to replace
     * @param arg    - the string, with which the range must be replaced
     *
     * @return In case of an error, produces DOMException:
     * - @c INDEX_SIZE_ERR: Raised if the specified offset is negative or
     *            greater than the number of characters in @c data,
     *            or if the specified @e count is negative.
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node is read-only.
     */
    virtual void replaceData(int offset,int count,const ixml::XMLChar* arg) = 0;

    /**
     * Sets the character data of the node that implements this interface.
     *
     * @descr @n Since DOM Level 1
     * @param arg - the string to set
     */
    virtual void setData(const XMLChar* arg) = 0;
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif
