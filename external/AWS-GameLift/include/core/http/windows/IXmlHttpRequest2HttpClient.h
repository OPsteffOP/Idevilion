/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <core/http/HttpClient.h>
#include <core/http/HttpRequest.h>
#include <core/client/ClientConfiguration.h>
#include <core/utils/ResourceManager.h>

#include <wrl.h>

struct IXMLHTTPRequest2;

namespace Aws
{
    namespace Http
    {
        typedef Microsoft::WRL::ComPtr<IXMLHTTPRequest2> HttpRequestComHandle;

        /**
         * COM-based http client. To use this client see the CMake option USE_IXML_HTTP_REQUEST_2.
         * Note this client is written for compatibility with windows versions that do not ship with winhttp, and will only work
         * on windows versions > 8.1. WinHttp should still be the default client for windows when possible. This client will run the IXMLHttpRequest2
         * COM module in CLSCTX_SERVER mode.
         */
        class AWS_CORE_API IXmlHttpRequest2HttpClient : public HttpClient
        {
        public:
            /**
             * Initialize client based on clientConfiguration. This will create a connection pool and configure client
             * wide parameters.
             */
            IXmlHttpRequest2HttpClient(const Aws::Client::ClientConfiguration& clientConfiguration);
            virtual ~IXmlHttpRequest2HttpClient();

            /**
             * Makes http request, returns http response.
             */
            virtual std::shared_ptr<HttpResponse> MakeRequest(const std::shared_ptr<HttpRequest>& request,
                Aws::Utils::RateLimits::RateLimiterInterface* readLimiter = nullptr,
                Aws::Utils::RateLimits::RateLimiterInterface* writeLimiter = nullptr) const override;

            /**
             * You must call this method before making any calls to the constructor, unless you have already
             * called CoInit elsewhere in your system.
             */
            static void InitCOM();

            /**
             * IXMLHTTPRequest2 doesn't support transfer-encoding:chunked
             */
            virtual bool SupportsChunkedTransferEncoding() const override { return false; }

        protected:
            /**
             * Override any configuration on request handle.
             * The usage is override this function in the subclass to configure whatever you want on request handle.
             */
            virtual void OverrideOptionsOnRequestHandle(const HttpRequestComHandle&) const {}

        private:
            void FillClientSettings(const HttpRequestComHandle&) const;

            //we can't reuse these com objects like we do in other http clients, just put a new one back into the resource manager.
            void ReturnHandleToResourceManager() const;

            mutable Aws::Utils::ExclusiveOwnershipResourceManager<HttpRequestComHandle> m_resourceManager;
            Aws::String m_proxyUserName;
            Aws::String m_proxyPassword;
            size_t m_poolSize;
            bool m_followRedirects;
            bool m_verifySSL;
            size_t m_totalTimeoutMs;
        };
    }
}

