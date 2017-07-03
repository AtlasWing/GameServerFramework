/*

 *---------------------------------------------------------------------------
 *
 *     Copyright 2008 Intel Corporation All Rights Reserved.
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
/**
* @file version.h Version
*/

#ifndef __COM_INTEL_XML_VERSION_H__
#define __COM_INTEL_XML_VERSION_H__

#include "include/common/types.h"

INTEL_XML_NAMESPACE_BEGIN

/**
 * Provide service to get the information about Intel@ XSS product.
 */
class COMMON_EXPORT Version
{
public:
    /**
     * Returns information about the active library version.
     */
    static const XSSVersion* getXSSVersion();

    /**
     * Returns information about the active library version as string.
     */
    static const XMLChar* getXSSVersionString();
};

INTEL_XML_NAMESPACE_END

#endif

