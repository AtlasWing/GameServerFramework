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

/** @file validationexception.h @brief Exceptions related to XML Schema validation  */

#ifndef __COM_INTEL_XML_VALIDATIONEXCEPTION_H__
#define  __COM_INTEL_XML_VALIDATIONEXCEPTION_H__
#include "include/common/exception.h"

INTEL_XML_NAMESPACE_BEGIN
/** 
 * Handles all validation exceptions, including
 * schema parsing and run-time validation.
 */
class VALIDATION_EXPORT ValidationException : public Exception
{
public:
       /** Default constructor */
       ValidationException(){}

       /** XMLString constructor 
         * @param message - error message with the XMLString type
      */
       ValidationException(const iutil::XMLString& message) : Exception(message) {}

       /** XMLChar constructor 
         * @param message - error message with the #XMLChar type
      */
       ValidationException(const ixml::XMLChar* message) : Exception(message) {}

       /** Copy constructor 
         * @param other - a ValidationException object
      */
       ValidationException(const ValidationException& other) : Exception(other._message) {}

       /** Destructor */
       virtual ~ValidationException() {}
};
INTEL_XML_NAMESPACE_END

#endif
