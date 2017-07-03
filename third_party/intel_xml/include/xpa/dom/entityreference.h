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

/** @file entityreference.h 
 *  @brief Entity reference object.
 * 
 * The EntityReference interface represents a node that 
 * refers to a Entity node. It is usually a copy of the 
 * Entity node subtree.
 */

#ifndef __COM_INTEL_XML_PARSE_DOM_ENTITYREFERENCE_H__
#define __COM_INTEL_XML_PARSE_DOM_ENTITYREFERENCE_H__

#include "include/xpa/dom/node.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

/**
 * Implements the EntityReference interface.
 * EntityReference objects can be inserted into the structure
 * model when an entity reference is in the source document, or when the
 * user wishes to insert an entity reference. Note that character references
 * and references to predefined entities are considered to be expanded by
 * the HTML or XML processor so that characters are represented by their
 * Unicode equivalent rather than by an entity reference. Moreover, an XML
 * processor can completely expand references to entities while building the
 * structure model instead of providing EntityReference
 * objects. If the processor provides such objects, no Entity node representing
 * the referenced entity for a given EntityReference node may be 
 * present. If such an Entity object exists, the subtree of the
 * EntityReference node is usually a copy of the Entity node subtree.
 * However, this may not be true when an entity contains an unbound namespace prefix.
 * In such a case, because the namespace prefix resolution depends on where the 
 * entity reference is, the descendants of the EntityReference
 * node may be bound to different namespace URIs.
 * 
 * As for Entity nodes, EntityReference nodes and
 * all their descendants are read-only.
 * @see The <a href='http://www.w3.org/TR/2000/REC-DOM-Level-2-Core-20001113'>
 * Document Object Model (DOM) Level 2 Core Specification</a>.
 *
 * Since DOM Level 1
 */
class DOM_EXPORT EntityReference : public Node
{
public:
    /**  
     * Destructor   
     */
    virtual ~EntityReference() {}
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif
