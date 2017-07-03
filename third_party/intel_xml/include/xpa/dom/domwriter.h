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

/** @file domwriter.h @brief DOM serialization interface
 *
 * This file provides an interface for serializing a DOM document, that is,
 * writing a DOM into an XML document. 
 * XML data is written to an output stream, the type of which depends on 
 * the used encoding bindings. During serialization of XML data, 
 * namespace fixup is done when possible. 
 */

#ifndef __COM_INTEL_XML_PARSE_DOM_DOMWRITER_H__
#define __COM_INTEL_XML_PARSE_DOM_DOMWRITER_H__

#include "include/common/types.h"
#include "include/common/defines.h"
#include "include/util/writer.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

class Document;

/**
 * Is responsible for writing the DOM into an XML document.
 *
 * DOMWriter accepts the Document node type for serialization.
 * If possible, well-formed XML data is created to be acceptable input for 
 * an XML parser. 
 *
 * @note  In the current version, the DOCTYPE part is not serialized.
 *
 */
class DOM_EXPORT DOMWriter 
{
public:
    /** 
     * Destructor
     */ 
    virtual ~DOMWriter() {}

public:

    /**
      * Gets the current output character encoding. 
     *
     * @descr @n The method returns the encoding for serializing the XML document.
     * If the setEncoding() method is not invoked, the default character encoding
     * @c UTF8_ENCODING is returned.
     * 
     * @return The current output character encoding.
     *
     */
    virtual ixml::EncodingStyle getEncoding() = 0;
    
    /**
     * Sets the character encoding for the output. 
     * 
     * @descr @n The encoding to use for writing the XML document is determined
     * as follows: if the encoding attribute has been set, that value is used;
     * otherwise, the default encoding #UTF_8 is used.
     *
     * @param encoding - the encoding for serialized output
     * 
     * @see getEncoding
     */
    virtual void setEncoding(const ixml::EncodingStyle encoding) = 0;

    /**
     * Serializes the specified Document to a writer object. 
     *
     * @descr @n Writing a Document node produces a serialized form of data that 
     * is well-formed XML. You need to specify the writer object handler for output.
     * 
     * @param writer - the writer object for output data
     * @param doc    - the Document to be serialized
     * 
     * @return TRUE if the serializing process successful; otherwise, 
     *         FALSE.
     */
    virtual bool serializeDocument(iutil::Writer* writer,Document* doc) = 0;
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif

