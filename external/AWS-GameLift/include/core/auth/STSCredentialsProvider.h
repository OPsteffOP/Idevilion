/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */


#pragma once

#include <core/Core_EXPORTS.h>
#include <core/utils/DateTime.h>
#include <core/utils/memory/stl/AWSString.h>
#include <core/internal/AWSHttpResourceClient.h>
#include <core/auth/AWSCredentialsProvider.h>
#include <memory>

namespace Aws
{
    namespace Auth
    {
        /**
         * To support retrieving credentials of STS AssumeRole with web identity.
         * Note that STS accepts request with protocol of queryxml. Calling GetAWSCredentials() will trigger (if expired)
         * a query request using AWSHttpResourceClient under the hood.
         */
        class AWS_CORE_API STSAssumeRoleWebIdentityCredentialsProvider : public AWSCredentialsProvider
        {
        public:
            STSAssumeRoleWebIdentityCredentialsProvider();

            /**
             * Retrieves the credentials if found, otherwise returns empty credential set.
             */
            AWSCredentials GetAWSCredentials() override;

        protected:
            void Reload() override;

        private:
            void RefreshIfExpired();
            Aws::String CalculateQueryString() const;

            Aws::UniquePtr<Aws::Internal::STSCredentialsClient> m_client;
            Aws::Auth::AWSCredentials m_credentials;
            Aws::String m_roleArn;
            Aws::String m_tokenFile;
            Aws::String m_sessionName;
            Aws::String m_token;
            bool m_initialized;
        };
    } // namespace Auth
} // namespace Aws