#pragma once

namespace SecuritySettings
{
	// Make sure to update the database layout when changing these variables
	inline constexpr const uint MIN_USERNAME_LENGTH = 3;
	inline constexpr const uint MIN_EMAIL_LENGTH = 5;
	inline constexpr const uint MIN_PASSWORD_LENGTH = 5;

	// Make sure to update the database layout when changing these variables
	inline constexpr const uint MAX_USERNAME_LENGTH = 15;
	inline constexpr const uint MAX_EMAIL_LENGTH = 255;
	inline constexpr const uint MAX_PASSWORD_LENGTH = 128;
}