/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <core/Core_EXPORTS.h>
#include <core/utils/Array.h>

namespace Aws
{
    namespace Utils
    {

        template< typename R, typename E > class Outcome;

        namespace Crypto
        {
            using HashResult = Outcome< ByteBuffer, bool >;

        } // namespace Crypto
    } // namespace Utils
} // namespace Aws

