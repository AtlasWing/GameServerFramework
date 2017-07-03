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

/**
 * @file xmldocfactory.h @brief The interface implementing XMLDocParser and
 * XMLDocWriter
 *
 * XMLDocFactory provides the interface for performing operations independent
 * of the XMLDocument instance.
 */

#ifndef __COM_INTEL_XML_PARSE_XMLDOC_XMLDOCFACTORY_H__
#define __COM_INTEL_XML_PARSE_XMLDOC_XMLDOCFACTORY_H__

#include "include/common/types.h"
#include "include/common/defines.h"

INTEL_XML_PARSE_XMLDOC_NAMESPACE_BEGIN

class XMLDocument;
class XMLDocParser;
class XMLDocWriter;

/**
 * @brief Provides a number of methods for performing operations independent
 * of the instance of the document object model.
 */
class XMLDOC_EXPORT XMLDocFactory
{
public:    
/** Destructor. */
    virtual ~XMLDocFactory() {}

public:

/**
 * @brief Creates the XMLDocParser object.
     * 
 * @descr @n XMLDocParser parses the XML document into the event stream.
     */
    virtual XMLDocParser* createXMLDocParser() = 0;

/**
 * @brief Creates the XMLDocWriter object.
     *
 * @descr @n XMLDocWriter serializes XMLDocument back to the XML document.
     */
    virtual XMLDocWriter* createXMLDocWriter() = 0;
    
/**
 * @brief Releases the XMLDocParser object created by
 * XMLDocFactory.
     *
 * @param parser - the object to release
     */
    virtual void releaseXMLDocParser(XMLDocParser* parser) = 0;

/**
 * @brief Releases the XMLDocWriter object created by
 * XMLDocFactory.
 *
 * @descr @n Intel extension.
     *
 * @param writer - the object to release
     */
    virtual void releaseXMLDocWriter(XMLDocWriter* writer) = 0;

/**
 * @brief Releases the XMLDocument object created by
 * XMLDocParser.
     *
 * @descr @n Intel extension.
 * @param xmlDoc - the object to release
     */
    virtual void releaseXMLDocument(XMLDocument* xmlDoc) = 0;

public:
/**
 * @brief Creates the XMLDocFactory object.
     *
     * @return The XMLDocFactory object.
     */
    static XMLDocFactory* newInstance();

/**
 * @brief Releases the XMLDocFactory object.
 *
 * @param factory - the object to release
     */
    static void releaseInstance(XMLDocFactory* factory);
};

INTEL_XML_PARSE_XMLDOC_NAMESPACE_END

#endif
 
