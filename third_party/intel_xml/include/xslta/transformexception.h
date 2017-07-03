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

/** @file Transformexception.h The interfaces of exceptions used in transformation
 *
 * Defines the TransformerBaseException, TransformerException and 
 * TransformerConfigurationException.
 */

#ifndef _INTEL_XML_TRANSFORM_TRANSFORMEXCEPTION_H_
#define _INTEL_XML_TRANSFORM_TRANSFORMEXCEPTION_H_

#include "include/common/defines.h"
#include "include/common/exception.h" 
#include "include/util/xmlstring.h" 


INTEL_XML_TRANSFORM_NAMESPACE_BEGIN

/**
 * Holds all exceptions thrown by Intel(R) XSLT Accelerator.
 */

class XSLT_EXPORT TransformerBaseException : public ixml::Exception {
public:

/**
 * Constructor.
 */
    TransformerBaseException();

/**
 * Constructor for the specified error massage.
 *
 * @param msg - the error massage
 */
    TransformerBaseException(const ixml::XMLChar* msg);

/**
 * Gets the error massage of the exception.
 *
 * @return The error message specified as a string, or the empty string if
 *         no error message has been set.
 */
    virtual iutil::XMLString getErrorMessage();
    
/**
 * Gives comprehensive information about an exception as a string.
 *
 * @descr @n Gets all available information and organizes it into a formatted
 * string. The information includes the system ID, the error massage
 * set in this exception.
 *
 * @return A string containing comprehensive error information.
 */
    virtual iutil::XMLString toString();

/**
 * Gets the system ID of this
 * TransformerBaseException.
 * 
 * @descr @n The system ID indicates which file is in processing when
 * TransformerBaseException is thrown.
 * 
 * @return The system ID specified as a string, or NULL
 *         if the information is not available.
 */
    const ixml::XMLChar* getSystemId() const;
   
/**
 * Sets the system ID of this
 * TransformerBaseException.
 * 
 * @descr @n The system ID indicates which file is in processing when
 * TransformerBaseException is thrown.
 * 
 * @param systemId - the system ID specified as an @c XMLChar
 *                       string
 */
    void setSystemId(const ixml::XMLChar* systemId);


protected:
/**
 * The system ID in the form of XMLString.
 *
 * @descr @n The system ID indicates which file is in processing when
 * TransformerBaseException is thrown. 
 */
    iutil::XMLString    _systemId;
};


/**
 * Is the exception class thrown during the execution of the XSL
 * transformation.
 */

class XSLT_EXPORT TransformerException : public TransformerBaseException {
public:

/**
 * Creates a TransformerException with the string
 * @e msg specified the error massage.
 *
 * @param msg - the error massage
 */
    TransformerException(const ixml::XMLChar* msg);
};

/**
 * Is the exception class thrown while configuring the
 * transformation.
 */
class XSLT_EXPORT TransformerConfigurationException : public TransformerBaseException {
public:
/**
 * Creates new TransformerConfigurationException with
 * the string @e msg specified as the error massage.
 *
 * @param msg - the error massage
 */
    TransformerConfigurationException(const ixml::XMLChar* msg);
};

INTEL_XML_TRANSFORM_NAMESPACE_END

#endif //_INTEL_XML_TRANSFORM_TRANSFORMEXCEPTION_H_
