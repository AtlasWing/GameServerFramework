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

/** @file pi.h @brief Processing instruction interface */


#ifndef __COM_INTEL_XML_PARSE_DOM_PI_H__
#define __COM_INTEL_XML_PARSE_DOM_PI_H__

#include "include/xpa/dom/node.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

/**
 * Represents a processing instruction node.
 *
 * @descr @n A processing instruction (PI) is used in XML as a way of keeping
 * processor-specific information in the text of the document.
 *
 * @b Example:
 * @code
 * <?xml-stylesheet href="sp.xsl" type="text/xsl"?>
 * @endcode
 *
 * This example PI provides an XSL transformation instruction to the application.
 *
 * Since DOM Level 1
 */
class DOM_EXPORT ProcessingInstruction : public Node
{
public:
    /** 
     * Destructor.
     */
    virtual ~ProcessingInstruction(){}

public:
    /**
     * Gets the target of this processing instruction.
     *
     * @descr @n XML defines the target as the first token following the markup that begins the 
     * processing instruction.
     *
     * Since DOM Level 1
     */
    virtual const ixml::XMLChar* getTarget() const  = 0;

    /**
     * Gets the content of this processing instruction.
     *
     * @descr @n The content of the processing instruction is everything from the first
     * non-whitespace character after the target to the character immediately 
     * preceding the @c ?&gt; delimiter.
     *
     * Since DOM Level 1
     * @return In case of an error, productes DOMException:
     * - @c NO_MODIFICATION_ALLOWED_ERR: Raised when the node is read-only.
     */
    virtual const ixml::XMLChar* getData() const = 0;

    /**
     * Sets the content of this processing instruction.
     *
     * @descr @n The content of the processing instruction is everything from the first
     * non-whitespace character after the target to the character immediately 
     * preceding the @c ?&gt; delimiter.
     *
     * Since DOM Level 1
     * @param data - the string containing the processing instruction
     */
    virtual void setData(const ixml::XMLChar* data) = 0;
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif
