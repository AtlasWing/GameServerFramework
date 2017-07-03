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

/** @file contenthandler.h @brief Interface for basic SAX parsing events */

#ifndef __COM_INTEL_XML_PARSE_SAX_CONTENTHANDLER_H__
#define __COM_INTEL_XML_PARSE_SAX_CONTENTHANDLER_H__

INTEL_XML_PARSE_SAX_NAMESPACE_BEGIN

struct SAXAttribute;
class Locator;

/**
  * Receives notifications of general document events.
  *
  * This is the main interface that most SAX2 applications
  * implement: if the application needs to be informed of basic parsing
  * events, it implements this interface and registers an instance with
  * the SAX2 parser using the SAXParser::setContentHandler() method.  The parser
  * uses the instance to report basic document-related events like
  * the start and end of elements and character data.
  *
  * The order of events in this interface is very important, and
  * mirrors the order of information in the document itself.  For
  * example, all of an element's content (character data, processing
  * instructions, and/or subelements) will appear, in order, between
  * the #startElement event and the corresponding #endElement event.
  *
  * Instead of implementing the entire interface, application writers can also 
  * derive a class from @c Sax2HandlerBase, which implements
  * the default functionality; parser writers can instantiate
  * @c Sax2HandlerBase to obtain a default handler.  The application can find
  * the location of any document event using the Locator interface
  * supplied by the parser through the setDocumentLocator() method.
  */

class SAX_EXPORT ContentHandler
{
public:
    /** @name Constructors and Destructor */
    //@{
    /** Default constructor */
    ContentHandler()
    {
    }

    /** Destructor */
    virtual ~ContentHandler()
    {
    }
    //@}

    /** @name The virtual document handler interface */

    //@{
   /**
    * Receives notifications of character data or @c CDATA.
    *
    * @descr @n The Parser will call this method to report each chunk of
    * character data. A long stream of content for character data and @c CDATA
    * can be split into multiple @c characters events by using
    * @e more to indicate that another @c characters event
    * follows. If the @e more indicator is FALSE, no character
    * data follows the current event. If the @c more indicator is 
    * TRUE, other data @b may follow.
    *
    * @param count      - the counter of character data that this event reported
    *                     this time
    * @param string     - the string to character data
    * @param more       - the indicator of character data continuity
    */
    
    virtual void               characters(unsigned int count, const ixml::XMLChar* string, bool more) = 0;


   /**
    * Receives notifications of the start of a namespace prefix mapping.
    *
    * @descr @n This method is called for each @c xmlns attribute in the XML
    * source before the element, in which the attribute appears.
    *
    * The parser uses arrays to allocate and index all namespace definitions, and
    * assigns a small integer @c namespaceTag to a namespace prefix and
    * its associated URI, sized 4KB.
    * The namespace tag #0 is reserved to indicate that the element or attribute
    * had no prefix, and tag #1 is for the XML namespace that is implicitly
    * defined for any XML document. According to 
    * <a href="http://www.w3.org/TR/xml-names/">Namespaces in XML 1.0
    * (Second Edition)</a>, this namespace should always be 
    * http://www.w3.org/XML/1998/namespace. Tag #2 is for the prefix xmlns which is
    * used only to declare namespace bindings. According to
    * <a href="http://www.w3.org/TR/xml-names/">Namespaces in XML 1.0
    * (Second Edition)</a>, it is by definition bound to the namespace name 
    * http://www.w3.org/2000/xmlns/. It MUST NOT be declared.
    *
    * The assigned values need not be in the numerical order and can be sparse.
    *
    * Once assigned, a namespace tag cannot be assigned again until a new
    * document is started. The same combination of a prefix and namespace can be
    * assigned to different namespace tags because the XML source may contain
    * multiple redundant namespace definitions. Do not assign the same tag value
    * to these definitions because each namespace definition is scoped
    * independently. Bundling the namespace definitions together makes it
    * difficult to replay the sub-tree when needed.
    *
    * An empty namespace declaration is identified by the @e prefixCount of 0
    * and the @e prefixString of zero, for example @c "". The empty
    * declaration is @b not represented by a null pointer.
    *
    * Namespace attributes are @b not passed via SAXAttribute arrays. 
    *
    * @param newNamespaceTag - the new tag used to identify this namespace
    * @param prefixCount     - the counter of the prefix string
    * @param prefixString    - the pointer to the prefix string
    * @param uriCount        - the counter of the URI this prefix is mapping to
    * @param uriString       - the pointer to the URI this prefix is mapping to
    */
    
    virtual void               startNamespace(unsigned int newNamespaceTag,
                                              unsigned int prefixCount,
                                              const ixml::XMLChar* prefixString,
                                              unsigned int uriCount,
                                              const ixml::XMLChar* uriString) = 0;


  /**
    * Receives notifications of the end of a namespace prefix mapping.
    *
    * @descr @n This method indicates the end of the scope of an @c xmlns attribute.  
    * The method is called after the #endElement event, in which the
    * attribute appears.
    *
    * @param namespaceTag - the tag of this namespace
    */
    
    virtual void               endNamespace(unsigned int namespaceTag) = 0;


  /**
    * Receives notifications of a processing instruction.
    *
    * @descr @n The parser invokes this method once for each processing instruction found.
    * A processing instruction can occur before or after the main document element.
    *
    * A SAX parser implementation should never use this method to report an XML
    * declaration (<a href="http://www.w3.org/TR/xml/">XML 1.0 standard</a>,
    * section 2.8) or a text declaration (<a href="http://www.w3.org/TR/xml/">
    * XML 1.0 standard</a>, section 4.3.1).
    *
    * @param targetCount   - the counter of this processing instruction target
    * @param targetString  - the pointer to the string of the processing instruction target
    * @param dataCount     - the counter of this processing instruction data
    * @param dataString    - the pointer to the string of the processing instruction
    *                        data or NULL if none has been supplied
    */
    virtual void               pi(unsigned int targetCount,
                                  const ixml::XMLChar* targetString,
                                  unsigned int dataCount,
                                  const ixml::XMLChar* dataString) = 0;


  /**
    * Receives notifications of the beginning of an element.
    *
    * @descr @n The parser invokes this method at the beginning of every element in the
    * XML document. The method reports the content of the element after the
    * #startElement and before the #endElement event following the original order.
    * The #endElement event is always present, even if the element is empty. 
    *
    * If the attribute value is a null pointer, user code routines should ignore
    * the attribute. This is convenient for callers that have static arrays with 
    * pre-set names and only need to fill in the value pointers. This does not
    * apply to an empty string, which is perfectly valid. 
    *
    * @param namespaceTag     - the prefix and its associated URI or zero if the 
    *                             local name has no associated namespace
    * @param elemNameCount    - the counter of the local name of this element
    * @param elemName         - the pointer to the local name string
    * @param nAttribute       - the counter of attributes that this element contains
    * @param attrlist         - the pointer to the attribute list
    */
    
    virtual void               startElement(unsigned int namespaceTag,
                                            unsigned int elemNameCount,
                                            const ixml::XMLChar* elemName,
                                            unsigned int nAttribute,
                                            SAXAttribute** attrlist) = 0;

  /**
    * Receives notifications of the end of an element.
    *
    * @descr @n The SAX parser invokes this method at the end of every element in the XML
    * document. An #endElement event always has a corresponding #startElement event
    * even when the element is empty: both forms "<e ...></e>" and "<e ... />" 
    * (the empty element tag) are accepted. 
    *
    * The local name and namespace tag of the element are not identified.  
    */
    
    virtual void               endElement() = 0;

    
    /**
     *
     * Receives notifications of the beginning of a document. 
     *
     * @descr @n The SAX parser invokes this method only once before other
     * methods in this interface.
     *
     */
    
    virtual void               startDocument() = 0;

  /**
    * Receives notifications of the end of a document.
    *
    * @descr @n The SAX parser invokes this method only once as the last method during
    * the parse operation. The parser only invokes this method when the end
    * of input is reached or the parsing operation is abandoned because of an
    * unrecoverable error.
    */

    virtual void               endDocument() = 0;


  /**
    * Receives an object for locating the origin of SAX document events.
    *
    * @descr @n SAX parsers are strongly encouraged (though not
    * required) to supply a locator: if it does so, it must supply
    * the locator to the application by invoking this method before
    * invoking any other method in the interface.
    *
    * The locator allows the application to determine the end
    * position of any document-related event, even if the parser is
    * not reporting an error.  Typically, the application will
    * use this information for reporting its own errors (such as
    * character content that does not match an application's
    * business rules). The information returned by the locator
    * is probably not sufficient for use with a search engine.
    *
    * @note  The locator will return correct information only
    * during the invocation of the events in this interface. The
    * application should not attempt to use it at any other time.
    *
    * @param locator        -  an object that can return the location of
    *                          any SAX document event; do @b not modify or
    *                          delete the object
    *
    * @see Locator
    */
    virtual void               setDocumentLocator(Locator* locator) = 0;

    
  /**
    * Receives notifications of a skipped entity
    *
    * @descr @n The parser will invoke this method once for each entity
    * skipped.  All processors may skip external entities,
    * depending on the values of the features:
    *
    * http://xml.org/sax/features/external-general-entities
    *
    * http://xml.org/sax/features/external-parameter-entities
    *
    * @param count          - the counter of the name of skipped entity
    * @param name           - the name of the skipped entity: for parameter entities,
    *                           begins with , for the external DTD subset, is the string [dtd]
    *
    */
    virtual void               skippedEntity(unsigned int count, const ixml::XMLChar* name) = 0;

    //@}
private :
    /* Unimplemented Constructors and operators */
    /* Copy constructor */
    ContentHandler(const ContentHandler&);
    /** Assignment operator */
    ContentHandler& operator=(const ContentHandler&);
};

INTEL_XML_PARSE_SAX_NAMESPACE_END

#endif
