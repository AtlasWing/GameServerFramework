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


/** 
 * @file attr.h 
 * @brief Definition of the XML Attribute element 
 *
 * In DOM, an attribute is considered a property of the element it is associated
 * with, and not a part of the document tree. This way, attribute nodes cannot
 * be immediate children of a DocumentFragment node but they can be associated
 * with Element nodes contained within the DocumentFragment node. 
 * Treating attributes as element properties increases effectiveness of such
 * features as default attributes associated with elements of a given type. 
 * 
 * This file defines the Attr class defining XML attributes handling.
*/


#ifndef __COM_INTEL_XML_PARSE_DOM_ATTR_H__
#define __COM_INTEL_XML_PARSE_DOM_ATTR_H__

#include "include/xpa/dom/node.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

class Element;
    
/**
 * Refers to an attribute of an XML element.
 * 
 * Attr objects inherit the Node interface but are not treated
 * as nodes in the document tree. Thus, the Node attributes
 * @c parentNode, @c previousSibling, and @c nextSibling
 *  have a  NULL value for Attr objects.
 *
 * Typically, allowed values for the attribute are defined in the document
 * type definition.
 *
 * Since DOM Level 1
 */
class DOM_EXPORT Attr : public Node
{
public:    
    /** 
     * Destructor 
     */
    virtual ~Attr() {}

public:

    /**
     * Gets the name of this attribute.
     *
     * @descr @n Since DOM Level 1
     * @return The Attr name.
     */
    virtual const ixml::XMLChar* getName() const=0;

    /**
     *
     * Checks whether the attribute value differs from the default value set in
     * the DTD.
     * @descr @n Since DOM Level 1
     *
     * @return TRUE if the attribute value was set
     * explicitly in the XML document or assigned programatically by the 
     * setValue() function; FALSE if the attribute value
     * is the same as the default value declared in the document type definition
     * (DTD).
     */
    virtual bool getSpecified() const = 0;
    
    /**
     * Gets the value of the attribute.
     *
     * @descr @n The method replaces character and general entity references
     * with their values.
     *
     * Since DOM Level 1
     *
     * @return The attribute value as a string. 
     */
    virtual const ixml::XMLChar* getValue() const =0;

    /**
     * Sets the value of the attribute. 
     * 
     * @descr @n This method creates a Text node with the unparsed contents
     * of the string.
     * 
     * Since DOM Level 1
     *
     * @param value - the value of the DOM attribute to be set
     *
     */
    virtual void setValue(const ixml::XMLChar* value)=0;

    /**
     * Gets the Element node, to which this attribute is attached.
     *
     * @descr @n Since DOM Level 2
     *
     * @return The requested Element node or NULL
     * if this attribute is not in use.
     */
    virtual Element* getOwnerElement() const=0;

    /**
     * Checks whether this attribute is of type ID. 
     *
     * @descr @n If the attribute matches the type ID and its value is unique,
     * the owner element of this attribute can be retrieved by calling
     * Document#getElementById().
     *
     * Since DOM level 3
     *
     * @return TRUE if this Attr object is of the ID;
     *           otherwise, FALSE.
     */
    virtual    bool isId() const = 0;

    /**
     * Checks whether the attribute node is a namespace node.
     *
     * @descr @n Intel DOM extension
     *
     * @return TRUE if this Attr object is a Namespace node;
     *           otherwise, FALSE.
     */
    virtual bool isNamespace() const = 0;
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif
