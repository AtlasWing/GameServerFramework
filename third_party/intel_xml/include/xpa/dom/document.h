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
 * @file document.h @brief Document object interface.
 *
 * The Document interface represents the entire XML document.
 * This object is the root of the document tree and provides primary access
 * to the document's data.
 * @see <a href='http://www.w3.org/TR/2000/REC-DOM-Level-2-Core-20001113'>The 
 * Document Object Model (DOM) Level 2 Core Specification</a>.
 */


#ifndef __COM_INTEL_XML_PARSE_DOM_DOCUMENT_H__
#define __COM_INTEL_XML_PARSE_DOM_DOCUMENT_H__

#include "include/xpa/dom/node.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

class Element;
class Attr;
class Text;
class CDATASection;
class Comment;
class ProcessingInstruction;
class DocumentFragment;
class DOMImplementation;
class DocumentType;
class EntityReference;

/**
 * Represents the entire XML document.
 *
 * Other elements, text nodes, comments, processing instructions, and so on
 * can only exist inside the context of a Document. The 
 * Document interface contains factory methods needed to create these
 * objects. The created Node object has the @c ownerDocument attribute
 * that associates it with the Document, in the context of which
 * the Node object was created.
 */

class DOM_EXPORT Document : public Node {

public:
    /** 
     * Destructor
     */
    virtual ~Document() {}

public:

    /**
     * Creates an element of the specified type. 
     * 
     * @descr @n The returned element instance implements the Element interface, so that
     * attributes can be specified directly on the returned object. For known
     * attributes with default values, Attr nodes representing them are 
     * automatically created and attached to the element.
     *
     * To create an element with a qualified name and namespace URI, use
     * the createElementNS() method.
     *
     * Since DOM Level 1
     * 
     * @param tagname - the case-sensitive name of the element type to instantiate
     *
     * @return A new Element object with the @c nodeName attribute 
     *         set to @c tagName, and @c localName, 
     *         @c prefix, and @c namespaceURI set to 
     *         NULL.
     *
     * @return In case of an error, produces DOMException:
     * - @c INVALID_CHARACTER_ERR: Raised if the specified name contains
     *            an illegal character.
     */
    virtual Element* createElement(const ixml::XMLChar* tagname) = 0;

    /**
     * Creates an empty DocumentFragment object.
     * @descr @n Since DOM Level 1
     * @return A new DocumentFragment.
     */
    virtual DocumentFragment* createDocumentFragment() = 0;

    /**
     * Creates a Text node given the specified string.
     * @descr @n Since DOM Level 1
     * @param value - the data for the node
     * @return The new Text object.
     */
    virtual Text* createTextNode(const ixml::XMLChar* value) = 0;

    /**
     * Creates a Comment node given the specified string.
     * @descr @n Since DOM Level 1
     * @param data - the data for the node
     * @return The new Comment object.
     */
    virtual Comment* createComment(const ixml::XMLChar* data) = 0;

    /**
     * Creates a CDATASection node with the value contained is the specified
     * string.
     * @descr @n Since DOM Level 1
     * @param data - the data for the CDATASection contents
     * @return The new CDATASection object.
     */
    virtual CDATASection* createCDATASection(const ixml::XMLChar* data) = 0;

    /**
     * Creates a ProcessingInstruction node given the specified
     * name and data strings.
     * @descr @n Since DOM Level 1
     *
     * @param target - the target part of the processing instruction
     * @param data   - the data for the node
     *
     * @return The new ProcessingInstruction object.
     * @return In case of an error, produces DOMException:
     * - @c INVALID_CHARACTER_ERR: Raised if the specified target contains
     *            an illegal character.
     */
    virtual ProcessingInstruction* createProcessingInstruction(const ixml::XMLChar* target,const ixml::XMLChar* data) = 0;

    /**
     * Creates an Attr object of the given name. 
     * 
     * @descr @n After instantiation, the Attr instance can be set on an Element
     * using the Element#setAttributeNode() method. To create an attribute 
     * with a qualified name and namespace URI, use
     * the createAttributeNS() method.
     *
     * Since DOM Level 1
     * @param name - the name of the attribute
     * 
     * @return A new Attr object with the @c nodeName attribute
     *         set to @c name, and @c localName,
     *         @c prefix, and @c namespaceURI set to
     *         NULL. 
     *
     *         The value of the attribute is an empty string.
     * @return In case of an error, produces DOMException:
     * - @c INVALID_CHARACTER_ERR: Raised if the specified name contains
     *            an illegal character.
     */
    virtual Attr* createAttribute(const ixml::XMLChar* name) = 0;

    /**
     * Creates an EntityReference object. 
     * 
     * @descr @n If the referenced entity is known, the child list of the EntityReference
     * node is made the same as that of the corresponding Entity node. If any
     * descendant of the Entity node has an unbound namespace prefix, the 
     * corresponding descendant of the created EntityReference node is also 
     * unbound: its @c namespaceURI is NULL. 
     * The DOM Level 2 specification supports no mechanism for resolving
     * namespace prefixes.
     *
     * Since DOM Level 1
     * @param name - the name of the entity to reference
     * @return The new EntityReference object.
     * @return In case of an error, produces DOMException:
     * - @c INVALID_CHARACTER_ERR: Raised if the specified name contains
     *            an illegal character.
     */
    virtual EntityReference* createEntityReference(const ixml::XMLChar* name) = 0;
    
    /**
     * Gets the Document Type Declaration (see DocumentType)
     * associated with this document. 
     * 
     * @descr @n The DOM Level 2 specification does not support editing the
     * Document Type Declaration. The value of @c docType cannot be altered, not
     * even by using methods inherited from the Node interface,
     * such as Node#insertBefore() and Node#removeChild().
     *
     *  Since DOM Level 1
     *
     * @return The requested DTD or NULL if the document has no DTD.
     */
    virtual DocumentType* getDoctype() const = 0;
    
    /**
     * Provides the DOMImplementation object that handles this document. 
     *
     * @descr @n A DOM application may use objects from multiple implementations.
     *
     * Since DOM Level 1
     *
     * @return The DOMImplementation object that handles this document.
     */
    virtual DOMImplementation* getImplementation() const = 0;

    /**
     * Returns a NodeList object of all the Element node(s) with the
     * given tag name.
     *
     * @descr @n The method lists the Element objects following the order, in which they
     * are encountered in the pre-order traversal of the Document tree.
     *
     * Changes to the document tree that made after this method returns the
     *  NodeList object are immediately reflected in the node list.
     *
     * Since DOM Level 1
     * 
     * @param tagname - the name of the tag to match on; the "*" value matches
     *                  all tags
     * @return A new NodeList object with all matched Element node(s).
     */
    virtual NodeList* getElementsByTagName(const ixml::XMLChar* tagname) const = 0;

    /**
     * Imports a Node object to this Document by copying it from another document. 
     * 
     * @descr @n For all nodes, the method creates a Node object owned by the
     * importing Document, with attribute values identical to the source
     * node @c nodeName and @c nodeType, plus the
     * attributes related to namespaces: @c prefix,
     * @c localName, and @c namespaceURI. As in the
     * cloneNode() operation on a Node, the source node is not altered or
     * removed from the original document. The returned Node has no parent:
     * @c parentNode is NULL.
     *
     * Additional information is copied as appropriate to @c nodeType
     * in order to mirror the expected behavior in case a fragment of XML
     * source has been copied from one document to another, recognizing that
     * the two documents may have different DTDs in the XML case. 
     *
     * The following list describes the specifics of the import operation 
     * for each type of node:
     *
     * - @c ATTRIBUTE_NODE: The method generates an Attr object, sets
     *    the @c ownerElement attribute to NULL and the @c specified flag - to TRUE, 
     *    and recursively imports descendants of the source object and 
     *    re-assembles the resulting nodes into the corresponding subtree. 
     * - @c DOCUMENT_FRAGMENT_NODE: The method generates an empty DocumentFragment object.
     * - @c DOCUMENT_NODE, @c DOCUMENT_TYPE_NODE: These nodes cannot be imported.
     * - @c ELEMENT_NODE: The method generates an Element object and attaches
     *    Attr nodes for attributes specified in the source element or 
     *    in the destination Document object for the Element of the specified name. 
     *    The method assigns the value specified in the Document object to the 
     *    generated Attr node. Other than that, default attributes with no specified
     *    values are not imported. 
     * - @c ENTITY_NODE: The method imports Entity nodes and copies
     *    the attributes @c publicId, @c systemId, and @c notationName. 
     * - @c ENTITY_REFERENCE_NODE: The method copies EntityReference nodes. If
     *    the destination Document provides a definition of this Entity name,
     *    the method assigns the specified value.
     * - @c NOTATION_NODE: The method imports Notation nodes and copies
     *   attributes @c publicId and @c systemId. 
     * - @c PROCESSING_INSTRUCTION_NODE: The method copies the object and values
     *    of @c target and @c data attributes. 
     * - @c TEXT_NODE, @c CDATA_SECTION_NODE, @c COMMENT_NODE: The method
     *    copies these node types inheriting from CharacterData and their
     *    @c data and @c length attributes.
     *
     *   @note In the current version of the DOM specification, DocumentType is read-only.
     *
     * For @c ELEMENT_NODE, @c DOCUMENT_FRAGMENT_NODE and @c ENTITY_NODE,
     * you can set the @e deep parameter TRUE to recursively import descendants
     * of the source node and to re-assemble the resulting nodes into the 
     * corresponding subtree. 
     * @n Setting @e deep to TRUE has no effect on other node types because:
     * 
     * - @c ATTRIBUTE_NODE: Attr nodes always carry their child nodes
     *   with them when imported.
     * - @c NOTATION_NODE: Notation nodes have no child nodes.
     * - @c ENTITY_REFERENCE_NODE: The source and destination documents
     *   might define the entity differently. 
     *
     * Since DOM Level 2
     * @param importedNode - the node to import
     * @param deep         - TRUE to import the node and it subtree,
     *                       FALSE to import only the node itself
     *
     * @return The imported node that belongs to this Document.
     * @return In case of an error, produces DOMException:
     * - @c NOT_SUPPORTED_ERR: Raised if the type of node being imported is not
     *            supported.
     */
    virtual Node* importNode(Node* importedNode, bool deep) = 0;

    /**
     * Creates an Element object of the given qualified name and namespace URI.
     * @descr @n Since DOM Level 2
     * @param namespaceURI  - the namespace URI of the element to create
     * @param qualifiedName - the qualified name of the element type to
     *                        instantiate
     * @return A new Element object with the following attributes:
     * 
     * 
     *  - @c Node.nodeName: @e qualifiedName
     *  - @c Node.namespaceURI: @e namespaceURI
     *  - @c Node.prefix: prefix extracted from @e qualifiedName or NULL if no prefix
     *  - @c Node.localName: local name extracted from @e qualifiedName
     *  - @c Element.tagName: @e qualifiedName
     * 
     * @return In case of an error, produces DOMException:
     * - @c INVALID_CHARACTER_ERR: Raised if the specified qualified name
     *   contains an illegal character, per the XML 1.0 specification.
     * - @c  NAMESPACE_ERR: Raised if: 
     *       - @e qualifiedName is malformed
     *       - @e qualifiedName has a prefix and @e namespaceURI
     *       is NULL
     *       - @e qualifiedName has the "xml" prefix and
     *       @e namespaceURI is not "http://www.w3.org/XML/1998/namespace"
     *      .
     *  - @c  NOT_SUPPORTED_ERR: Always thrown if the current document does not
     *   support the "XML" feature because namespaces are defined by XML.
     */
    virtual Element* createElementNS(const ixml::XMLChar* namespaceURI, const ixml::XMLChar* qualifiedName) = 0;

