/*

 *---------------------------------------------------------------------------
 *
 *     Copyright 2007~2008 Intel Corporation All Rights Reserved.
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
 * @file saxparser.h @brief Standard SAX Parser interface
 */


#ifndef __COM_INTEL_XML_PARSE_SAX_SAXPARSER_H__
#define __COM_INTEL_XML_PARSE_SAX_SAXPARSER_H__

#include "include/common/types.h"
#include "include/common/defines.h"

INTEL_XML_NAMESPACE_BEGIN
class StreamSource;
INTEL_XML_NAMESPACE_END

INTEL_XML_UTIL_NAMESPACE_BEGIN
class Reader;
INTEL_XML_UTIL_NAMESPACE_END

INTEL_XML_VALIDATOR_NAMESPACE_BEGIN
class Schema;
INTEL_XML_VALIDATOR_NAMESPACE_END


INTEL_XML_PARSE_SAX_NAMESPACE_BEGIN

class ContentHandler;
class DTDHandler;
class LexicalHandler;
class EntityResolver;

/**
  * Implements the SAX Parser interface.
  *
  * Use this class in your application to parse XML data following
  * the SAX model. The class enables the user code to install
  * SAX handlers for event callbacks. If the user application
  * code does not call setContentHandler() to register a new handler,
  * the parser will produce SAXException when parsing XML data.
  * You may also want to set lexical and DTD event handlers.
  * For that, call setDTDHandler() or setLexicalHandler(); otherwise, all
  * DTD or lexical events encountered during data processing will be ignored.
  *
  * You can also instantiate a well-formed or a non-validating parser
  * by the validating level set using method setValidationLevel().
  */
class SAX_EXPORT SAXParser
{
public:
    /**
     * Destructor
     */
    virtual                     ~SAXParser() {}

public:

    /**
     * Parses the XML document from the Reader object.
     *
     * @descr @n To use this method, set the Reader object explicitly to access
     * the data. At the same time, source URI for parser must be set by @e uri.
     *
     * @note The external resourc bease URI (Uniform Resource Identifier) will be extacted
     * from the uri parameter unless it is set by setExternalResourceBaseURI().
     *
     * @param reader - the virtual reader interface that can retrieve XML content;
     *                 if it is set to NULL, an exception is thrown
     * @param uri    - the constant character pointer to the native string containing
     *                 the path to the XML file to be parsed
     *
     * @return The parsed document. In case of an error, returns SAXException,
     *         IOException, ValidationException.
     *
     */
    virtual bool                parse(iutil::Reader* reader,const ixml::XMLChar* uri=NULL) = 0;

    /**
     * Parses the XML document of the specified URI.
     *
     * @descr @n The external resourc bease URI (Uniform Resource Identifier) will be extacted
     * from the uri parameter unless it is set by setExternalResourceBaseURI().
     *
     * @note  At present, only the format of "file:///" is supported.
     *
     * @param uri    - the constant character pointer to the native string containing
     *                 the path to the XML file to be parsed
     *
     * @return The parsed document. In case of an error, returns SAXException,
     *         IOException, ValidationException.
     */
    virtual bool                parse(const ixml::XMLChar* uri) = 0;

   /**
    * Parses the XML document from the StreamSource object.
    *
    * @descr @n To use this method, set the StreamSource object explicitly to access
    * the data.
    *
    * @param source - the virtual StreamSource interface that identify XML content;
    *                 if it is set to NULL, an exception is thrown
    *
    * @return The parsed document. In case of an error, returns SAXException,
    *         IOException, ValidationException.
    *
    */
    virtual bool                parse(ixml::StreamSource* source) = 0;

    /**
     * Configures the parser to support XML namespaces.
     *
     * @descr @n Specifies that whether the parser will provide support for
     * XML namespaces. By default the value of this is set to true.
     * 
     * @param namespaceAware - the state to set: TRUE for namespace aware;
     *                         otherwise, FALSE
     *
     * @see setNamespaceAware()
     */
    virtual void                setNamespaceAware(bool namespaceAware) = 0;

    /**
     * Indicates whether or not the parser is configured to parse XML
     * with namespace aware style.
     *
     * @return  TRUE if support XML namespaces;
     *          otherwise, FALSE.
     */
    virtual bool                isNamespaceAware() = 0;

    /**
     * Configures the parser to ignore the encoding setting.
     *
     * @descr @n When the XML source has been converted to UTF-8 or UTF-16 with the byte
     * order mark (BOM) by an external piece of code, you should ignore the encoding
     * setting in the document, as in "<?xml ... encoding= ...?>".
     * Calling this method after the first read operation can produce
     * unpredictable results.
     *
     * @param ignoreEncoding - the state to set: TRUE for ignoring encoding;
     *                         otherwise, FALSE
     *
     * @see setExternalEncoding()
     */
    virtual void                setIgnoreEncoding(bool ignoreEncoding) = 0;

    /**
     * Sets the encoding explicitly.
     *
     * @descr @n When encoding is known from an external source (e.g., when parsing http headers),
     * you can pre-set encoding explicitly.
     * If @e externalEncoding is #UTF_16 or
     * #UTF_32, endianness is inferred from the source document.
     *
     * @param externalEncoding - the encoding to set
     *
     * @return TRUE if the input is a legal encoding; otherwise, FALSE. 
     */
    virtual bool                setExternalEncoding(ixml::EncodingStyle externalEncoding) = 0;

    /**
     * Gets the current encoding.
     *
     * @descr @n Returns the encoding used for the current document as an
     * enumaration value. This is only valid when the encoding XML declaration
     * (if any) has been processed. In general, the method should only be called
     * after reaching the end of document or getting an error.
     *
     * Possible values are all members of the #EncodingStyle enum, not
     * necessarily those supplied by getSupportedEncodings(). Specifically, for
     * 16-bit encodings, this method specifies endianness, while getSupportedEncodings()
     * does not.
     *
     * @return The enumaration value for the current encoding.
     */
    virtual ixml::EncodingStyle       getEncodingAsEnum() = 0;

    /**
     * Gets the current encoding as a string.
     *
     * @descr @n  Returns the encoding used for the current document
     *  with the same specifics as in getEncodingAsEnum().
     *
     * @return The pointer to the current encoding string.
     */
    virtual const ixml::XMLChar*      getEncodingAsString() = 0;

    /**
     * Gets the encodings supported by the parser.
     *
     * @descr @n Returns an array of pointers to strings giving the names
     * of the supported encodings. Each array ends with a NULL pointer.
     *
     * @return The array of pointers to encoding strings.
     */
    static const ixml::XMLChar**      getSupportedEncodings();

    /**
     * Configures the parser to reject HTML and XHTML documents.
     *
     * @descr @n If you want to exclude HTML and XHTML documents from processing, set
     * @e value to TRUE. The setting is FALSE by
     * default.
     *
     * @note  A document is considered to be HTML if the root element is "html" with
     * a case-blind compare operation. A better test would be to check whether the
     * document (a) has no namespace and matches "html" via a case-blind compare
     * or (b) is in the xhtml namespace ("http://www.w3.org/1999/xhtml") and matches
     * the local name "html" via a case-sensitive compare. However, this
     * would require more complicated checking.
     *
     * @param value - the state to set: TRUE for rejecting HTML and XHTML
     *                documents; otherwise, FALSE
     */
    virtual void                setRejectHtml(bool value) = 0;

    /**
     * Checks whether an HTML or XHTML document declaration has been found.
     *
     * @descr @n If this method returns TRUE, an HTML or XHTML document
     * has been found. This does not depend on calling setRejectHtml().
     *
     * @return TRUE if an HTML or XHTML document was found;
     *         otherwise, FALSE.
     */
    virtual bool                containsHtml() = 0;

    /**
     * Configures the parser to reject the processing instruction declaration.
     *
     * @descr @n If the value is set to TRUE, the processing instruction declaration
     * attached to the input data produces an error. The setting is FALSE by
     * default.
     * For instance, you would call this method to avoid processing instruction
     * declarations when parsing SOAP messages, in which such declarations are
     * not allowed.
     *
     * @param value - the state to set: TRUE for rejecting processing
     *                instruction declarations; otherwise, FALSE
     */
    virtual void                setRejectPI(bool value) = 0;

    /**
     * Checks whether a processing instruction declaration has been found.
     *
     * @descr @n If this method returns TRUE, a processing instruction
     * declaration has been found. This does not depend on calling setRejectPI().
     *
     * @return  TRUE if a processing instruction declaration
     *          has been found; otherwise, FALSE.
     */
    virtual bool                containsPI() = 0;

    /**
     * Configures the parser to reject the DOCTYPE declaration.
     *
     * @descr @n If this method returns TRUE, a DOCTYPE declaration
     * attached to input data produces an error. The setting is FALSE
     * by default.
     * For instance, you would call this method to avoid DOCTYPE declarations
     * when parsing SOAP messages, in which such declarations are not allowed.
     *
     * @param value - the state to set: TRUE for rejecting DOCTYPE
     *                declarations; otherwise, FALSE
     * @return 
     */
    virtual void                setRejectDOCTYPE(bool value) = 0;

    /**
     * Checks whether a DOCTYPE declaration has been found.
     *
     * @descr @n If this method returns TRUE, a DOCTYPE declaration
     * has been found. This does not depend on calling setRejectDOCTYPE().
     *
     * @return TRUE if a DOCTYPE declaration has been found;
     *         otherwise, FALSE.
     */
    virtual bool                containsDOCTYPE() = 0;

    /**
     * Sets the validation level of the parser.
     *
     * @descr @n Currently, two levels are supported: well-formed and DTD non-validating.
     * Use this method to specify the validation level with #WELL_FORMED or
     * #DTD_NON_VALIDATING. The setting is #DTD_NON_VALIDATING
     * by default.
     *
     * If setRejectDOCTYPE() is set to TRUE, setting the validation
     * level #DTD_NON_VALIDATING has no effect.
     *
     * @param level - the validation level of the SAX parser
     *
     */
    virtual void                setValidationLevel(ixml::ValidationLevel level) = 0;

    /**
     * Sets the URI for external entity resolving.
     *
     * @descr @n If you supply the URI attached to input data as the parameter when calling
     * parse() methods, SAXParser intercepts such URI information
     * and replaces it with the URI set by this method internally.
     * For instance, you would call this method to specify a block of memory for parsing.
     * In this case, the base URI for external entity resolving is absent and should be set.
     *
     * @param baseURI - the pointer to the base URI string
     */
    virtual void                setExternalResourceBaseURI(const ixml::XMLChar* baseURI) = 0;

    /**
     * Returns the URI for external entity resolving.
     *
     * @descr @n If the base URI for an external resource is set explicitly, this method
     * returns the value set by setExternalResourceBaseURI(). If the URI is set
     * by parse() methods implicitly, Returns the URI
     * separated from the URI attached parameter.
     *
     * @note  If the input XML contains DOCTYPE information and #ValidationLevel
     * is set to #DTD_NON_VALIDATING, the return URI may differ at different
     * stages of parsing because of nested external subset resolving
     * during the DTD parsing process.
     *
     * @return The @c baseURI pointer to the base URI string.
     */
    virtual const ixml::XMLChar*      getExternalResourceBaseURI() = 0;

  /**
    * Enables an application to register a document event handler.
    *
    * @descr @n If the application does not register a document handler,
    * the parser will throw SAXException during data processing.
    *
    * Applications can register a new or a different handler in the
    * middle of a parse, and the SAX parser must begin using the new
    * handler immediately.
    *
    * @param handler          - the document handler
    * @see ContentHandler
    */
    virtual void setContentHandler(ContentHandler* handler) = 0;

  /**
    * Enables an application to register a DTD event handler.
    *
    * @descr @n If the application does not register a DTD handler, all DTD
    * events reported by the SAX parser will be silently ignored.
    *
    * Applications can register a new or a different handler in the middle
    * of a parse, and the SAX parser must begin using the new handler
    * immediately.
    *
    * @param handler          - the DTD handler
    * @see DTDHandler
    */
    virtual void setDTDHandler(DTDHandler* handler) = 0;

  /**
    * Enables an application to register a lexical event handler.
    *
    * @descr @n If the application does not register a lexical handler, all lexical
    * events reported by the SAX parser will be silently ignored.
    *
    * Applications can register a new or a different handler in the middle
    * of a parse, and the SAX parser must begin using the new handler
    * immediately.
    *
    * @param handler          - the lexical handler
    * @see LexicalHandler
    */
    virtual void setLexicalHandler(LexicalHandler* handler) = 0;

  /**
    * Enables an application to register a custom entity resolver.
    *
    * @descr @n If the application does not register an entity resolver, the
    * SAX parser resolves system identifiers and opens connections
    * to entities on its own.
    *
    * Applications can register a new or a different entity resolver
    * in the middle of a parse operation, and the SAX parser must begin using
    * the new resolver immediately.
    *
    * @param resolver              - the object for resolving entities
    * @see EntityResolver
    */
    virtual void setEntityResolver(EntityResolver* resolver) = 0;

    /**
      * Returns the installed content handler.
      *
      * @return A pointer to the installed content handler object.
      */
    virtual ContentHandler* getContentHandler() = 0 ;

    /**
      * Returns the installed DTD handler.
      *
      * @return A pointer to the installed DTD handler object.
      */
    virtual DTDHandler* getDTDHandler() = 0;

    /**
      * Returns the installed lexical handler.
      *
      * @return A pointer to the installed lexical handler object.
      */
    virtual LexicalHandler* getLexicalHandler() = 0;

    /**
      * Returns the installed entity resolver.
      *
      * @return A pointer to the installed entity resolver object.
      */
    virtual EntityResolver* getEntityResolver() = 0 ;

    /**
     * Gets the number of the line that is currently processed.
     *
     * @return The number of the line being processed.
     */
    virtual unsigned int        getLineNumber() = 0;

    /**
    * Parses data with schema validation support.
    *
    * @descr @n Initially, Schema is set to NULL. This method
    * changes Schema to a non-null value, so that the parser will use the Validator object created
    * from it to validate documents before it passes information down
    * to the application.
    *
    * If you want cancel validation in
    * the next round parsing, use @c setSchema(0) to drop the schema validation.
    *
    * @param schema - the object for validation
    */
    virtual void setSchema(ival::Schema* schema) = 0;

    /**
     * Gets the Schema object which is used to validate the xml instance.
     *
     * @return The pointer of the Schema object.
     */
    virtual ival::Schema* getSchema() = 0;

};

INTEL_XML_PARSE_SAX_NAMESPACE_END

#endif
