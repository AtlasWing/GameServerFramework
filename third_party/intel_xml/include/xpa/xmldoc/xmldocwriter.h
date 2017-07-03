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

/** @file xmldocwriter.h @brief The XMLDocument serialization interface
 *
 * This file defines an interface for the DOM document serialization, that is,
 * writing XMLDocument into an XML document. XML data is written to an output
 * stream, which type depends on the used encoding bindings. During the XML
 * data serialization, the namespace fixup is done when possible.
 */

#ifndef __COM_INTEL_XML_PARSE_XMLDOC_XMLDOCWRITER_H__
#define __COM_INTEL_XML_PARSE_XMLDOC_XMLDOCWRITER_H__

#include "include/common/types.h"
#include "include/common/defines.h"
#include "include/util/writer.h"

INTEL_XML_PARSE_XMLDOC_NAMESPACE_BEGIN

class XMLDocument;

/**
 * @brief Is responsible for writing the XMLDocument node into an XML document.
 *
 * @descr @n XMLDocWriter accepts the XMLDocument type for
 * serialization. If possible, well-formed XML data is created to be an
 * acceptable input for an XML parser.
 *
 * @note  In the current version, the DOCTYPE part is
 * not serialized.
 */
class XMLDOC_EXPORT XMLDocWriter 
{
public:
/** @brief Destructor.*/
    virtual ~XMLDocWriter() {}

public:

/**
 * @brief Gets the current output character encoding.
     *
 * @descr @n The method returns the encoding for serializing XMLDocument.
     * If the setEncoding() method is not invoked, the default character encoding
     * @c UTF8_ENCODING is returned.
     * 
     * @return The current output character encoding.
     *
 * @see setEncoding()
     */
    virtual ixml::EncodingStyle getEncoding() = 0;
    
/**
 * @brief Sets the character encoding for the output.
     * 
 * @descr @n The encoding used for writing XMLDocument is determined as follows:
 * if the encoding attribute has been set, that value is used;
 * otherwise, the default encoding @c UTF8_ENCODING value is used.
     *
 * @param encoding - the encoding for the serialized output
     * 
 * @see getEncoding()
     */
    virtual void setEncoding(const ixml::EncodingStyle encoding) = 0;

/**
 * @brief Serializes the specified XMLDocument to a writer object.
     *
 * @descr @n Writing an XMLDocument node produces a serialized form of data that 
 * is well-formed XML. You need to specify the writer object handler for the
 * output.
     * 
     * @param writer - the writer object for output data
 * @param doc    - XMLDocument to be serialized
     * 
 * @return TRUE if the serializing process is successful; otherwise,
 *         FALSE.
     */
    virtual bool serializeXMLDocument(iutil::Writer* writer,XMLDocument* doc) = 0;
};

INTEL_XML_PARSE_XMLDOC_NAMESPACE_END

#endif

