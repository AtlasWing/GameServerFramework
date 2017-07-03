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

/** @file memoryreader.h Mechanism for reading the source data from memory */
#ifndef __COM_INTEL_XML_UTIL_MEMORYREADER_H__
#define __COM_INTEL_XML_UTIL_MEMORYREADER_H__

#include "include/util/reader.h"

INTEL_XML_UTIL_NAMESPACE_BEGIN

/**
 * Is a simple implementation 
 * to read memory.   
 *
 * @see Reader
 */
class UTIL_EXPORT MemoryReader: public Reader
{
public:
    /** 
     * Destructor 
     */
    virtual ~MemoryReader() {}

public:
    /**
     * Provides access to memory.
     * 
     * 
     * @param buffer - The data resoure name or URL
     * @param size - The size of data
     * 
     * @return TRUE if the data resource is opened successfully, 
     * otherwise FALSE.
     */
    virtual bool open(const ixml::XMLChar* buffer, ixml::XMLUInt64 size) = 0;
 
    /**
     * Gets the default empty memory reader. 
     * 
     * @descr @n Users can use open() to open
     * a memory buffer.
     *
     * @return  A MemoryReader pointer that represents the default file reader.
     */
    static MemoryReader* createMemoryReader();

    /**
     * Gets a specific memory reader through a buffer.
     *
     * @param buffer - the pointer to the buffer start address
     * @param size -  the buffer size you want to open
     *
     * @return   A MemoryReader pointer that represents the default memory reader. 
     * This Reader has opened the buffer you specified.
     */
    static MemoryReader* createMemoryReader(const ixml::XMLChar* buffer, ixml::XMLUInt64 size);

    /**
     * Releases a  MemoryReader that you get before.
     *
     * @param reader - point to the MemoryReader you want to release.
     */
    static void releaseMemoryReader(MemoryReader* reader);
};

INTEL_XML_UTIL_NAMESPACE_END

#endif
