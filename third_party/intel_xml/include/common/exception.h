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
/**
* @file exception.h Exceptions
*/


#ifndef __COM_INTEL_XML_EXCEPTION_H__
#define __COM_INTEL_XML_EXCEPTION_H__

#include "include/common/constants.h"
#include "include/common/types.h"
#include "include/common/defines.h"
#include "include/util/xmlstring.h"

INTEL_XML_NAMESPACE_BEGIN

/**
 * Is responsible for all exceptions, pure virtual
 */
class COMMON_EXPORT Exception
{
public:
    /**
     * Default constructor.
     */
    Exception();

    /**
     * Constructor with XMLChar*.
     *
     * @param message - the character sequence describing the exception information 
     */
    Exception(const XMLChar* message); 

    /**
     * Constructor with XMLString.
     * @param message - the string describing the exception information 
     */
    Exception(const iutil::XMLString& message);

    /**
     * Destructor.
     */
    virtual ~Exception();

    /**
     * Gets the description of the error.
     *
     * @return The error description as a string.
     */
    virtual iutil::XMLString getErrorMessage(); 
    
    /**
     * Provides the combination of the error message and attached data;
     * for example, the line number or exception code. 
     * 
     * @return A combination of error description and attached information,
     * if any, as a string.
     */
    virtual iutil::XMLString toString();

protected:
    /**
     * Message for exception
     */
    iutil::XMLString _message;
};

/**
 * Handles input and output exceptions.
 *
 */
class COMMON_EXPORT IOException : public Exception
{
public:
    /** Default constructor */
    IOException();

    /** XMLChar constructor 
     *
     * @param message - the character sequence describing the exception information 
     *
     */
    IOException(const XMLChar* message);

    /** XMLString constructor 
     *
     * @param message - the string describing the exception information 
     *
     */
    IOException(const iutil::XMLString& message);

    /** Copy constructor
     *
     * @param other - the object to be copied
     */
    IOException(const IOException& other);

    /** Destructor */
    virtual ~IOException();
};

/** 
 * Handles illegal argument exceptions.
 */
class COMMON_EXPORT IllegalArgumentException : public Exception
{
public:
    /** Default constructor */
    IllegalArgumentException();

    /** XMLChar constructor.
     *
     * @param message - the character sequence describing the exception information 
     *
     */
    IllegalArgumentException(const XMLChar* message);

    /** XMLString constructor.
     *
     * @param message - the string describing the exception information 
     */
    IllegalArgumentException(const iutil::XMLString& message); 

    /** Copy constructor.
     *
     * @param other - the object to be copied
     */
    IllegalArgumentException(const IllegalArgumentException& other); 

    /** Destructor. */
    virtual ~IllegalArgumentException();
};

/**
 * Indicates invoking an unsupported operation.
 *
 * Default error message is ERRORMASSAGE_UNSUPPORTED_OPERATION
 * defined in constants.h.
 */
class COMMON_EXPORT UnsupportedOperationException : public Exception {
public:

    /**
     * Creates a default UnsupportedOperationException.
     */
    UnsupportedOperationException();
    /**
     * Creates a UnsupportedOperationException by the failed operation name.
     *
     * @param operationName - the failed operation name
     */
    UnsupportedOperationException(const ixml::XMLChar* operationName);

    virtual iutil::XMLString toString();

protected:
    /**
     * Operation name
     */
    iutil::XMLString _operationName;
};

INTEL_XML_NAMESPACE_END

#endif

