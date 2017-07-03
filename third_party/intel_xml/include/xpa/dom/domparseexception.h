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

/** @file domparseexception.h @brief Interface for handling
 * DOM Parser specific exceptions. 
 *
 * This exception is different from DOMException. it is raised in 
 * the process of DOM tree building. During this period, exceptions
 * can point out the line number of the original XML document. 
 *
 */


#ifndef __COM_INTEL_XML_PARSE_DOM_DOMPARSEEXCEPTION_H__
#define __COM_INTEL_XML_PARSE_DOM_DOMPARSEEXCEPTION_H__

#include "include/common/defines.h"
#include "include/common/exception.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

/**
 * Is responsible for all DOM exceptions
 *
 */
class DOM_EXPORT DOMParseException : public ixml::Exception
{ 
public:

    /** 
     * Default constructor 
     */
    DOMParseException() {}

    /** 
     *
     * XMLString constructor 
     *
     * @param message - the string to describe the exception information.
     * @param line - the line number of exception raised.
     *
     */
    DOMParseException(const iutil::XMLString& message, unsigned int line=0)
        : ixml::Exception(message),_line(line) {}

    /** 
     * Char constructor
     *
     *
     * @param message - the char sequence to describe the exception information.
     * @param line - the line number of exception raised.
     */
    DOMParseException(const ixml::XMLChar* message, unsigned int line=0)
        : ixml::Exception(message),_line(line) {}

    /** 
     *
     * Copy constructor 
     *
     * @param other - the reference of another exception which should be copied.
     *
     */
    DOMParseException(const DOMParseException& other)
        : ixml::Exception(other._message),_line(other._line) {}

    /** 
     * Destructor
     */
    virtual ~DOMParseException() {}

    /**
     * Gets the line number in the parser process.
     * 
     * @return The line number where the exception occurred.
     * 
     */
    unsigned int getLineNumber() {return _line;}
    
    /** 
     * Gets the description of an error. 
     *
     * @return The description for an error as a string.
     */
    virtual iutil::XMLString getErrorMessage() {return _message;}

    /**
     * Provides a combination of error message and attached 
     * information if it exists; for example, the line number or exception code. 
     * 
     *
     * @return The detailed error description as a string.
     */
    virtual iutil::XMLString toString();

protected:    
    /**
     * Gets the current line number of the parsed XML document.
     *
     */    
    unsigned int            _line;
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif



