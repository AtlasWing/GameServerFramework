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

/** @file lexicalhandler.h @brief Interface for receiving lexical events in SAX parsing */


#ifndef __COM_INTEL_XML_PARSE_SAX_LEXICALHANDLER_H__
#define __COM_INTEL_XML_PARSE_SAX_LEXICALHANDLER_H__

INTEL_XML_PARSE_SAX_NAMESPACE_BEGIN


/**
  * Receives notification of lexical events.
  *
  * This is an extension handler that provides lexical information
  * about an XML document.  It does not provide information about document
  * content, which is provided by another handler: ContentHandler.
  *
  * The order of events in this interface is very important, and
  * mirrors the order of information in the document itself.  For
  * example, startDTD() and endDTD() events will occur before the
  * first element in the document.
  *
  * @see SAXParser#setLexicalHandler()
  * @see  SAXParser#setContentHandler()
  */

class SAX_EXPORT LexicalHandler
{
public:
    /** @name Constructors and Destructor */
    //@{
    /** Default constructor */
    LexicalHandler()
    {
    }

    /** Destructor */
    virtual ~LexicalHandler()
    {
    }
    //@}

    /** @name The virtual document handler interface */

    //@{
   /**
    * Receives notification of comments.
    *
    * @descr @n The Parser will call this method to report each occurence of
    * a comment in the XML document.
    *
    * The application must not attempt to read from the array
    * outside of the specified range.
    *
    * @param count  - the counter of this comment
    * @param string - the pointer to the string of this comment
    *
    */
    virtual void               comment(unsigned int count, const ixml::XMLChar* string) = 0;

  /**
    * Receives notifications of the end of a CDATA section.
    *
    * @descr @n The SAX parser will invoke this method at the end of
    * each CDATA parsed.
    *
    */
    virtual void               endCDATA() = 0;

  /**
    * Receives notifications of the end of the DTD declarations.
    *
    * @descr @n The SAX parser will invoke this method at the end of the
    * DTD.
    *
    */
    virtual void              endDTD () = 0;

  /**
    * Receives notifications of the end of an entity.
    *
    * @descr @n The SAX parser will invoke this method at the end of an
    * entity.
    *
    */
    virtual void              endEntity () = 0;

  /**
    * Receives notifications of the start of a CDATA section.
    *
    * @descr @n The SAX parser will invoke this method at the start of
    * each CDATA parsed.
    *
    * 
    */

    virtual void               startCDATA() = 0;

  /**
    * Receives notifications of the start of the DTD declarations.
    *
    * @descr @n The SAX parser will invoke this method at the start of the
    * DTD
    *
    * @param nameCount               - the counter of the DTD name
    * @param name                   - the pointer to the DTD name 
    * @param publicIdCount           - the counter of the DTD public identifier, or 0 if none was given
    * @param publicId               - the pointer to the DTD public identifier, or NULL if none was given 
    * @param systemIdCount           - the counter of the DTD system identifier, or 0 if none was given
    * @param systemId               - the pointer to the DTD system identifier, or NULL if none was given
    */
    virtual void              startDTD(unsigned int nameCount,
                                       const ixml::XMLChar* name,
                                       unsigned int publicIdCount,
                                       const ixml::XMLChar* publicId,
                                       unsigned int systemIdCount,
                                       const ixml::XMLChar* systemId) = 0;


  /**
    * Receives notifications of the start of an entity.
    *
    * @descr @n The SAX parser will invoke this method at the start of an
    * entity.
    *
    * @param count  - the counter of the name of this entity
    * @param name - the pointer to the name of this entity
    */
    virtual void              startEntity (unsigned int count, const ixml::XMLChar* name) = 0;

    //@}
private :
    /* Unimplemented Constructors and operators */
    /* Copy constructor */
    LexicalHandler(const LexicalHandler&);
    /** Assignment operator */
    LexicalHandler& operator=(const LexicalHandler&);
};

INTEL_XML_PARSE_SAX_NAMESPACE_END

#endif
