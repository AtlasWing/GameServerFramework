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

/** @file filereader.h Mechanism for reading the source data from file */
#ifndef __COM_INTEL_XML_UTIL_FILEREADER_H__
#define __COM_INTEL_XML_UTIL_FILEREADER_H__

#include "include/util/reader.h"

INTEL_XML_UTIL_NAMESPACE_BEGIN

/**
 * Is a simple implementation 
 * to read local disk files. The class uses 
 * @c fopen and @c fread to open and read data.  
 * 
 * @see Reader
 */

class UTIL_EXPORT FileReader : public Reader
{
public:
    /** 
     * Destructor
     */
    virtual    ~FileReader() {}

public:
    /**
     * Opens a file.
     * 
     * @param filename - the data file name or URL
     * 
     * @return TRUE if the data resource is opened successfully, 
     * otherwise FALSE.
     */
     virtual bool open(const ixml::XMLChar* filename) = 0;

    /**
     * Gets the default empty file reader. 
     *
     * @descr @n You can use @c open to open
     * a specific file.
     *
     * @return  A FileReader pointer that represented the default file reader.
     */
    static FileReader* createFileReader();

    /**
     * Gets a specific file reader through a file name.
     *
     * @param  filename - the data file name or URL
     * @return  A FileReader pointer that represented the default file reader. This
     * Reader has opened the file you specified.
     */
    static FileReader* createFileReader(const ixml::XMLChar* filename);

    /**
     * Releases a FileReader that you get before.
     *
     * @param reader - point to the FileReader you want to release
     */
    static void releaseFileReader(FileReader* reader);
};

INTEL_XML_UTIL_NAMESPACE_END

#endif
