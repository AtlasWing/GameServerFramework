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

/** @file stdoutwriter.h Interface for writing to @c stdout. */


#ifndef __COM_INTEL_XML_UTIL_STDOUTWRITER_H__
#define __COM_INTEL_XML_UTIL_STDOUTWRITER_H__

#include "include/util/writer.h"

INTEL_XML_UTIL_NAMESPACE_BEGIN

/**
 * Enables writing to @c stdout.
 */
class UTIL_EXPORT StdOutWriter : public Writer
{
public:
    /** 
     * Destructor
     */
    virtual    ~StdOutWriter() {}

public:
    /**
     * Gets the specific @c stdout writer.
     *
     * @return The pointer that represents the default @c stdout writer. 
     */
    static StdOutWriter* createStdOutWriter();
    
    /**
     * Releases the Writer object.
     *
     * @param writer - the pointer to the StdOutWriter object that
     *                 you want to release
     */
    static void releaseStdOutWriter(StdOutWriter* writer);
};

INTEL_XML_UTIL_NAMESPACE_END

#endif
