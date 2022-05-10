#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <crt/StlAllocator.h>
#include <crt/Types.h>

#include <common/uuid.h>

namespace Aws
{
    namespace Crt
    {
        class AWS_CRT_CPP_API UUID final
        {
          public:
            UUID() noexcept;
            UUID(const String &str) noexcept;

            UUID &operator=(const String &str) noexcept;

            bool operator==(const UUID &other) noexcept;
            bool operator!=(const UUID &other) noexcept;
            operator String() const;
            operator ByteBuf() const noexcept;

            inline operator bool() const noexcept { return m_good; }

            int GetLastError() const noexcept;

            String ToString() const;

          private:
            aws_uuid m_uuid;
            bool m_good;
        };
    } // namespace Crt
} // namespace Aws
