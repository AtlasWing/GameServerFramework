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
 * @file xfunction.h User-defined function base class
 */

#ifndef __COM_INTEL_XML_XFUNCTION_H__
#define __COM_INTEL_XML_XFUNCTION_H__

#include "include/common/xobject.h"
#include "include/util/xvector.h"


INTEL_XML_NAMESPACE_BEGIN


/**
 * Works as the user-defined function base class, used by Intel(R) XPath Accelerator and Intel(R) XSLT Accelerator.
 * @descr @n All user-defined extension functions should inherit from this class. 
 */
class COMMON_EXPORT XFunction {
public:
     virtual ~XFunction(){}
     /**
      * Implements the evaluate function when the user code defines its own extension function.
      * 
      * @descr @n This evaluate function will be called when the
      * corresponding function is met in an XPath expression.
      *
      * @param argumentList - a list of input function arguments
      * 
      * @param result - the object to hold the result of this function evaluation
      *
      * @return TRUE if successfuly evaluating this function, otherwise FALSE.
     */
    virtual bool evaluate(const iutil::XVector<XObject*>& argumentList,XObject* result) = 0;
};

  /**
   * Provides access to the user-defined @c XPathFunction set.
   *
   * @descr @n XPath functions are resolved by name and arity. The resolver is not
   * needed for XPath built-in functions and cannot be used to override those
   * functions. In particular, the resolver is only called for functions in
   * another namespace (functions with an explicit prefix). This means that you
   * cannot use XFunctionResolver to implement specifications
   * like XML-Signature Syntax and Processing, which extend the function library
   * of XPath 1.0 in the same namespace. This is a consequence of the resolver
   * design. To implement additional built-in functions, you need to extend the
   * underlying implementation directly.
   */
  class COMMON_EXPORT XFunctionResolver {
  public:
      virtual ~XFunctionResolver() {}
    /**
     * Finds a function in the set of available functions.
     *
     * @param funcURI - the function name (URI part)
     * @param funcName - the function name (local name part)
     * @param arity        - the number of arguments that the returned function
     *                       must accept
     *
     * @return The function or NULL if no functionName function
     *         with arity arguments exists.
     */
      virtual XFunction* resolveFunction(const XMLChar* funcURI, const XMLChar* funcName,int arity) =0;
  };

INTEL_XML_NAMESPACE_END
#endif


