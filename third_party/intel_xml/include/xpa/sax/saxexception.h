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

/** @file saxexception.h Interface for processing SAX exceptions */


#ifndef __COM_INTEL_XML_PARSE_SAX_SAXEXCEPTION_H__
#define __COM_INTEL_XML_PARSE_SAX_SAXEXCEPTION_H__

#include "include/common/exception.h"
#include "include/common/defines.h"

INTEL_XML_PARSE_SAX_NAMESPACE_BEGIN
/**
 * Manages exceptions for all SAX exceptions.
 */
class SAX_EXPORT SAXException : public ixml::Exception
{
public:

    /** Default constructor */
    SAXException():_line(0) {}

    /** 
     *
     * XMLString constructor 
     *
     * @param message - the string to describe the exception information
     * @param line    - the line number of the raised exception 
     *
     */
    SAXException(const iutil::XMLString& message, unsigned int line=0)
        : ixml::Exception(message), _line(line) {}

    /** 
     * Char constructor
     *
     *
     * @param message - the character sequence describing the exception information
     * @param line    - the line number of the raised exception 
     */
    SAXException(const ixml::XMLChar* message, unsigned int line=0)
        : ixml::Exception(message),_line(line) {}

    /** 
     *
     * Copy constructor 
     *
     * @param other - the reference of another exception which should be copied.
     *
     */
    SAXException(const SAXException& other)
        : ixml::Exception(other._message),_line(other._line) {}

    /** Destructor */
    virtual ~SAXException() {}

    /**
     * Gets the line number that produced the exception
     * in the parsed XML document.
     * 
     * @return The number of the line where the occurred exception.
     */
    virtual unsigned int getLineNumber() {return _line;}

    /**
     * Provides the description of an error.
     *
     * @return The error description.
     */
    virtual iutil::XMLString getErrorMessage() {return _message;}

    /**
     * Provides the combination of an error message and attached data, if any
     *
     * @descr @n For example, the method can provide the line number or exception code. 
     *
     * @return The detailed description of an error.
     */
    virtual iutil::XMLString toString();

protected:

    /**
     * Gets the current line number of the parsed XML document.
     *
     */    
    unsigned int             _line;
};

INTEL_XML_PARSE_SAX_NAMESPACE_END

#endif

