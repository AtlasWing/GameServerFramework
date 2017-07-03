/*
 *---------------------------------------------------------------------------
 *
 *     Copyright 2007~2008 Intel Corporation All Rights Reserved.
 *     The source code contained or described herein and all documents
 *     related to the source code ("Material") are owned by Intel
 *     Corporation or its suppliers or licensors. Title to the Material
 *     remains with Intel Corporation or its suppliers and licensors
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

/**@file xmldocparser.h @brief The XMLDocument parsing interface
 *
 * Use this interface to parse XML data to the stream-based XML Document.
 */

#ifndef __COM_INTEL_XML_PARSE_XMLDOC_XMLDOCPARSER_H__
#define __COM_INTEL_XML_PARSE_XMLDOC_XMLDOCPARSER_H__

#include "include/common/defines.h"
#include "include/common/types.h"

INTEL_XML_NAMESPACE_BEGIN
class StreamSource;
INTEL_XML_NAMESPACE_END

INTEL_XML_UTIL_NAMESPACE_BEGIN
class Reader;
INTEL_XML_UTIL_NAMESPACE_END

INTEL_XML_PARSE_SAX_NAMESPACE_BEGIN
class EntityResolver;
INTEL_XML_PARSE_SAX_NAMESPACE_END

INTEL_XML_VALIDATOR_NAMESPACE_BEGIN
class Schema;
INTEL_XML_VALIDATOR_NAMESPACE_END

INTEL_XML_PARSE_XMLDOC_NAMESPACE_BEGIN

class XMLDocument;

/**
 * @brief Implements the XMLDocParser interface.
 *
 * Use this class in your application to parse XML data to the
 * stream-based XMLDocument. For a definition of the XMLDocument
 * compact representation format, please refer to the User Guide.
 *
 * You can also instantiate a well-formed or a non-validating parser
 * by the validating level set using method setValidationLevel().
 */
class XMLDOC_EXPORT XMLDocParser
{
public:
    /** @brief Desctructor.*/
    virtual ~XMLDocParser() {}

public:

    /**
     * @brief Parses the Reader object to an XML document.
     *
     * @descr @n Set the Reader object explicitly to provide the method with the access
     * to data and set the @e uri parameter to the source URI for the parser.
     *
     * @note The external resourc bease URI (Uniform Resource Identifier) will be extacted
     * from the uri parameter unless it is set by setExternalResourceBaseURI().
     *
     * @param reader - the virtual Reader interface that can a retrieve XML content;
     *                 if set to NULL, an exception is thrown
     * @param uri    - the constant char pointer to the native string with the path
     *                 to the XML file to be parsed
     *
     * @return The parsed document.
     * @return In case of an error, produces XMLDocParseException, IOException, ValidationException.
     */
    virtual XMLDocument* parse(iutil::Reader* reader,const ixml::XMLChar* uri=NULL) = 0;

    /**
     * @brief Parses the XML file specified by URI to an XML document.
     *
     * @descr @n The external resourc bease URI (Uniform Resource Identifier) will be extacted
     * from the uri parameter unless it is set by setExternalResourceBaseURI().
     *
     * @note  Currently, the "file:///" and "http:///" formats are supported.
     *
     * @param uri    - the constant char pointer to the native string with the path
     *                 to the XML file to be parsed
     *
     * @return The parsed document, or XMLDocParseException, IOException, 
     *         ValidationException in case of an error.
     */
    virtual XMLDocument* parse(const ixml::XMLChar* uri) = 0;

    /**
     * @brief Parses the XML document from the StreamSource object.
     *
     * @descr @n To use this method, set the StreamSource object explicitly to access
     * the data.
     *
     * @param source - the virtual StreamSource interface identifying the XML
     *                 content; if it is set to NULL, an exception
     *                 is thrown
     *
     * @return The parsed document, or XMLDocParseException, IOException,
     *         ValidationException in case of an error.
     */
    virtual XMLDocument* parse(ixml::StreamSource* source) = 0;

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
    virtual void setNamespaceAware(bool namespaceAware) = 0;

    /**
     * Indicates whether or not the parser is configured to parse XML
     * with namespace aware style.
     *
     * @return  TRUE if support XML namespaces;
     *          otherwise, FALSE.
     */
    virtual bool isNamespaceAware() = 0;

    /**
     * @brief Configures the parser to strip whitespace nodes.
     *
     * @descr @n A whitespace node is a Text node with the value containing
     * only "&#x20;", "&#x9;", "&#xD" or "&#xA;".
     *
     * The parser is set to FALSE by default.
     *
     * @param stripWSNodes - TRUE for stripping whitespace nodes;
     *                       otherwise, FALSE
     */
    virtual void setStripWhiteSpace(bool stripWSNodes) = 0;

    /**
     * @brief Checks whether the parser strips whitespace nodes.
     *
     * @return The parser setting to strip whitespace nodes or not.
     */
    virtual bool getStripWhiteSpace() = 0;

    /**
     * @brief Configures the parser to ignore the encoding setting.
     *
     * @descr @n Use this method when the XML document has been converted to UTF-8 or UTF-16
     * with the byte order mark (BOM) by some external piece of code. In this case,
     * the setting "<?xml ... encoding= ...?>" should be ignored.
     *
     * Calling this method after the first read operation can produce unpredictable
     * results.
     *
     * @param ignoreEncoding - the state to set: TRUE to ignore the
     *                         the encoding; otherwise, FALSE
     *
     * @see setExternalEncoding.
     */
    virtual void setIgnoreEncoding(bool ignoreEncoding) = 0;

    /**
     * @brief Sets the encoding explicitly.
     *
     * @descr @n When encoding is known from an external source (e.g., when parsing
     * http headers), you can pre-set encoding explicitly.
     * If @e externalEncoding is #UTF_16 or
     * #UTF_32, endianness is inferred from the source document.
     *
     * @param externalEncoding - the encoding to set
     *
     * @return TRUE if the input is a legal encoding; otherwise, FALSE.
     */
    virtual bool setExternalEncoding(ixml::EncodingStyle externalEncoding) = 0;

    /**
     * @brief Gets the current encoding as an enumerator.
     *
     * @descr @n This method returns the encoding used for the current document as an
     * enumeration value. This is only valid when the encoding XML declaration
     * (if any) has been processed. In general, the method should only be called
     * after reaching the end of the document or getting an error.
     *
     * Possible values are all members of the #EncodingStyle enum, not
     * necessarily those supplied by getSupportedEncodings(). Specifically, for
     * 16-bit encodings, this method specifies endianness, while
     * getSupportedEncodings() does not.
     *
     * @return The enumaration value for the current encoding.
     */
    virtual ixml::EncodingStyle getEncodingAsEnum() = 0;

    /**
     * @brief Gets the current encoding as a string.
     *
     * @descr @n This method returns the encoding used for the current document
     * as a string value, with the same specifics as in getEncodingAsEnum().
     *
     * @return The pointer to the current encoding string.
     */
    virtual const ixml::XMLChar* getEncodingAsString() = 0;

    /**
     * @brief Gets the encodings supported by the parser.
     *
     * @descr @n This method returns an array of pointers to strings giving the names
     * of supported encodings. Each array ends with a NULL pointer.
     *
     * @return The array of pointers to encoding strings.
     */
    static const ixml::XMLChar** getSupportedEncodings();

    /**
     * @brief Configures the parser to reject HTML or XHTML documents.
     *
     * @descr @n If you want to exclude HTML or XHTML documents from processing, set
     * @e value to TRUE. The setting is FALSE by default.
     * 
     * A document is considered to be HTML if the root element is "html" with
     * a case-blind compare operation. A better test would be to check whether the
     * document matches one of the following:
     * - has no namespace and matches "html" via a case-blind compare
     * - is in the XHTML namespace ("http://www.w3.org/1999/xhtml") and matches
     * the local name "html" via a case-sensitive compare.
     * However, this would require more complicated checking.
     *
     * @param value - the state to set: TRUE for rejecting HTML and XHTML
     *                documents; otherwise, FALSE
     */
    virtual void setRejectHtml(bool value) = 0;

    /**
     * @brief Checks whether an HTML or XHTML document declaration has been found.
     *
     * @descr @n If this method returns TRUE, an HTML or XHTML document
     * has been found. This does not depend on calling setRejectHtml().
     *
     * @return TRUE, if an HTML or XHTML document was found;
     *         otherwise, FALSE.
     */
    virtual bool containsHtml() = 0;

    /**
     * @brief Configures the parser to reject the processing instruction declaration.
     *
     * @descr @n If the value is set to TRUE, the processing instruction declaration
     * attached to the input data produces an error. The setting is FALSE by
     * default.
     *
     * For example, you would call this method to avoid processing instruction
     * declarations when parsing SOAP messages, in which such declarations are
     * not allowed.
     *
     * @param value - the state to set: TRUE for rejecting processing
     *                instruction declarations; otherwise, FALSE
     */
    virtual void setRejectPI(bool value) = 0;

    /**
     * @brief Checks whether a processing instruction declaration has been found.
     *
     * @descr @n If this method returns TRUE, a processing instruction
     * declaration has been found. This does not depend on calling setRejectPI().
     *
     * @return  TRUE, if a processing instruction declaration
     *          has been found; otherwise, FALSE
     */
    virtual bool containsPI() = 0;

    /**
     * @brief Configures the parser to reject the DOCTYPE declaration.
     *
     * @descr @n If this method returns TRUE, a DOCTYPE declaration
     * attached to input data produces an error. The setting is FALSE
     * by default.
     *
     * For example, you would call this method to avoid DOCTYPE declarations
     * when parsing SOAP messages, in which such declarations are not allowed.
     *
     * @param value - the state to set: TRUE for rejecting DOCTYPE
     *                declarations; otherwise, FALSE
     */
    virtual void setRejectDOCTYPE(bool value) = 0;

    /**
     * @brief Checks whether a DOCTYPE declaration has been found.
     *
     * @descr @n If this method returns TRUE, a DOCTYPE declaration
     * has been found. This does not depend on calling setRejectDOCTYPE().
     *
     * @return TRUE, if a DOCTYPE declaration has been found;
     *         otherwise, FALSE.
     */
    virtual bool containsDOCTYPE() = 0;

    /**
     * @brief Sets the validation level of the parser.
     *
     * @descr @n Currently, two levels are supported: well-formed and DTD non-validating.
     * Use this method to specify the validation level with #WELL_FORMED or
     * #DTD_NON_VALIDATING. The setting is #DTD_NON_VALIDATING by default.
     *
     * If setRejectDOCTYPE() is set to TRUE, setting the validation
     * level #DTD_NON_VALIDATING has no effect because Lexer will not work with DTD.
     *
     * @param level - the validation level of the parser
     */
    virtual void setValidationLevel(ixml::ValidationLevel level) = 0;

    /**
     * @brief Sets the URI for external entity resolving.
     *
     * @descr @n If you supply the URI attached to input data as the parameter when calling
     * parse() methods, XMLDocParser intercepts such URI
     * information and replaces it with the URI set by this method internally.
     * 
     * For example, you would call this method to specify a block of memory for parsing.
     * In this case, the base URI for external entity resolving is absent and should be set.
     *
     * @param baseURI - the pointer to the base URI string
     */
    virtual void setExternalResourceBaseURI(const ixml::XMLChar* baseURI) = 0;

    /**
     * @brief Returns the URI for external entity resolving.
     *
     * @descr @n If the base URI for an external resource is set explicitly, this method
     * returns the value set by setExternalResourceBaseURI(). If the URI is set
     * by parse() methods implicitly, this method returns the URI
     * seperated from the URI attached parameter.
     *
     * @note  If the input XML contains DOCTYPE information and #ValidationLevel
     * is set to #DTD_NON_VALIDATING, the return URI may differ at different
     * stages of parsing because of nested external subset resolving
     * during the DTD parsing process.
     *
     * @return The @c baseURI pointer to the base URI string.
     */
    virtual const ixml::XMLChar* getExternalResourceBaseURI() = 0;

    /**
     * @brief Parses with schema validation support.
     *
     * @descr @n When schema is non-null, a parser will use a validator created
     * from it to validate documents before it passes information down
     * to the application.
     *
     * Initially, NULL is set as the Schema. If you want cancel validation in
     * the next round parsing, use setSchema() to drop the schema validation.
     *
     * @param schema - The object for validation.
     */
    virtual void setSchema(ival::Schema* schema) = 0;

    /**
     * @brief Gets the Schema object used to validate the XML instance.
     *
     * @return The pointer of the Schema object.
     */
    virtual ival::Schema* getSchema() = 0;

    /**
    * Enables an application to register a custom entity resolver.
    *
    * @descr @n If the application does not register an entity resolver, the
    * XMLDocument parser resolves system identifiers and opens connections
    * to entities on its own.
    *
    * Applications can register a new or a different entity resolver
    * in the middle of a parse operation, and the XMLDocument parser 
    * must begin using the new resolver immediately.
    *
    * @param resolver              - the object for resolving entities
    * @see EntityResolver
    */
    virtual void setEntityResolver(isax::EntityResolver* resolver) = 0;

    /**
    * Returns the installed entity resolver.
    *
    * @return A pointer to the installed entity resolver object.
    */
    virtual isax::EntityResolver* getEntityResolver() = 0 ;

};

INTEL_XML_PARSE_XMLDOC_NAMESPACE_END

#endif

