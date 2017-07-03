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

/**  @file namespace.h  @brief Namespace interface.
 * 
 * Represents a namespace that is used 
 * in XML as a way to keep namespace in the text of the document.
 */

#ifndef __COM_INTEL_XML_PARSE_DOM_NAMESPACE_H__
#define __COM_INTEL_XML_PARSE_DOM_NAMESPACE_H__

#include "include/xpa/dom/attr.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

/**
 * Inherits the Attr interface. A Namespace objects is a special
 * Attr with the definite prefix @c xmlns, and
 * @c namespaceURI "http://www.w3.org/2000/xmlns/".
 *
 * @see Node
 */
class DOM_EXPORT Namespace : public Attr
{
public:
    /** 
     * Destructor
     */
    virtual ~Namespace(){}

public:

    /**
     * Gets the prefix part of a Namespace node.
     *
     * @descr @n Intel DOM extension
     */
    virtual const ixml::XMLChar* prefix() const = 0;

    /**
     * Gets the URI part of a Namespace node.
     *
     * @descr @n Intel DOM extension
     */
    virtual const ixml::XMLChar* uri() const = 0;
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif

