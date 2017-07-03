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
/** @file schema.h @brief The Schema interface
 *
 * Defines the Schema interface.
 */
 
#ifndef __COM_INTEL_XML_VALIDATOR_SCHEMA_H__
#define  __COM_INTEL_XML_VALIDATOR_SCHEMA_H__

#include "include/common/defines.h"

INTEL_XML_VALIDATOR_NAMESPACE_BEGIN

class Validator;
/**
 * Represents the compiled, re-usable restricted Schema set.
 *
 * A Schema object is used to generate Validator objects.
 * A Schema object can be shared by multiple threads.
 * It is the responsibility of the user code to release the Validator object 
 * when it is no longer needed.
 */
class VALIDATION_EXPORT Schema
{
public:
    /** Destructor */
    virtual ~Schema() {};
    
public:
    /**
     * Creates a Validator object.
     *
     * @descr @n Creates a new Validator object from the Schema object.
     *
     * @return A Validator object.
     */
    virtual Validator* createValidator() = 0;
    
    /**
     * Releases a Validator object.
     *
     * @descr @n Use this interface instead of calling C++ native "delete"
     * to delete a Validator instance.
     *
     * @param validator - a Validator object
     */
    virtual void releaseValidator(Validator* validator) = 0;
    
};

INTEL_XML_VALIDATOR_NAMESPACE_END 

#endif
