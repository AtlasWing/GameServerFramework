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

/** @file notation.h @brief Notation declaration.
 *
 * This file describes the notation declared in the DTD. 
 * A notation is a declaration, by name, of the format of an unparsed entity 
 * (see section 4.7 of the <a href="http://www.w3.org/TR/xml/">XML 1.0 
 * specification</a>) or a formal declaration of ProcessingInstruction targets
 * (see section 2.6 of the XML 1.0 specification). 
 */


#ifndef __COM_INTEL_XML_PARSE_DOM_NOTATION_H__
#define __COM_INTEL_XML_PARSE_DOM_NOTATION_H__

#include "include/xpa/dom/node.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

/**
 * Represents the interface to the notation. 
 * 
 * A Notation node has no parent node. Its @c nodeName
 * attribute inherited from Node is set to the declared notation name.
 * Notation nodes are read-only because the DOM Level 1 specification
 * does not support editing them. 
 *
 * Since DOM Level 1
 */
class DOM_EXPORT Notation : public Node
{
public:
    /** 
     * Destructor
     */
    virtual ~Notation() {}

public:
    /**
     * Gets the public identifier of this notation.
     * @descr @n Since DOM Level 1
     * @return The public identifier of the notation or NULL
     * if it is not specified. 
     *
     */
    virtual const ixml::XMLChar* getPublicId() const = 0;

    /**
     * Gets the system identifier of this notation.
     * @descr @n Since DOM Level 1
     * @return The system identifier of the notation or NULL
     * if it is not specified. 
     * 
     */
    virtual const ixml::XMLChar* getSystemId() const = 0;
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif
