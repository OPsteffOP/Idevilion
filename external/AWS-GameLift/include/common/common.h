#ifndef AWS_COMMON_COMMON_H
#define AWS_COMMON_COMMON_H

/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <common/config.h>
#include <common/exports.h>

#include <common/allocator.h>
#include <common/assert.h>
#include <common/error.h>
#include <common/macros.h>
#include <common/platform.h>
#include <common/predicates.h>
#include <common/stdbool.h>
#include <common/stdint.h>
#include <common/zero.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h> /* for abort() */
#include <string.h>

AWS_EXTERN_C_BEGIN

/**
 * Initializes internal datastructures used by aws-c-common.
 * Must be called before using any functionality in aws-c-common.
 */
AWS_COMMON_API
void aws_common_library_init(struct aws_allocator *allocator);

/**
 * Shuts down the internal datastructures used by aws-c-common.
 */
AWS_COMMON_API
void aws_common_library_clean_up(void);

AWS_COMMON_API
void aws_common_fatal_assert_library_initialized(void);

AWS_EXTERN_C_END

#endif /* AWS_COMMON_COMMON_H */
