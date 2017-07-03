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

/** @file entityresolver.h @brief Interface for resolving entities in SAX parsing */

#ifndef __COM_INTEL_XML_PARSE_SAX_ENTITYRESOLVER_H__
#define __COM_INTEL_XML_PARSE_SAX_ENTITYRESOLVER_H__

#include "include/util/xmlstring.h"

INTEL_XML_NAMESPACE_BEGIN
class StreamSource;
INTEL_XML_NAMESPACE_END

INTEL_XML_PARSE_SAX_NAMESPACE_BEGIN

/**
  * Is the basic interface for resolving entities.
  *
  * If a SAX application needs to implement customized handling
  * for external entities, it must implement this interface and
  * register an instance with the SAX parser using the 
  * SAXParser::setEntityResolver() method.
  *
  * The parser will then allow the application to intercept any
  * external entities (including the external DTD subset and external
  * parameter entities, if any) before including them.
  *
  * Many SAX applications will not need to implement this interface,
  * but it will be especially useful for applications that build
  * XML documents from databases or other specialised input sources,
  * or for applications that use URI types other than URLs.
  *
  * The application can also use this interface to redirect system
  * identifiers to local URIs or to look up replacements in a catalog
  * (possibly by using the public identifier).
  */
class SAX_EXPORT EntityResolver
{
public:
    /** @name Constructors and Destructor */
    //@{

    /** Default Constructor */
    EntityResolver()
    {
    }

    /** Destructor */
    virtual ~EntityResolver()
    {
    }

    //@}

    /** @name The EntityResolver interface */
    //@{

  /**
    * Allows the application to resolve external entities.
    *
    * @descr @n The Parser will call this method before opening any external
    * entity except the top-level document entity (including the
    * external DTD subset, external entities referenced within the
    * DTD, and external entities referenced within the document
    * element): the application may request that the parser resolve
    * the entity itself, that it use an alternative URI, or that it
    * use an entirely different input source
    *
    * Application writers can use this method to redirect external
    * system identifiers to secure and/or local URIs, to look up
    * public identifiers in a catalogue, or to read an entity from a
    * database or another input source (including, for example, a dialog
    * box).
    *
    * If the system identifier is a URL, the SAX parser must
    * resolve it fully before reporting it to the application.
    *
    * @param publicIdCount           - the counter of the public identifier of the 
    *                                  external entity being referenced, or 0
    *                                  if if none was supplied
    * @param publicId               - the pointer to the public identifier of the 
    *                                  external entity being referenced, or 
    *                                  NULL if if none was supplied
    * @param systemIdCount           - the counter of the system identifier of the 
    *                                  external entity being referenced, or 0 
    *                                  if if none was supplied
    * @param systemId               - the pointer to the system identifier of the 
    *                                  external entity being referenced, or 
    *                                  NULL if if none was supplied
    * 
    */
    virtual ixml::StreamSource*               resolveEntity(unsigned int publicIdCount,
                                                            const ixml::XMLChar* publicId,
                                                            unsigned int systemIdCount,
                                                            const ixml::XMLChar* systemId) = 0;

    //@}

private :
    /* Unimplemented constructors and operators */


    /* Copy constructor */
    EntityResolver(const EntityResolver&);

    /* Assignment operator */
    EntityResolver& operator=(const EntityResolver&);

};

INTEL_XML_PARSE_SAX_NAMESPACE_END

#endif
