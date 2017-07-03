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

/** @file node.h @brief Definition of nodes in DOM
 *
 * The Node interface is the primary datatype for the entire
 * Document Object Model and represents a single node in the document tree.
 *
 * @see The <a href='http://www.w3.org/TR/2000/REC-DOM-Level-2-Core-20001113'>
 * Document Object Model (DOM) Level 2 Core Specification</a>.
 */

#ifndef __COM_INTEL_XML_PARSE_DOM_NODE_H__
#define __COM_INTEL_XML_PARSE_DOM_NODE_H__

#include "include/common/types.h"
#include "include/common/defines.h"
#include "include/util/xmlstring.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

class NodeList;
class Namespace;
class Attr;
class Document;
class NamedNodeMap;

/**
 *
 * Implements the Node interface.
 * 
 * All objects implementing the Node interface expose
 * methods for dealing with child nodes, but not all objects implementing
 * this interface can have child nodes. For example, Text nodes can have
 * no child nodes, and adding a child node raises a DOMException.
 *
 * The Node interface includes the attributes @c 
 * nodeName, @c nodeValue and @c attributes,
 * with values varying according to the node type, as follows:
 * <table border='1'>
 * <tr>
 * <th>Interface</th><th>nodeName</th><th>nodeValue</th><th>attributes</th>
 * </tr>
 * <tr>
 * <td class="TableCell">Attr</td>
 * <td class="TableCell">name of attribute</td>
 * <td class="TableCell">value of attribute</td>
 * <td class="TableCell">NULL</td>
 * </tr>
 * <tr>
 * <td class="TableCell">CDATASection</td>
 * <td class="TableCell">&quot;cdata-section&quot;</td>
 * <td class="TableCell">content of the CDATA Section</td>
 * <td class="TableCell">NULL</td>
 * </tr>
 * <tr>
 * <td class="TableCell">Comment</td>
 * <td class="TableCell">&quot;comment&quot;</td>
 * <td class="TableCell">content of the comment</td>
 * <td class="TableCell">NULL</td>
 * </tr>
 * <tr>
 * <td class="TableCell">Document</td>
 * <td class="TableCell">&quot;document&quot;</td>
 * <td class="TableCell">NULL</td>
 * <td class="TableCell">NULL</td>
 * </tr>
 * <tr>
 * <td class="TableCell">DocumentFragment</td>
 * <td class="TableCell">&quot;document-fragment&quot;</td>
 * <td class="TableCell">NULL</td>
 * <td class="TableCell">NULL</td>
 * </tr>
 * <tr>
 * <td class="TableCell">DocumentType</td>
 * <td class="TableCell">document type name</td>
 * <td class="TableCell">NULL</td>
 * <td class="TableCell">NULL</td>
 * </tr>
 * <tr>
 * <td class="TableCell">Element</td>
 * <td class="TableCell">tag name</td>
 * <td class="TableCell">NULL</td>
 * <td class="TableCell">NamedNodeMap</td>
 * </tr>
 * <tr>
 * <td class="TableCell">Entity</td>
 * <td class="TableCell">entity name</td>
 * <td class="TableCell">NULL</td>
 * <td class="TableCell">NULL</td>
 * </tr>
 * <tr>
 * <td class="TableCell">EntityReference</td>
 * <td class="TableCell">name of entity referenced</td>
 * <td class="TableCell">NULL</td>
 * <td class="TableCell">NULL</td>
 * </tr>
 * <tr>
 * <td class="TableCell">Notation</td>
 * <td class="TableCell">notation name</td>
 * <td class="TableCell">NULL</td>
 * <td class="TableCell">NULL</td>
 * </tr>
 * <tr>
 * <td class="TableCell">ProcessingInstruction</td>
 * <td class="TableCell">target</td>
 * <td class="TableCell">entire content excluding the target</td>
 * <td class="TableCell">NULL</td>
 * </tr>
 * <tr>
 * <td class="TableCell">Text</td>
 * <td class="TableCell">&quot;text&quot;</td>
 * <td class="TableCell">content of the text node</td>
 * <td class="TableCell">NULL</td>
 * </tr>
 * </table>

 * The values @c nodeName, @c nodeValue and @c 
 * attributes are used as a mechanism of getting node information
 * without casting down to the specific derived interface.
 * This returns NULL when the attributes cannot be unambiguously
 * mapped to specific @c nodeType, e.g., @c nodeValue for
 * an Element node or @c attributes for a Comment node. Specialized
 * interfaces may also contain additional and more convenient mechanisms of
 * getting and setting relevant information.
 *
 * Since DOM Level 1
 */
class DOM_EXPORT Node
{
public:

    /**
     * NodeType
     *
     * Since DOM Level 1
     */
    enum NodeType {
        ELEMENT_NODE                = 1,         ///< Element node
        ATTRIBUTE_NODE              = 2,         ///< Attr (attribute) node
        TEXT_NODE                   = 3,         ///< Text node
        CDATA_SECTION_NODE          = 4,         ///< CDATASection node
        ENTITY_REFERENCE_NODE       = 5,         ///< Entity reference node
        ENTITY_NODE                 = 6,         ///< Entity node
        PROCESSING_INSTRUCTION_NODE = 7,         ///< ProcessingInstruction node
        COMMENT_NODE                = 8,         ///< Comment node
        DOCUMENT_NODE               = 9,         ///< Document node
        DOCUMENT_TYPE_NODE          = 10,         ///< Document type node
        DOCUMENT_FRAGMENT_NODE      = 11,         ///< Document fragment node
        NOTATION_NODE               = 12         ///< Notation node
    };

    /** 
     * Destructor
     */
    virtual ~Node() {}

public:

    /**
     * Gets the name of this node depending on its type
     *
     * @descr @n Since DOM Level 1
     * @see The table in the Node class definition.
     */
    virtual    const ixml::XMLChar* getNodeName() const = 0;

    /**
     * Gets the value of this node depending on its type.
     * @descr @n Since DOM Level 1
     */
    virtual const ixml::XMLChar* getNodeValue() const = 0;
    
    /**
     * Gets the enum value representing the type of the underlying object.
     *  @descr @n Since DOM Level 1
     */
    virtual short getNodeType() const = 0;

    /**
     * Gets the parent of this node.
     *
     * @descr @n All nodes except Document, DocumentFragment, and Attr can have a parent.
     * However, if a node has just been created and not yet added to the tree,
     * or if it has been removed from the tree, this method returns a
     * NULL Node.
     *
     * Since DOM Level 1
     * @return The parent Node object.
     *
     */
    virtual Node* getParentNode() const = 0;

    /**
     * Gets the NodeList object with all children of this node.
     *
     * @descr @n If the node has no children, this method returns a NodeList object
     * with no nodes. The content of the returned NodeList is not a static
     * snapshot, it is subject to change, for instance, to reflect changes
     * in the child nodes of this Node. This is true for all
     * NodeList objects, including those that the Document::getElementsByTagName()
     * method returns. 
     *
     * Since DOM Level 1
     * @return The NodeList object with all child nodes of this Node
     * object.
     */
    virtual    NodeList* getChildNodes() const = 0;

    /**
     * Gets the first child of this node.
     * @descr @n Since DOM Level 1
     * @return The requested Node object or NULL
     *         if it is not found.
     */
    virtual    Node* getFirstChild() const = 0;

    /**
     * Gets the last child of this node.
     * @descr @n Since DOM Level 1
     * @return The requested Node object or NULL
     *         if it is not found.
     */
    virtual    Node* getLastChild() const = 0;

    /**
     * Gets the node immediately preceding this node.
     * @descr @n Since DOM Level 1
     * @return The requested Node object or NULL
     *         if it is not found.
     */
    virtual Node* getPreviousSibling() const = 0;

    /**
     * Gets the node immediately following this node.
     * @descr @n Since DOM Level 1
     * @return The requested Node object or NULL
     *         if it is not found.
     */
    virtual Node* getNextSibling() const = 0;

    /**
     * Gets a NamedNodeMap object with the attributes of this node for an
     * Element node.
     * @descr @n Since DOM Level 1
     * @return The requested NamedNodeMap object or NULL
     *         if it is not an Element node.
     */
    virtual    NamedNodeMap* getAttributes() const = 0;

    /**
     * Gets the Document object associated with this node.
     *
     * @descr @n This Document object is also used to create new nodes. 
     *
     * Since DOM Level 1
     * @return The requested Document object or NULL
     *         if this node is a Document or a DocumentType that is not used
     *         with any Document yet.
     */
    virtual Document* getOwnerDocument() const = 0;

    /**
     * Returns a copy of this node.
     *
     * @descr @n This method serves as a generic copy constructor for nodes.
     * For an Element node, it copies all attributes and their values, including
     * those generated by the XML processor to represent default attributes.
     * However, the method does not copy any text that the Element node
     * contains because text is contained in a child Text node. This can be
     * overridden by setting the @e deep parameter to @c 
     * true. For other node types, the method returns a copy of the node.
     *
     * The clone node has no parent: parentNode() returns 
     * NULL. 
     *
     * Since DOM Level 1
     * @param deep - TRUE to recursively clone the subtree of the
     *               specified node, FALSE to clone only the node
     *               itself with attributes for Element nodes 
     * 
     * @return The duplicate node.
     */
    virtual    Node* cloneNode(bool deep=false) const = 0;

    /**
     * Inserts the node @e newChild before the existing child node
     * @e refChild.
     *
     * @descr @n If @e refChild is NULL, this method inserts
     * @e newChild at the end of the list of children.
     * 
     * If @e newChild is a DocumentFragment object, this method inserts
     * all its children in the same order, before @e refChild. If @e 
     * newChild is already in the document tree, it is first removed. 
     *
     * Since DOM Level 1
     * @param newChild - the node to insert
     * @param refChild - the reference node, i.e., the node before which the new
     *                   node must be inserted
     * @return The inserted node.
     *
     * @return In case of an error, produces DOMException:
     * - @c HIERARCHY_REQUEST_ERR: Raised if this node is of a type that does not
     *            allow children of the type of the @e newChild node, or if
     *            the node to insert is one of this node's ancestors.
     * - @c WRONG_DOCUMENT_ERR: Raised if @e newChild was created in a
     *            document different from the one that created this node.
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node or the node being
     *            inserted is read-only.
     * - @c NOT_FOUND_ERR: Raised if @e refChild is not a child of this node.
     */
    virtual    Node* insertBefore(Node *newChild,Node *refChild) = 0;

    /**
     * Replaces the child node @e oldChild with @e newChild
     * in the list of children, and returns the @e oldChild node.
     *
     * @descr @n If @e newChild is a DocumentFragment object,
     * @e oldChild is replaced by all the DocumentFragment object
     * children in the same order.
     *
     * If @e newChild is already in the tree, it is first removed.
     *
     * Since DOM Level 1
     * @param newChild - the new node to put in the child list
     * @param oldChild - the node being replaced in the child list
     * @return The node replaced.
     * @return In case of an error, produces DOMException:
     * - @c HIERARCHY_REQUEST_ERR: Raised if this node is of a type that does not
     *            allow children of the type of the @e newChild node, or if
     *            the node to put in is one of this node's ancestors.
     * - @c WRONG_DOCUMENT_ERR: Raised if @e newChild was created in a
     *            document different from the one that created this node.
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node or the new node is
     *            read-only.
     * - @c NOT_FOUND_ERR: Raised if @e oldChild is not a child of
     *            this node.
     */
    virtual    Node* replaceChild(Node *newChild,Node *oldChild) = 0;

    /**
     * Removes the child node indicated by @e oldChild from the list
     * of children, and returns it.
     * @descr @n Since DOM Level 1
     * @param oldChild - the node being removed
     * @return The node removed.
     * @return In case of an error, produces DOMException:
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node is read-only.
     * - @c NOT_FOUND_ERR: Raised if @e oldChild is not a child of
     *   this node.
     */
    virtual    Node* removeChild(Node *oldChild) = 0;


    /**
     * Adds the node @e newChild to the end of the list of children of
     * this node.
     *
     * @descr @n If the added node is a DocumentFragment object, the entire contents
     * of the document fragment are moved into the child list of this node.
     *
     * If @e newChild is already in the tree, it is
     * first removed.
     *
     * Since DOM Level 1
     * @param newChild - the node to add
     * @return The node added.
     * @return In case of an error, produces DOMException:
     * - @c HIERARCHY_REQUEST_ERR: Raised if this node is of a type that does not
     *            allow children of the type of the @e newChild node, or if
     *            the node to append is one of this node's ancestors.
     * - @c WRONG_DOCUMENT_ERR: Raised if @e newChild was created in a
     *            document different from the one that created this node.
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node or the appended
     *            node is read-only.
     */
    virtual    Node* appendChild(Node *newChild) = 0;

    /**
     * Checks whether a node has children.
     * @descr @n Since DOM Level 1
     * @return TRUE if the node has children; otherwise,
     *         FALSE.
     */
    virtual    bool hasChildNodes() const = 0;

    /**
     * Sets the value of the node.
     *
     * @descr @n A node that can have a value @c nodeValue can also accept
     * requests to set it to a string. The exact response to this request
     * varies depending on the node type; for example, an attribute node stores
     * its values in its child nodes and has to replace them with a new Text
     * node holding the replacement value.
     *
     * For most types of Node, the value is NULL and an attempt to set
     * it throws DOMException#NO_MODIFICATION_ALLOWED_ERR. The same exception
     * is also thrown if the node is read-only.
     *
     * Since DOM Level 1
     * @param nodeValue - The value of node
     * @see getNodeValue()
     */
    virtual    void setNodeValue(const ixml::XMLChar* nodeValue) = 0;

    /**
     * Changes all Text nodes in the full depth of the sub-tree underneath
     * this Node into a "normal" form.
     *
     * @descr @n This method operates with all Text nodes, including attribute nodes, and
     * changes them so that individual Text nodes are separated by markup only:
     * by tags, comments, processing instructions, CDATA sections, and entity 
     * references. As a result of this operation, no adjacent or empty Text
     * nodes remain. 
     * 
     * You can use this operation to ensure that the DOM view of a document 
     * is the same as if it were saved and reloaded. The check proves helpful
     * when you need to use operations that depend on a particular document tree
     * structure, such as XPointer lookups.
     *
     * @note  When the document contains CDATASection objects, this method alone
     * may be insufficient because XPointers do not differentiate between Text
     * nodes and CDATASection nodes.
     *
     * Since DOM Level 2
     */
    virtual void normalize() = 0;

    /**
     * Tests whether the DOM implementation contains a specific feature and
     * that the feature is supported by this node.
     * 
     * @descr @n The method takes the string values of the feature and version, and if
     * the version is not set, returns TRUE.
     *
     * Since DOM Level 2
     * @param feature - the string of the feature to test; the same name as the
     *                  one passed to the method DOMImplementation#hasFeature()
     * @param version - the version number of the feature to test; in DOM
     *                  Level 2 Specification version 1, this is the string "2.0"
     *                  
     * @return TRUE if the specified feature is supported
     * on this node; otherwise, FALSE.
     */
    virtual bool isSupported(const ixml::XMLChar* feature,const ixml::XMLChar* version) const = 0;
    
    /**
     * Gets the namespace URI of this node.
     * 
     * @descr @n The namespace URI is the URI given to a Node at creation time, and not
     * a computed value that is the result of a namespace lookup operation based
     * on an examination of the namespace declarations in scope. 
     *
     * For nodes of any type other than @c ELEMENT_NODE and
     * @c ATTRIBUTE_NODE and nodes created with a DOM Level 1 method,
     * such as Document#createElement(), the URI is always NULL.
     *
     * Since DOM Level 2
     * @return The namespace URI or NULL if it is not specified.
     */
    virtual    const ixml::XMLChar* getNamespaceURI() const = 0;

    /**
     * Gets the namespace prefix of this node.
     * @descr @n Since DOM Level 2
     * @return The namespace prefix or NULL if it is not specified.
     *
     */
    virtual const ixml::XMLChar* getPrefix() const = 0;

    /**
     * Gets the local part of the qualified name of this node.
     * 
     * @descr @n For nodes created with a DOM Level 1 method, such as
     * Document#createElement, the returned value is NULL.
     *
     * Since DOM Level 2
     * @return The requested value or NULL.
     */
    virtual const ixml::XMLChar* getLocalName() const = 0;

    /**
     * Sets the namespace prefix of this node.
     * 
     * @descr @n Setting this attribute changes the @c nodeName attribute that
     * contains the @c qualifiedName, @c tagName and @c 
     * name attributes of the Element and Attr nodes, when applicable.
     *
     * Changing the prefix of an attribute that is known to have a default value
     * does not make a new attribute with the default value and the original
     * prefix appear because @c namespaceURI and @c localName
     * do not change.
     *
     *  Since DOM Level 2
     * @param prefix - the prefix of this node
     * @return In case of an error, produces DOMException:
     * - @c INVALID_CHARACTER_ERR: Raised if the specified prefix contains
     *                          an illegal character.
     *  - @c NO_MODIFICATION_ALLOWED_ERR: Raised if this node is read-only.
     * - @c NAMESPACE_ERR: Raised if one of the following conditions is true: 
     *      - @e prefix is malformed
     *      - @c namespaceURI of this node is NULL
     *      - @e prefix is "xml" and the @c 
     *      namespaceURI of this node is not "http://www.w3.org/XML/1998/namespace"   
     *      - @e prefix is "xmlns", this is an Attr node and the
     *      @c namespaceURI of this node is not "http://www.w3.org/2000/xmlns/"
     *      - this is an Attr node and the @c qualifiedName of this node
     *      is "xmlns"
     */
    virtual    void setPrefix(const ixml::XMLChar*  prefix) = 0;

    /**
     * Checks whether this Element node has attributes.
     * @descr @n Since DOM Level 2
     * @return TRUE if this node has attributes; otherwise,
     *         FALSE.
     */
    virtual bool hasAttributes() const = 0;

    /**
     * Checks whether two nodes are the same.
     * 
     * @descr @n This method provides a way of checking whether two Node references
     * returned by the implementation reference the same object. When it is
     * so, even if through a proxy, the references can be used interchangeably,
     * so that all attributes have the same values and calling the same DOM
     * method on either reference always has exactly the same effect.
     *
     * Since DOM Level 3
     * @param other - the node to check against
     * @return TRUE if the nodes are the same; otherwise,
     *         FALSE.
     */
    virtual    bool isSameNode(const Node* other) const = 0;

    /**
     * Checks whether two nodes are equal.
     *
     * @descr @n This method checks nodes for equality, not sameness (whether two nodes
     * are pointers to the same object), which can be tested with isSameNode(). 
     * All nodes that are the same are also equal, though the reverse may not
     * be true.
     *
     * Two nodes are equal if and only if the following conditions are
     * satisfied: 
     *  - The two nodes are of the same type.
     *  - The two nodes have equal string attributes @c nodeName,
     *    @c localName, @c namespaceURI, @c prefix,
     *    @c nodeValue and @c baseURI. 
     *    Equality of these components means that they are both NULL
     *    or have the same length and are character-for-character identical.
     *  - The @c attributes NamedNodeMap are equal.
     *    This means that they are both NULL or have the same
     *    length, and each node in one map matches an equal node in the other map,
     *    although not necessarily at the same index.
     *  - The @c childNodes NodeList are
     *    equal. This means that the child nodes are both NULL or have the
     *    same length and contain equal nodes at the same index. 
     * 
     * Additionally, for two DocumentType nodes to be equal, the following
     * conditions must be satisfied: 
     * 
     *  - The two nodes have equal string attributes @c publicId,
     *  @c systemId and @c internalSubset.
     *  - The @c entities NamedNodeMap are equal.
     *  - The @c notations NamedNodeMap are equal.
     * 
     * At the same time, the following do not relate to the equality of two nodes:
     * the @c ownerDocument attribute, the @c specified
     * attribute for Attr nodes, the @c isWhitespaceInElementContent
     * attribute for Text nodes, as well as any user data or event listeners
     * registered on the nodes.
     *
     * @note  Normalization can affect equality; to avoid this, call normalize()
     * before comparing the nodes.
     *
     * Since DOM Level 3
     * @param node - the node to compare against
     * @return TRUE if the nodes and possibly subtrees are equal;
     *         otherwise, FALSE.
     */
    virtual bool isEqualNode(const Node* node) const = 0;

    /**
     * Gets the absolute base URI of this node.
     * 
     * @descr @n The absolute base URI of this node or NULL if undefined.
     * This value is computed according to Base URIs as defined in the DOM Level 3
     * core standard. However, when the DOM Document supports the feature "HTML", 
     * the base URI is first computed using the value of the @c href
     * attribute of the HTML BASE element,if any; otherwise, the value of the 
     * @c documentURI attribute from the Document interface.
     *
     * Since DOM Level 3
     * @return The base URI for this node or NULL if undefined.
     */
    virtual const ixml::XMLChar* getBaseURI() const = 0;

    /**
     * Gets the text content of this node and its descendants. 
     * 
     * @descr @n With this method, no serialization is performed, whitespace is not
     * normalized, and the returned string contains no markup or white 
     * spaces in Element content.
     *
     * The string returned by the method is made of the text content of this node
     * depending on its type, as defined below:
     * - @c ELEMENT_NODE, @c ENTITY_NODE, @c ENTITY_REFERENCE_NODE,
     *   @c DOCUMENT_FRAGMENT_NODE: Concatenation of the @c textContent
     *   attribute value of every child node, excluding nodes @c COMMENT_NODE and
     *   @c PROCESSING_INSTRUCTION_NODE nodes
     * - @c ATTRIBUTE_NODE, @c TEXT_NODE, @c CDATA_SECTION_NODE, @c COMMENT_NODE, 
     *   @c PROCESSING_INSTRUCTION_NODE: @c nodeValue
     * - @c DOCUMENT_NODE, @c DOCUMENT_TYPE_NODE, @c NOTATION_NODE: NULL
     *
     * Since DOM Level 3
     * @see setTextContext()
     * @param content - The content of text
     *
     * @return In case of an error, produces DOMException:
     * - @c DOMSTRING_SIZE_ERR: Raised when the method should return more characters than
     *   fit in the @c XMLString variable on the implementation platform.
     *   
     */
    virtual    void getTextContent(iutil::XMLString& content) const = 0;

    /**
     * Removes any children this node may have and, if the 
     * new string is not empty or NULL, replaces it with a single Text 
     * node containing the string this attribute is set to. 
     * 
     * @descr @n No parsing is performed, the input string is taken as pure textual content.  
     * 
     * Since DOM Level 3 
     * @see getTextContext()
     * 
     * @param textContent - The content of text
     *
     * @return In case of an error, produces DOMException:
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised when the node is read-only.  
     */
    virtual    void setTextContent(const ixml::XMLChar*  textContent) = 0;

    /**
     * Searches for the prefix associated with the given namespace URI
     * starting from this node.
     *
     * @descr @n If more than one prefix is  associated with the namespace prefix, the
     * returned namespace prefix depends on the implementation.
     *
     * Since DOM Level 3
     * @param namespaceURI - the namespace URI to look for
     * @return An associated namespace prefix if found; otherwise, 
     *   NULL.
     */
    virtual const ixml::XMLChar* lookupPrefix(const ixml::XMLChar* namespaceURI) const = 0;

    /**
     * Checks whether the specified @e namespaceURI is the
     * default namespace.
     * @descr @n Since DOM Level 3
     * @param namespaceURI - the namespace URI to look for
     * @return  TRUE if the specified @e namespaceURI
     *   is the default namespace; otherwise, FALSE.
     */
    virtual    bool isDefaultNamespace(const ixml::XMLChar* namespaceURI) const = 0;

    /**
     * Searches for the namespace URI associated with the given prefix starting 
     * from this node.
     * @descr @n Since DOM Level 3
     * @param prefix - the prefix to look for; if NULL, the method 
     *                 returns the default namespace URI if any
     *
     * @return The associated namespace URI or NULL if none is found.
     */
    virtual const ixml::XMLChar* lookupNamespaceURI(const ixml::XMLChar* prefix) const  = 0;

public:
    /**
     * Compares the Node order.
     * 
     * @descr @n If the DOM tree is not changed after it is built up by DOMParser, the order 
     * of nodes is defined by their position in the original document.
     *
     * @note  After the DOM tree is updated, comparison between the order in
     * the original node and the updated one or between two updated ones returns
     * meaningless results.
     *
     *  Intel DOM extension
     * @param other - the pointer to the node that needs to be compared
     * @return @c -1 if the original document position of the @e 
     * other node is after that of @e this node, @c 1
     * if it is before that of @e this node,  
     * @c 0 if the @e other node is just @e this node, and
     * @c -2 if the two nodes' order are undefined.   
     */
    virtual short compareNodeOrder(const Node* other) const = 0;

    /**
     * Gets the line number of the node in the original XML document.
     * 
     * @descr @n For an Element node, the position is the line number of the start tag,
     * for a Text node, the position is the last line where it appears.
     *
     * @note  If you have made any changes to the XML Document without changing
     * the position of the Node, include @c setStripWhiteSpace
     * when parsing the Document. 
     *
     * Intel DOM extension
     *
     * @return The requested line number.
     */    
    virtual unsigned int getPosition() const = 0;
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif

