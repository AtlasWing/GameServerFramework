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

/** @file locator2.h @brief The locator interface  */

#ifndef __COM_INTEL_XML_PARSE_SAX_LOCATOR2_H__
#define __COM_INTEL_XML_PARSE_SAX_LOCATOR2_H__

#include "include/xpa/sax/locator.h"

INTEL_XML_PARSE_SAX_NAMESPACE_BEGIN

/**
  * Associates a SAX extension event with a document location.
  *
  * SAX2 extension to augment the entity information provided though a Locator. 
  * If an implementation supports this extension, the Locator provided in 
  * ContentHandler.setDocumentLocator() will implement this interface, 
  */

class SAX_EXPORT Locator2 : public Locator
{
public:

    /** @name Constructors and Destructor */
    //@{
    /** Default constructor */
    Locator2()
    {
    }

    /** Destructor */
    virtual ~Locator2()
    {
    }

    //@}

    /** @name The locator interface */
    //@{
  /**
    * Returns the version of XML used for the entity. This will normally be the 
    * identifier from the current entity declaration "<?xml version='...' ...?>", 
    * or be defaulted by the parser.
    * 
    * @return Identifier for the XML version being used to interpret the entity's text,
    * or NULL if that information is not yet available in the current parsing state.
    */
    virtual const iutil::XMLString getXMLVersion() = 0;

  /**
   *
   * Returns the name of the character encoding for the entity.
   *
   * @descr @n The method searches for the encoding in the locations specified below, following
   * the specified order.
   * 
   * If the encoding was declared externally (for example, in a MIME Content-Type header),
   * that will be the name returned. Else if there was an @c <?xml ...encoding='...'?> 
   * declaration at the start of the document, that encoding name will be returned. 
   * Otherwise the encoding will been inferred (normally to be UTF-8, or some UTF-16 variant),
   * and that inferred name will be returned. 
   *
   * When an InputSource is used to provide an entity's character stream, 
   * this method returns the encoding provided in that input stream. 
   *
   * @note  some recent W3C specifications require that text in some encodings 
   * be normalized, using Unicode Normalization Form C, before processing. 
   * Such normalization must be performed by applications, and would normally be 
   * triggered based on the value returned by this method. 
   *
   * Encoding names may be those used by the underlying JVM*, and comparisons should be case-insensitive.
    * 
    */
    virtual const iutil::XMLString getEncoding() = 0;

    //@}

private :
    /* Copy constructor */
    Locator2(const Locator2&);

    /* Assignment operator */
    Locator2& operator=(const Locator2&);
};

INTEL_XML_PARSE_SAX_NAMESPACE_END

#endif