    /**
     * Creates an attribute of the given qualified name and namespace URI.
     * @descr @n Since DOM Level 2
     * @param namespaceURI  - the namespace URI of the attribute to create
     * @param qualifiedName - the qualified name of the attribute to
     *                        instantiate
     * @return A new Attr object with the following attributes:
     * 
     *
     *  - @c Node.nodeName: @e qualifiedName
     *  - @c Node.namespaceURI: @e namespaceURI
     *  - @c Node.prefix: prefix extracted from @e qualifiedName or NULL if no prefix
     *  - @c Node.localName: local name extracted from @e qualifiedName
     *  - @c Attr.name: @e qualifiedName
     *  - @c Node.nodeValue: the empty string
     * 
     * @return In case of an error, produces DOMException:
     * - @c  INVALID_CHARACTER_ERR: Raised if the specified qualified name
     *   contains an illegal character, per the XML 1.0 specification.
     * - @c  NAMESPACE_ERR: Raised if: 
     *       - @e qualifiedName is malformed
     *       - @e qualifiedName has a prefix and @e namespaceURI is NULL
     *       - @e qualifiedName has the "xml" prefix and
     *            @e namespaceURI is not "http://www.w3.org/XML/1998/namespace"
     *       - @e qualifiedName or its prefix is "xmlns" and the
     *       @e namespaceURI is not "http://www.w3.org/2000/xmlns/"
     *      .
     *   - @c  NOT_SUPPORTED_ERR: Always thrown if the current document does not
     *    support the "XML" feature because namespaces are defined by XML.
     */
    virtual Attr* createAttributeNS(const ixml::XMLChar* namespaceURI,const ixml::XMLChar* qualifiedName) = 0;

    /**
     * Returns a NodeList object of all the Element node(s) with the
     * given local name and namespace URI.
     *
     * @descr @n The method lists the Element objects following the order, in which they
     * are encountered in the pre-order traversal of the Document tree.
     *
     * Since DOM Level 2
     * @param namespaceURI - the namespace URI of the elements to match on; the
     *                       value "*" matches all namespaces
     * @param localName    - the local name of the elements to match on; the
     *                       value "*" matches all local names
     * @return A new NodeList object with all matched Element node(s).
     */
    virtual NodeList* getElementsByTagNameNS(const ixml::XMLChar* namespaceURI,const ixml::XMLChar* localName) const = 0;
    
    /**
     * Gets the Element with the ID specified in @e elementId. 
     * 
     * @descr @n Behavior is not defined for more than one element with this ID. 
     * The DOM implementation must have information to define the attributes
     * of type ID. Attributes with the name "ID" are not of type ID unless
     * so defined. Implementations that cannot determine whether attributes are
     * of type ID are expected to return NULL.
     *
     * Since DOM Level 2
     * @param elementId - the unique @c ID value of an element
     * @return The matching element or NULL if no such element
     *         exists.
     */
    virtual Element* getElementById(const ixml::XMLChar* elementId) = 0;

    /**
     * Gets the location of Document.
     * 
     * @descr @n When the Document object supports the feature "HTML", the @c 
     * href attribute of the HTML @c BASE element takes precedence
     * over this attribute.
     *
     * Since DOM Level 3
     * @return The location of the document or NULL if undefined.
     */
    virtual const ixml::XMLChar* getDocumentURI() const = 0;

    /**
     * Sets the location of the document. 
     *
     * @descr @n If the document location is undefined, 
     * or if the document is created by DOMImplementation#createDocument(), 
     * the method sets the location to NULL.
     *
     * Beware that when the Document supports the feature
     * "HTML" , the href attribute of the HTML BASE element takes precedence
     * over this attribute.
     *
     * Since DOM Level 3
     * @param uri - the document uri user set.
     */
    virtual void setDocumentURI(const ixml::XMLChar* uri) = 0;

    /**
     * Provides direct access to the child node that is the root element 
     * of the document.
     * @descr @n  Intel DOM extension.
     * 
     * @return The pointer to the root element node.
     */
    virtual Element* getDocumentElement() = 0;

    /**
     * Creates a Namespace node with the specified prefix and URI.
     *
     * @descr @n  Intel DOM extension.
     * @param prefix - the prefix of Namespace equal to the value 
     *                 of Attr#getLocalName()
     * @param uri    - the URI of Namespace equal to the value
     *                 of Attr#getNodeValue()
     * @return The new Namespace object. 
     */ 
    virtual Namespace* createNamespace(const ixml::XMLChar* prefix,const ixml::XMLChar* uri) = 0;
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif
