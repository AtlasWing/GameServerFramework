/*
 
 *---------------------------------------------------------------------------
 *
 *     Copyright 2007~2008 Intel Corporation All Rights Reserved.
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

#ifndef __COM_INTEL_XML_ERRORHANDLER_H__
#define __COM_INTEL_XML_ERRORHANDLER_H__

#include "include/common/exception.h"
INTEL_XML_NAMESPACE_BEGIN
/** @file errorhandler.h
 *
 *   The default error handler
 */

/**
 * Deals with all errors. Developers can create their own ErrorHandler
 * by inheriting this class and implement 4 functions: two error()s, 
 * fatalError(), and warning().
 */
class COMMON_EXPORT ErrorHandler
{
public:
    /** Default constructor */
    ErrorHandler() {}
    /** Destructor */
    virtual ~ErrorHandler() {}
    
public:
    /**
     * Receives notification of an error.
     * @param e - Exception object
     */
    virtual void error(Exception &e) = 0;
    /**
     * Receives notification of a fatal error.
     * @param e - Exception object
     */
    virtual void fatalError(Exception &e) = 0;
    /**
     * Receives notification of a warning.
     * @param e - Exception object
     */
    virtual void warning(Exception &e) = 0;

    /**
     * Receives notification of an error and
     * makes the validation stop or continue.
     * @descr @n
     * Deals with all errors occurred in runtime of schema validation.
     * Errors of the schema compiling are not handled by this method.
     * It is used to report errors from schema validation to 
     * Developers. This is similar to error(Exception &e).
     * The difference is that developer can tell the validator
     * to continue validating or stop validating by inheritting
     * its method 'bool error(const ixml::XMLChar*)'.
     *
     * Once an error occurs in runtime of schema validation,
     * if the ErrorHanlder object was set, then validator passed
     * the error message to developer by invoking this method 
     * and checks the returned value of this method. If returned 
     * value is TRUE, then validation continues until the validation
     * can not continue, for example, parsing error occurs.
     * Otherwise, the validation stops. So, if developer wants to 
     * stop validation once an error occurs, FALSE should be returned. 
     * Defaultly, validator continues doing validation if error occurs.
     *
     * @param errMsg - error content
     * @return a boolean value to tell the validator
     *             to continue validation or stop at once.
     *             TRUE tells the validator to continue
     *             validating, and FALSE will stop the 
     *             validation at once.
     */
    virtual bool error(const ixml::XMLChar* errMsg) = 0;
};

INTEL_XML_NAMESPACE_END

#endif
