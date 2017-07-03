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
/** @file entity.h @brief Entity interface.
 *
 * This file represents an entity, parsed or unparsed, in an XML
 * document. Note that this interface models the entity itself and not the entity
 * declaration. Entity declaration modeling has been left for a
 * next level of the DOM specification.
 * @see The <a href='http://www.w3.org/TR/2000/REC-DOM-Level-2-Core-20001113'>
 * Document Object Model (DOM) Level 2 Core Specification</a>.
 */

#ifndef __COM_INTEL_XML_PARSE_DOM_ENTITY_H__
#define __COM_INTEL_XML_PARSE_DOM_ENTITY_H__

#include "include/xpa/dom/node.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

/**
 * Represents the Entity interface.
 *
 * An XML processor can fully expand entities before the structure model is passed
 * to the DOM, so that the document tree will have no EntityReference nodes.
 * XML does not require that a non-validating XML processor reads and
 * processes entity declarations made in the external subset or declared in
 * external parameter entities. This means that parsed entities declared in
 * the external subset need not be expanded by application classes,
 * and that the replacement value of the entity may not be available. When
 * the replacement value is available, the corresponding Entity
 * node's child list represents the structure of that replacement text.
 * Otherwise, the child list is empty.
 * The DOM Level 2 specification does not support editing Entity nodes.
 * To change the contents of an Entity node, the user application
 * should replace every related EntityReference node in the
 * structure model with a clone of the Entity object contents.
 * You can then edit those clone copies.
 *
 * Entity nodes and their descendants are read-only. The name of
 * the entity is stored in the @c nodeName attribute inherited from Node. 
 * An Entity node has no parent. If the entity
 * contains an unbound namespace prefix, the @c namespaceURI of
 * the corresponding node in the Entity node subtree is
 * NULL. The same is true for EntityReference
 * nodes that refer to this entity when they are created using the
 * Document#createEntityReference() method. The DOM Level 2 does not support a
 * mechanism for resolving namespace prefixes.
 *
 * Since DOM Level 1
 */
class DOM_EXPORT Entity : public Node
{
public:
    /**
     * Destructor
     */
    virtual ~Entity() {};

    /**
     * Gets the public identifier associated with the entity, if specified.
     *
     * @descr @n Since DOM Level 1
     * @return The public identifier specified or NULL if it is not
     * specified.
     *
     */
    virtual const ixml::XMLChar*        getPublicId() const = 0;

    /**
     * Gets the system identifier associated with the entity, if specified.
     *
     * @descr @n Since DOM Level 1
     * @return The system identifier specified or NULL if it is not
     * specified.
     *
     */
    virtual const ixml::XMLChar*        getSystemId() const = 0;

    /**
     * Gets the name of the notation for the entity for unparsed entities.
     *
     * @descr @n Since DOM Level 1
     * @return The notation name for an unparsed entity or NULL for
     * parsed entities.
     *
     */
    virtual const ixml::XMLChar*        getNotationName() const = 0;
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif
