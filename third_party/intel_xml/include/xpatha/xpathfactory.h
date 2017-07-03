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

/** @file xpathfactory.h Interface for creating XPath objects */

#ifndef __COM_INTEL_XML_XPATH_XPATHFACTORY_H__
#define __COM_INTEL_XML_XPATH_XPATHFACTORY_H__


#include "include/common/xobject.h"
#include "include/common/xfunction.h"
#include "include/common/source.h"
#include "include/util/xmlstring.h"



INTEL_XML_XPATH_NAMESPACE_BEGIN

class XPath;
class XPathVariableResolver;

  /**
   * Creates XPath objects.
   *
   * @descr @n Use newInstance() to create an instance of an XPath object.
   * The XPathFactory class is not thread-safe. In other words, it is the 
   * responsibility of the application to ensure that one thread at most is 
   * using an XPathFactory object at any given moment. 
   */
class COMMON_EXPORT XPathFactory {
public:
    virtual ~XPathFactory() {}
    
    /**
     * Gets the state of the named feature.
     *
     * @descr @n Feature names are fully qualified URIs. 
     *  It is possible 
     * for an XPathFactory object to expose a feature value but be unable to change its state. 
     * 
  * Currently, the supported features are: 
     *  -  @c FEATURE_XPATH_FORWARD_ONLY for restrict xpath axis type.
     *  -  @c FEATURE_XPATH_SECURE_PROCESSING for not calling any outside function. 
     *  -  @c FEATURE_XPATH_MATCH_FIRST is reserved for future use. 
     * 
     * @param name - feature name
     * @return State of the feature or XPathFactoryConfigurationException if this XPathFactory 
     * or the XPath objects it creates cannot support the feature.
     */
    virtual bool getFeature(const ixml::XMLChar* name) const = 0;

    /**
     * Checks whether the specified object model is supported by this XPathFactory.
     *
     * @descr @n Currently, the supported object models are: 
     *   - @c XPATH_DEFAULT_OBJECT_MODEL  DOM model, using @c idom::Node as implementation. 
     *   - @c XPATH_INTEL_XML_DOCUMENT_MODEL Intel XMLDocument model, using @c idoc::XMLDocNode as implementation. 
     * 
     * @param objectModel - the object model which the returned XPathFactory can parse
     * @return TRUE if XPathFactory supports objectModel, else FALSE.
     */
    virtual bool isObjectModelSupported(const ixml::XMLChar* objectModel) const = 0;

    /**
     * Sets a feature for this XPathFactory and XPath objects created by this factory.
     *
     * @descr @n Feature names are fully qualified URIs. 
     * An XPathFactoryConfigurationException is thrown if this XPathFactory or 
     * the XPath object it creates cannot support the feature. It is possible for an 
     * XPathFactory object to expose a feature value but be unable to change its state.
     * 
  * Currently, the only feature that allow to set is @c FEATURE_XPATH_SECURE_PROCESSING.
     *
     * @param name - feature name
     * @param value - feature state: TRUE or FALSE
     * @return XPathFactoryConfigurationException if this XPathFactory or the XPath objects it creates 
     *         cannot support this feature.
     */
    virtual void setFeature(const ixml::XMLChar* name, bool value) = 0;

    /**
     * Sets the default function resolver.
 
     * @descr @n Any XPath objects constructed from this factory will use the specified resolver by default.
     *
     * @param resolver - XPath function resolver
     */
    virtual void setFunctionResolver(XFunctionResolver* resolver) = 0;

    /**
     * Establishes the default variable resolver.
     *
     *@descr @n  Any XPath objects constructed from this factory will use the specified resolver by default.

     *@param  resolver - variable resolver
     */
    virtual void setVariableResolver(XPathVariableResolver* resolver) = 0;

    /**
     * Gets a new XPathFactory instance using the default W3C DOM data model.
     *
     * @descr @n This method is equivalent to @c newInstance(XPATH_DEFAULT_OBJECT_MODEL).
     * Since the implementation for the W3C DOM is always available, this method will never fail.
     *
     * @return An instance of an XPathFactory object.
     */
    static XPathFactory* newInstance();

    /** Gets a new XPathFactory instance using the specified object model.
     *
     * @descr @n To find a XPathFactory object, this method first check the specified object model is supported or not.
     * If supported, an XPathFactory object with this specified object model is created and returned.
     * 
     * @param uri - the underlying object model: @c XPATH_DEFAULT_OBJECT_MODEL or @c XPATH_INTEL_XML_DOCUMENT_MODEL
     * @return An instance of XPathFactory if specified object model is supported; otherwise, an XPathFactoryConfigurationException.  
     * */
    static XPathFactory* newInstance(ixml::XMLChar* uri);

    /**
     * Releases the XPathFactory instance. 
     * @descr @n After this operation, the instance can no longer be used.

     *@param obj - the XPathFactory object to be released
     */
    static void releaseInstance(XPathFactory* obj);

    /**
     * Returns a new XPath object using the underlying
     * object model determined when the factory was instantiated.
     * 
     * @return A new instance of an XPath object.
     */
    virtual XPath* newXPath() = 0;

    /**
     * Releases the XPath instance created by this XPathFactory. 
     * @descr @n After this operation, the XPath instance can no longer be used.
     *
     * @param obj - the XPath object to be released
     */
    virtual void releaseXPath(XPath* obj) = 0;
  };

INTEL_XML_XPATH_NAMESPACE_END

#endif
