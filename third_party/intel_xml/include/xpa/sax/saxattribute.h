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
 *@file saxattribute.h
 *@brief Definition of the Attribute structure in SAX
 */


#ifndef __COM_INTEL_XML_PARSE_SAX_SAXATTRIBUTE_H__
#define __COM_INTEL_XML_PARSE_SAX_SAXATTRIBUTE_H__

#include "include/common/types.h"

INTEL_XML_PARSE_SAX_NAMESPACE_BEGIN

/**
 * Attribute type declaration in DOCTYPE
 */
typedef enum AttributeType
{
    attTypeCDATA        = 0,   ///< CDATA
    attTypeID           = 1,   ///< ID
    attTypeIDREF        = 2,   ///< IDREF
    attTypeIDREFS       = 3,   ///< IDREFS
    attTypeENTITY       = 4,   ///< ENTITY
    attTypeENTITIES     = 5,   ///< ENTITIES
    attTypeNMTOKEN      = 6,   ///< NMTOKEN
    attTypeNMTOKENS     = 7,   ///< NMTOKENS
    attTypeNotation     = 8,   ///< Notation
    attTypeEnumeration  = 9,   ///< Enumeration
    attTypeCount        = 10   ///< Count
} AttributeType;

/**
 * Describes the SAX attribute. 
 *
 * Attribute events are saved in a SAXAttribute array until a
 * ContentHandler#startElement event is encountered. The array of these attributes 
 * is then passed as an additional argument to the ContentHandler#startElement()
 * method.
 * 
 * Namespace @c xmlns attributes are not passed via the
 * SAXAttribute structure, but via ContentHandler#startNamespace
 * and ContentHandler#endNamespace events.
 *
 * If the @e valueString is a null pointer, routines should ignore
 * the attribute. This is convenient for callers who have static arrays with
 * pre-set names and only need to fill in value pointers.
 * Ignoring a NULL pointer does not apply to empty strings, which are fully valid. 
 */
struct SAX_EXPORT SAXAttribute
{
    /**
     * The prefix and associated URI of this attribute.
     */
      unsigned int                namespaceTag; 
    
    /**
     * The length of the string of this attribute name.
     */
      unsigned int                nameCount;     
    
    /**
     * The pointer to the string of this attribute name.
     */
      const ixml::XMLChar*        nameString;    
    
    /**
     * The length of the string of this attribute value.
     */
      unsigned int                valueCount;   

    /**
     * The pointer to the string of this attribute value.
     */
      const ixml::XMLChar*        valueString;
    
    /**
     * The attribute type if DTD includes such definitions; otherwise, 
     * the default CDATA.
     */
    AttributeType    type;      
};

INTEL_XML_PARSE_SAX_NAMESPACE_END

#endif

