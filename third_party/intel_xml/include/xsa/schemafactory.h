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

/** @file schemafactory.h @brief The SchemaFactory interface
 *
 * Defines the SchemaFactory interface, which is the origination of all
 * schema validation operations.
 */
 
#ifndef __COM_INTEL_XML_VALIDATOR_SCHEMAFACTORY_H__
#define  __COM_INTEL_XML_VALIDATOR_SCHEMAFACTORY_H__

#include "include/common/defines.h"
#include "include/common/types.h"

INTEL_XML_NAMESPACE_BEGIN
class ErrorHandler;
class StreamSource;
INTEL_XML_NAMESPACE_END

INTEL_XML_UTIL_NAMESPACE_BEGIN
class Reader;
INTEL_XML_UTIL_NAMESPACE_END

INTEL_XML_VALIDATOR_NAMESPACE_BEGIN
class Schema;
class ResourceResolver;

/**
  * Provide methods to produce Schema objects.
  *
  * SchemaFactory is the object where all Schema
  * validation operations originate. To perform a schema validation,
  * follow the instructions of the API reference,
  * @ref interface_groups page.
  *
  * @note An object of this class may not be used in multiple threads running concurrently.
  * Different threads may use different SchemaFactory concurrently.
  */
class VALIDATION_EXPORT SchemaFactory
{ 
public:
    /** Destructor */
    virtual ~ SchemaFactory(){}
public:
    /**
     * Checks whether the specified schema language supported by this SchemaFactory.
     *
     * @descr @n Now SchemaFactory only supports W3C XML Schema 1.0 
     * that is specified by the constant #W3C_XML_SCHEMA_NS_URI in constants.h, other values are invalid.
     * @param language - the schema language which is a full URI
     * @return TRUE if the language is supported; otherwise, FALSE.
     */
     virtual bool isSchemaLanguageSupported(const ixml::XMLChar* language) = 0;
    
    /**
     * Creates a special Schema object with an empty restriciton set.
     *
     * @descr @n It is assumed that the schema can be found in the location of @c "<xsi:schemaLocation>" or 
     * @c "<xsi:NoNamespaceSchemaLocation>" during XML document parsing.
     *
     * This method creates a special Schema object that performs validation by using 
     * location hints specified in the XML document.
     *
     * @note  The use of schema location hints introduces a vulnerability 
     * to denial-of-service attacks. This method is not recommended because it does not conduct
     * schema compiling but just returns a special Schema object. It delays the schema compiling to validation runtime.
     * @return The compiled Schema object. In case of an error, reports it to ErrorHandler; 
     * if ErrorHandler is not set or set NULL, returns ValidationException if the compiling process failed.
     */
     virtual Schema* createSchema() = 0;
    
    /**
     * Creates a Schema object according to the stream source.
     *
     * @descr @n The Schema object is created through a schema source and a base URI.
     * The base URI is used to locate the external resource. A StreamSource object contains 
     * a Reader and @c systemId. This method checks them in the following order to get the 
     * schema source and base URI:
     * - Reader is taken as the schema source if it is not NULL. The base URI is 
     * computed from @c systemId by truncating the file name, if @c systemId is not NULL. If @c systemId is 
     * a file name without directory information or NULL, the base URI is considered as ".", 
     * the working directory.
     * - @c systemId is considered as the schema source if Reader is NULL, meanwhile,
     * the base URI can also be obtained by truncating the file name. If the file URI is a file name without 
     * directory information, the base URI is ".".
     *
     * @param streamSource - a StreamSource object
     * @return The compiled Schema object. In case of an error, reports it to ErrorHandler; 
     * if ErrorHandler is not set or set NULL, returns:
     * -  IllegalArgumentException - if StreamSource object is NULL, 
     *                        or Reader cannot read if Reader is not NULL,
     *                        or systemId cannot be resolved.
     * -  ValidationException - if the compiling process failed.
     */
     virtual Schema* createSchema(ixml::StreamSource* streamSource) = 0;
    
    /**
     * Creates a Schema object according to a file URI.
     * @param systemId - an systemId which gives a file location
     * @return The compiled Schema. In case of an error, reports it to ErrorHandler; 
     * if ErrorHandler is not set or set NULL, returns:
     * -  IllegalArgumentException - if systemId is NULL or cannot be resolved.
     * -  ValidationException - if the compiling process failed.
     */
     virtual Schema* createSchema(const ixml::XMLChar* systemId) = 0;

    /**
     * Creates a Schema object according to the @e systemId array.
     * @descr @n The method combines all schema sources to a single restriction set
     * and validate the input XML document against this set.
     * @param systemId - an array of systemId
     * @param size - the size of systemId array
     * @return The compiled Schema. In case of an error, reports it to ErrorHandler; 
     * if ErrorHandler is not set or set NULL, returns:
     * - @c IllegalArgumentException - if any systemId is NULL or cannot be resolved.
     * - @c ValidationException - if the compiling process failed.
     */
     virtual Schema* createSchema(const ixml::XMLChar* systemId[], unsigned size) = 0;
    
    /**
     * Releases a Schema object.
     * @param schema - a Schema object to be released
     */
     virtual void releaseSchema(Schema* schema) = 0;
    
    /**
     * Configures the schema validation process.
     * @descr @n Enable or disable the features that are used to control the way of schema compiling process.
     * 
     * @note  These features set by applications will be passed to the Validator
     * object defaultly through the creation chain: createSchema() -> #Schema -> Validator.
     * Validator also provides setFeature() to reset the features.
     * @param name - feature's name which is any fully-qualified URI
     * @param value - The requested value of the feature (TRUE or FALSE).
     * @return IllegalArgumentException - if the specified feature value can't be assigned or retrieved.
     */
     virtual void setFeature(const ixml::XMLChar* name, bool value) = 0;
    
    /**
     * Sets the ErrorHandler to receive errors encountered during the schema validation.
     * @descr @n The error handler can be used to customize the error handling process during schema validation. 
     * If an ErrorHandler is set, errors found during the schema parsing will be first sent to the ErrorHandler.
     * Similar to setFeature method, the error handler set here can be passed to Validator object defaultly through the creation chain:
     * createSchema() -> #Schema -> Validator. Application code can change the default behavior by setting a new ErrorHandler
     * object by calling Validator#setErrorHandler(). If the ErrorHandler is not set, 
     * the implementation will behave as if the following ErrorHandler is set:
     * 
     * @code
     * class MyErrorHandler: public ErrorHandler{
     *    void fatalError(Exception &e){
     *       throw e;
     *  }
     *  void error(Exception &e){
     *       throw e;
     *  }
     *  void warning(Exception &e){
     *  }
     * };
     * @endcode
     * 
     * @note  C++ API of Intel(R) XML Schema Accelerator does not distinguish "warning", "error" and "fatal error", so that all errors
     * are reported to ErrorHandler::error().
     * @param errorHandler - the object of ErrorHandler
     */
     virtual void setErrorHandler(ixml::ErrorHandler* errorHandler) = 0;
    
    /**
     * Sets the ResourceResolver to customize external resource resolution when parsing schemas.
     * @descr @n SchemaFactory uses a ResourceResolver object when it needs to locate external resources while
     * parsing schemas. Similar to setFeature and setErrorHandler, the value setting in this method can be passed to Validator object defaultly.
     * Application can change the default behavior by setting a new ResourceResolver object by calling Validator#setResourceResolver().
     * If the ResourceResolver is not set, the implementation will behave as if the following ResourceResolver is set:
     * 
     * @code
     * class MyResourceResolver: public ResourceResolver{
     *   ixml::StreamSource* resolveResource(const ixml::XMLChar* nameSpace, const ixml::XMLChar* resourceURI, const ixml::XMLChar* baseURI) {
     *     return NULL;
     *   }
     * };
     * @endcode
     * @param resourceResolver - a ResourceResolver object
     */
     virtual void setResourceResolver(ResourceResolver* resourceResolver) = 0;
    
    /**
     * Checks whether the feature is set.
     * @param name - feature name is any fully-qualified URI
     * @return Value of the feature (TRUE or FALSE).
     */
     virtual bool getFeature(const ixml::XMLChar* name) = 0;
    
    /**
     * Gets the current ErrorHandler of this SchemaFactory.
     * @return An ErrorHandler object.
     */
     virtual ixml::ErrorHandler* getErrorHandler() = 0;
    
    /**
     * Gets the ResourceResolver of this SchemaFactory.
     * @return A ResourceResolver object.
     */
     virtual ResourceResolver* getResourceResolver() = 0;
public:

       /**
        * Creates a SchemaFactory instance.
        *
        * @descr @n It returns a new SchemaFactory object for each call to this method, 
        * and every object has its own properties. Application code must release the object 
        * after using it.
        *
        * @param language - schame language which is a full-qualified URI, the valid parameter is 
        *                     #W3C_XML_SCHEMA_NS_URI defined in constants.h
     * @return A SchemaFactory object.
        */
    static SchemaFactory* newInstance(const ixml::XMLChar* language);

       /**
        * Releases a SchemaFactory object which is created by newInstance().
        *
        * @descr @n Use this interface instead of calling native C++ "delete" to delete a
        * SchemaFactory instance.
        *
        * @param schemaFactory - a SchemaFactory object created by newInstance()
        */
    static void releaseInstance(SchemaFactory* schemaFactory);
};

INTEL_XML_VALIDATOR_NAMESPACE_END
    
#endif

