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

/** @file domexception.h @brief Definition of exceptions in the DOM model
 *
 * DOM operations only raise exceptions in extra-ordinary circumstances,
 * that is, when an operation cannot be performed for logical reasons,
 * because of data loss or implementation instability. In ordinary processing
 * situations, DOM methods generally return specific error values, for example,
 * out-of-bound errors when using @c DOMNodeList.
 *
 * For implementation-dependent circumstances, client applications should raise
 * other exceptions, for example, if a NULL argument is passed.
 * 
 * For languages and object systems that do not support the concept of
 * exceptions, error conditions can be indicated using native error reporting 
 * mechanisms. For some bindings, for example, methods can return error codes
 * similar to those listed in the corresponding method descriptions.
 * @see <a href='http://www.w3.org/TR/2000/REC-DOM-Level-2-Core-20001113'>
 * Document Object Model (DOM) Level 2 Core Specification</a>
 *
 */



#ifndef __COM_INTEL_XML_PARSE_DOM_DOMEXCEPTION_H__
#define __COM_INTEL_XML_PARSE_DOM_DOMEXCEPTION_H__

#include "include/common/defines.h"
#include "include/common/exception.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN

/**
 * Is responsible for handling exceptions in DOM.
 *
 * Since DOM Level 1
 */
class DOM_EXPORT DOMException : public ixml::Exception
{
public:

    /**
      * Default constructor.
      *
      */
    DOMException():_code((ExceptionCode) 0) {}

    /**
      * The constructor taking an error code and a message.
      *
      * @param code    - the error code which indicates the exception
      * @param message - the string XMLString with the error message
      */
    DOMException(short code, const iutil::XMLString& message)
        : ixml::Exception(message), _code(code) {}

    /**
      * The constructor taking an error code and a message.
      *
      * @param code    - the error code which indicates the exception
      * @param message - the string @c XMLChar* with the error message
      */
    DOMException(short code, const ixml::XMLChar* message)
        : ixml::Exception(message), _code(code) {}

    /**
      * The copy constructor.
      *
      * @param other - the object to be copied
      */
    DOMException(const DOMException& other)
        : ixml::Exception(other._message), _code(other._code) {}

    /**
      * Destructor.
      *
      */
    virtual ~DOMException() {}

public:

    /**
     * Exception codes for produced DOM exceptions.
     *
     * Since DOM Level 1: @e INDEX_SIZE_ERR, @e DOMSTRING_SIZE_ERR, 
     * @e HIERARCHY_REQUEST_ERR, @e WRONG_DOCUMENT_ERR,
     * @e INVALID_CHARACTER_ERR, @e NO_DATA_ALLOWED_ERR,
     * @e NO_MODIFICATION_ALLOWED_ERR, @e NOT_FOUND_ERR,
     * @e NOT_SUPPORTED_ERR, and @e INUSE_ATTRIBUTE_ERR.
     * 
     * Since DOM Level 2: @e INVALID_STATE_ERR, @e SYNTAX_ERR,
     * @e INVALID_MODIFICATION_ERR, @e NAMESPACE_ERR, and
     * @e INVALID_ACCESS_ERR.
     *
     * Since DOM Level 3: @e VALIDATION_ERR.
     */

    enum ExceptionCode {
         INDEX_SIZE_ERR                    = 1, ///< The index or size is negative or greater than allowed
         DOMSTRING_SIZE_ERR                = 2, ///< The specified range of text does not fit into DOMString
         HIERARCHY_REQUEST_ERR            = 3, ///< The node is inserted in a place where it does not belong
         WRONG_DOCUMENT_ERR                = 4, ///< The node is a document that does not support it
         INVALID_CHARACTER_ERR            = 5, ///< The specified character is invalid or illegal,
                                             ///< for example, in a name. For a definition of a legal
                                             ///< character and a legal name character, see the 
                                             ///< <a href="http://www.w3.org/TR/xml/">XML specification</a>
         NO_DATA_ALLOWED_ERR            = 6, ///< Data is specified for a node that does not support data
         NO_MODIFICATION_ALLOWED_ERR    = 7, ///< An attempt is made to modify an object where it is not allowed
         NOT_FOUND_ERR                    = 8, ///< An attempt is made to reference a node in a context where it does not exist
         NOT_SUPPORTED_ERR                = 9, ///< The implementation does not support the requested object type
                                             ///< or operation
         INUSE_ATTRIBUTE_ERR            = 10, ///< An attempt is made to add an attribute that is in use elsewhere
         INVALID_STATE_ERR                = 11, ///< An attempt is made to use an object that is not or no longer usable
         SYNTAX_ERR                        = 12, ///< The specified string is invalid or illegal
         INVALID_MODIFICATION_ERR        = 13, ///< An attempt is made to modify the type of the underlying object
         NAMESPACE_ERR                    = 14, ///< An attempt is made to create or change an object incorrectly 
                                              ///< with regard to namespaces
         INVALID_ACCESS_ERR                = 15, ///< The parameter or operation is not supported by the underlying object
         VALIDATION_ERR                    = 16  ///< The Node becomes invalid after calling @c insertBefore() 
                                              ///< or @c removeChild(); the operation is aborted
        };

    /**
     * Gets the error code in the parser process.
     * 
     * @return The error code of the occurred exception.
     */
    virtual short                getErrorCode() {return _code;}

    /** 
     * Gets the description of an error. 
     *
     * @return The error description.
     */
    virtual iutil::XMLString    getErrorMessage() {return _message;}
    
    /**
     * Provides a combination of error message and attached data, if any.
     * @descr @n Data attached to the error can be the line number 
     * or exception code. 
     * 
     * @return The detailed error description.
     */
    virtual iutil::XMLString    toString();

protected:
    
    /**
     * Returns the type of error as a code value from the DOMException#ExceptionCode enum.
     */
    short            _code;
};

INTEL_XML_PARSE_DOM_NAMESPACE_END

#endif



