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

#ifndef _RDM_OPENSSL_H_
#define _RDM_OPENSSL_H_

#define RDM_DOWNLOADS_DIR       "/rdm/downloads/"
#define RDM_MANIFEST_DIR        "/etc/rdm/"
#define RDM_TMP_SIGFILE         "/tmp/sig.truncated"
#define RDM_KMS_PUB_KEY         "/tmp/rdmconfig.file"
#define RDM_KMS_PADDING_FILE    "pkg_padding"
#define RDM_SIGFILE_SUFFIX      "-pkg.sig"
#define RDM_MANIFEST_SUFFIX     "_cpemanifest"
#define ENABLE_DEBUG_FLAG       "/tmp/debug_rdmopenssl"

/**
 * Obfuscated error return values.
 * Inital status returns to splunk will consist of the string:
 * "performance status bla bla: " followed by the hex ascii of one of
 * the following values.
 */
#define retcode_param_error     0x5165C860              /*!< -1 */
#define retcode_success         0x15245EAD              /*!< 0 */
#define retcode_datafile_err    0x3560800C              /*!< 1 */
#define retcode_sigfile_err     0x59A67B29              /*!< 2 */
#define retcode_ssl_err         0x716A311F              /*!< 3 */
#define retcode_verify_fail     0x151358C6              /*!< 4 */
#define retcode_keyfile_err     0x389CD6A0

/**< Minimum bufferlength for reply strings */
#define REPLY_MSG_LEN   40

/**< buffer sizes */
#define SHA256_DIGEST_LENGTH 32
#define SHA256_ASCII_DIGEST_LENGTH (SHA256_DIGEST_LENGTH * 2)
#define RSA2048_SIGNATURE_LEN 256
#define RSA2048_ASCII_SIGNATURE_LEN ( RSA2048_SIGNATURE_LEN * 2 )

#define RDM_SIG_TYPE_KMS     0
#define RDM_SIG_TYPE_OPENSSL 1

INT32 rdmOpensslRsafileSignatureVerify(const CHAR *data_file,
                                       size_t      file_len,
                                       const CHAR *sig_file,
                                       const CHAR *vkey_file,
                                       CHAR       *reply_msg,
                                       INT32      *reply_msg_len);

INT32 rdmSignatureVerify(CHAR *cache_dir,
                         CHAR *app_name,
                         INT32 prepare_files);

INT32 rdmDecryptKey(CHAR *out_key);

VOID rdmInitSslLib(VOID);



#endif //_RDM_TYPES_H_
