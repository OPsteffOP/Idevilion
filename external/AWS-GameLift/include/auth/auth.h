#ifndef AWS_AUTH_AUTH_H
#define AWS_AUTH_AUTH_H

/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <auth/exports.h>

#include <io/logging.h>

#define AWS_C_AUTH_PACKAGE_ID 6

/**
 * Auth-specific error codes
 */
enum aws_auth_errors {
    AWS_AUTH_PROFILE_PARSE_RECOVERABLE_ERROR = AWS_ERROR_ENUM_BEGIN_RANGE(AWS_C_AUTH_PACKAGE_ID),
    AWS_AUTH_PROFILE_PARSE_FATAL_ERROR,
    AWS_AUTH_SIGNING_UNSUPPORTED_ALGORITHM,
    AWS_AUTH_SIGNING_MISMATCHED_CONFIGURATION,
    AWS_AUTH_SIGNING_NO_CREDENTIALS,
    AWS_AUTH_SIGNING_ILLEGAL_REQUEST_QUERY_PARAM,
    AWS_AUTH_SIGNING_ILLEGAL_REQUEST_HEADER,
    AWS_AUTH_SIGNING_INVALID_CONFIGURATION,
    AWS_AUTH_CREDENTIALS_PROVIDER_INVALID_ENVIRONMENT,
    AWS_AUTH_CREDENTIALS_PROVIDER_INVALID_DELEGATE,
    AWS_AUTH_CREDENTIALS_PROVIDER_PROFILE_SOURCE_FAILURE,
    AWS_AUTH_CREDENTIALS_PROVIDER_IMDS_SOURCE_FAILURE,
    AWS_AUTH_CREDENTIALS_PROVIDER_STS_SOURCE_FAILURE,
    AWS_AUTH_CREDENTIALS_PROVIDER_HTTP_STATUS_FAILURE,
    AWS_AUTH_PROVIDER_PARSER_UNEXPECTED_RESPONSE,
    AWS_AUTH_CREDENTIALS_PROVIDER_ECS_SOURCE_FAILURE,
    AWS_AUTH_CREDENTIALS_PROVIDER_X509_SOURCE_FAILURE,
    AWS_AUTH_CREDENTIALS_PROVIDER_PROCESS_SOURCE_FAILURE,
    AWS_AUTH_CREDENTIALS_PROVIDER_STS_WEB_IDENTITY_SOURCE_FAILURE,
    AWS_AUTH_SIGNING_UNSUPPORTED_SIGNATURE_TYPE,
    AWS_AUTH_SIGNING_MISSING_PREVIOUS_SIGNATURE,
    AWS_AUTH_SIGNING_INVALID_CREDENTIALS,
    AWS_AUTH_CANONICAL_REQUEST_MISMATCH,
    AWS_AUTH_SIGV4A_SIGNATURE_VALIDATION_FAILURE,

    AWS_AUTH_ERROR_END_RANGE = AWS_ERROR_ENUM_END_RANGE(AWS_C_AUTH_PACKAGE_ID)
};

/**
 * Auth-specific logging subjects
 */
enum aws_auth_log_subject {
    AWS_LS_AUTH_GENERAL = AWS_LOG_SUBJECT_BEGIN_RANGE(AWS_C_AUTH_PACKAGE_ID),
    AWS_LS_AUTH_PROFILE,
    AWS_LS_AUTH_CREDENTIALS_PROVIDER,
    AWS_LS_AUTH_SIGNING,
    AWS_LS_IMDS_CLIENT,

    AWS_LS_AUTH_LAST = AWS_LOG_SUBJECT_END_RANGE(AWS_C_AUTH_PACKAGE_ID)
};

AWS_EXTERN_C_BEGIN

/**
 * Initializes internal datastructures used by aws-c-auth.
 * Must be called before using any functionality in aws-c-auth.
 *
 * @param allocator memory allocator to use for any module-level memory allocation
 */
AWS_AUTH_API
void aws_auth_library_init(struct aws_allocator *allocator);

/**
 * Clean up internal datastructures used by aws-c-auth.
 * Must not be called until application is done using functionality in aws-c-auth.
 */
AWS_AUTH_API
void aws_auth_library_clean_up(void);

AWS_EXTERN_C_END

#endif /* AWS_AUTH_AUTH_H */
