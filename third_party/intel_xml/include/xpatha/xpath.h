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

/** @file xpath.h The main interface file for Intel(R) XPath Accelerator
 * 
 */

#ifndef __COM_INTEL_XML_XPATH_XPATH_H__
#define __COM_INTEL_XML_XPATH_XPATH_H__


#include "include/common/xobject.h"
#include "include/common/xfunction.h"
#include "include/common/source.h"
#include "include/util/xmlstring.h"



INTEL_XML_XPATH_NAMESPACE_BEGIN

  /**
   * Provides access to the set of user-defined XPath variables.
   *
   * XPathVariableResolver and the XPath evaluator must 
   * adhere to a contract that cannot be directly enforced by API. Although,
   * variables may be mutable. That is, an application can evaluate the same
   * XPath expression more than once with different variable values. In the
   * course of evaluating any single XPath expression, 
   * a variable value MUST be immutable.
   */
class COMMON_EXPORT XPathVariableResolver {
public:
    /** Destructor. */
    virtual ~XPathVariableResolver() {}
   
    /**
     * Finds a variable in the set of available variables.
     * @descr @n  The value returned must be of a type appropriate for the underlying object model.
     *
     * @param varUri - the URI of the variable name
     * @param varName - The local name of the variable name
     * @param result - the XObject object to hold the result
     *
     *
     * @return XPathExpressionException if the variable cannot be found.
     */
      virtual void resolveVariable(const ixml::XMLChar* varUri, const ixml::XMLChar* varName, XObject* result) =0;
  };

  /**
   * Is responsible for read-only XML namespace context processing.
   *
   * An XML namespace has the property Namespace URI, where the Namespace name expressed as a URI, to which the prefix is bound.
   *                  A prefix can only be bound to a single namespace URI in the current scope.
   *
   * The getNamespaceURI() methods operate in the current scope for namespace URI and prefix resolution.
   */
class COMMON_EXPORT NamespaceContext {
public:

    /** Destructor. */
    virtual ~NamespaceContext() {}

    /**
     * Gets namespace URI bound to a prefix in the current scope.
     *
     * @param prefix - prefix to look up 
     * @return %Namespace URI bound to prefix in the current scope. 
     * @return If not found, XPathExpressionException.
     */
    virtual const ixml::XMLChar* getNamespaceURI(const ixml::XMLChar* prefix)  = 0;
};
                
/**
 * Provides access to compiled XPath expressions.
 * 
 * @descr @n 
 *
 * Evaluation of XPath expressions:
 *  
 * - @b context: If no context item is present, NULL is used as the context.
 * - @b variables: Values of variable references must be immutable
 *   through the course of any single evaluation; variables are found through
 *   XPathVariableResolver and if it is not identified, XPathExpressionException
 *   is raised.
 * - @b functions: functions are found through XFunctionResolver
 *   and if it is not defined, XPathExpressionException is raised.
 * - @b QNames: QNames in the expression are resolved against the XPath namespace context.
 * - @b result: The result of evaluating an expression using evaluate() method is an XObject; 
 *   the result of evaluating an expression using evaluateAsString() method is an XMLChar* string.
 * 
 *  An XPath object and all its created XPathExpression objects as a whole are not thread-safe and not re-entrant.
 *  In other words, it is the responsibility of the application to make
 *  sure that none of these objects are used from
 *  more than one thread at any given time, and while the evaluate()
 *  method is invoked, applications may not recursively call the evaluate() method.
 */
class COMMON_EXPORT XPathExpression {
public:
    virtual ~XPathExpression() {}

    /**
     * Evaluates the compiled XPath expression in the context of the specified ixml::Source.
     * @descr @n Depending on the document model used to create XPathFactory, this object belongs to a
     * specific type, as follows.
     *    
     *    - @c XPATH_DEFAULT_OBJECT_MODEL: input source SAXSource, StreamSource and DOMSource 
     *    are allowed. SAXSource and StreamSource are converted to DOMSource first.
     *    - @c XPATH_INTEL_XML_DOCUMENT_MODEL: only input source as XMLDocSource is allowed.
     *    
     * 
     * @param inputSource - the @c ixml::Source of the document to evaluate over
     * @return The XObject that is the result of evaluating the expression. User code needs not release this object. 
     *    In case of an error, the exception is thrown: 
     * @return    - IOException when access the soruce is failed.
     * @return    - DOMParseException when the source cannot be parsed as DOM.
     * @return    - ValidationException when the source cannot pass the validation of Schema or DTD.
     * @return    - XPathExpressionException when any error in xpath evaluation is occurred.
     *    
     */
    virtual XObject* evaluate(ixml::Source* inputSource) = 0;

    /**
     * Evaluates the compiled XPath expression in the specified context.
     *
     * @param contextNode - the starting context
     * @return The XObject that is the result of evaluating the expression. User code needs not release this object. 
     *    XPathExpressionException is raised when an error occurs
     */
    virtual XObject* evaluate(idom::Node* contextNode) = 0;

    /**
     * Evaluates the compiled XPath expression in the context of the specified @c ixml::Source 
     * and returns the result as XMLString.
     * @descr @n Depending on the document model used to create XPathFactory, this object belongs to a
     * specific type, as follows.
     *    
     *    - @c XPATH_DEFAULT_OBJECT_MODEL: input source SAXSource, StreamSource and DOMSource 
     *    are allowed. SAXSource and StreamSource are converted to DOMSource first.
     *    - @c XPATH_INTEL_XML_DOCUMENT_MODEL: only input source as XMLDocSource is allowed.
     *    
     * 
     * @param inputSource - the @c ixml::Source of the document to evaluate over
     * @return The string that is the result of evaluating the expression. User code needs not free this string.
     *    In case of an error, the exception is thrown:
     * @return    - IOException when access the soruce is failed.
     * @return    - DOMParseException when the source cannot be parsed as DOM.
     * @return    - ValidationException when the source cannot pass the validation of Schema or DTD.
     * @return    - XPathExpressionException when any error in xpath evaluation is occurred.
     *    
     */
    virtual ixml::XMLChar* evaluateAsString(ixml::Source*  inputSource) = 0;

    /**
     * Evaluates the compiled XPath expression in the specified context and returns the result as the XMLString.
     *
     * @param contextNode - the starting context
     * @return The string that is the result of evaluating the expression. User code needs not free this string.
     *    XPathExpressionException is raised when any error is occurred.
     */
    virtual ixml::XMLChar* evaluateAsString(idom::Node* contextNode) = 0;
};


/**
 *  Provides access to the XPath evaluation environment and expressions.
 *  
 *  @descr @n 
 *
 * Evaluation of XPath Expressions. 
 *  
 * - @b context: If no context item is present, NULL is used as the context.
 * - @b variables: Values of variable references must be immutable
 *   through the course of any single evaluation; variables are found through
 *   XPathVariableResolver and if it is not identified, XPathExpressionException
 *   is raised.
 * - @b functions: functions are found through XFunctionResolver
 *   and if it is not defined, XPathExpressionException is raised.
 * - @b QNames: QNames in the expression are resolved against the XPath namespace context.
 * - @b result: The result of evaluating an expression using evaluate() method is an XObject; 
 *   the result of evaluating an expression using evaluateAsString() method is an XMLChar* string.
 *
 *  An XPath object and all its created XPathExpression objects as a whole are not thread-safe and not reentrant.
 *  In other words, it is the application's responsibility to make
 *  sure that no any of these objects are used from
 *  more than one thread at any given time, and while the evaluate()
 *  method is invoked, applications may not recursively call the evaluate() method.
 */
class COMMON_EXPORT XPath {
public:
    virtual ~XPath() {}
    /**
     * Evaluates the XPath expression in the context of the specified @c ixml::Source.
     *
     * @descr @n Depending on the document model used to create XPathFactory, this object belongs to a
     * specific type, as follows.
     *    
     *    - @c XPATH_DEFAULT_OBJECT_MODEL: input source SAXSource, StreamSource and DOMSource 
     *    are allowed. SAXSource and StreamSource are converted to DOMSource first.
     *    - @c XPATH_INTEL_XML_DOCUMENT_MODEL: only input source as XMLDocSource is allowed.
     *    
     * @param expression  - the XPath expression
     * @param inputSource - the @c ixml::Source of the document to evaluate over
     * @return The XObject that is the result of evaluating the expression. User code needs not release this object. 
     *    In case of an error, the exception is thrown:          
     * @return     - IOException when access the soruce is failed.
     * @return     - DOMParseException when the source cannot be parsed as DOM.
     * @return     - ValidationException when the source cannot pass the validation of Schema or DTD.
     * @return     - XPathExpressionException when any error in xpath evaluation is occurred.
     *    
     */
    virtual XObject* evaluate(const iutil::XMLString& expression,ixml::Source* inputSource) = 0;

    /**
     * Evaluates the XPath expression in the specified context.
     *
     * @param expression  - the XPath expression
     * @param contextNode - the starting context
     * @return The XObject that is the result of evaluating the expression. User code needs not release this object. 
     *    XPathExpressionException is raised when any error is occurred.
     */
    virtual XObject* evaluate(const ixml::XMLChar* expression, idom::Node* contextNode) = 0;

    /**
     * Evaluates the XPath expression in the context of the specified @c ixml::Source 
     * and return the result as XMLString.
     * @descr @n Depending on the document model used to create XPathFactory, this object belongs to a
     * specific type, as follows.
     *    
     *    - @c XPATH_DEFAULT_OBJECT_MODEL: input source SAXSource, StreamSource and DOMSource 
     *    are allowed. SAXSource and StreamSource are converted to DOMSource first.
     *    - @c XPATH_INTEL_XML_DOCUMENT_MODEL: only input source as XMLDocSource is allowed.
     *    
     * @param expression - the XPath expression
     * @param inputSource - the @c ixml::Source of the document to evaluate over
     * @return The string that is the result of evaluating the expression. User code needs not free this string.
     *    In case of an error, the exception is thrown: 
     * @return    - IOException when access the soruce is failed.
     * @return    - DOMParseException when the source cannot be parsed as DOM.
     * @return    - ValidationException when the source cannot pass the validation of Schema or DTD.
     * @return    - XPathExpressionException when any error in xpath evaluation is occurred.
     *    
     */
    virtual ixml::XMLChar* evaluateAsString(const ixml::XMLChar* expression, ixml::Source* inputSource) = 0; 

    /**
     * Evaluates the XPath expression in the specified context and return the result as the XMLString.
     *
     * @param expression - the XPath expression
     * @param contextNode - the starting context
     * @return The string that is the result of evaluating the expression. User code needs not free this string.
     *    XPathExpressionException is raised when any error is occurred.
     */
    virtual ixml::XMLChar* evaluateAsString(const ixml::XMLChar* expression, idom::Node*  contextNode) = 0;

    /**
     * Compiles an XPath expression for later evaluation.
     *
     * @descr @n If expression contains any XFunction, they must be available via the XFunctionResolver. 
     *
     * @param expression - the XPath expression
     * @return Compiled XPath expression. 
     *         XPathExpressionException is raised when an error occurred.
     */
    virtual XPathExpression* compile(const ixml::XMLChar* expression) = 0;

    /**
     * Releases an XPathExpression object created by this XPath object.
     *
     * @descr @n Releases an XPathExpression object. After this operation, 
     * user can no longer use this XPathExpression object.
     * But, the XObject returned by this XPathExpression object is still can be used.
     *
     * @param obj - the XPathExpression object
     */    
    virtual void releaseXPathExpression(XPathExpression* obj) = 0;

    /**
     * Returns the current namespace context.
     *
     * @return The current namespace context or NULL if no namespace context is in effect.
     */
    virtual const NamespaceContext* getNamespaceContext() const = 0;

    /**
     * Returns the current function resolver. 
     *
     * @return The current function resolver or NULL if no function resolver is in effect.
     */
    virtual const XFunctionResolver* getFunctionResolver() const = 0;

    /**
     * Returns the current variable resolver. 
     *
     * @return The current variable resolver or NULL if no variable resolver is in effect.
     */
    virtual const XPathVariableResolver* getVariableResolver() const = 0;

    /**
     * Establishes a namespace context.
     *
     * @param nsContext - namespace context to use
     */
    virtual void setNamespaceContext(NamespaceContext* nsContext) = 0;

    /**
     * Establishes a function resolver.
     *
     * @param resolver - XPath function resolver
     */
    virtual void setFunctionResolver(XFunctionResolver* resolver)  = 0;

    /**
     * Establishes a variable resolver.
     *
     * @param resolver - variable resolver
     */
    virtual void setVariableResolver(XPathVariableResolver* resolver) = 0;

    /**
     * Resets this XPath to its original configuration.
     *
     * @descr @n XPath is reset to the same state as when it was created with XPathFactory::newXPath(). 
     * reset() is designed to allow the reuse of existing XPaths thus saving resources 
     * associated with the creation of new XPath objects.
     * 
     * After invoke this method, all the XObject that returned from this XPath or 
     * XPathExpression created by this XPath will become invalided.
     */
    virtual void reset() = 0;

    /**
     * Frees the memory of this XPath object.
     *
     * @descr @n After invoking this method, all the XObject that returned from this XPath or 
     * XPathExpression created by this XPath will become invalided.
     */
    virtual void purgeMemory() = 0;
};

INTEL_XML_XPATH_NAMESPACE_END

#endif
