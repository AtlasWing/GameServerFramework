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

/** @file text.h @brief Text data interface.
 *
 * This file describes the Text interface representing textual
 * content (character data) in XML.
 * @see The <a href='http://www.w3.org/TR/2000/REC-DOM-Level-2-Core-20001113'>
 * Document Object Model (DOM) Level 2 Core Specification</a>.
 */


#ifndef __COM_INTEL_XML_PARSE_DOM_TEXT_H__
#define __COM_INTEL_XML_PARSE_DOM_TEXT_H__

#include "include/xpa/dom/node.h"
#include "include/xpa/dom/characterdata.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

/**
 * Inherits from CharacterData
 * and presents character data of an Element or Attr node. Textual data
 * with no markup is stored in a single object implementing the
 * Text interface that is the only child of the element.
 * Textual data with markup is parsed into multiple information items, such as
 * elements and comments, and Text nodes that form the list
 * of children of the element.
 *
 * When a document is first made available via the DOM, only one Text
 * node corresponds to each block of text. You can create adjacent
 * Text nodes that represent the content of a given element
 * with no intervening markup. However, because you cannot represent such split
 * nodes in XML or HTML, they will normally disappear between DOM editing
 * sessions. The Node#normalize() method merges such adjacent Text
 * objects into a single node for each block of text.
 * 
 */
class DOM_EXPORT Text : public CharacterData
{
public:
    /** 
     * Destructor
     */
    virtual ~Text() {}

public:
    /**
     * Breaks this node into two nodes at the specified @e offset
     * and stores both nodes in the tree as siblings. 
     * 
     * @descr @n After the split operation, this node contains the content up to the 
     * @e offset point. A new node of the same type with all the content at
     * and after the @e offset point, is returned. If the original
     * node has a parent node, the new node is inserted as the next sibling
     * of the original node. 
     *
     * When the @e offset is equal to the length of this node, the new node
     * has no data.
     *
     * Since DOM Level 1
     * @param offset - the 8-bit unit offset at which to split the node, starting from
     *                 @c 0
     * @return The new node of the same type as this node.
     * @return In case of an error, produces DOMException:
     * - @c INDEX_SIZE_ERR: Raised if the specified offset is negative or greater
     *   than the number of 8-bit units in @c data.
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node is read-only.
     */
    virtual Text* splitText(int offset) = 0;

    /**
     * Checks whether this text node is a whitespace node.
     * @descr @n Intel DOM extension
     *
     * @return TRUE for a whitespace node; otherwise, FALSE.
     *
     */
    virtual bool isContentWhitespace() = 0;
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif
