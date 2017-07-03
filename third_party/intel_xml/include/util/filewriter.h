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

/** @file filewriter.h Mechanism for writing the source data to file*/
#ifndef __COM_INTEL_XML_UTIL_FILEWRITER_H__
#define __COM_INTEL_XML_UTIL_FILEWRITER_H__

#include "include/util/writer.h"

INTEL_XML_UTIL_NAMESPACE_BEGIN

/**
 * Is a simple implementation 
 * to write local disk file. The class uses 
 * @c fopen and @c fwrite to open and write data.  
 *
 * @see Writer
 */
class UTIL_EXPORT FileWriter : public Writer
{
public:
    /** 
     * Destructor.
     */
    virtual    ~FileWriter() {}

public:
    /**
     * Opens the file by its filename or URL.
     * 
     * @param filename - the file name or URL
     * 
     * @return TRUE if the file is opened successfully;
     *         otherwise FALSE.
     */
    virtual bool open(const ixml::XMLChar* filename) = 0;

    /**
     * Gets the default empty file writer. 
     * 
     * @descr @n To open a specific file, use the open() method.
     *
     * @return The pointer that represents the default file writer.
     */
    static FileWriter* createFileWriter();

    /**
     * Gets the file writer by the specified file name.
     *
     * @param filename - the file name or URL
     * @return The pointer that represents the default file writer. This
     * Writer pointer opened the file that you specified.
     */
    static FileWriter* createFileWriter(const ixml::XMLChar* filename);

    /**
     * Releases the FileWriter object.
     *
     * @param writer - the pointer to the FileWriter object that
     *                 you want to release
     */
    static void releaseFileWriter(FileWriter* writer);
};

INTEL_XML_UTIL_NAMESPACE_END

#endif
