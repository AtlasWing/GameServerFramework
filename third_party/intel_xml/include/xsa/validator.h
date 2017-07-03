/*
 
 *---------------------------------------------------------------------------
 *
 *     Copyright 2007~2008 Intel Corporation All Rights Reserved.
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

/** @file validator.h @brief The Validator interface
 *
 * Defines the Validator interface that performs schema
 * validation on a source tree.
 */
 
#ifndef __COM_INTEL_XML_VALIDATOR_VALIDATOR_H__
#define  __COM_INTEL_XML_VALIDATOR_VALIDATOR_H__

#include "include/common/defines.h"
#include "include/common/types.h"

INTEL_XML_NAMESPACE_BEGIN
class ErrorHandler;
class Source;
class Result;
INTEL_XML_NAMESPACE_END
    
INTEL_XML_UTIL_NAMESPACE_BEGIN
class Reader;
INTEL_XML_UTIL_NAMESPACE_END

INTEL_XML_VALIDATOR_NAMESPACE_BEGIN
class ResourceResolver;
/**
 * Validates the XML document against the specified schema.
 *
 * To get an instance of the Validator, call Schema#createValidator().
 * This instance can validate XML data in a variety of input formats.
 *
 * An object of this class may not be used in multiple threads running concurrrently.
 * Different threads may use different Validator objects concurrently.
 */
class VALIDATION_EXPORT Validator
{
public:
    /** Destructor */
    virtual ~Validator() {};
    
public:
    /**
     * Resets the validator. 
     * @descr @n Validator is reset to the same state as when it was created
     * with Schema::createValidator(). For example, reset the ErrorHandler,
     * ResourceResolver and features.
     * 
     * This function allows the re-use of an existing Validator object
     * to save resources associated with the creation of a new Validator object.
     */
    virtual void reset() = 0;
    /**
     * Configures the schema validation process.
     * @descr @n Enables or disables the features that are used to control
     * the way Validator parses the schema when external schema resources 
     * need to be compiled. Configuration setting values are passed from
     * the SchemaFactory object by default. Applications can modify the
     * settings by setting them with this method.
     * @param name  - feature name, in the format of a fully-qualified URI
     * @param value - the requested value of the feature: TRUE or FALSE
     * @return In case of an error, the function returns
     * IllegalArgumentException - if the specified feature value cannot be
     * assigned or retrieved.
     */
     virtual void setFeature(const ixml::XMLChar* name, bool value) = 0;
    /**
     * Sets the ErrorHandler to receive errors encountered during the validate
     * function invocation.
     * @descr @n The error handler can be used to customize the error handling
     * process during schema validation. If an ErrorHandler is set, errors found
     * during the validation are first sent to the ErrorHandler.
     *
     * @note  The current version only supports "error", the other two functions
     * are not invoked even though they are implemented by applications. 
     * 
     * If the ErrorHandler is set to NULL, the implementation behaves
     * as if the following ErrorHandler is set:
     *
     * @code
     * class MyErrorHandler: public ErrorHandler{
     *    void fatalError(Exception &e){
     *    }
     *    void error(Exception &e){
     *    }
     *    void warning(Exception &e){
     *    }
     *    bool error(const ixml::XMLChar* errMsg){
     *        return true;
     *    }
     * };
     * @endcode
     * @param errorHandler - the object of ErrorHandler
     */
     virtual void setErrorHandler(ixml::ErrorHandler* errorHandler) = 0;
    
    /**
     * Sets the ResourceResolver to customize external resource resolution
     * while in validation operation.
     * @descr @n Validator uses a ResourceResolver object to locate external
     * resources during validation process. For example, for W3C XML Schema,
     * Validator needs to locate the schema file in 
     * @c "<xsi:noNamespaceSchemaLocation>".
     * @param resourceResolver - a ResourceResolver object
     */
     virtual void setResourceResolver(ResourceResolver* resourceResolver) = 0;
    
    /**
     * Checks whether the feature is set.
     * @param name - feature name, in the format of a fully-qualified URI
     * @return The value of the feature (TRUE or FALSE).
     */
     virtual bool getFeature(const ixml::XMLChar* name) = 0;
    
    /**
     * Gets the current ErrorHandler of this Validator object.
     * @return The ErrorHandler object.
     */
     virtual ixml::ErrorHandler* getErrorHandler() = 0;
    
    /**
     * Gets the ResourceResolver object of this Validator.
     * @return The ResourceResolver object.
     */
     virtual ResourceResolver* getResourceResolver() = 0;

    /**
     * Validates the input from a Source object.
     * @descr @n This method validates the XML instance according to the
     * specified XML source and a base URI. The base URI is used to locate
     * external resource. For different objects of Source class or Source
     * derivation classes, the validation process finds the XML source and
     * base URI as the following rules: 
     * - @b Source of base class is validated only through @c systemId. 
     *   The @c systemId is a file URI, from which
     *   both the XML source and base URI can be obtained. The base URI can be
     *   obtained by truncating the file name.
     *   If the file URI is a file name with no directory information,
     *   the base URI is ".".    
     * - @b StreamSource and @b SAXSource provide the XML instance with
     *   Reader and @c systemId.
     *   - Reader is taken as the instance source if it is not NULL. 
     *     The base URI is computed form @c systemId by truncating the file 
     *     name if @c systemId is not NULL. 
     *       If the @c systemId is a file name without directory information or NULL,
     *     the base URI is ".".            
     *   - @c systemId is considered as the XML instance source if Reader
     *     is NULL, meanwhile, the base URI can be obtained by truncating 
     *     the file name. If the file URI is a file name without directory
     *      information, the base URI is ".".
     * - @b DOMSource has a root Node. If this root Node is not NULL, the 
     *   validation starts from this Node. The base URI can also be obtained
     *   from this Node. And the systemId is ignored. Otherwise, if the root
     *   Node is NULL, the validation process degenerates like the validation
     *   of an object of Source base class.
     * - @b XMLDocSource has a XMLDocument member variable. If it is NULL, 
     *   the validation get both the XML instance and base URI from this
     *   variable; otherwise, the validation process degenerates as when
     *   validating an object of Source base class.
     *
     * @param source - the Source object or its derived object
     * @return In case of an error, the function reports it to the custom
     *             ErrorHandler, if it is set by the application code;
     *             otherwise, throws ValidationException.
     * - IllegalArgumentException - if the specified Source object is NULL, 
     *                            or xml instance source cannot be resolved.
     * - ValidationException - if the ErrorHandler is not set and error
     *   occurs during validation.
     */
     virtual void validate(ixml::Source *source) = 0;

    /**
     * Validates the input from a Source object and writes the
     * validated result to a Result object and no augmentation is attached;
     * specifics vary by input object type. 
     *
     * @descr @n For the current version, only SAXSource / SAXResult is a valid
     * parameter pair because PSVI is not supported. You can consider this 
     * validation process as a filter, and it does schema validation and 
     * replays the events to the event handler registered in SAXResult. 
     * The other Source / Result pairs, such as StreamSource / StreamResult, 
     * DOMSource / DOMResult and XMLDocSource / XMLDocResult are considered
     * invalid, and would be accepted when PSVI is supported. 
     *
     * @param source - the Source object or its derived object
     * @param result - the Result object or its derived object
     */
     virtual void validate(ixml::Source* source, ixml::Result* result) = 0;

    /**
      * Validates the input from a Source object.
      * @descr This method has the same functionality as validate(ixml::Source*).
      * The difference is that this method tells the input of Source object is valid
      * or not by the returned value instead of throwing exception. 
      * If the input is valid against the schema, the 
      * returned value is TRUE, otherwise, the returned value is FALSE. 
      * Error message can be obtained by the method getErrorMessage().
      * Since exception causes bad performance during error reporting,
      * if developers care much about the performance of invalid
      * xml validation, this method is recommended.
      * @param source - the Source object or its derived object
      * @return The validation result. If the input from the Source object
      *            is valid, then the value is TRUE. Otherwise, it is FALSE.
      *            Error message can be obtained by getErrorMessage().
      */
      virtual bool doValidation(ixml::Source *source) = 0;
     /**
       * Validates the input from a Source object and writes the
       * validated result to a Result object and no augmentation is attached;
       * specifics vary by input object type. 
       *
       * @descr This method has the same functionality as 
       * validate(ixml::Source* source, ixml::Result* result).
       * The difference is that this method tells the validation is successful or
       * not by the returned value instead of throwing exception.
       * If the input is valid against the schema, the 
       * returned value is TRUE, otherwise, the returned value is FALSE.
       * Error message can be obtained by the method getErrorMessage().
       * Since exception causes bad performance during error reporting,
       * if developers care much about the performance of invalid
       * xml validation, this method is recommended.
       * @param source - the Source object or its derived object
       * @param result - the Result object or its derived object
       * @return The validation result. If the input from the Source object
       *            is valid, then the value is TRUE. Otherwise, it is FALSE.
       *            Error message can be obtained by getErrorMessage().
       */
      virtual bool doValidation(ixml::Source* source, ixml::Result* result) = 0;
      /**
       * Returns the last error message if doValidation returns FALSE.
       * @return The error message.
        */
      virtual const ixml::XMLChar*  getErrorMessage() = 0;
};

INTEL_XML_VALIDATOR_NAMESPACE_END

#endif

