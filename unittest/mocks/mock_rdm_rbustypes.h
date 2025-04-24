/**
 * Copyright 2023 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include "rdm_types.h"
#include "rdm_utils.h"
#include "rdm_rbus.h"

#define RBUS_ERROR_SUCCESS 0
#define RBUS_ENABLED 1
typedef enum
{
    RBUS_BOOLEAN  = 0x500,  /**< bool true or false */
    RBUS_CHAR,              /**< char of size 1 byte*/
    RBUS_BYTE,              /**< unsigned char */
    RBUS_INT8,              /**< 8 bit int */
    RBUS_UINT8,             /**< 8 bit unsigned int */
    RBUS_INT16,             /**< 16 bit int */
    RBUS_UINT16,            /**< 16 bit unsigned int */
    RBUS_INT32,             /**< 32 bit int */
    RBUS_UINT32,            /**< 32 bit unsigned int */
    RBUS_INT64,             /**< 64 bit int */
    RBUS_UINT64,            /**< 64 bit unsigned int */
    RBUS_SINGLE,            /**< 32 bit float */
    RBUS_DOUBLE,            /**< 64 bit float */
    RBUS_DATETIME,          /**< rbusDateTime_t structure for Date/Time */
    RBUS_STRING,            /**< null terminated C style string */
    RBUS_BYTES,             /**< byte array */
    RBUS_PROPERTY,          /**< property instance */
    RBUS_OBJECT,            /**< object instance */
    RBUS_NONE
} rbusValueType_t;

typedef void* rbusHandle_t;
typedef void* rbusValue_t;

