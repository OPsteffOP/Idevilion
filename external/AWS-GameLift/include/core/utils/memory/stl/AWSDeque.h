/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <core/Core_EXPORTS.h>

#include <core/utils/memory/stl/AWSAllocator.h>

#include <deque>

namespace Aws
{

template< typename T > using Deque = std::deque< T, Aws::Allocator< T > >;

} // namespace Aws
