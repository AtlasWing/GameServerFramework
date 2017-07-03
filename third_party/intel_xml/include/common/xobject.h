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

/** @file xobject.h The XObject interface
 * 
 * @descr @n XObject are used by Intel(R) XPath Accelerator and Intel(R) XSLT Accelerator
 * to pass information between the user application and the XML processor.
 */

#ifndef __COM_INTEL_XML_XOBJECT_H__
#define __COM_INTEL_XML_XOBJECT_H__

#include "include/common/defines.h"
#include "include/common/types.h"

INTEL_XML_PARSE_DOM_NAMESPACE_BEGIN
class Node;
INTEL_XML_PARSE_DOM_NAMESPACE_END

INTEL_XML_PARSE_XMLDOC_NAMESPACE_BEGIN
class XMLDocNode;
INTEL_XML_PARSE_XMLDOC_NAMESPACE_END

INTEL_XML_UTIL_NAMESPACE_BEGIN
template <class T>
class XVector;
INTEL_XML_UTIL_NAMESPACE_END

INTEL_XML_NAMESPACE_BEGIN

/**
 * Represents the nodeset iterator for XMLDocNode set 
 *
 * */
class COMMON_EXPORT XMLDocNodesetIterator {
public:
    /** Destructor. */
    virtual ~XMLDocNodesetIterator() {}
    
    /** Resets the internal cursor to the beginning of this nodeset. */
    virtual void reset() =0;
    
    /** Moves the internal cursor to the next node in this nodeset. */
    virtual void next() =0;

    /** Checks whether the internal cursor has reached the end of this nodeset.
     *
     * @return TRUE if the end of nodeset is reached; otherwise, FALSE. */
    virtual bool end() =0;

    /** Gets the node at the current cursor position. 
     *
     * @return The node at the current cursor position.
     * */
    virtual idoc::XMLDocNode* element() =0;       
};

/**
 * Iterates over the DOM node set.
 * */
class COMMON_EXPORT DOMNodesetIterator {
public:
    /** Destroys the object. */
    virtual ~DOMNodesetIterator() {}

    /** Resets the internal cursor to the beginning of this nodeset. */
    virtual void reset() =0;
    
    /** Moves the internal cursor to the next node in this nodeset. */
    virtual void next() =0;

   /** Checks whether the internal cursor has reached the end of this nodeset.
     *
     * @return TRUE if reach the end of nodeset; otherwise, FALSE. */
    virtual bool end() =0;

    /** Gets the node at the current cursor position. 
     *
     * @return The node at the current cursor position.
     * */
    virtual idom::Node* element() =0;       
};

/** The enumeration for the data types in XObject class. */
enum ObjectType{
    X_BOOLEAN,        ///<For Boolean type.
    X_NUMBER,         ///<For Number type.
    X_STRING,         ///<For String type.
    X_XMLDOCNODESET,  ///<For XMLDocNode set type.
    X_DOMNODESET,     ///<For DOM node set type.
    X_USER_DEFINED    ///<For user-defined objects as result.
};

/**
 * Wraps both XSLT & XPath data.
 *
 * @descr @n XObject is used by Intel(R) XPath Accelerator and Intel(R) XSLT Accelerator
 * to pass information between the user application and the XML processor.
 */
class COMMON_EXPORT XObject {
public:
    XObject(){}
    virtual ~XObject(){}

    /** Gets the type of internal data type. 
     *
     * @return The internal data type. */
    virtual ObjectType getType() const = 0;

    //===============set operations========
    /** Sets the internal data as @c X_BOOLEAN type, and its value is Boolean @e val. 
     * @param val - the value
     * @return TRUE if set successfully; otherwise, FALSE. */
    virtual bool setBooleanValue(bool val) =0;

    /** Sets the internal data as @c X_NUMBER type, and its value is double @e val. 
     * @param val - the value
     * @return TRUE if set successfully; otherwise FALSE. */
    virtual bool setNumberValue(double val) =0;

    /** Sets the internal data as @c X_STRING type, and the string is @e val. 
     * @param val - the string
     * @return TRUE if set successfully; otherwise FALSE. */
    virtual bool setStringValue(const XMLChar* val) =0;

    /** Sets the internal data as @c X_XMLDOCNODESET type, and the nodeset is @e val. 
     *
     * @descr @n This XObject object must be generated by an XPath 
     *        object which supports @c XPATH_INTEL_XML_DOCUMENT_MODEL.
     * @param val - the nodeset
     * @return TRUE if set successfully; otherwise FALSE. */
    virtual bool setNodesetValue(const iutil::XVector<idoc::XMLDocNode*>& val) =0;

    /** Sets the internal data as @c X_DOMNODESET type, and the nodeset is @e val. 
     *
     * @descr @n This XObject object must be generated by an XPath 
     *        object which supports @c XPATH_DEFAULT_OBJECT_MODEL.
     * @param val - the nodeset
     * @return TRUE if set successfully; otherwise FALSE. */
    virtual bool setNodesetValue(const iutil::XVector<idom::Node*>& val) =0;

    /** Sets the internal data as @c X_USER_DEFINED type, and the user object is @e val. 
     * @param val - the user object
     * @return TRUE if set successfully; otherwise FALSE. */
    virtual bool setUserValue(void* val)=0;

    //===============get operations=========
    /** Gets the content of this XObject object as Boolean.
     *
     * @descr @n If the internal data type is not @c X_BOOLEAN, this function will 
     * implicitly convert the internal data to Boolean type according to 
     * XPath 1.0 Recommendation.
     *
     * @return The content as Boolean or FALSE if no conversion rule can be found.*/
    virtual bool getBooleanValue() =0;

    /** Gets the content of this XObject object as number.
     *
     * @descr @n If the internal data type is not @c X_NUMBER, this function will 
     * implicitly convert the internal data to number type according to 
     * XPath 1.0 Recommendation. 
     *
     * @return The content as number or @c NaN if no conversion rule can be found
     * or if the conversion failed. */
    virtual double getNumberValue() =0;

    /** Gets the content of this XObject object as string.
     *
     * @descr @n If the internal data type is not @c X_STRING, this function will 
     * implicitly convert the internal data to string type according to 
     * XPath 1.0 Recommendation. 
     *
     * @return The content as number or an empty string if no conversion rule can be found. */
    virtual const XMLChar* getStringValue() =0;

    /** Gets the content of this XObject object as idoc::XMLDocNode set iterator.
     *
     * @return The iterator object if XMLDocNode set can be returned; 
     *          otherwise NULL. */
    virtual XMLDocNodesetIterator* getXMLDocIterator() =0;

    /** Gets the content of this XObject object as @c idom::Node set iterator.
     *
     * @return The iterator object if Node set can be returned, 
     *          otherwise NULL. */
    virtual DOMNodesetIterator* getDOMNodeIterator() =0;

    /** Gets the content of this XObject object as user object pointer.
     *
     * @return The user object pointer if internal type is @c X_USER_DEFINED;
     *          otherwise NULL. */
    virtual void* getUserValue() =0;

};

INTEL_XML_NAMESPACE_END

#endif //__COM_INTEL_XML_OBJECT_H__


