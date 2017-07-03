/*
 *---------------------------------------------------------------------------
 *
 *     Copyright 2007 Intel Corporation All Rights Reserved.
 *     The source code contained or described herein and all documents
 *     related to the source code ("Material") are owned by Intel
 *     Corporation or its suppliers or licensors. Title to the Material
 *     remains with Intel Corporation or its suppliers and licensors
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
 * @file xmldocument.h @brief The XMLDocument object interface
 *
 * The XMLDocument interface represents the stream-based XML Document.
 */


#ifndef __COM_INTEL_XML_PARSE_XMLDOC_XMLDOCUMENT_H__
#define __COM_INTEL_XML_PARSE_XMLDOC_XMLDOCUMENT_H__

#include "include/xpa/dom/node.h"

INTEL_XML_PARSE_XMLDOC_NAMESPACE_BEGIN

/** @brief Represents the stream-based XML Document.
 *
 *  XMLDocument is Intel proprietary compact description format
 *  of an XML document. This format is an efficient intermediate
 *  XML data representation across different components.
 *  For details, please refer to the User Guide.
 *
 */

class XMLDOC_EXPORT XMLDocument
{
public:
    /** 
     *@brief Destructor.
     */
    virtual ~XMLDocument() {}
};

INTEL_XML_PARSE_XMLDOC_NAMESPACE_END

#endif

