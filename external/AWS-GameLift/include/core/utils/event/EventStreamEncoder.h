/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <core/Core_EXPORTS.h>
#include <core/utils/memory/stl/AWSVector.h>
#include <event-stream/event_stream.h>

namespace Aws
{
    namespace Client
    {
        class AWSAuthSigner;
    }

    namespace Utils
    {
        namespace Event
        {
            /**
             * Utility class that wraps the logic of translating event messages to their binary format according to the
             * AWS streaming specification.
             * This class is _not_ thread-safe.
             */
            class AWS_CORE_API EventStreamEncoder
            {
            public:
                EventStreamEncoder(Aws::Client::AWSAuthSigner* signer = nullptr);


                void SetSignatureSeed(const Aws::String& seed) { m_signatureSeed = seed; }

                void SetSigner(Aws::Client::AWSAuthSigner* signer) { m_signer = signer; }

                /**
                 * Encodes the input message in the event-stream binary format and signs the resulting bits.
                 * The signing is done via the signer member.
                 */
                Aws::Vector<unsigned char> EncodeAndSign(const Aws::Utils::Event::Message& msg);
            private:
                aws_event_stream_message Encode(const Aws::Utils::Event::Message& msg);
                aws_event_stream_message Sign(aws_event_stream_message* msg);
                Aws::Client::AWSAuthSigner* m_signer;
                Aws::String m_signatureSeed;
            };
        }
    }
}
