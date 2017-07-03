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

/** @file xmldocnode.h 
 * @brief The XMLDocument node definition
 *
 * The XMLDocNode interface is the primary datatype for the entire
 * XMLDocument Model and represents a single node in the XMLDocument object.
 */

#ifndef __COM_INTEL_XML_PARSE_XMLDOC_XMLDOCNODE_H__
#define __COM_INTEL_XML_PARSE_XMLDOC_XMLDOCNODE_H__

#include "include/common/types.h"
#include "include/common/defines.h"

INTEL_XML_PARSE_XMLDOC_NAMESPACE_BEGIN

/** Represents a single node in the XMLDocument object. */

class XMLDOC_EXPORT XMLDocNode
{
public:

    /** @brief Types of an XMLDocNode object */
   enum XMLDocNodeType{
        ROOT                   = 1,         ///< Root node
        ELEMENT                = 2,         ///< Element node
        ATTRIBUTE              = 3,         ///< Attribute node
        NAMESPACE              = 4,         ///< Namespace node
        TEXT                   = 5,         ///< Text node
        COMMENT                = 6,         ///< Comment node
        PROCESSING_INSTRUCTION = 7          ///< ProcessingInstruction node
    };

    /** @brief Destructor.*/
    virtual ~XMLDocNode() {}

public:
/**
 * @brief Gets the enumeration value representing the type of the underlying
 * object.
 * @return The type of the XMLDocNode object.
     */
    virtual short getNodeType() const = 0;

/**
 * @brief Gets the namespace URI of this node.
 * 
 * @descr @n The namespace URI is given to the node at the creation time.
 *
 * If the node is not of the @c ELEMENT or
 * @c ATTRIBUTE type, the URI is NULL.
     * 
 * @return The namespace URI, or NULL if the URI is not specified.
     */
    virtual    const ixml::XMLChar* getNamespaceURI() const = 0;

/**
 * @brief Gets the namespace prefix of this node.
     * 
 * @return The namespace prefix, or NULL if the prefix is not
 * specified.
     */
    virtual const ixml::XMLChar* getPrefix() const = 0;

/** @brief Gets the name of this node depending on its type.
 * @return The name of the XMLDocNode object.
     */
    virtual    const ixml::XMLChar* getName() const = 0;

/** @brief Gets the value of this node depending on its type.
 *  @return The value of the XMLDocNode object.
     */
    virtual const ixml::XMLChar* getValue() const = 0;

/**
 * @brief Moves to the root of this node.
 *
 * @return TRUE if the move is successful; otherwise
 *         FALSE.
     */
    virtual bool moveRootNode() = 0;
    
/**
 * @brief Moves to the parent of this node.
 *
 * @return TRUE if the move is successful; otherwise
 *         FALSE.
     */
    virtual bool moveParentNode() = 0;

/**
 * @brief Moves to the first child of this node.
 *
 * @return TRUE if the move is successful; otherwise
 *         FALSE.
     */
    virtual    bool moveFirstChild() = 0;

/**
 * @brief Moves to the first child of this node.
 *
 * @return TRUE if the move is successful; otherwise
 *         FALSE.
     */
    virtual    bool moveFirstAttribute() = 0;

/**
 * @brief Moves to the first child of this node.
 *
 * @return TRUE if the move is successful; otherwise
 *         FALSE.
     */
    virtual    bool moveFirstNamespace() = 0;

/**
 * @brief Moves to the node immediately preceding this node.
 *
 * @return TRUE if the move is successful; otherwise
 *         FALSE.
     */
    virtual bool movePreviousSibling() = 0;

/**
 * @brief Moves to the node immediately following this node.
 *
 * @return TRUE if the move is successful; otherwise
 *         FALSE.
     */
    virtual bool moveNextSibling() = 0;

/**
 * @brief Returns a copy of this node.
     * 
     * @return The duplicate node.
     */
    virtual    XMLDocNode* cloneNode() const = 0;

/**
 * @brief Checks whether two nodes are equal.
 *
 * @param node - the node to compare with
     *
 * @return TRUE if the nodes and subtrees are equal;
 *         otherwise FALSE.
     */
    virtual bool isEqualNode(const XMLDocNode* node) const = 0;
};

INTEL_XML_PARSE_XMLDOC_NAMESPACE_END

#endif


