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

/** @file namednodemap.h @brief Named node map interface
 *
 * NamedNodeMap is used to
 * represent collections of nodes that can be accessed by name.
 */

#ifndef __COM_INTEL_XML_PARSE_DOM_NAMEDNODEMAP_H__
#define __COM_INTEL_XML_PARSE_DOM_NAMEDNODEMAP_H__

#include "include/common/types.h"
#include "include/common/defines.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

class Node;

/**
 * Implements the NamedNodeMap interface.
 * NamedNodeMap objects do not inherit from NodeList and
 * are not maintained in any particular order. Nodes in a @c 
 * NamedNodeMap object can also be accessed by an ordinal index, but 
 * this is only to allow convenient enumeration of the contents, and
 * does not imply that the DOM specifies the order of these Node objects.
 *
 * Since DOM Level 1
 */
class DOM_EXPORT NamedNodeMap 
{
public:
    /** 
     * Destructor 
     */
    virtual ~NamedNodeMap() {}

public:
    /**
     * Adds a node to the named nodemap using its @c nodeName attribute.
     *
     * @descr @n This method uses the @c nodeName attribute to derive the name
     * where the node must be stored, so that multiple nodes of certain types
     * (with a "special" string value) cannot be stored to avoid clashing the
     * names. This operation is seen preferable to allowing nodes to be aliased.
     *
     * If a node with the specified name is already present in the map, the method
     * replaces it with the new one. After it is added to the nodemap, this node
     * can be accessed by the value of the @c nodeName attribute of
     * the node. 
     *
     * Since DOM Level 1
     * @param arg - the node to store in a named node map
     * @return The replaced Node if the new Node replaces an existing one;
     *         otherwise, NULL.
     *
     * @return In case of an error, produces Exception.
     *
     *   WRONG_DOCUMENT_ERR: Raised if @e arg was created from a
     *                       document different from the one that created the
     *                       NamedNodeMap object.
     *
     *   NO_MODIFICATION_ALLOWED_ERR: Raised if this @c 
     *                       NamedNodeMap is read-only.
     *
     *   INUSE_ATTRIBUTE_ERR: Raised if @e arg is an Attr object already
     *                       attached as an attribute of another Element
     *                       object. The DOM user must explicitly clone
     *                       Attr nodes to re-use them in other elements.
     */
    virtual Node*   setNamedItem(Node *arg) = 0;

    /**
     * Gets the node in NamedNodeMap by its @c index.
     *
     * @descr @n If @e index is greater than or equal to the number of nodes in the
     * map, this method returns NULL.
     *
     * Since DOM Level 1
     * @param index - the index in the map
     *
     * @return The node at the @e index position in the
     *         NamedNodeMap or NULL for an invalid
     *         index.
     */
    virtual Node*    item(unsigned int index) const = 0;

    /**
     * Retrieves the specified node by name.
     * @descr @n Since DOM Level 1
     *
     * @param name - the @c nodeName attribute of the node to retrieve
     * @return The Node object of any type with the specified @c 
     *         nodeName, or NULL if no node in the map
     *         is identified by the specified name.
     */
    virtual Node*    getNamedItem(const ixml::XMLChar* name) const = 0;

    /**
     * Gets the number of nodes in the map.
     * @descr @n  The range of valid child node indices is from 0 to @c length-1
     * inclusive.
     *
     * Since DOM Level 1
     */
    virtual unsigned int   getLength() const = 0;

    /**
     * Removes the specified node by name.
     *
     * @descr @n If the removed node is an Attr object with the default value, it is 
     * immediately replaced.
     *
     * Since DOM Level 1
     * @param name - the @c nodeName attribute of the node to remove
     * 
     * @return The node removed from the map if a node with such name exists; 
     *         otherwise, NULL.
     * @return In case of an error, produces DOMException.
     *
     *  NOT_FOUND_ERR: Raised if there is no node named @c name in
     *   the map.
     *
     *  NO_MODIFICATION_ALLOWED_ERR: Raised if this NamedNodeMap
     *   is read-only.
     */
    virtual Node*   removeNamedItem(const ixml::XMLChar *name) = 0;

    /**
     * Retrieves the specified node by local name and namespace URI.
     * @descr @n Since DOM Level 2
     * @param namespaceURI - the namespace URI of the node to retrieve
     * @param localName    - the local name of the node to retrieve
     *
     * @return The Node object of any type with the specified
     *    local name and namespace URI, or NULL if these parameters
     *    identify no node in the map.
     */
    virtual Node*   getNamedItemNS(const ixml::XMLChar *namespaceURI,
                                            const ixml::XMLChar *localName) const = 0;

    /**
     * Adds a node using its @c namespaceURI and @c localName.
     *
     * @descr @n If a node with the specified namespace URI and local name is already present
     * in the map, the method replaces it with the new one. After it is added to the
     * nodemap, this node can be accessed by the values of its 
     * @c namespaceURI and @c localName attributes. 
     *
     * Since DOM Level 2
     * @param arg - the node to store in NamedNodeMap
     * @return The replaced Node if the new Node replaces an existing one;
     *         otherwise, NULL.
     *
     * @return In case of an error, produces DOMException.
     *
     *   WRONG_DOCUMENT_ERR: Raised if @e arg was created from a
     *                       document different from the one that created the
     *                       NamedNodeMap object.
     *
     *   NO_MODIFICATION_ALLOWED_ERR: Raised if this @c 
     *                       NamedNodeMap is read-only.
     *
     *   INUSE_ATTRIBUTE_ERR: Raised if @e arg is an Attr object already
     *                       attached as an attribute of another Element
     *                       object. The DOM user must explicitly clone
     *                       Attr nodes to re-use them in other elements.
     */
    virtual Node*   setNamedItemNS(Node *arg) = 0;

    /**
     * Removes the specified node by the namespace URI and local name.
     *
     * @descr @n When this NamedNodeMap contains attributes 
     * attached to an element, and the removed attribute is known to have 
     * a default value, the method immediately adds the new default attribute 
     * with the default value, as well as the corresponding namespace URI, local name, and prefix.
     *
     * Since DOM Level 2
     * @param namespaceURI - the namespace URI of the node to retrieve
     * @param localName    - the local name of the node to retrieve
     *   
     * @return The node removed from the map if a node with such a local name
     *    and namespace URI exists; otherwise, NULL.
     * @return In case of an error, produces DOMException.
     *
     *   NOT_FOUND_ERR: Raised if there is no node named @c name in
     *   the map.
     *
     *   NO_MODIFICATION_ALLOWED_ERR: Raised if this NamedNodeMap
     *   is read-only.
     */
    virtual Node*   removeNamedItemNS(const ixml::XMLChar* namespaceURI,
                                              const ixml::XMLChar *localName) = 0;
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif

