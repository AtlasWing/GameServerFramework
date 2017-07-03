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
/** @file source.h The Source interface
 * 
 * @descr @n Source and Result mainly serve as
 * input and output parameters during schema validation and XSLT transformation.
 * Source is also used as input for XPath processing.
 *
 */
#ifndef __COM_INTEL_XML_SOURCE_H__
#define __COM_INTEL_XML_SOURCE_H__

#include "include/common/types.h"
#include "include/util/xmlstring.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN
class Node;
INTEL_XML_PARSE_DOM_NAMESPACE_END

INTEL_XML_PARSE_SAX_NAMESPACE_BEGIN
class SAXParser;
INTEL_XML_PARSE_SAX_NAMESPACE_END

INTEL_XML_PARSE_XMLDOC_NAMESPACE_BEGIN
class XMLDocument;
INTEL_XML_PARSE_XMLDOC_NAMESPACE_END

INTEL_XML_UTIL_NAMESPACE_BEGIN
class Reader;
INTEL_XML_UTIL_NAMESPACE_END

INTEL_XML_NAMESPACE_BEGIN

/** The enumerations for all possible Source types. */
typedef enum {
    DOM,    ///< DOM source/result type
    SAX,    ///< SAX source/result type 
    STREAM, ///< Stream source/result type
    XMLDOC, ///< XMLDoc source/result type
    UNKNOWN ///< Base Source that has only @c systemId and @c publicId
} SourceType;

/**
 * Is the base class for all sources.
 */
class COMMON_EXPORT Source {
public:

    /**
     * Default constructor.
     */
    Source();
    /**
     * Creates a new source with @e systemId, 
     * which indicates URI.
     *
     * @param systemId - the source URI
     */
    Source(const XMLChar *systemId);

    /**
     * Destructor.
     */  
    virtual ~Source();

    /**
     * Gets the type of this Source object.
     *
     * @descr @n This method returns @c UNKNOWN if the calling 
     * instance is not SAXSource, DOMSource, StreamSource, or XMLDocSource.
     * This is to avoid the case when user code creates an instance of the base class 
     * Source instead of a specfic one.
     * 
     * @return Enumeration @c UNKNOWN if called from base class. 
     */
    virtual SourceType type() const;

    /**
     * Gets the public identifier set with setPublicId().
     *
     * @return The public identifier string set with setPublicId().
     */
    virtual const XMLChar* getPublicId();

    /**
     * Sets the public identifier for this Source.
     *
     * @param publicId - the public identifier string
     */
    virtual void setPublicId(const XMLChar* publicId);

    /**
     * Gets the system identifier set with setSystemId().
     *
     * @return A string that conforms to the URI syntax.
     */
    virtual const XMLChar* getSystemId();

    /**
     * Sets the system ID from URI.
     *
     * @param uri - a string conforming to the URI syntax
     */
    virtual void setSystemId(const XMLChar* uri);

protected:
    /**
     * The system identifier.
     */
    iutil::XMLString _systemId;
    /**
     * The public identifier.
     */
    iutil::XMLString _publicId;
};

/**
 * Contains the DOM tree as source.
 */
class COMMON_EXPORT DOMSource : public Source {
public:

    /**
     * Default constructor.
     */
    DOMSource();

    /**
     * Creates a new source with a DOM node.
     *
     * @descr @n The operation on this source is applied to the subtree rooted
     * at this node.
     *
     * @param node - the node representing a Source DOM tree
     */
    DOMSource(idom::Node* node);

    /**
     * Creates a new source with a DOM node and with @e systemId.
     * @descr @n The URL resolving behavior may vary between different components of 
     * Intel(R) XML Software Suite. Please check the points where DOMSource is used 
     * for specifics. 
     *
     * @param node - the node representing a Source DOM tree
     * @param systemId - the system identifier 
     */
    DOMSource(idom::Node* node, const XMLChar* systemId);

    /**
     * Destructor.
     */
    virtual ~DOMSource();
    
   /**
     * Gets the type of this Source object.
     *
     * @descr @n This method returns @c UNKNOWN if the calling 
     * instance is not SAXSource, DOMSource, StreamSource, or XMLDocSource.
     * This is to avoid the case when user code creates an instance of the base class 
     * Source instead of a specfic one.
     * 
     * @return Enumeration @c DOM if called from the base class.
     */
    virtual SourceType type() const;

    /**
     * Gets the node representing a Source DOM tree.
     *
     * @return The node representing a Source DOM tree.
     */
    virtual idom::Node* getNode();


    /**
     * Sets the node representing a Source DOM tree.
     *
     * @param node - the node representing a Source DOM tree
     */
    virtual void setNode(idom::Node* node);

    /**
     * Gets the base ID (URL or system ID) from where URLs will be resolved.
     * 
     * @descr @n The URL resolving behavior may vary between different components of 
     * Intel(R) XML Software Suite. Please check the points where DOMSource is used 
     * for specifics.   
     *
     * @return A string conforming to the URI syntax.
     */
    virtual const XMLChar* getSystemId();

    /**
     * Sets the base ID (URL or system ID) from where URLs will be resolved.
     * 
     * @descr @n The URL resolving behavior may vary between different components of 
     * Intel(R) XML Software Suite. Please check the points where DOMSource is used 
     * for specifics.  
     *
     * @param systemId - a string conforming to the URI syntax
     */
    virtual void setSystemId(const XMLChar* systemId);

protected:
     /** 
      * The node representing a Source DOM tree.
      */
    idom::Node* _root;
};

/**
 * Contains the SAX event as input source.
 */
class COMMON_EXPORT SAXSource : public Source {
public:

    /**
     * Default constructor.
     */
    SAXSource();

    /**
     * Creates a new source with @e systemId.
     *
     * @descr @n Usage of this object may vary between different 
     * components of Intel(R) XML Software Suite. Please check the points where 
     * SAXSource is used for specifics. 
     *
     * @param systemId - a string conforming to the URI syntax
     */
    SAXSource(const XMLChar* systemId);

    /**
     * Creates SAXSource, using the Reader.
     * 
     * @descr @n Usage of this object may vary between different 
     * components of Intel(R) XML Software Suite. Please check the points where 
     * SAXSource is used for specifics.   
     *
     * @param reader - an input source passed to the SAXParser
     *                 to parse
     */
    SAXSource(iutil::Reader* reader);

    /**
     * Creates SAXSource, using the reader and a base URI.
     *
     * @descr @n Usage of this object may vary between different 
     * components of Intel(R) XML Software Suite. Please check the points where 
     * SAXSource is used for specifics.  
     *
     * @param reader   - an input source passed to SAXParser to
     *                   parse
     * @param systemId - the base URI
     */
    SAXSource(iutil::Reader* reader, const XMLChar* systemId);

    /**
     * Creates SAXSource, using SAXParser and
     * Reader.
     *
     * @descr @n Usage of this object may vary between different 
     * components of Intel(R) XML Software Suite. Please check the points where 
     * SAXSource is used for specifics. 
     *
     * @param parser - SAXParser to be used for SAX parse
     * @param reader - an input source passed to SAXParser to parse
     */
    SAXSource(isax::SAXParser* parser, iutil::Reader* reader);

    /**
     * Creates SAXSource, using SAXParser and
     * reader and system identifier.
     *
     * @descr @n Usage of this object may vary between different 
     * components of Intel(R) XML Software Suite. Please check the points where 
     * SAXSource is used for specifics. 
     *
     * @param parser - SAXParser to be used for SAX parse
     * @param reader - an input source passed to SAXParser to parse
     * @param systemId - the system identifier
     */
    SAXSource(isax::SAXParser* parser, iutil::Reader* reader, const XMLChar* systemId);

    /**
     * Destructor.
     */
    virtual ~SAXSource();

    /**
     * Gets the type of this Source object.
     *
     * @descr @n This method returns @c UNKNOWN if the calling 
     * instance is not SAXSource, DOMSource, StreamSource, or XMLDocSource.
     * This is to avoid the case when user code creates an instance of the base class 
     * Source instead of a specfic one.
     * 
     * @return Enumeration SAX if called from base class. 
     */
    virtual SourceType type() const;

    /**
     * Gets the sysem identifier set with setSystemId() or the onstructor method.
     *
     * @return A string conforming to the URI syntax.
     */
    virtual const XMLChar* getSystemId();

    /**
     * Sets the system identifier for this Source.
     *
     * @descr @n Usage of this object may vary between different 
     * components of Intel(R) XML Software Suite. Please check the points where 
     * SAXSource is used for specifics. 
     *
     * @param systemId - a string that conforms to the URI syntax
     */
    virtual void setSystemId(const XMLChar* systemId);

    /**
     * Gets SAXParser set with setSAXParser()
     * or constructor.
     *
     * @descr @n Usage of this object may vary between different 
     * components of Intel(R) XML Software Suite. Please check the points where 
     * SAXSource is used for specifics.
     *
     * @return The SAXParser set with setSAXParser() or constructor.
     */
    virtual isax::SAXParser* getSAXParser();

    /**
     * Sets SAXParser to be used for the Source.
     *
     * @param parser - the SAXParser for this input source
     */
    virtual void setSAXParser(isax::SAXParser* parser);

    /**
     * Gets the source reader for the input source.
     *
     * @descr @n Usage of this object may vary between different 
     * components of Intel(R) XML Software Suite. Please check the points where 
     * SAXSource is used for specifics.
     * 
     * @return An input reader passed to SAXParser to parse.
     */
    virtual iutil::Reader* getReader();

    /**
     * Sets the reader to be used for the SAX Source.
     *
     * @descr @n Usage of this object may vary between different 
     * components of Intel(R) XML Software Suite. Please check the points where 
     * SAXSource is used for specifics.
     * 
     * @param reader - an input source passed to the SAXParser
     *                 to parse
     */
    virtual void setReader(iutil::Reader* reader);
    
    /**
     * Gets the public identifier for this input source.
     *
     * @return The public identifier for this input source.
     */
    virtual const XMLChar* getPublicId(); 

    /**
     * Sets the public identifier for this input source.
     *
     * @param publicId - the public identifier for this input source
     */ 
    virtual void setPublicId(const XMLChar* publicId); 

protected:
    /**
     * The SAXParser for this input source.
     *
     * @descr @n Usage of this object may vary between different 
     * components of Intel(R) XML Software Suite. Please check the points where 
     * SAXSource is used for specifics.
     */
    isax::SAXParser* _parser;
    /**
     * The Reader for this input source.
     *
     * @descr @n Usage of this object may vary between different 
     * components of Intel(R) XML Software Suite. Please check the points where 
     * SAXSource is used for specifics.
     */
    iutil::Reader* _reader;
};

/**
 * Contains the stream as input source.
 *
 * @descr @n The input of StreamSource is identified by
 * @c _systemId and @c _publicId.
 */
class COMMON_EXPORT StreamSource : public Source {
public:

    /**
     * Default constructor.
     */
    StreamSource();

    /**
     * Constructs StreamSource from a file addressed by URI.
     *
     * @param systemId - a string conforming to the URI syntax
     */
    StreamSource(const XMLChar *systemId);

    /**
     * Constructs StreamSource from a reader.
     *
     * @descr @n Transformer or TransformFactory use
     * this reader to get the XML stream.
     *
     * @param reader - a valid reader reference to an XML
     *                 character stream
     */
    StreamSource(iutil::Reader* reader);

    /**
     * Constructs StreamSource from a reader and URI.
     *
     * @descr @n Transformer or TransformFactory use
     * this reader to get the XML stream.
     *
     * @param reader - a valid reader reference to an XML character
     *                 stream
     *
     * @param systemId - a string conforming to the URI syntax
     */
    StreamSource(iutil::Reader* reader, const XMLChar *systemId);

    /**
     * Destructor.
     */
    virtual ~StreamSource();

    /**
     * Gets the type of this Source object.
     *
     * @descr @n This method returns @c UNKNOWN if the calling 
     * instance is not SAXSource, DOMSource, StreamSource, or XMLDocSource.
     * This is to avoid the case when user code creates an instance of the base class 
     * Source instead of a specfic one.
     * 
     * @return Enumeration @c STREAM if called from the base class. 
     */
    virtual SourceType type() const;

    /**
     * Gets the public identifier set with setPublicId().
     *
     * @return The public identifier.
     */
    virtual const XMLChar* getPublicId();

    /**
     * Sets the public identifier for this Source.
     *
     * @param publicId - the public identifier
     */
    void setPublicId(const XMLChar* publicId);

    /**
     * Gets the source reader that was set with setReader().
     *
     * @return A reader reference to an XML character stream.
     */
    iutil::Reader* getReader();

    /**
     * Sets the input to be a source reader.
     *
     * @param reader - a valid reader reference to an XML character
     *                 stream
     */
    void setReader(iutil::Reader* reader);

    /**
     * Gets the system identifier set with setSystemId().
     *
     * @return A string that conforms to the URI syntax.
     */
    virtual const XMLChar* getSystemId();

    /**
     * Sets the system ID from URI.
     *
     * @param uri - a string conforming to the URI syntax
     */
    virtual void setSystemId(const XMLChar* uri);

protected:
    /**
     * The reader for this input source.
     */
    iutil::Reader* _reader;
};

/**
 * Contains the document parsed in the Intel proprietary format.
 * @note It is recommended that user code does not inherit this class.
 */
class COMMON_EXPORT XMLDocSource : public Source {
public:

    /**
     * Default constructor.
     */
    XMLDocSource();

    /**
     * Creates the Intel-specified Source by a handler of the document parsed
     * in the Intel proprietary format.
     *
     * @param doc - a handler of the document parsed in the Intel proprietary
     *              format
     */
    XMLDocSource(idoc::XMLDocument* doc);

    /**
     * Destructor.
     */
    ~XMLDocSource();

    /**
     * Gets the type of this Source object.
     *
     * @descr @n This method returns @c UNKNOWN if the calling 
     * instance is not SAXSource, DOMSource, StreamSource, or XMLDocSource.
     * This is to avoid the case when user code creates an instance of the base class 
     * Source instead of a specfic one.
     * 
     * @return Enumeration XMLDOC if called from base class. 
     */
    virtual SourceType type() const;

    /**
     * Gets the handler to parses the document in the Intel proprietary format.
     *
     * @descr @n This handler can be got from an @c IntelResult output
     * from one Transformer and set into XMLDocSource as the
     * input of another Transformer directly.
     *
     * @return A handler of an Intel-parsed document.
     */
    idoc::XMLDocument* getXMLDocument() const;

    /**
     * Sets the document parsed in the Intel proprietary format as the input document.
     *
     * @param doc - a handler of the document parsed in the Intel proprietary
     *              format
     */
    void setXMLDocument(idoc::XMLDocument* doc);

protected:
    /**
     * The document parsed in the Intel proprietary format 
     * as the input document.
     */
    idoc::XMLDocument* _doc;
};


INTEL_XML_NAMESPACE_END

#endif

