/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */


#pragma once

#include <core/Core_EXPORTS.h>
#include <core/utils/memory/stl/AWSString.h>
#include <core/utils/memory/stl/AWSVector.h>
#include <core/auth/AWSAuthSigner.h>
#include <memory>


namespace Aws
{
    namespace Client
    {
        class AWSAuthSigner;
    }
    namespace Auth
    {
        class AWSCredentialsProvider;

        class AWS_CORE_API AWSAuthSignerProvider
        {
        public:
            virtual std::shared_ptr<Aws::Client::AWSAuthSigner> GetSigner(const Aws::String& signerName) const = 0;
            virtual void AddSigner(std::shared_ptr<Aws::Client::AWSAuthSigner>& signer) = 0;
            virtual ~AWSAuthSignerProvider() = default;
        };

        class AWS_CORE_API DefaultAuthSignerProvider : public AWSAuthSignerProvider
        {
        public:
            /**
             * Creates a Signature-V4 signer provider that supports the different implementations of Signature-V4
             * used for standard and event-stream requests.
             *
             * @param credentialsProvider A provider to retrieve the access/secret key used to derive the signing
             * @param serviceName The canonical name of the AWS service to be used in the signature
             * @param region The AWS region in which the requests will be made.
             */
            DefaultAuthSignerProvider(const std::shared_ptr<AWSCredentialsProvider>& credentialsProvider,
                const Aws::String& serviceName, const Aws::String& region,
                Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy signingPolicy = Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy::RequestDependent,
                bool urlEscapePath = true);
            explicit DefaultAuthSignerProvider(const std::shared_ptr<Aws::Client::AWSAuthSigner>& signer);
            void AddSigner(std::shared_ptr<Aws::Client::AWSAuthSigner>& signer) override;
            std::shared_ptr<Aws::Client::AWSAuthSigner> GetSigner(const Aws::String& signerName) const override;
        private:
            Aws::Vector<std::shared_ptr<Aws::Client::AWSAuthSigner>> m_signers;
        };
    }
}
