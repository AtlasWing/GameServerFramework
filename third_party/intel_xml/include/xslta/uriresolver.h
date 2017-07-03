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

/**
 * @file URIResolver.h The URIResolver interface
 *
 * Defines the URIResolver interface resolving URIs in @c document(),
 * @c xsl:import, or @c xsl:include.
 */


#ifndef _INTEL_XML_TRANSFORM_URIRESOLVER_H_
#define _INTEL_XML_TRANSFORM_URIRESOLVER_H_

#include "include/common/defines.h"
#include "include/common/source.h"
#include "include/xslta/transformexception.h"

INTEL_XML_TRANSFORM_NAMESPACE_BEGIN

/**
 * Used to resolve URIs.
 *
 * @descr @n The user implements the interface and registers the URIResolver
 * implementation through TransformFactory#setURIResolver(),
 * TransformFactory#getURIResolver(), Transformer#setURIResolver(), and
 * Transformer#getURIResolver(). Intel(R) XSLT Accelerator calls the
 * registered implementation to turn a URI used in @c document(),
 * @c xsl:import, or @c xsl:include into a Source object.
 */
class XSLT_EXPORT URIResolver {
public:

/** Destructor.*/

    virtual ~URIResolver() {}

/**
 * The implementation of this interface will be called by Intel(R)
 * XSLT Accelerator when it encounters an @c xsl:include,
 * @c xsl:import, or XPath @c document() method.
 *
 * @descr @n Intel(R) XSLT Accelerator has a built-in URI resolver. If
 * user-registered URIResolver implementation cannot resolve @e href, the
 * Intel(R) XSLT Accelerator tries to resolve the URI with its built-in
 * URI resolver.
 *
 * @param href - the URI to resolve (relative or absolute)
 * @param base - the base URI against which the first argument is made
 *                   absolute if the absolute URI is required
 *
 * @return A Source object, or NULL if @e href cannot be
 *         resolved.
 *
 * @return TransformerException if an error occurs while trying to resolve
 *         the URI.
 */
    virtual ixml::StreamSource* resolve(const ixml::XMLChar* href, const ixml::XMLChar* base) = 0;
};

INTEL_XML_TRANSFORM_NAMESPACE_END

#endif //_INTEL_XML_TRANSFORM_URIRESOLVER_H_
