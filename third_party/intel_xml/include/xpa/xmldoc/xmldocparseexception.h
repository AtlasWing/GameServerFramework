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

/** @file xmldocparseexception.h @brief The interface for handling XMLDocParser
 * specific exceptions
 */


#ifndef __COM_INTEL_XML_PARSE_XMLDOC_XMLDOCPARSEEXCEPTION_H__
#define __COM_INTEL_XML_PARSE_XMLDOC_XMLDOCPARSEEXCEPTION_H__

#include "include/common/defines.h"
#include "include/common/exception.h"

INTEL_XML_PARSE_XMLDOC_NAMESPACE_BEGIN

/** @brief Is responsible for all XMLDocument exceptions.*/

class XMLDOC_EXPORT XMLDocParseException : public ixml::Exception
{ 
public:

/** @brief Default constructor.*/
    XMLDocParseException() {}

/** @brief XMLString constructor.
     *
 * @param message - the string describing the exception information
     */
    XMLDocParseException(const iutil::XMLString& message)
        : ixml::Exception(message) {}

/**
 * @brief Char constructor.
     *
 * @param message - the character sequence describing the exception information
     */
    XMLDocParseException(const ixml::XMLChar* message)
        : ixml::Exception(message) {}

/**
 * @brief Copy constructor.
     *
 * @param other - the reference of another exception that should be copied
     *
     */
    XMLDocParseException(const XMLDocParseException& other)
        : ixml::Exception(other._message) {}


/** @brief Destructor.*/
    virtual ~XMLDocParseException() {}
};

INTEL_XML_PARSE_XMLDOC_NAMESPACE_END

#endif

