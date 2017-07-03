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

/** @file result.h Interfaces of various types of Result
 *
 * Defines the interfaces of class Result, StreamResult, DOMResult 
 * and XMLDocResult. They can be used as output of XSL transformation. 
 * 
 */
#ifndef __COM_INTEL_XML_RESULT_H__
#define __COM_INTEL_XML_RESULT_H__

#include "include/common/types.h"
#include "include/common/source.h"
#include "include/util/xmlstring.h"


INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN
class Node;
INTEL_XML_PARSE_DOM_NAMESPACE_END

INTEL_XML_PARSE_SAX_NAMESPACE_BEGIN
class ContentHandler;
class LexicalHandler;
INTEL_XML_PARSE_SAX_NAMESPACE_END

INTEL_XML_TRANSFORM_NAMESPACE_BEGIN
class Transformer; 
INTEL_XML_TRANSFORM_NAMESPACE_END

INTEL_XML_UTIL_NAMESPACE_BEGIN
class Writer;
INTEL_XML_UTIL_NAMESPACE_END

INTEL_XML_NAMESPACE_BEGIN

/** The enumerations for all possible Result types. */
typedef SourceType ResultType;

/**
 * Is the base class for all result types.
 *
 * @descr @n %Result should not be provided directly as an argument of method 
 * Transformer#transform() as XSL transformation output. Use more specific 
 * StreamResult,  SAXResult, DOMResult, and XMLDocResult.
 *
 */
class COMMON_EXPORT Result {
public:

    /**
     * Default constructor.
     */
    Result(); 

    /**
     * Creates a Result with the @e systemId specified.
     *
     * @param systemId - the system ID specified as an @c XMLChar
     *                       string
     */
    Result(const XMLChar* systemId);

    /**
     * Destructor. 
     */
    virtual ~Result();

    /**
     * Gets the type of this Result object.
     *
     * @descr @n This method returns @c UNKNOWN if the calling 
     * instance is not SAXResult, DOMResult, StreamResult, or XMLDocResult.
     * This is to avoid the case when user code creates an instance of the base class 
     * Result instead of a specfic one.
     * 
     * @return Enumeration @c UNKNOWN if called from the base class.
     */
    virtual ResultType type() const;

    /**
     * Gets the system identifier.
     *
     * @return The system identifier that was set by 
     * setSystemId(), 
     * NULL if this method has not been called.
     */
    virtual const XMLChar* getSystemId();

    /**
     * Sets the  the system identifier.
     *
     * @param systemId - the system ID specified as an @c XMLChar
     *                       string
     */
    virtual void setSystemId(const XMLChar* systemId);

protected:
    /**
     * The system identifier.
     *
     * @descr @n The system identifier may be used when the 
     * @c Result is to be written to a file.
     */
    iutil::XMLString _systemId;
};

/**
 * Contains the SAX output of the XSL transformation.
 */
class COMMON_EXPORT SAXResult : public Result {
public:

    /**
     * Default constructor.
     */
    SAXResult(); 

    /**
     * Creates a %SAXResult that targets a 
     * Intel(R) XML Parsing Accelerator SAX ContentHandler.
     *
     * @param handler - the target non-null ContentHandler
     */
    SAXResult(isax::ContentHandler* handler);

    /**
     * Destructor.
     */    
    virtual ~SAXResult();

   /**
     * Gets the type of this Result object.
     *
     * @descr @n This method returns @c UNKNOWN if the calling 
     * instance is not SAXResult, DOMResult, StreamResult, or XMLDocResult.
     * This is to avoid the case when user code creates an instance of the base class 
     * Result instead of a specfic one.
     * 
     * @return Enumeration value @c SAX.
     */    
    virtual ResultType type() const;

    /**
     * Gets the ContentHandler that is set by 
     * setHandler().
     * 
     * @return The ContentHandler set by setHandler()
     *  or obtained with constructor SAXResult(isax::ContentHandler* handler). 
     */
    virtual isax::ContentHandler* getHandler(); 

    /**
     * Sets the target Intel(R) XML Parsing Accelerator SAX ContentHandler.
     *
     * @param handler - the target non-null ContentHandler
     */
    virtual void setHandler(isax::ContentHandler* handler);

    /**
     * Gets the LexicalHandler that was set with 
     * setLexicalHandler().  
     *
     * @return The target LexicalHandler set with
     * setLexicalHandler(). 
     */
    virtual isax::LexicalHandler* getLexicalHandler();

    /**
     * Sets the target Intel(R) XML Parsing Accelerator SAX LexicalHandler 
     * for the output.
     *
     * @param handler - the target LexicalHandler
     */
    virtual void setLexicalHandler(isax::LexicalHandler* handler);

    /**
     * Gets the system identifier that was set with setSystemId().
     *
     * @return The system identifier.
     */
    virtual const XMLChar* getSystemId();

    /**
     * Sets the system identifier that may be used in association 
     * with the ContentHandler.
     *
     * @param systemId - the system identifier  
     */
    virtual void setSystemId(const XMLChar* systemId);

protected:
    /**
     * The target Intel(R) XML Parsing Accelerator SAX ContentHandler 
     * for the output.
     */
    isax::ContentHandler* _handler;
    /** 
     * The target Intel(R) XML Parsing Accelerator SAX LexicalHandler 
     * for the output.
     */
    isax::LexicalHandler* _handlerLexical;
};

/**
 * Contains the DOM tree output of the XSL transformation.
 */
class COMMON_EXPORT DOMResult : public Result {
public:

    /**
     * Default constructor.
     */
    DOMResult();

    /**
     * Creates a DOMResult with a system identifier
     * 
     * @descr @n System identifier is used as file URL when DOM is to be 
     * serialized into a file.
     *
     * @param systemId - the system identifier
     */
    DOMResult(const XMLChar* systemId);

    /**
     * Destructor.
     */
    virtual ~DOMResult();

   /**
     * Gets the type of this Result object.
     *
     * @descr @n This method returns @c UNKNOWN if the calling 
     * instance is not SAXResult, DOMResult, StreamResult, or XMLDocResult.
     * This is to avoid the case when user code creates an instance of the base class 
     * Result instead of a specfic one.
     * 
     * @return Enumeration value DOM.
     */ 
    virtual ResultType type() const;

    /**
     * Gets the root node of the result DOM tree. 
     *
     * @descr @n When XSL transformation completes, Intel(R) XSLT 
     * Accelerator calls setNode() to set the 
     * root node of the result DOM tree. 
     *
     * @return The root node of the result DOM tree.
     */
    virtual idom::Node* getNode();

    /**
     * Gets the system identifier, if specified.
     *
     * @return The system identifier.
     */
    virtual const XMLChar* getSystemId();

    /**
     * Sets the node that contains the result DOM tree.
     * 
    * @descr @n When XSL transformation completes, Intel(R) XSLT 
     * Accelerator calls setNode() to set the 
     * root node of the result DOM tree. If user calls
     * setNode() by mistake, Intel(R) XSLT 
     * Accelerator overrides the original one and set the new 
     * root node obtained from transform output.
     *
     * @param node - an Intel(R) XML Parsing Accelerator defined DOM node
     */
    virtual void setNode(idom::Node* node);

    /**
     * Sets the system identifier that may be used in association 
     * with the node.
     *
     * @param systemId - the system identifier
     */
    virtual void setSystemId(const XMLChar* systemId);

protected:
    /**
     *The root node of the result DOM tree of XSL transformation.
     */
    idom::Node* _root;
};


/**
 * Contains the stream output of the XSL transformation. 
 * The format may be XML, plain text, or HTML. 
 */
class COMMON_EXPORT StreamResult : public Result {
public:

    /**
     * Default constructor. 
     * 
     * @descr @n If neither of system identifier or Writer is specified, 
     * the result will be serialized to standard output.
     */
    StreamResult();

    /**
     * Writes the result to a file; the filename will be automatically set as @c systemId.
     * @descr @n Calling this contructor, the system identifier will be filled 
     * with @e filename. If Writer is not specified, Transformer 
     * or TransformFactory will create a default writer with 
     * FileWriter::createFileWriter(), and use it together with 
     * the provided filename to write the target output file. 
     * 
     * @param filename - the name of the file that the result will be write to
     *                    
     */
    StreamResult(const XMLChar* filename);

    /**
     * Writes the result using the specified Writer.
     *
     * @param writer - the writer used to write target file
     */
    StreamResult(iutil::Writer* writer);

    /**
     * Destructor.
     */
    virtual ~StreamResult();

    /**
     * Gets the type of this Result object.
     *
     * @descr @n This method returns @c UNKNOWN if the calling 
     * instance is not SAXResult, DOMResult, StreamResult, or XMLDocResult.
     * This is to avoid the case when user code creates an instance of the base class 
     * Result instead of a specfic one.
     * 
     * @return Enumeration value @c STREAM.
     */ 
    virtual ResultType type() const;

    /**
     * Gets the system identifier that was set with setSystemId().
     *
     * @return The system identifier.
     */
    virtual const XMLChar* getSystemId();

    /**
     * Sets the system identifier that may be used in association 
     * with the writers stream. If no writer is set, the
     * system identifer will be used as a writeable URI (probably a file name), 
     * working together with the default writer to write the result to target.
     *
     * @param systemId -  the system identifier
     */ 
    virtual void setSystemId(const XMLChar* systemId);

    /**
     * Gets the Writer that was set with 
     * setWriter() or constructor 
     * StreamResult(iutil::Writer* writer);
     *
     * @return The pre-set writer.
     */
    virtual iutil::Writer* getWriter();

    /**
     * Sets the Intel(R) proprietary Writer to write the result to 
     * target.
     *
     * @param writer - the Writer to write the result to target
     */
    virtual void setWriter(iutil::Writer* writer);

protected:
    /** The Writer to write the result to target. */
    iutil::Writer* _writer;
};

/**
 * Contains the Intel proprietary Document that is the output 
 * of XSL transformation.
 */
class COMMON_EXPORT XMLDocResult : public Result {
public:

    /**
     * Default constructor.
     */
    XMLDocResult();
    /**
     * Destructor.
     */
    ~XMLDocResult();

    /**
     * Gets the type of this Result object.
     *
     * @descr @n This method returns @c UNKNOWN if the calling 
     * instance is not SAXResult, DOMResult, StreamResult, or XMLDocResult.
     * This is to avoid the case when user code creates an instance of the base class 
     * Result instead of a specfic one.
     * 
     * @return Enumeration value @c XMLDOC.
     */
    virtual ResultType type() const;
    /**
     * Sets Intel(R) proprietary XMLDocument that is the result of 
     * transformation.
     *
     * @descr @n When the transformation completes, Intel(R) XSLT Accelerator
     * calls this method to set the output of transformation into this 
     * XMLDocResult. Calling this method by mistake produces
     * an excetion during transformation.
     * 
     * @param handle - the XMLDocument
     */
    void setXMLDocument(idoc::XMLDocument* handle);

    /**
     * Gets the result Intel(R) proprietary XMLDocument.
     *
     * @return The result XMLDocument or NULL if transformation
     * result is not generated.
     */
    idoc::XMLDocument* getXMLDocument() const;

protected:
    /**
     * The Intel(R) proprietary XMLDocument that is the 
     * output of XSL transformation.
     */
    idoc::XMLDocument* _doc;
};

INTEL_XML_NAMESPACE_END

#endif //__COM_INTEL_XML_RESULT_H__

