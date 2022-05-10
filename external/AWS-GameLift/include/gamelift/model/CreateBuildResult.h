﻿/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once
#include <gamelift/GameLift_EXPORTS.h>
#include <gamelift/model/Build.h>
#include <gamelift/model/AwsCredentials.h>
#include <gamelift/model/S3Location.h>
#include <utility>

namespace Aws
{
template<typename RESULT_TYPE>
class AmazonWebServiceResult;

namespace Utils
{
namespace Json
{
  class JsonValue;
} // namespace Json
} // namespace Utils
namespace GameLift
{
namespace Model
{
  /**
   * <p>Represents the returned data in response to a request
   * operation.</p><p><h3>See Also:</h3>   <a
   * href="http://docs.aws.amazon.com/goto/WebAPI/gamelift-2015-10-01/CreateBuildOutput">AWS
   * API Reference</a></p>
   */
  class AWS_GAMELIFT_API CreateBuildResult
  {
  public:
    CreateBuildResult();
    CreateBuildResult(const Aws::AmazonWebServiceResult<Aws::Utils::Json::JsonValue>& result);
    CreateBuildResult& operator=(const Aws::AmazonWebServiceResult<Aws::Utils::Json::JsonValue>& result);


    /**
     * <p>The newly created build resource, including a unique build IDs and status.
     * </p>
     */
    inline const Build& GetBuild() const{ return m_build; }

    /**
     * <p>The newly created build resource, including a unique build IDs and status.
     * </p>
     */
    inline void SetBuild(const Build& value) { m_build = value; }

    /**
     * <p>The newly created build resource, including a unique build IDs and status.
     * </p>
     */
    inline void SetBuild(Build&& value) { m_build = std::move(value); }

    /**
     * <p>The newly created build resource, including a unique build IDs and status.
     * </p>
     */
    inline CreateBuildResult& WithBuild(const Build& value) { SetBuild(value); return *this;}

    /**
     * <p>The newly created build resource, including a unique build IDs and status.
     * </p>
     */
    inline CreateBuildResult& WithBuild(Build&& value) { SetBuild(std::move(value)); return *this;}


    /**
     * <p>This element is returned only when the operation is called without a storage
     * location. It contains credentials to use when you are uploading a build file to
     * an Amazon S3 bucket that is owned by Amazon Web Services. Credentials have a
     * limited life span. To refresh these credentials, call
     * <a>RequestUploadCredentials</a>. </p>
     */
    inline const AwsCredentials& GetUploadCredentials() const{ return m_uploadCredentials; }

    /**
     * <p>This element is returned only when the operation is called without a storage
     * location. It contains credentials to use when you are uploading a build file to
     * an Amazon S3 bucket that is owned by Amazon Web Services. Credentials have a
     * limited life span. To refresh these credentials, call
     * <a>RequestUploadCredentials</a>. </p>
     */
    inline void SetUploadCredentials(const AwsCredentials& value) { m_uploadCredentials = value; }

    /**
     * <p>This element is returned only when the operation is called without a storage
     * location. It contains credentials to use when you are uploading a build file to
     * an Amazon S3 bucket that is owned by Amazon Web Services. Credentials have a
     * limited life span. To refresh these credentials, call
     * <a>RequestUploadCredentials</a>. </p>
     */
    inline void SetUploadCredentials(AwsCredentials&& value) { m_uploadCredentials = std::move(value); }

    /**
     * <p>This element is returned only when the operation is called without a storage
     * location. It contains credentials to use when you are uploading a build file to
     * an Amazon S3 bucket that is owned by Amazon Web Services. Credentials have a
     * limited life span. To refresh these credentials, call
     * <a>RequestUploadCredentials</a>. </p>
     */
    inline CreateBuildResult& WithUploadCredentials(const AwsCredentials& value) { SetUploadCredentials(value); return *this;}

    /**
     * <p>This element is returned only when the operation is called without a storage
     * location. It contains credentials to use when you are uploading a build file to
     * an Amazon S3 bucket that is owned by Amazon Web Services. Credentials have a
     * limited life span. To refresh these credentials, call
     * <a>RequestUploadCredentials</a>. </p>
     */
    inline CreateBuildResult& WithUploadCredentials(AwsCredentials&& value) { SetUploadCredentials(std::move(value)); return *this;}


    /**
     * <p>Amazon S3 location for your game build file, including bucket name and
     * key.</p>
     */
    inline const S3Location& GetStorageLocation() const{ return m_storageLocation; }

    /**
     * <p>Amazon S3 location for your game build file, including bucket name and
     * key.</p>
     */
    inline void SetStorageLocation(const S3Location& value) { m_storageLocation = value; }

    /**
     * <p>Amazon S3 location for your game build file, including bucket name and
     * key.</p>
     */
    inline void SetStorageLocation(S3Location&& value) { m_storageLocation = std::move(value); }

    /**
     * <p>Amazon S3 location for your game build file, including bucket name and
     * key.</p>
     */
    inline CreateBuildResult& WithStorageLocation(const S3Location& value) { SetStorageLocation(value); return *this;}

    /**
     * <p>Amazon S3 location for your game build file, including bucket name and
     * key.</p>
     */
    inline CreateBuildResult& WithStorageLocation(S3Location&& value) { SetStorageLocation(std::move(value)); return *this;}

  private:

    Build m_build;

    AwsCredentials m_uploadCredentials;

    S3Location m_storageLocation;
  };

} // namespace Model
} // namespace GameLift
} // namespace Aws
