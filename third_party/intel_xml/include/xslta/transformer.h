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

/** @file Transformer.h The Transformer interface
 *
 * Defines the Transformer interface that performs XSL
 * transformations on a source tree and obtains a result tree.
 */

#ifndef _INTEL_XML_TRANSFORM_TRANSFORMER_H_
#define _INTEL_XML_TRANSFORM_TRANSFORMER_H_

#include "include/common/xobject.h"
#include "include/common/source.h"
#include "include/common/result.h"
#include "include/xslta/transformtypes.h"
#include "include/xslta/uriresolver.h"


INTEL_XML_TRANSFORM_NAMESPACE_BEGIN


/**
 * Transforms a source node tree into a result tree.
 *
 * @descr @n To obtain an instance of the XSL transformation class,
 * call Templates#newTransformer(). This instance can process
 * XML data in a variety of input and output formats.
 * 
  *An object of this class may not be used in multiple threads running
 * concurrently.
 * 
  *Different threads may use different transformers concurrently.
 * Parameters and output properties are preserved across transformations 
 * launched by a given Transformer object.
 */

class XSLT_EXPORT Transformer {
public:

/**
 * Transforms XML Source to Result.
 * 
 * @descr @n Specific transformation behavior is determined by
 * the settings of in-effect TransformFactory when
 * Transformer is instantiated and any modifications
 * are made to the Transformer instance.
 * 
  *Source is validated before transformation. Empty Source is not allowed.
 * 
  *The current version of Intel(R) XSLT Accelerator enables four types
 * of XML Source:
 * 
 *  - StreamSource
 *  - SAXSource
 *  - DOMSource
 *  - XMLDocSource
 * 
 * The current version of Intel(R) XSLT Accelerator enables four types of
 * Result:
 * 
 *  - StreamResult
 *  - SAXResult
 *  - DOMResult
 *  - XMLDocResult
 * 
 * The specifics of using certain Source and Result types are listed below:
 * 
 *  -# For StreamSource: if Reader is set, Reader is 
 * used to provide source data, and @c systemId is ignored if any; if
 * Reader is not set, @c systemId is used as the file
 * URL to provide source data.
 *  -# For SAXSource:  The current version of Intel(R) XSLT Accelerator uses
 * @c systemId to obtain source data, either SAXParser is 
 * set or not, as if it were StreamSource. So @c systemId should be set
 * for SAXSource.
 *  -# For DOMSource: If Node is valid, that Node is 
 * used to provide source data; if Node is not set, or NULL, 
 * @c systemId is used as the file URL to provide source data, as if it 
 * were StreamSource. 
 *  -# For XMLDocSource: If XMLDocument is valid, that 
 * XMLDocument is used to provide source data; if XMLDocument 
 * is not set, @c systemId is used as the file URL to provide source data, 
 * as if it were StreamSource.
 *  -# For XMLDocResult: Before tranformation, if XMLDocResult already contains
 * a non-empty XMLDocument, that is, XMLDocResult#getXMLDocument() returns 
 * a non-null value, an exception is thrown. The XMLDocResult should be empty,
 * and the transformation sets the result XMLDocument into
 * XMLDocResult.
 *
 * @param xmlSource     - the XML input to transform
 * @param[out] outputTarget - the transformation result
 *
 * @return TransformerException if an unrecoverable error occurs during the
 *         transformation process.
 */
    virtual void transform(ixml::Source* xmlSource, ixml::Result* outputTarget) = 0;

/**
 * Sets a stylesheet parameter for the transformation.
 * 
 * @descr @n Provides the name, URI and the value of the stylesheet parameter. 
 * The parameter may be referenced in the XSLT stylesheet.
 * 
 * @param uri   - the namespace URI of the stylesheet parameter
 * @param name  - the local name of the stylesheet parameter
 * @param value - the value object of the stylesheet parameter
 *
 * @return TransformerConfigurationException if the value is invalid.
 */
    virtual void setParameter(const ixml::XMLChar* uri, const ixml::XMLChar* name, ParamData& value) = 0;

/**
 * Gets the stylesheet parameter that was explicitly set with setParameter().
 * 
 * @descr @n This method does not return the default parameter value specified in the
 * stylesheet, because the value cannot be determined until the node context is
 * evaluated during the transformation process.
 *
 * @param uri  - the namespace URI of the stylesheet parameter
 * @param name - the local name of the stylesheet parameter
 *
 * @return The stylesheet parameter set with setParameter(), or
 *         TransformerConfigurationException if none of existing parameters
 *         set with setParameter() matches the specified @e name and @e uri.
 */
    virtual ParamData getParameter(const ixml::XMLChar* uri, const ixml::XMLChar* name) = 0;

/** Clears all stylesheet parameter values set with setParameter().*/

    virtual void clearParameters() = 0;

/**
 * Sets an object for resolving URIs used by @c xsl:include,
 * @c xsl:import, or the  XPath @c document()
 * function. 
 *
 * @descr @n If this method is not called before transformation, the system 
 * uses the resolver set by TransformFactory#setURIResolver().
 * If the latter method is not called either or if you set URIResolver
 * to NULL explicitly, the system uses the built-in resolver.
 * 
  *If this method is called several times, the latest value overrides 
 * the previous ones.   
 *
 * @param resolver -  an object implementing the URIResolver interface
 */
    virtual void setURIResolver(URIResolver* resolver) = 0;

/**
 * Gets an object for resolving URIs used by @c xsl:include,
 * @c xsl:import, or the  XPath @c  document()
 * function.
 *
 * @descr @n The method returns the URIResolver value taken from one of the following,
 * and searches for the value following the specified order:
 * 
 *  -# URIResolver set by Transformer#setURIResolver(URIResolver* resolver)
 *  -# URIResolver set in the TransformFactory object, from
 * which this Transformer object is produced
 *  -# NULL if the URIResolver is not set or if you
 * deliberately set it to NULL
 *  
 * If this method returns NULL, the system uses the built-in
 * URIResolver to perform URI resolution tasks. This method cannot return
 * this built-in URIResolver. 
 *
 * @return An object implementing the URIResolver interface, or
 *         NULL.
 */
    virtual URIResolver* getURIResolver() = 0;

/**
 * Sets the output properties used for the transformation.
 *
 * @descr @n The provided properties @e oformat will override any properties 
 * defined in the stylesheet with @c xsl:output. If @e oformat
 * is NULL, all properties previously set by setOutputProperties()
 * or setOutputProperty() are removed, and the value is revert to the value defined 
 * in the Templates object.
 * 
 * @param oformat - a set of output properties
 *
 * @return TransformerConfigurationException if any property value is not
 *         recognized.
 */
    virtual void setOutputProperties(const XSLOutputProperties* oformat) = 0;

/**
 * Gets a copy of the output properties for the transformation.
 *
 * @descr @n The method can return properties set by the user,
 * the stylesheet and missing properties defaulted according to the 
 * <a href='http://www.w3.org/TR/xslt#output'>XSLT Recommendation, section 16</a>.
 *
 * @param[out] oformat - a copy of the output properties for the transformation
 */
    virtual void getOutputProperties(XSLOutputProperties& oformat) = 0;

/**
 * Sets the output property used for the transformation.
 *
 * @descr @n The property overrides any property of the same name defined
 * in the stylesheet.
 * The current version of Intel(R) XSLT Accelerator recogizes below names specified as 
 * @e name:
 * 
 *  - "method"
 *  - "version"
 *  - "encoding"
 *  - "omit-xml-declaration"
 *  - "standalone"
 *  - "doctype-public"
 *  - "doctype-system"
 *  - "cdata-section-elements"
 *  - "indent"
 *  - "media-type"
 * 
 *
 * @param name  - an @c XMLChar string that specifies the
 *                    property name
 * @param value - the XSLOutputProperty enumeration value
 *
 * @return TransformerConfigurationException if the property name or value can
 *         not be recognized.
 */
    virtual void setOutputProperty(const ixml::XMLChar* name, XSLOutputProperty value) = 0;

/**
 * Gets an output property used for the transformer.
 *
 * @descr @n The method searches for the property values in the following locations,
 * in the specified order, and returns the first value encountered in the list:
 * 
 *  -# user-defined properties set by calling setOutputProperty() or setOutputProperties()
 *  -# properties explicitly defined in the stylesheet
 *  -# default property values as defined by <a href='http://www.w3.org/TR/xslt#output'>XSLT 
 * Recommendation, section 16</a>.
 * 
 *
 * @param name - the property name specified as XMLChar string
 *
 * @return TransformerConfigurationException if the property name is not
 *         recognized.
 */
    virtual XSLOutputProperty getOutputProperty(const ixml::XMLChar* name) = 0;

/**
 * Resets Transformer to its original configuration.
 * 
 * @descr @n Transformer is reset to the state, in which it was
 * when created by Templates#newTransformer().
 * 
  *The method reset() is designed to enable re-use of existing
 * Transformer objects thus saving resources associated with the
 * creation of new Transformer objects.
 *
 * @return UnsupportedOperationException if the underlying 
 * Transformer implementation does not support this operation.
 */
    virtual void reset() = 0;

/** Destructor.*/
    virtual ~Transformer() { }

protected:

/** Default constructor protected on purpose.*/
    Transformer() { }

};

INTEL_XML_TRANSFORM_NAMESPACE_END

#endif //_INTEL_XML_TRANSFORM_TRANSFORMER_H_
