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
 *-----------------------------------------------------*/


#ifndef __COM_INTEL_XML_UTIL_RESOURCERESOLVER_H__
#define  __COM_INTEL_XML_UTIL_RESOURCERESOLVER_H__

#include "include/common/defines.h"
#include "include/common/types.h"
INTEL_XML_NAMESPACE_BEGIN
class StreamSource;
INTEL_XML_NAMESPACE_END
INTEL_XML_VALIDATOR_NAMESPACE_BEGIN
class Schema;

/** @file resourceresolver.h  @brief ResourceResolver
 * This file defines the interface of ResourceResolver
 */
 
/**
 * Provides an interface (call-back functions) for applications to redirect references to external resources.
 * 
 * Applications needing to implement custom handling for external resources can implement
 * this interface and register their implementation, for example, setting the @c resource-resolver parameter of
 * SchemaFactory or Validator objects.
 */

class VALIDATION_EXPORT ResourceResolver
{
public:
    /** Default constructor. */
    ResourceResolver() {};

    /** Destructor. */
    virtual ~ResourceResolver() {};
    
public:
    /**
     * Resolves the external resource.
     * @descr @n When encounter \<import\> or \<include\> instruction during schema parsing, this method allows
     * applications to redirect the external resource according to the namespace, resource URI and base URI.
     * @param nameSpace - the namespace of the resource
     * @param resourceURI - the location of the resource
     * @param baseURI - the base URI
     * @return A resolved StreamSource object. If users create
     *                                  the StreamSource, they should be responsible for 
     *                                  deleting this object.
     */
    virtual ixml::StreamSource* resolveResource(const ixml::XMLChar* nameSpace, const ixml::XMLChar* resourceURI, const ixml::XMLChar* baseURI) = 0;
};

INTEL_XML_VALIDATOR_NAMESPACE_END

#endif

