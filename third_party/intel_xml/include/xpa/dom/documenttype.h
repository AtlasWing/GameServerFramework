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
 * @file documenttype.h @brief Definition of the document type
 *
 * Each Document can have a @c doctype attribute, with the DocumentType value in it.
 * Only one document type is allowed for a Document.
 */

#ifndef __COM_INTEL_XML_PARSE_DOM_DOCUMENTTYPE_H__
#define __COM_INTEL_XML_PARSE_DOM_DOCUMENTTYPE_H__

#include "include/xpa/dom/node.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

/**
 * Implements the document type definition interface.
 *
 * @descr @n According to the <a href='http://www.w3.org/TR/2000/REC-DOM-Level-2-Core-20001113'>
 * Document Object Model (DOM) Level 2 Core Specification</a>,
 * DocumentType provides an interface to the list
 * of entities that are defined for the document. You cannot
 * edit DocumentType nodes, because the effect of namespaces and XML schema efforts
 * on the document type definition (DTD) representation are not clearly
 * defined at the moment.
 *
 * A Document object can have only one @doctype attribute definition;
 * to get the DocumentType of a document, call Document::getDoctype().
 *
 * Since DOM Level 1
 */
class DOM_EXPORT DocumentType : public Node 
{
public:
    /**
     * Destructor
     */
    virtual ~DocumentType() {}

    /**
     * Gets the name of the document type definition (DTD) immediately following
     * the @c DOCTYPE keyword.
     *
     * @descr @n Since DOM Level 1
     */
    virtual const ixml::XMLChar* getName() const = 0;

    /**
     * Gets NamedNodeMap containing the general external and internal entities
     * declared in the DTD.
     *
     * @descr @n This method excludes parameter entities and discards duplicates,
     * for example:
     * @code
     * <!DOCTYPE
     * ex SYSTEM "ex.dtd" [ <!ENTITY foo "foo"> <!ENTITY bar
     * "bar"> <!ENTITY bar "bar2"> <!ENTITY  baz "baz">
     * ]> <ex/>
     * @endcode
     * In the example, the interface provides access to @c foo
     * and the first declaration of @c bar but not the second
     * declaration of @c bar or @c baz. Every node in
     * this map also implements the Entity interface.
     *
     * According to the DOM Level 2 specification, @c entities cannot
     * be edited.
     *
     * Since DOM Level 1
     */
    virtual NamedNodeMap* getEntities() const = 0;


    /**
     * Gets NamedNodeMap containing the notations declared in the DTD. 
     *
     * @descr @n This method discards any duplicates. Every node in this map also implements
     * the Notation interface. 
     *
     * According to the DOM Level 2 specification, @c entities cannot
     * be edited. 
     *
     * Since DOM Level 1
     */
    virtual NamedNodeMap* getNotations() const = 0;

    /**
     * Gets the public identifier of the external subset.
     * @descr @n Since DOM Level 2
     * @return The public identifier of the external subset.

     */
    virtual const ixml::XMLChar* getPublicId() const = 0;

    /**
     * Gets the system identifier of the external subset.
     * @descr @n Since DOM Level 2
     *
     * @return The system identifier of the external subset.
     */
    virtual const ixml::XMLChar* getSystemId() const = 0;

    /**
     * Gets the internal subset as a string.
     *
     * @descr @n This method returns the internal subset as a string with no delimiting
     * square brackets. The actual content returned depends on the amount of
     * information available to the implementation. This may vary based on
     * different parameters, including the XML processor that built the document.
     *
     * Since DOM Level 2
     *
     * @return The internal subset as a string or NULL if there is
     *         none.
     */
    virtual const ixml::XMLChar* getInternalSubset() const = 0;

};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif
