/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once
#include <gamelift/GameLift_EXPORTS.h>
#include <core/utils/memory/stl/AWSVector.h>
#include <gamelift/model/PlayerSession.h>
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
   * href="http://docs.aws.amazon.com/goto/WebAPI/gamelift-2015-10-01/CreatePlayerSessionsOutput">AWS
   * API Reference</a></p>
   */
  class AWS_GAMELIFT_API CreatePlayerSessionsResult
  {
  public:
    CreatePlayerSessionsResult();
    CreatePlayerSessionsResult(const Aws::AmazonWebServiceResult<Aws::Utils::Json::JsonValue>& result);
    CreatePlayerSessionsResult& operator=(const Aws::AmazonWebServiceResult<Aws::Utils::Json::JsonValue>& result);


    /**
     * <p>A collection of player session objects created for the added players.</p>
     */
    inline const Aws::Vector<PlayerSession>& GetPlayerSessions() const{ return m_playerSessions; }

    /**
     * <p>A collection of player session objects created for the added players.</p>
     */
    inline void SetPlayerSessions(const Aws::Vector<PlayerSession>& value) { m_playerSessions = value; }

    /**
     * <p>A collection of player session objects created for the added players.</p>
     */
    inline void SetPlayerSessions(Aws::Vector<PlayerSession>&& value) { m_playerSessions = std::move(value); }

    /**
     * <p>A collection of player session objects created for the added players.</p>
     */
    inline CreatePlayerSessionsResult& WithPlayerSessions(const Aws::Vector<PlayerSession>& value) { SetPlayerSessions(value); return *this;}

    /**
     * <p>A collection of player session objects created for the added players.</p>
     */
    inline CreatePlayerSessionsResult& WithPlayerSessions(Aws::Vector<PlayerSession>&& value) { SetPlayerSessions(std::move(value)); return *this;}

    /**
     * <p>A collection of player session objects created for the added players.</p>
     */
    inline CreatePlayerSessionsResult& AddPlayerSessions(const PlayerSession& value) { m_playerSessions.push_back(value); return *this; }

    /**
     * <p>A collection of player session objects created for the added players.</p>
     */
    inline CreatePlayerSessionsResult& AddPlayerSessions(PlayerSession&& value) { m_playerSessions.push_back(std::move(value)); return *this; }

  private:

    Aws::Vector<PlayerSession> m_playerSessions;
  };

} // namespace Model
} // namespace GameLift
} // namespace Aws
