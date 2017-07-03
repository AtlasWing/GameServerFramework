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

/** @file xpathexception.h Exceptions related to XPath */

#ifndef _INTEL_XML_XPATH_XPATHEXCEPTION_H_
#define _INTEL_XML_XPATH_XPATHEXCEPTION_H_

#include "include/common/defines.h"
#include "include/common/exception.h" //Exception
#include "include/util/xmlstring.h" //XMLString

INTEL_XML_XPATH_NAMESPACE_BEGIN

/** Is the base class for all XPath related exceptions. */
class COMMON_EXPORT XPathException : public ixml::Exception {
public:
    /** Constructs a new XPathException with the specified detail message.
    * @param msg - the error message
    */
    XPathException(const ixml::XMLChar* msg) : ixml::Exception(msg) {}

    /** Constructs a new XPathException with the specified detail message.
    * @param msg - the error message
    */
    XPathException(const iutil::XMLString& msg) : ixml::Exception(msg) {}
};

/** Handles all errors occurring during evaluation of an expression. */
class COMMON_EXPORT XPathExpressionException: public XPathException {
public:
    /** Constructs a new XPathExpressionException with the specified detail message.
    * @param msg - the error message
    */
    XPathExpressionException(const ixml::XMLChar* msg) : XPathException(msg) {}

    /** Constructs a new XPathException with the specified detail message.
    * @param msg - the error message
    */
    XPathExpressionException(const iutil::XMLString& msg) : XPathException(msg) {}    
};

/** Handles errors occurring during configuration of XPathFactory. */
class COMMON_EXPORT XPathFactoryConfigurationException: public XPathException {
public:
    /** Constructs a new XPathExpressionException with the specified detail message.
     * @param msg - the error message
    */
    XPathFactoryConfigurationException(const ixml::XMLChar* msg) : XPathException(msg) {}

    /** Constructs a new XPathFactoryConfigurationException with the specified detail message.
    * @param msg - the error message
    */
    XPathFactoryConfigurationException(const iutil::XMLString& msg) : XPathException(msg) {}    
};
    
INTEL_XML_XPATH_NAMESPACE_END

#endif
