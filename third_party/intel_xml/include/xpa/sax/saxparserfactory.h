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
/** @file saxparserfactory.h Definition of the factory object for SAXParser */


#ifndef __COM_INTEL_XML_PARSE_SAX_SAXPARSERFACTORY_H__
#define __COM_INTEL_XML_PARSE_SAX_SAXPARSERFACTORY_H__

#include "include/common/defines.h"

INTEL_XML_PARSE_SAX_NAMESPACE_BEGIN

class SAXParser;

/**
 * Manages SAX Parser objects. 
 *
 * Use this class to create and release SAXParser objects for
 * parsing XML documents. 
 *
 */
class SAX_EXPORT SAXParserFactory
{
public:    
    /** 
     * Destructor
     */
    virtual ~SAXParserFactory() {}

public:

    /**
     * Creates a new SAXParser object.
     * 
     * @return The created SAXParser object.
     */
    virtual SAXParser* createSAXParser() = 0;

    /**
     * Releases the SAXParser object.
     * 
     * @param parser - the SAXParser object to be used for performing
     *                 the parsing process; should be released when not needed
     */
    virtual void releaseSAXParser(SAXParser* parser) = 0;

public:

    /**
     *  Creates the SAXParserFactory object.
     *
     * @return The SAXParserFactory object.
     */
    static SAXParserFactory* newInstance();

    /**
     *  Releases the SAXParserFactory object.
     *
     *    @param factory - the SAXParserFactory object to be released
     */
    static void releaseInstance(SAXParserFactory* factory);
};

INTEL_XML_PARSE_SAX_NAMESPACE_END

#endif

