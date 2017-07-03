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

/** @file transformtypes.h Data structures used in the transformation
 *
 * Defines @c ParamDataType, ParamData for the stylesheet parameter,
 * and @c XSLOutputMethod, @c XSLOutputIndent,
 * @c XSLOutputEncoding, XSLOutputProperty and XSLOutputProperties
 * for output properties.
 */

#ifndef __COM_INTEL_XML_TRANSFORM_COMMON_H__
#define __COM_INTEL_XML_TRANSFORM_COMMON_H__

#include "include/common/types.h"
#include "include/common/defines.h"

INTEL_XML_TRANSFORM_NAMESPACE_BEGIN

/**
 * Is the stylesheet parameter type enumeration.
 *
 * @descr @n Used in ParamData to indicate the type of the stylesheet parameter.
 */

typedef enum {
    STP_BOOLEAN, ///< An XSL @c Boolean type.
    STP_NUMBER,  ///< An XSL @c Number type.
    STP_STRING,  ///< An XSL @c String type.
    STP_USER     ///< A Non-XSL user-defined type.
} ParamDataType;

/**
 * Is the stylesheet parameter structure.
 * 
 * @descr @n Used as the argument in Transformer#setParameter() and
 * Transformer#getParameter() to set/get the stylesheet parameter.
 * When creating a stylesheet parameter, the user should specify @e type, 
 * and set the @e data according to the specified @e type.
 * If the @e data content is not consistent with @e type, you will get
 * unexpected result.
 */
typedef struct{
    union{
        bool                    boolean; ///< Holds a boolean value, if the 
                                         ///< specified type is 
                                         ///< @c STP_BOOLEAN.
                                         
        double                  number;  ///< Holds a double value, if the 
                                         ///< specified type is @c STP_NUMBER.
                                         
        const ixml::XMLChar*    string;  ///< Holds an XMLChar string,
                                         ///< if the specified type is 
                                         ///< @c STP_STRING.

        void*                   user;    ///< Holds user-defined data as void*,
                                         ///< if the specified type is 
                                         ///< @c STP_USER.
                                         ///< The user-defined type is not 
                                         ///< recognized by Intel(R) XSLT Accelerator,
                                         ///< but can be passed through unchanged 
                                         ///< as arguments to
                                         ///< user-defined extension functions
    } data;                                 ///< Union of parameter value    
    ParamDataType type;                  ///<Indicating the type of the value 
                                         ///< this parameter holds.
} ParamData;


/**
 * The output method enumeration.
 * 
 * @descr @n One of the output properties for the transformation. Refer to
 * <a href='http://www.w3.org/TR/xslt#output'>XSLT Recommendation, section 16
 * </a> for the definition and specifics of the output properties.
 */
typedef enum {
    UNKNOWN_METHOD = 0, ///< The output method is not defined in the stylesheet; 
                        ///< the actual method is 
                        ///< "defaulted" according to 
                        ///< <a href='http://www.w3.org/TR/xslt#output'>XSLT 
                        ///< Recommendation, section 16</a> 

    XML_METHOD = 1,     ///< The stylesheet explicitly defines the output method 
                        ///< as the xml output method.

    HTML_METHOD = 2,    ///< The stylesheet explicitly defines the output method 
                        ///< as the html output method.
    TEXT_METHOD = 3     ///< The stylesheet explicitly defines the output method 
                        ///< as the text output method.
} XSLOutputMethod;


/**
 * Is the output encoding enumeration.
 * 
 * @descr @n One of the output properties for the transformation. Refer to
 * <a href='http://www.w3.org/TR/xslt#output'>XSLT Recommendation, section 16
 * </a> for the definition and specifics of the output properties.
 * 
 * 
  *This enumeration lists the encodings supported in current version of 
 * Intel(R) XSLT Accelerator. See @c types.h for a 
 * full enumeration definition. If stylesheet does not explicitly define output encoding, 
 * system uses UTF-8 as default. If stylesheet explicitly defines the output encoding as UTF-16 
 * (LE, unspedified), system uses UTF-16LE. If stylesheet explicitly defines the output encoding 
 * as UTF-32 (LE, unspedified), system uses UTF-32LE. You will notice this when using 
 * Transformer#getOutputProperties(), Transformer#getOutputProperty(), Templates#getOutputProperties().  
 *  
 */
typedef ixml::EncodingStyle XSLOutputEncoding;

/**
 * The output indent enumeration.
 * 
 * @descr @n One of the output properties for the transformation. Refer to
 * <a href='http://www.w3.org/TR/xslt#output'>XSLT Recommendation, section 16
 * </a> for the definition and specifics of the output properties.
 */
typedef enum {
    DEFAULT = 0,///< The output indent is not defined in the stylesheet. 
                ///< the actual method is 
                ///< "defaulted" according to 
                ///< <a href='http://www.w3.org/TR/xslt#output'>XSLT 
                ///< Recommendation, section 16</a> 

    ON = 1,     ///< The stylesheet explicitly defines the output indent as "yes".

    OFF = 2     ///< The stylesheet explicitly defines the output indent as "no".
} XSLOutputIndent;

/**
 * Is the output property union.
 * 
 * @descr @n Used as argument in Transformer#getOutputProperty() and
 * Transformer#setOutputProperty() to retrieve or set one of the output
 * transformation settings.
 * 
  *One of the output properties for the transformation. Refer to
 * <a href='http://www.w3.org/TR/xslt#output'>XSLT Recommendation, section 16
 * </a> for the definition and specifics of the output properties.
 */
typedef union {
    XSLOutputMethod     method;     ///< The overall method that should be used for 
                                    ///< outputting the
                                    ///< result tree, specified as the @c method
                                    ///< attribute value of @c xsl:output.

    XSLOutputEncoding   encoding;   ///< The preferred character encoding that Intel(R) 
                                    ///< XSLT Accelerator
                                    ///< should use to encode sequences of characters 
                                    ///< as sequences of bytes, specified as the 
                                    ///< @c encoding attribute value
                                    ///< of @c xsl:output.

    XSLOutputIndent     indent;     ///< The public identifier indicating whether 
                                    ///< Intel(R) XSLT Accelerator may add an 
                                    ///< additional whitespace when
                                    ///< outputting the result tree, specified as the
                                    ///< @c indent attribute value of 
                                    ///< @c xsl:output.
    ixml::XMLChar*      version;    ///< The version of the output method, specified 
                                    ///< as the @e version attribute value 
                                    ///< of @c xsl:output

    bool                omit_decl;  ///< The public identifier indicating whether 
                                    ///< Intel(R) XSLT Accelerator should output an 
                                    ///< XML declaration, specified as the 
                                    ///< @c omit-xml-declaration.
                                    ///< attribute value of @c xsl:output.

    bool                standalone; ///< The public identifier indicating whether 
                                    ///< Intel(R) XSLT Accelerator should output a 
                                    ///< standalone document declaration, specified 
                                    ///< as the @c standalone attribute 
                                    ///< value of @c xsl:output.

    ixml::XMLChar*      dpublic;    ///< The public identifier to be used in the 
                                    ///< document type declaration, specified as 
                                    ///< the @c doctype-public 
                                    ///< attribute value of @c xsl:output.

    ixml::XMLChar*      dsystem;    ///< The system identifier to be used in the 
                                    ///< document type declaration, specified as 
                                    ///< the @c doctype-system 
                                    ///< attribute value of @c xsl:output.

    ixml::XMLChar*      cdata;      ///< A list of the names of elements whose text 
                                    ///< node children should be 
                                    ///< output using CDATA sections, specified as 
                                    ///< the @c cdata-section-elements 
                                    ///< attribute value of @c xsl:output.

    ixml::XMLChar*      media_type; ///< The media type (MIME content type) of the data
                                    ///< that results from outputting the result 
                                    ///< tree, specified as the 
                                    ///< @c media-type attribute value 
                                    ///< of @c xsl:output.
} XSLOutputProperty;


/**
 * Is the output properties structure.
 *
 * @descr @n Used as arguments in Transformer#getOutputProperties(),
 * Transformer#setOutputProperties(), and
 * Templates#getOutputProperties() to retrieve the entire set of output
 * settings.
 * 
  *One of the output properties for the transformation. Refer to
 * <a href='http://www.w3.org/TR/xslt#output'>XSLT Recommendation, section 16
 * </a> for the definition and specifics of the output properties.
 */
typedef struct {
    XSLOutputMethod     method;     ///< The overall method that should be used for 
                                    ///< outputting the 
                                    ///< result tree, specified as the @c method
                                    ///< attribute value of @c xsl:output.

    XSLOutputEncoding   encoding;   ///< The preferred character encoding that 
                                    ///< Intel(R) XSLT Accelerator should use to 
                                    ///< encode sequences of characters as sequences 
                                    ///< of bytes, specified as the 
                                    ///< @c encoding attribute  value
                                    ///< of @c xsl:output.

    XSLOutputIndent     indent;     ///< The public identifier indicating whether 
                                    ///< Intel(R) XSLT Accelerator can supply
                                    ///< an additionnal whitespace when outputting 
                                    ///< the result tree, specified as the
                                    ///< @c indent attribute value of 
                                    ///< @c xsl:output

    ixml::XMLChar*      version;    ///< The version of the output method, specified 
                                    ///< as the @c version attribute value 
                                    ///< of @c xsl:output

    bool                omit_decl;  ///< The public identifier indicating whether 
                                    ///< Intel(R) XSLT Accelerator should output an 
                                    ///< XML declaration,
                                    ///< specified as the 
                                    ///< @c omit-xml-declaration value
                                    ///< attribute on @c xsl:output.

    bool                standalone; ///< The public identifier indicating whether 
                                    ///< Intel(R) XSLT Accelerator should output a 
                                    ///< standalone document 
                                    ///< declaration, specified as the 
                                    ///< @c standalone attribute value of
                                    ///< @c xsl:output.

    ixml::XMLChar*      dpublic;    ///< The public identifier to be used in the 
                                    ///< document type
                                    ///< declaration, specified as the 
                                    ///< @c doctype-public 
                                    ///< attribute value of @c xsl:output.

    ixml::XMLChar*      dsystem;    ///< The public identifier to be used in the 
                                    ///< document type 
                                    ///< declaration, specified as the 
                                    ///< @c doctype-system 
                                    ///< attribute value of @c xsl:output.

    ixml::XMLChar*      cdata;      ///< A list of the names of elements whose text 
                                    ///< node children should be 
                                    ///< output using CDATA sections, specified as 
                                    ///< the @c cdata-section-elements 
                                    ///< attribute value of @c xsl:output.

    ixml::XMLChar*      media_type; ///< The media type (MIME content type) of the data
                                    ///< that results from outputting the result 
                                    ///< tree, specified as the 
                                    ///< @c media-type attribute value of 
                                    ///< @c xsl:output.    
} XSLOutputProperties;

INTEL_XML_TRANSFORM_NAMESPACE_END

#endif //__COM_INTEL_XML_TRANSFORM_COMMON_H__


