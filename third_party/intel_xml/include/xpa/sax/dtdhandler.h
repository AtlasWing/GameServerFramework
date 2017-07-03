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


/** @file dtdhandler.h @brief Interface for receiving DTD-related events in SAX parsing */


#ifndef __COM_INTEL_XML_PARSE_SAX_DTDHANDLER_H__
#define __COM_INTEL_XML_PARSE_SAX_DTDHANDLER_H__

INTEL_XML_PARSE_SAX_NAMESPACE_BEGIN

/**
  * Receives notifications of basic DTD-related events.
  *
  * If a SAX application needs information about notations and
  * unparsed entities, then the application implements this
  * interface and registers an instance with the SAX parser using
  * the SAXParser::setDTDHandler() method. The parser uses the
  * instance to report notation and unparsed entity declarations to
  * the application.
  *
  * The SAX parser may report these events in any order, regardless
  * of the order in which the notations and unparsed entities were
  * declared; however, all DTD events must be reported after the
  * ContentHandler#startDocument event, and before the first
  * ContentHandler#startElement event.
  *
  * It is up to the application to store the information for
  * future use (perhaps in a hash table or object tree).
  * If the application encounters attributes of type "NOTATION",
  * "ENTITY", or "ENTITIES", it can use the information that it
  * obtained through this interface to find the entity and/or
  * notation corresponding with the attribute value.
  *
  * @see SAXParser#setDTDHandler()
  */

class SAX_EXPORT DTDHandler
{
public:
    /** @name Constructors and Destructor */
    //@{
    /** Default Constructor. */
    DTDHandler()
    {
    }

    /** Destructor. */
    virtual ~DTDHandler()
    {
    }

    //@}

    /** @name The DTD handler interface */
    //@{
  /**
    * Receives notifications of a notation declaration event.
    *
    * @descr @n It is up to the application to record the notation for later
    * reference, if necessary.
    *
    * If a system identifier is present, and it is a URL, the SAX
    * parser must resolve it fully before passing it to the
    * application.
    *
    * @param nameCount               - the counter of the notation name
    * @param name                   - the pointer to the notation name 
    * @param publicIdCount           - the counter of the notation's public identifier, or 0 if none was given
    * @param publicId               - the pointer to the notation's public identifier, or NULL if none was given 
    * @param systemIdCount           - the counter of the notation's system identifier, or 0 if none was given
    * @param systemId               - the pointer to the notation's system identifier, or NULL if none was given
    *
    * */
    virtual void               notationDecl(unsigned int nameCount,
                                            const ixml::XMLChar* name,
                                            unsigned int publicIdCount,
                                            const ixml::XMLChar* publicId,
                                            unsigned int systemIdCount,
                                            const ixml::XMLChar* systemId) = 0;

  /**
    * Receives notifications of an unparsed entity declaration event.
    *
    * @descr @n Note that the notation name corresponds to a notation
    * reported by the notationDecl() event.  It is up to the
    * application to record the entity for later reference, if
    * necessary.
    *
    * If the system identifier is a URL, the parser must resolve it
    * fully before passing it to the application.
    *
    * @param nameCount               - the counter of the unparsed entity name
    * @param name                   - the pointer to the unparsed entity name 
    * @param publicIdCount           - the counter of the entity public identifier, or 0 if none was given
    * @param publicId               - the pointer to the entity public identifier, or NULL if none was given 
    * @param systemIdCount           - the counter of the entity system identifier, or 0 if none was given
    * @param systemId               - the pointer to the entity system identifier, or NULL if none was given
    * @param notationNameCount       - the counter of the name of the associated notation
    * @param notationName           - the pointer to the name of the associated notation
    *
    */
    virtual void               unparsedEntityDecl(unsigned int nameCount,
                                                  const ixml::XMLChar* name,
                                                  unsigned int publicIdCount,
                                                  const ixml::XMLChar* publicId,
                                                  unsigned int systemIdCount,
                                                  const ixml::XMLChar* systemId,
                                                  unsigned int notationNameCount,
                                                  const ixml::XMLChar* notationName) = 0;

    //@}

private :
    /* Unimplemented constructors and operators */

    /* Copy constructor */
    DTDHandler(const DTDHandler&);

    /* Assignment operator */
    DTDHandler& operator=(const DTDHandler&);

};

INTEL_XML_PARSE_SAX_NAMESPACE_END

#endif
