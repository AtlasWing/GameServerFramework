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

/** @file Templates.h The Templates interface
 *
 * Defines the Templates interface.
 */

#ifndef _INTEL_XML_TRANSFORM_TEMPLATES_H_
#define _INTEL_XML_TRANSFORM_TEMPLATES_H_

#include "include/common/xfunction.h"
#include "include/xslta/transformtypes.h"
#include "include/xslta/transformer.h"
#include "include/xslta/transformexception.h"


INTEL_XML_TRANSFORM_NAMESPACE_BEGIN


/**
 * Is a compiled, re-usable XSL stylesheet.
 *
 * @descr @n A Templates object is used to generate Transformer objects.
 * Usually a Templates object can be shared by multiple threads.
 * However, you may get undexpected result, if you use 
 * registerExternalFunctionResolver() method 
 * on a given Templates object in multiple threads running 
 * concurrently.
 */

class XSLT_EXPORT Templates {
public:
/**
 * Creates a Transformer object.
 *
 * @descr @n Creates a new transformation context from the Templates
 * object.
 *
 * @return A valid non-null Transformer instance or
 *         TransformerConfigurationException in case of an error.
 */
    virtual Transformer* newTransformer() = 0;

/**
 * Deletes a Transformer instance created by Templates.
 *
 * @descr @n Use this interface instead of calling C++ native "delete" 
 * to delete a Transformer instance.
 *
 * @param transformer - a Transformer instance created by
 *                          Templates
 *
 * @return TransformerConfigurationException in case of an error.
 */
    virtual void releaseTransformer(Transformer* transformer) = 0;

/**
 * Gets the properties corresponding to the effective
 * @c xsl:output element.
 *
 * @descr @n The internal values are copied into the parameter
 * @e oformat. You can mutate this parameter without mutating the
 * Templates object, and then hand it in to
 * Transformer#setOutputProperties() properties.
 * 
  * Note that @e dpublic, @e dsystem, @e cdata and
 * @e media_type are XMLChar pointers in
 * XSLOutputProperties. So, you should mutate these members by
 * mutating the pointer itself, but not the characters the pointer points to.
 * Otherwise, you may mutate the Templates object by mistake.
 * 
  * The @e oformat parameter should contain properties
 * set by the stylesheet, and these properties are "defaulted" according to 
 * <a href="http://www.w3.org/TR/xslt#output">XSLT Recommendation,
 * section 16</a>.
 *
 * @param[out] oformat - the properties object reference; the internal value
 *                       is set into this object
 */
    virtual void getOutputProperties(XSLOutputProperties& oformat) = 0;

/**
 * Registers the user-defined function resolver.
 * 
 * @descr @n This interface registers the user-defined function resolver.
 * The registered resolver is visible in all Transformer objects
 * created from this Templates object.
 * 
  *Only one or no function resolver is effective at a time. If
 * this method is called several times, the
 * last one registered overrides the previous ones. The overriden resolver 
 * is returned if any.
 * 
  *The system does not provide default function resolver implementation.
 * So, the function resolver exists only if the user registers one.
 * @note Do not to use this method for a given
 * Templates object in multiple threads running concurrently.
 *
 * @param fnR - the user-defined function resolver to be registered
 *
 * @return The overriden XFunctionResolver object, or NULL
 *         if this function
 *         is called for the first time.
 */
    virtual ixml::XFunctionResolver* registerExternalFunctionResolver(ixml::XFunctionResolver* fnR)=0;

/**
 * Destructor.
 *
 * @descr @n Use TransformFactory::releaseTemplates(Templates* templates) 
 * instead of calling the native C++ "delete" to delete a Templates 
 * instance.
 *
 * @return TransformerConfigurationException in case of an error.
 */
    virtual ~Templates() { };
};



INTEL_XML_TRANSFORM_NAMESPACE_END

#endif //_INTEL_XML_TRANSFORM_TEMPLATES_H_
