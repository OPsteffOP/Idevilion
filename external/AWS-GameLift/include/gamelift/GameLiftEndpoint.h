/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once
#include <gamelift/GameLift_EXPORTS.h>
#include <core/Region.h>
#include <core/utils/memory/stl/AWSString.h>

namespace Aws
{

namespace GameLift
{
namespace GameLiftEndpoint
{
AWS_GAMELIFT_API Aws::String ForRegion(const Aws::String& regionName, bool useDualStack = false);
} // namespace GameLiftEndpoint
} // namespace GameLift
} // namespace Aws
