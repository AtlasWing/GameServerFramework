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
 * @file domimplementation.h @brief Interface for implementing a DOM Parser
 *
 * DOMImplementation provides an interface for performing
 * operations that are independent of the DOM instance.
 */



#ifndef __COM_INTEL_XML_PARSE_DOM_DOMIMPLEMENTATION_H__
#define __COM_INTEL_XML_PARSE_DOM_DOMIMPLEMENTATION_H__

#include "include/common/types.h"
#include "include/common/defines.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

class Document;
class DOMParser;
class DOMWriter;
class DocumentType;

/**
 * Provides a number of methods for performing operations independent of the
 * instance of the document object model.
 */
class DOM_EXPORT DOMImplementation
{
public:    
    /** Destructor. */
    virtual ~DOMImplementation() {}

public:

    /**
     * Tests whether the DOM implementation contains a specific feature.
     *
     * @descr @n The method takes the feature name and version as input parameters, where
     * the @e name of the feature to test must be an XML case-sensitive name 
     * and must conform to the DOM Level 2 specification. To avoid possible
     * conflicts, as a convention, names referring to features defined outside
     * the DOM specification should be unique.
     *
     * According to DOM Level 2 specification, the @e version string must
     * be "2.0" or "1.0". If the version is not specified, the method returns
     * TRUE.
     *
     * Since DOM Level 1
     * @param feature - the case-sensitive name of the feature to test
     * @param version - the version number of the feature to test
     *
     * @return TRUE if the feature is implemented in the
     *         specified version; otherwise FALSE.
     */
    virtual bool  hasFeature(const ixml::XMLChar* feature,const ixml::XMLChar* version) const = 0;
    
    /**
     * Creates an empty DocumentType node. 
     * 
     * @descr @n Entity declarations and notations are not made available, and entity
     * reference expansions and default attribute additions do not occur.
     * A future version of the DOM standard may provide a way for populating
     * a DocumentType object.
     *
     * Since DOM Level 2
     * @param qualifiedName - the qualified name of the document type 
     *                        to be created
     * @param publicId      - the external subset public identifier
     * @param systemId      - the external subset system identifier
     *
     * @return A new DOM DocumentType node with
     *   @c ownerDocument set to NULL.
     *
     * @return In case of an error, produces DOMException:
     * - @c INVALID_CHARACTER_ERR: Raised if the specified qualified name
     *            contains an illegal character.
     * - @c NAMESPACE_ERR: Raised if the @c qualifiedName is
     *            malformed.
     * - @c NOT_SUPPORTED_ERR: Can be raised by a DOM implementation
     *            that does not support the @c "XML" feature, in case
     *            the implementation is set not to support this method; other
     *            future features might also demand support for this method;
     *            please consult the feature definition to check whether it
     *            requires this method.
     */
     virtual DocumentType* createDocumentType(const ixml::XMLChar* qualifiedName,
                                              const ixml::XMLChar* publicId,
                                              const ixml::XMLChar* systemId) = 0;
    
    /**
     * Creates a Document object of the specified type with its document
     * element.
     *     
     * @descr @n When @e doctype is not NULL, its @c ownerDocument
     *  attribute is set to the document being created.
     *
     * Since DOM Level 2
     * @param namespaceURI  - the namespace URI of the document element 
     *                        to create
     * @param qualifiedName - the qualified name of the document element
     *                        to be created
     * @param doctype       - the type of document to be created or 
                             NULL

     * @return A new Document object.
     *
     * @return In case of an error, produces DOMException:
     * - @c INVALID_CHARACTER_ERR: Raised if the specified qualified name
     *            contains an illegal character
     * - @c NAMESPACE_ERR: Raised if: 
     *            - @e qualifiedName is malformed
     *            - @e qualifiedName has a prefix and 
     *            @e namespaceURI is NULL
     *            - @e qualifiedName has the "xml" prefix and
     *            @e namespaceURI is not "http://www.w3.org/XML/1998/namespace"
     *            - the DOM implementation does not support the "XML"
     *             feature but a non-null namespace URI was provided,
     *             because namespaces have been defined by XML.
     *            WRONG_DOCUMENT_ERR: Raised if @c doctype has
     *            already been used with a different document or was created
     *            from a different implementation.
     *          .
     * - @c NOT_SUPPORTED_ERR: May be raised by a DOM implementation
     *            that does not support the "XML" feature, if it is set not to 
     *            support this method; other future features might also demand
     *            support for this method; please consult the feature definition 
     *            to check whether it requires this method.
     */
    virtual Document* createDocument(const ixml::XMLChar* namespaceURI,
                                     const ixml::XMLChar* qualifiedName,
                                     DocumentType *doctype) = 0;

    
    /**
     * Creates a DOMParser object. 
     * 
     * @descr @n The DOM parser is used to parse a XML document into a DOM tree.
     *
     * Intel DOM extension
     * @return - A new DOMParser object created
     */
    virtual DOMParser* createDOMParser() = 0;

    /**
     * Creates a DOMWriter object. 
     *
     * @descr @n The DOM writer is used to serialize a DOM tree back to a XML document.
     *
     * Intel DOM extension
     * @return - A new DOMWriter oblect created
     */
    virtual DOMWriter* createDOMWriter() = 0;

    /**
     * Releases the DOMParser object created by DOMImplementation.
     *
     * @descr @n  Intel DOM extension
     * @param parser - the object will be released.
     */
    virtual void releaseDOMParser(DOMParser* parser) = 0;

    /**
     * Releases the DOMWriter object created by DOMImplementation.
     *
     * @descr @n Intel DOM extension
     * @param writer - the object will be released.
     */
    virtual void releaseDOMWriter(DOMWriter* writer) = 0;

    /**
     * Releases the Document object.
     *
     * @descr @n Intel DOM extension
     * @param doc - the object will be released.
     */
    virtual void releaseDocument(Document* doc) = 0;

    /**
     * Releases the DocumentType object.
     *
     * @descr @n Intel DOM extension
     * @param docType - the object will be released.
     */
    virtual void releaseDocumentType(DocumentType* docType) = 0;
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif

