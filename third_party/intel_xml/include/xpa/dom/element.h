
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
 
/** @file element.h @brief Element interface.
 *
 * The Element interface represents an element used in XML 
 * as a way to store elements in the text of a document.
 */

#ifndef __COM_INTEL_XML_PARSE_DOM_ELEMENT_H__
#define __COM_INTEL_XML_PARSE_DOM_ELEMENT_H__

#include "include/xpa/dom/node.h"
#include "include/xpa/dom/nodelist.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

/**
 * Implements the Element interface 
 * 
 * The vast majority of objects (apart from text) when traversing 
 * a document are Element nodes.
 *
 * @b Example:
 * @code
 * <elementExample id="demo">
 * <subelement1/>
 * <subelement2><subsubelement/></subelement2>
 * </elementExample>
 * @endcode
 *
 * When represented using DOM, the top node is an Element node
 * for @c elementExample, which contains two child Element
 * nodes: for @c subelement1 and one for @c subelement2. 
 * @c subelement1 contains no child nodes.
 * 
 * Elements may have attributes associated with them; since the
 * Element interface inherits from Node, the generic method
 * Node#getAttributes() may be used
 * to retrieve the set of all attributes for an element.  There are methods on
 * the Element interface to retrieve either an Attr
 * object by name or an attribute value by name. In XML, where an attribute
 * value may contain entity references, an Attr object should be
 * retrieved to examine the possibly fairly complex sub-tree representing the
 * attribute value. On the other hand, in HTML, where all attributes have
 * simple string values, methods to directly access an attribute value can
 * safely be used as a convenience.
 *
 * Since DOM Level 1
 */

class DOM_EXPORT Element : public Node
{
public:
    /** 
     * Destructor
     */
    virtual ~Element() {}

public:

    /**
     * Gets the name of the element.
     *
     * @descr @n This function returns the name of the requested Element. The operation is
     * case-preserving in XML just like all other DOM operations.
     *
     * Since DOM Level 1
     *
     * For example, in the following line, @c tagName has the value @c elementExample:
     *
     * @code
     *  <elementExample id="demo">  ... </elementExample>
     * @endcode
     *
     * @return The @c tagName of the element.
     */
    virtual const ixml::XMLChar* getTagName() const = 0;

    /**
     * Retrieves the attribute value by name.
     *
     * @descr @n Since DOM Level 1
     * @param name - the name of the attribute to retrieve
     * @return The Attr value as a string, or an empty  string if that attribute
     *         does not have a specified or default value.
     */
    virtual const ixml::XMLChar* getAttribute(const ixml::XMLChar* name) = 0;

    /**
     * Retrieves the Attr node by name.
     *
     * @descr @n Since DOM Level 1
     * @param name - the name @c nodeName of the attribute to retrieve
     * @return The Attr node with the specified name or
     *         NULL if no such attribute is found.
     */
    virtual Attr* getAttributeNode(const ixml::XMLChar* name) = 0; 

    /**
     * Returns the NodeList of all descendant elements with the given tag name.
     *
     * @descr @n This method returns the descendants in the order, in which they are
     * encountered in a pre-order traversal of the Element tree.
     *
     * Since DOM Level 1
     * @param name - the name of the tag to match on; the value "*"
     *               matches all tags
     * @return The list of matching Element nodes.
     */
    virtual NodeList* getElementsByTagName(const ixml::XMLChar* name) const = 0; 
    
    /**
     * Adds a new attribute by name.
     *
     * @descr @n If an attribute with that name is already present in the element,
     * the method changes its value to be that of the @e value parameter.
     *
     * The assigned value is a simple string and it is not parsed, so that 
     * any markup, such as syntax to be recognized as an entity reference, is
     * treated as literal text. The markup contained in the value needs to be 
     * appropriately escaped by the implementation when it is printed out. 
     *
     * To assign an attribute value that contains entity references, 
     * create an Attr node plus Text and EntityReference nodes, build the 
     * appropriate subtree, and use setAttributeNode() to assign it as the 
     * value of an attribute.
     *
     * Since DOM Level 1
     *
     * @param name  - the name of the attribute to create or alter
     * @param value - the string value to set
     * @return In case of an error, returns DOMException:
     * - @c INVALID_CHARACTER_ERR: Raised if the specified name contains an
     *   illegal character.
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node is read-only.
     */
    virtual void setAttribute(const ixml::XMLChar* name, const ixml::XMLChar* value) = 0;

    /**
     * Adds a new attribute node.
     *
     * @descr @n If an attribute with that name @c nodeName is already present
     * in the element, the method replaces it with the new one.
     *
     * Since DOM Level 1
     * @param newAttr - the Attr node to add to the attribute list
     * @return The replaced Attr node if the @e newAttr attribute replaces
     * an existing attribute; otherwise, NULL.
     *
     * In case of an error, returns DOMException:
     * - @c WRONG_DOCUMENT_ERR: Raised if @e newAttr attribute was created from
     *            document different from the one that created the element.
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node is read-only.
     * - @c INUSE_ATTRIBUTE_ERR: Raised if @e newAttr is already an
     *            attribute of another Element object. The DOM user must
     *            explicitly clone Attr nodes to re-use them in other elements.
     */
    virtual Attr* setAttributeNode(Attr* newAttr) = 0;

    /**
     * Removes the specified attribute node.
     *
     * @descr @n If the removed Attr object has a default value, it is immediately
     * replaced. The replacing attribute has the same namespace URI and local
     * name, as well as the original prefix, when applicable.
     *
     * Since DOM Level 1
     * @param oldAttr - the Attr node to remove from the attribute list
     * @return The Attr node that was removed.
     * @return In case of an error, returns DOMException:
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node is read-only.
     * - @c NOT_FOUND_ERR: Raised if @e oldAttr is not an attribute
     *            of the element.
     */
    virtual Attr* removeAttributeNode(Attr* oldAttr) = 0;

    /**
     * Removes the attribute by name.
     *
     * @descr @n If the removed attribute has a default value, an attribute immediately
     * appears with the default value, as well as the corresponding namespace
     * URI, the local name, and prefix when applicable.
     *
     * To remove an attribute by local name and namespace URI, use the 
     * removeAttributeNS() method.
     *
     * Since DOM Level 1
     * @param name - the name of the attribute to remove
     * @return In case of an error, returns DOMException:
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node is read-only.
     */
    virtual void removeAttribute(const ixml::XMLChar* name) = 0;

    /**
     * Retrieves the attribute value by local name and namespace URI.
     * 
     * @descr @n Since DOM Level 2
     * @param namespaceURI - the namespace URI of the attribute to retrieve
     * @param localName    - the local name of the attribute to retrieve
     * @return The Attr value as a string, or NULL if
     *    that attribute does not have the specified or default value.
     */
    virtual const ixml::XMLChar* getAttributeNS(const ixml::XMLChar* namespaceURI, const ixml::XMLChar* localName) = 0;

    /**
     * Retrieves the Attr node by local name and namespace URI.
     * 
     * @descr @n Since DOM Level 2
     * @param namespaceURI - the namespace URI of the attribute to retrieve
     * @param localName    - the local name of the attribute to retrieve
     * @return The Attr node with the specified attribute local name and
     * namespace URI or NULL if no such attribute is found.
     */
    virtual Attr* getAttributeNodeNS(const ixml::XMLChar* namespaceURI, const ixml::XMLChar* localName) = 0;

    /**
     * Adds a new attribute by namespace URI and qualified name.
     * 
     * @descr @n If an attribute with the same local name and namespace URI is already
     * present on the element, the method changes its prefix to the prefix part
     * of the @e qualifiedName parameter, and its value to the @e value
     * parameter. 
     *
     * The assigned value is a simple string and it is not parsed, so that 
     * any markup, such as syntax to be recognized as an entity reference, is
     * treated as literal text. The markup contained in the value needs to be 
     * appropriately escaped by the implementation when it is printed out. 
     *
     * To assign an attribute value that contains entity references, 
     * create an Attr node plus Text and EntityReference nodes, build the 
     * appropriate subtree, and use setAttributeNode() to assign it as the 
     * value of an attribute.
     *
     * Since DOM Level 2
     * 
     * @param namespaceURI  - the namespace URI of the attribute to create or alter
     * @param qualifiedName - the qualified name of the attribute to create or alter
     * @param value         - the string value to set
     *
     * @return In case of an error, returns DOMException:
     * - @c INVALID_CHARACTER_ERR: Raised if the specified qualified name contains an
     *   illegal character.
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node is read-only.
     * - @c NAMESPACE_ERR: Raised if on the following conditions is true: 
     *        - the @e qualifiedName parameter is malformed
     *        - the @e qualifiedName parameter has a prefix and the
     *        @e namespaceURI parameter is NULL or an empty string      
     *        - the @e qualifiedName parameter has the prefix "xml" and the
     *        @e namespaceURI parameter is not "http://www.w3.org/XML/1998/namespace"
     *        - the @e qualifiedName parameter has the prefix "xmlns" and
     *        @e namespaceURI is not "http://www.w3.org/2000/xmlns/"
     *        - the @e qualifiedName parameter is "xmlns" and the
     *        @e namespaceURI parameter is not "http://www.w3.org/2000/xmlns/"
     *        
     */
    virtual void setAttributeNS(const ixml::XMLChar* namespaceURI, const ixml::XMLChar* qualifiedName, const ixml::XMLChar* value) = 0;

    /**
     * Adds a new attribute node with namespace.
     *
     * @descr @n If an attribute with that local name and namespace URI is already present
     * in the element, the element replaces it with the new one.
     *
     * Since DOM Level 2
     * @param newAttr - the Attr node to add to the attribute list
     * @return The replaced Attr node, if the @e newAttr attribute replaces
     *         an existing attribute with the same @e local name and @e 
     *         namespace URI; otherwise, NULL.
     * @return In case of an error, returns DOMException:
     * - @c WRONG_DOCUMENT_ERR: Raised if @e newAttr is created from a
     *   document different from the one that created the element.
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node is read-only.
     * - @c INUSE_ATTRIBUTE_ERR: Raised if @e newAttr is already an
     *   attribute of another Element object. The DOM user must  explicitly clone
     *   Attr nodes to re-use them in other elements.
     */
    virtual Attr* setAttributeNodeNS(Attr* newAttr) = 0;

    /**
     * Removes the attribute by namespace URI and local name. 
     * 
     * @descr @n If the removed attribute has a default value, it is immediately replaced.
     * The replacing attribute has the same namespace URI and local name, and
     * the original prefix.
     *
     * Since DOM Level 2
     * @param namespaceURI - the namespace URI of the attribute to remove
     * @param localName    - the local name of the attribute to remove
     * @return In case of an error, returns DOMException:
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node is read-only.
     */
    virtual void removeAttributeNS(const ixml::XMLChar* namespaceURI, const ixml::XMLChar* localName) = 0;

    /**
     * Returns the NodeList with the given local name and namespace URI.
     *
     * @descr @n The method returns the Element nodes in the order in which they are
     * encountered in a pre-order traversal of the Document tree starting from
     * this node.
     *
     * Since DOM Level 2
     *
     * @param namespaceURI - the namespace URI of the Element to match;
     *                       the value "*" matches all namespaces
     * @param localName    - the local name of the Element to match; the value
     *                       "*" matches all local names
     *
     * @return The new NodeList object with all matched Element nodes.
     * 
     */
    virtual NodeList* getElementsByTagNameNS(const ixml::XMLChar* namespaceURI,const ixml::XMLChar* localName) const = 0;

    /**
     * Checks whether the attribute with the given name is specified for
     * this element or has a default value.
     * @descr @n Since DOM Level 2
     *
     * @param name - the name of the attribute
     *
     * @return TRUE if an attribute with the given name is
     *         specified for this element or has a default value; otherwise,
     *         FALSE.
     */
    virtual bool hasAttribute(const ixml::XMLChar* name) = 0;

    /**
     * Checks whether the attribute with the given local name and namespace URI
     * is specified for this element or has a default value.
     * 
     * @descr @n HTML-only DOM implementations do not need to implement this method.
     *
     * Since DOM Level 2
     * @param namespaceURI - the namespace URI of the attribute
     * @param localName    - the local name of the attribute
     *
     * @return TRUE if an attribute with the given local name
     *   and namespace URI is specified or has a default value for this
     *   element; otherwise, FALSE.
     */
    virtual bool hasAttributeNS(const ixml::XMLChar* namespaceURI, const ixml::XMLChar* localName) = 0;

    /**
     * Declares the Attr specified by name to be of type ID. 
     * 
     * @descr @n If the value of the specified Attr object is unique, this Element node
     * can later be retrieved using Document#getElementById(). However, this only
     * affects this node and does not change grammar that may be in use.    
     *
     * To specify an Attr object by local name and namespace URI, use the
     * setIdAttributeNS() method.
     *
     * Since DOM Level 3
     * @param name - the name of the @c Attr
     * @param isId - TRUE for set the type of node as ID; otherwise, 
     *               FALSE
     * 
     * @return In case of an error, returns DOMException:
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node is read-only.
     * - @c NOT_FOUND_ERR: Raised if the specified node is not an Attr of this Element.
     */
    virtual void setIdAttribute(const ixml::XMLChar* name, bool isId) = 0;

    /**
     * Declares the Attr specified by local name and namespace URI to be of
     * type ID. 
     * 
     * @descr @n If the value of the specified Attr object is unique, this Element node
     * can later be retrieved using Document#getElementById(). However, this only
     * affects this node and does not change grammar that may be in use.
     *
     * Since DOM Level 3
     * @param namespaceURI - the namespace URI of the Attr node
     * @param localName    - the local name of the Attr node
     * @param isId         - TRUE for set the type of node as ID;
     *                       otherwise, FALSE
     * @return In case of an error, returns DOMException:
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node is read-only.
     * - @c NOT_FOUND_ERR: Raised if the specified node is not an Attr of this Element.
     */
    virtual void setIdAttributeNS(const ixml::XMLChar* namespaceURI, const ixml::XMLChar* localName, bool isId) = 0;
    
    /**
     * Declares the Attr specified by node to be of type ID.
     *
     * @descr @n If the value of the specified Attr object is unique, this Element node
     * can later be retrieved using Document#getElementById(). However, this only
     * affects this node and does not change grammar that may be in use.
     *
     * Since DOM Level 3
     * @param idAttr - the Attr node
     * @param isId   - TRUE for set the type of node as ID; otherwise, 
     *                 FALSE
     * 
     * @return In case of an error, returns DOMException:
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node is read-only.
     * - @c NOT_FOUND_ERR: Raised if the specified node is not an Attr of this Element.
     */
    virtual void setIdAttributeNode(Attr* idAttr, bool isId) = 0; 
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif
