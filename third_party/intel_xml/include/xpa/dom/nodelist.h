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

/** @file nodelist.h @brief Node list interface.
 * 
 * The NodeList interface provides the abstraction of an ordered
 * collection of nodes.
 */

#ifndef __COM_INTEL_XML_PARSE_DOM_NODELIST_H__
#define __COM_INTEL_XML_PARSE_DOM_NODELIST_H__

#include "include/common/defines.h"
#include "include/common/types.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

class Node;

/**
 * Implements the NodeList interface.
 * NodeList objects are created by Document::getElementsByTagName()
 * and Node::getChildNodes().
 * The items in the NodeList object are accessible via an integral
 * index starting from 0.
 * Changes to the document tree are immediately reflected in any NodeList
 * objects that may have been created for that tree.
 */
class DOM_EXPORT NodeList
{
public:
    /** 
     * Destructor 
     */
    virtual ~NodeList() {}

public:
    /**
     * Returns the @e index item in the collection.
     *
     * @descr @n If @e index is greater than or equal to the number of nodes in
     * the list, this returns NULL.
     *
     * Since DOM Level 1
     * @param index - the index of the collection
     * @return The node at the @e index position in the
     *   NodeList, or NULL for an invalid index. 
     */
    virtual Node* item(unsigned int index) const = 0;

    /**
     * Returns the number of nodes in the list.
     *
     * @descr @n The range of valid child node indices is from 0 to @c length-1
     * inclusive.
     *
     * Since DOM Level 1
     */
    virtual unsigned int getLength() const = 0;

    /**
     * Returns the first node in the list.
     *
     * @descr @n Intel DOM extension
     */
    virtual Node* first() const = 0;

    /**
     * Returns the last node in the list.
     *
     *  @descr @n Intel DOM extension
     */
    virtual Node* last() const = 0;
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif
