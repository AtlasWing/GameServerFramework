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

/** @file locator.h @brief The locator interface  */

#ifndef __COM_INTEL_XML_PARSE_SAX_LOCATOR_H__
#define __COM_INTEL_XML_PARSE_SAX_LOCATOR_H__

#include "include/util/xmlstring.h"

INTEL_XML_PARSE_SAX_NAMESPACE_BEGIN

/**
  * Associates a SAX event with a document location.
  *
  * If a SAX parser provides location information to the SAX
  * application, it does so by implementing this interface and then
  * passing an instance to the application using the
  * ContentHandler::setDocumentLocator() method.  The application can use the
  * object to obtain the location of another document handler event
  * in the XML source document.
  *
  * Note that the results returned by the object will be valid only
  * during the scope of each document handler method: the application
  * will receive unpredictable results if it attempts to use the
  * locator at any other time.
  *
  * SAX parsers are not required to supply a locator, but they are
  * very strongly encouraged to do so. If the parser supplies a
  * locator, it must do so before reporting other document events.
  * If no locator has been set by the time the application receives
  * the @c startDocument() event, the application should assume that a
  * locator is not available.
  */

class SAX_EXPORT Locator
{
public:

    /** @name Constructors and Destructor */
    //@{
    /** Default constructor */
    Locator()
    {
    }

    /** Destructor */
    virtual ~Locator()
    {
    }

    //@}

    /** @name The locator interface */
    //@{
  /**
    * Returns the public identifier for the current document event.
    * 
    * @descr @n The return value is the public identifier of the document entity
    * or of the external parsed entity in which the markup triggering the event appears.
    * 
    * @return A string containing the public identifier, or NULL if none is available.
    * @see #getSystemId()
    */
    virtual const iutil::XMLString getPublicId() = 0;

  /**
    * Return the system identifier for the current document event.
    *
    * @descr @n The return value is the system identifier of the document entity 
    * or of the external parsed entity in which the markup triggering the event appears.
    *
    * If the system identifier is a URL, the parser must resolve it fully
    * before passing it to the application. For example, a file name must 
    * always be provided as a file:... URL, and other kinds of relative URI
    * are also resolved against their bases.
    * 
    *
    * @return A string containing the system identifier, or NULL
    *         if none is available.
    * @see #getPublicId()
    */
    virtual const iutil::XMLString getSystemId() = 0;

  /**
    * Returns the line number where the current document event ends.
    * 
    * @descr @n @b WARNING: The return value from the method is intended only as 
    * an approximation for the sake of diagnostics; it is not intended 
    * to provide sufficient information to edit the character content 
    * of the original XML document. In some cases, these "line" numbers 
    * match what would be displayed as columns, and in others they may not
    * match the source text due to internal entity expansion. 
    *
    * The return value is an approximation of the line number in the 
    * document entity or external parsed entity where the markup triggering 
    * the event appears.
    *
    * If possible, the SAX driver should provide the line position of the 
    * first character after the text associated with the document event. 
    * The first line is line 1.
    *
    * @return The line number, or -1 if none is available.
    */
    virtual unsigned int getLineNumber() = 0;

    //@}

private :
    /* Copy constructor */
    Locator(const Locator&);

    /* Assignment operator */
    Locator& operator=(const Locator&);
};

INTEL_XML_PARSE_SAX_NAMESPACE_END

#endif
