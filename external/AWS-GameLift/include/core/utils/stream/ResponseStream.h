/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <core/Core_EXPORTS.h>
#include <core/utils/memory/AWSMemory.h>
#include <core/utils/memory/stl/AWSStreamFwd.h>

namespace Aws
{
    namespace Utils
    {
        namespace Stream
        {
            /**
             * Encapsulates and manages ownership of custom response streams. This is a move only type.
             */
            class AWS_CORE_API ResponseStream
            {
            public:
                /**
                 * sets underlying stream to nullptr
                 */
                ResponseStream();
                /**
                 * moves the underlying stream
                 */
                ResponseStream(ResponseStream&&);
                /**
                 * Uses factory to allocate underlying stream
                 */
                ResponseStream(const Aws::IOStreamFactory& factory);
                /**
                 * Takes ownership of an underlying stream.
                 */
                ResponseStream(IOStream* underlyingStreamToManage);
                ResponseStream(const ResponseStream&) = delete;
                ~ResponseStream();

                /**
                 * moves the underlying stream
                 */
                ResponseStream& operator=(ResponseStream&&);
                ResponseStream& operator=(const ResponseStream&) = delete;

                /**
                 * Gives access to underlying stream, but keep in mind that this changes state of the stream
                 */
                inline Aws::IOStream& GetUnderlyingStream() const { return *m_underlyingStream; }

            private:
                void ReleaseStream();

                Aws::IOStream* m_underlyingStream;
            };

            class AWS_CORE_API DefaultUnderlyingStream : public Aws::IOStream
            {
            public:
                using Base = Aws::IOStream;

                DefaultUnderlyingStream();
                DefaultUnderlyingStream(Aws::UniquePtr<std::streambuf> buf);
                virtual ~DefaultUnderlyingStream();
            };

            AWS_CORE_API Aws::IOStream* DefaultResponseStreamFactoryMethod();

        } //namespace Stream
    } //namespace Utils
} //namespace Aws
