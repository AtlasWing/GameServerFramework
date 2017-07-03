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


/** @file memorywriter.h Mechanism for writing the source data to memory*/
#ifndef __COM_INTEL_XML_UTIL_MEMORYWRITER_H__
#define __COM_INTEL_XML_UTIL_MEMORYWRITER_H__

#include "include/util/writer.h"

INTEL_XML_UTIL_NAMESPACE_BEGIN

/**
 * Is a simple implementation 
 * to write memory.
 *
 * @see Writer
 */
class UTIL_EXPORT MemoryWriter: public Writer
{
public:
    /** 
     * Destructor.
     */
    virtual ~MemoryWriter() {}

public:
    /**
     * Opens the memory buffer.
     * 
     * @param buffer - memory buffer pointer to the destination of the data
     * @param size   - the size of the memory buffer
     * 
     * @return TRUE if the buffer is opened successfully;
     *         otherwise FALSE.
     */
    virtual bool open(const ixml::XMLChar* buffer, ixml::XMLUInt64 size) = 0;

    /**
     * Gets the default empty memory writer. 
     * 
     * @descr @n To open a specific memory buffer, use the open() method.
     *
     * @return The pointer that represents the default memory writer.
     */
    static MemoryWriter* createMemoryWriter();

    /**
     * Gets the memory writer by the specified buffer.
     *
     * @param buffer - the pointer to the buffer start address
     * @param size   - the size of the buffer that you need to open
     *
     * @return The pointer that represents the default memory writer. 
     * This MemoryWriter pointer opened the buffer that you specified.
     */
    static MemoryWriter* createMemoryWriter(const ixml::XMLChar* buffer, ixml::XMLUInt64 size);

    /**
     * Releases the MemoryWriter object.
     *
     * @param writer - the pointer to the MemoryWriter object that
     *                 you want to release
     */
    static void releaseMemoryWriter(MemoryWriter* writer);

};

INTEL_XML_UTIL_NAMESPACE_END

#endif

