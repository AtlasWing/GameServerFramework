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

#ifndef __COM_INTEL_XML_CONSTANTS_H__
#define  __COM_INTEL_XML_CONSTANTS_H__
#include "include/common/defines.h"

/** @file constants.h
 *  Constants used in all components of Intel(R) XML Software Suite.
 */
/************************************
 *      Macros used for all         *
 ************************************/


/************************************
 *      Macros used for parse       *
 ************************************/


/************************************
 *    Macros used for validation    *
 ************************************/
/** Represents the supported schema language URI by Intel(R) XML Schema Accelerator. */
#define W3C_XML_SCHEMA_NS_URI                     "http://www.w3.org/2001/XMLSchema"

/* Feature names for schema validation */
/** Represents the full source schema check by Intel(R) XML Schema Accelerator. */
#define FEATURE_VALIDATOR_SCHEMA_FULL_CHECKING                "http://xml.intel.com/validator/schema_full_checking"
/** Represents the identity constraint check by Intel(R) XML Schema Accelerator. */
#define FEATURE_VALIDATOR_IDENTITY_CONSTRAINT_CHECKING        "http://xml.intel.com/validator/identity_constraint_checking"


/************************************
 *  Macros used for transformation  *
 ************************************/

/**
 * Represents the key "version" for TransformFactory::setAttribute() by Intel(R) XSLT Accelerator.
 */
#define ATTRIBUTE_XSLT_VERSION  "{http://xml.intel.com/xslt/attributes}version"



/************************************
 *      Macros used for XPath       *
 ************************************/
/** Represents XML namespace URI. */
#define XML_NAMESPACE_URI         "http://www.w3.org/XML/1998/namespace"
 
/* constants used by XPath processor*/
/** Represents the DOM model supported by Intel(R) XPath Accelerator. */
#define XPATH_DEFAULT_OBJECT_MODEL  "http://xml.intel.com/dom"

/** Represents the Intel proprietary XMLDocument model supported by Intel(R) XPath Accelerator. */
#define XPATH_INTEL_XML_DOCUMENT_MODEL  "http://xml.intel.com/xmldoc"

/** Represents the secure processing feature supported by Intel(R) XPath Accelerator.
 *
 * @descr @n When this feature is on, user is not allowed to call any user-defined functions. */
#define FEATURE_XPATH_SECURE_PROCESSING  "http://xml.intel.com/xpath/xpathonly"


#endif

