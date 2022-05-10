﻿/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once
#include <gamelift/GameLift_EXPORTS.h>
#include <core/utils/memory/stl/AWSString.h>

namespace Aws
{
namespace GameLift
{
namespace Model
{
  enum class FleetStatus
  {
    NOT_SET,
    NEW_,
    DOWNLOADING,
    VALIDATING,
    BUILDING,
    ACTIVATING,
    ACTIVE,
    DELETING,
    ERROR_,
    TERMINATED
  };

namespace FleetStatusMapper
{
AWS_GAMELIFT_API FleetStatus GetFleetStatusForName(const Aws::String& name);

AWS_GAMELIFT_API Aws::String GetNameForFleetStatus(FleetStatus value);
} // namespace FleetStatusMapper
} // namespace Model
} // namespace GameLift
} // namespace Aws
