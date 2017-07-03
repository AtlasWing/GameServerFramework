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

/** @file writer.h Interface for writing into the specified destination
 *
 * All interfaces are abstract virtual functions. 
 */

#ifndef __COM_INTEL_XML_UTIL_WRITER_H__
#define __COM_INTEL_XML_UTIL_WRITER_H__

#include "include/common/types.h"
#include "include/common/defines.h"
#include "include/common/exception.h"

INTEL_XML_UTIL_NAMESPACE_BEGIN

/**
 * Enables writing to the specified destination.
 *
 * User applications can inherit this base class to build a new class.
 *
 * 
 */
class UTIL_EXPORT Writer
{
public:
    /** 
     * Destructor
     */
    virtual    ~Writer() {}

public:
    /** 
     * Tests whether the data resource is open.
     *
     * @return TRUE if the data resource is open; 
     *         otherwise FALSE.
     */
    virtual bool isOpen() = 0;    
    

    /**
     * Closes the data resource and frees the buffer for writing.
     * 
     * @descr @n If the resource is not open, this method does nothing.
     *
     * @return @c 0 if the data resource is successfully closed
     * or is not open.
     */
    virtual int close() = 0;
        
    /**
     * Writes data to the resource. 
     * 
     * @descr @n This method attempts to write the data and returns the actual 
     * number of written bytes.
     *
     * @param buffer - the address of the data
     * @param size   - the number of bytes of the actual write data or 0 if
     *                 there is no data
     * 
     * @return The actual number of written bytes on success; otherwise, 
     *         @c 0xFFFF FFFF FFFF FFFF.The method getErrorMessage()
     *         can get the error message.
     */
    virtual ixml::XMLUInt64 write(const ixml::XMLChar* buffer, ixml::XMLUInt64 size) = 0;

    /**
     * Gets the current filled size of the destination.
     *
     * @return The size of the destination, in bytes.
     * 
     */
    virtual ixml::XMLUInt64 getSize() = 0;

    /**
     * Notifies that the end of data has been reached.
     *
     * @return TRUE if the end of data has been reached;
     *         otherwise; FALSE.
     */
    virtual bool eof() = 0;
    
    /**
     * Gets the error message. 
     * 
     * @descr @n When an error occurs, use this method to get 
     * the detailed error message.
     *
     * @return The error message in case an error occurs; otherwise,
     *         NULL.
     */
    virtual const ixml::XMLChar* getErrorMessage() = 0;
};

INTEL_XML_UTIL_NAMESPACE_END

#endif


