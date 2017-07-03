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

/** @file transformfactory.h The TransformFactory interface
 *
 * Defines the TransformFactory interface which is the origination of all
 * XSL transformations.
 */

#ifndef __COM_INTEL_XML_TRANSFORM_TRANSFORMFACTORY_H__
#define __COM_INTEL_XML_TRANSFORM_TRANSFORMFACTORY_H__

#include "include/xslta/transformtypes.h"
#include "include/xslta/transformexception.h"
#include "include/common/source.h"
#include "include/xslta/uriresolver.h"
#include "include/xslta/templates.h"

INTEL_XML_TRANSFORM_NAMESPACE_BEGIN

/**
 * Is a factory mainly used to produce Templates.
 *
 * @descr @n TransformFactory is the object where all XSL 
 * transformations originate from. To perform an XSL transformation, 
 * do the following:
 * 
 *  -# Create a Templates object from TransformFactory.
 *  -# Create a Transformer object from the created Templates.
 *  -# Call Transformer#transform().
 * 
 */
class XSLT_EXPORT TransformFactory {
public:

/**
 * Obtains a TransformFactory instance.
 *
 * @descr @n This static method creates a TransformFactory instance. It returns a
 * reference to the underlying factory implementation, which can be used to configure
 * and obtain Templates instances.
 *
 * @return A new TransformFactory instance, never
 *         NULL.
 * @return TransformerConfigurationException if no underlying implementation
 *         is available, or error occurs when instantiating the factory.
 */
    static TransformFactory* newInstance() ;

/**
 * Deletes a TransformFactory instance.
 *
 * @descr @n Use this interface instead of calling native C++ "delete" to delete a 
 * TransformFactory instance. 
 *
 * @param factory - a TransformFactory instance created by
 *                  newInstance()
 *
 * @return TransformerConfigurationException if an error occurs when releasing
 *         the resources in the factory.
 */
    static void releaseInstance(TransformFactory* factory);

/**
 * Processes Source into a Templates object, which is a
 * compiled representation of the XSL stylesheet.
 *
 * @descr @n Creating a Templates object enables detailed performance optimization
 * to be done on transformation instructions, without penalizing run-time
 * transformation.
 * 
  *Source is an XSLT stylesheet that conforms to
 * <a href="http://www.w3.org/TR/xslt">XSL Transformations (XSLT) Version
 * 1.0 </a>.
 * The current version of Intel(R) XSLT Accelerator enables three types of
 * Source provided as stylesheet:
 * 
 *  - StreamSource
 *  - SAXSource
 *  - DOMSource
 * 
 * 
  *The specifics of using certain types of Source are listed
 * below:
 * 
 *  -# For StreamSource: if Reader is set, Reader is 
 * used to provide source data, and @c systemId is ignored if any; if
 * Reader is not set, @c systemId is used as the file
 * URL to provide source data. The base URI is calculated according to @c systemId.
 *  -# For SAXSource:  The current version of Intel(R) XSLT Accelerator uses
 * @c systemId to obtain source data, either SAXParser is 
 * set or not, as if it were StreamSource. So @c systemId should be set
 * for SAXSource. The base URI is calculated according to @c systemId.
 *  -# For DOMSource: If Node is valid, that Node is 
 * used to provide source data, and the base URI is calculated according to the information 
 * contained in Node; if Node is not valid, or NULL, 
 * @c systemId is used as the file URL to provide source data, as if it 
 * were StreamSource; the base URI is calculated according to @c systemId. 
 * 
 *
 * 
 * The created Templates object may be used concurrently across
 * multiple threads. However, the user must take care when using
 * Templates object in multiple threads. See document of
 * Templates class for limitations.
 *
 * @param source - a Source object holding an input stream, a
 *                     SAX event or DOM
 *
 * @return A Templates object that can be used for transformation purposes,
 *         never NULL.
 * @return TransformerConfigurationException if an error occurs during the
 *         stylesheet compilation or the Templates object construction.
 */
    virtual Templates* newTemplates(ixml::Source* source) = 0;

/**
 * Creates a Templates object for identity transformation.
 * 
 * @descr @n The created Templates object may be used concurrently across
 * multiple threads. This Templates object enables creating transformers that
 * can do identity transformation, namely, the input is just "copied" to the
 * output, the only change is the data format. 
 * @note The output encoding of identity transformation is fixed to UTF-8.
 *
 * @return A Templates object enabling identity transformers creation, never
 *         NULL.
 *
 * @return TransformerConfigurationException if an error occurs during the
 *         Templates object construction.
 */
    virtual Templates* newTemplates() = 0;
    
/**
 * Deletes a Templates instance created by
 * newTemplates() or newTemplates(ixml::Source* source).
 *
 * @descr @n Use this interface instead of calling the native C++ "delete" to 
 * delete a Templates instance.
 *
 * @param templates - a Templates instance created by
 *                    newTemplates() or newTemplates(ixml::Source* source)
 *
 * @return TransformerConfigurationException if an error occurs while releasing
 *         the resources in the Templates object.
 */
    virtual void releaseTemplates(Templates* templates) = 0;

/**
 * Sets an object used during the transformation to resolve the URIs in
 *        @c document(), @c xsl:import, or
 *        @c xsl:include.
 *
 * @descr @n Once set, the resolver is visible by all Templates objects created by
 * this TransformFactory and all Transformer object created by
 * these Templates. It will be used during transformation if it's not overriden 
 * by resolver set with Transformer#setURIResolver().  
 *
 * @param resolver - the resolver object
 */
    virtual void setURIResolver(URIResolver* resolver) = 0;

/**
 * Gets the resolver object used by the transformation to resolve the URIs 
 * in @c document(), @c xsl:import, or
 * @c xsl:include.
 *
 * @descr @n This method returns the URIResolver set by method Transformer#setURIResolver()
 * or NULL if no resolver has been set. This method cannot get the
 * built-in URIResolver that Intel(R) XSLT Accelerator uses for URI resolution when 
 * no other resolver is set.
 *
 * @return The resolver object reference.
 */
    virtual URIResolver* getURIResolver() = 0;

/**
 * Sets an attribute on the underlying factory implementation.
 *
 * @descr @n Enables the user to set specific attributes on the underlying
 * factory implementation. An attribute in this context is defined to be an option
 * that the implementation provides. The current version of Intel(R) XSLT
 * Accelerator does not provide any attributes that can be set by the user.
 * This interface is preserved to be extended in future versions. 
 * 
 * @param name  - the attribute name
 * @param value - the value to assign
 *
 * @return TransformerConfigurationException if the specified attribute cannot
 *         be recognized or the attribute is read-only.
 */
    virtual void setAttribute(const ixml::XMLChar* name, const ixml::XMLChar* value) = 0;

/**
 * Retrieves the specific attribute value of the underlying factory
 * implementation.
 * 
 * @descr @n The current version of Intel(R) XSLT Accelerator supports a list
 * of certain implementation specific attributes to be retrieved through this
 * interface. These attributes are strings defined as macros. Currently, the
 * list includes just one item: @c ATTRIBUTE_XSLT_VERSION.
 * 
  *Find the attributes definitions in constants.h.
 * This list is to be extended in future versions.
 * 
 * @param name - the attribute name
 *
 * @return The attribute value obtained from the underlying factory implementation.
 * @return TransformerConfigurationException if the underlying implementation
 *         does not recognize the attribute.
 */
    virtual const ixml::XMLChar* getAttribute(const ixml::XMLChar* name) = 0;

protected:

/** Default constructor protected on purpose.*/
    TransformFactory() { }
/** Default destructor protected on purpose.*/
    virtual ~TransformFactory() { }
};

INTEL_XML_TRANSFORM_NAMESPACE_END

#endif //__COM_INTEL_XML_TRANSFORM_TRANSFORMFACTORY_H__
