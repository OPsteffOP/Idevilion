#pragma once
#include "Utils.h"

#pragma region RAIIMod
TEST_FUNCTION_BEGIN_MODULE(Utils, RAIIMod_Set)
{
	bool test = true;

	{
		RAIIMod temp(test, false);
		TEST_VALIDATE(test, false);
	}
}
TEST_FUNCTION_END_MODULE(Utils, RAIIMod_Set)

TEST_FUNCTION_BEGIN_MODULE(Utils, RAIIMod_Undo)
{
	bool test = true;

	{
		RAIIMod temp(test, false);
	}

	TEST_VALIDATE(test, true);
}
TEST_FUNCTION_END_MODULE(Utils, RAIIMod_Undo)
#pragma endregion

#pragma region Point
TEST_FUNCTION_BEGIN_MODULE(Utils, Point_Comparison)
{
	{
		Point2i point1(5, 5);
		Point2i point2(5, 5);
		Point2i point3(10, 10);

		TEST_VALIDATE(point1 == point1, true);
		TEST_VALIDATE(point1 == point2, true);
		TEST_VALIDATE(point1 == point3, false);

		TEST_VALIDATE(point1 != point1, false);
		TEST_VALIDATE(point1 != point2, false);
		TEST_VALIDATE(point1 != point3, true);
	}

	{
		Point2f point1(5.f, 5.f);
		Point2f point2(5.f, 5.f);
		Point2f point3(10.f, 10.f);

		TEST_VALIDATE(point1 == point1, true);
		TEST_VALIDATE(point1 == point2, true);
		TEST_VALIDATE(point1 == point3, false);

		TEST_VALIDATE(point1 != point1, false);
		TEST_VALIDATE(point1 != point2, false);
		TEST_VALIDATE(point1 != point3, true);
	}
}
TEST_FUNCTION_END_MODULE(Utils, Point_Comparison)

TEST_FUNCTION_BEGIN_MODULE(Utils, Point_Arithmetics)
{
	// Addition
	{
		Point2i point1(5, 5);
		Point2i point2(5, 5);
		Point2i point3(10, 10);

		Point2i result1 = point1 + point1;
		Point2i result2 = point1 + point2;
		Point2i result3 = point1 + point3;

		Point2i expectedResult1(10, 10);
		Point2i expectedResult2(10, 10);
		Point2i expectedResult3(15, 15);

		TEST_VALIDATE(result1, expectedResult1);
		TEST_VALIDATE(result2, expectedResult2);
		TEST_VALIDATE(result3, expectedResult3);
	}

	{
		Point2f point1(5.f, 5.f);
		Point2f point2(5.f, 5.f);
		Point2f point3(10.f, 10.f);

		Point2f result1 = point1 + point1;
		Point2f result2 = point1 + point2;
		Point2f result3 = point1 + point3;

		Point2f expectedResult1(10.f, 10.f);
		Point2f expectedResult2(10.f, 10.f);
		Point2f expectedResult3(15.f, 15.f);

		TEST_VALIDATE(result1, expectedResult1);
		TEST_VALIDATE(result2, expectedResult2);
		TEST_VALIDATE(result3, expectedResult3);
	}

	// Multiplication
	{
		Point2i point1(5, 5);
		Point2i point2(5, 5);
		Point2i point3(10, 10);

		Point2i result1 = point1 * point1;
		Point2i result2 = point1 * point2;
		Point2i result3 = point1 * point3;

		Point2i expectedResult1(25, 25);
		Point2i expectedResult2(25, 25);
		Point2i expectedResult3(50, 50);

		TEST_VALIDATE(result1, expectedResult1);
		TEST_VALIDATE(result2, expectedResult2);
		TEST_VALIDATE(result3, expectedResult3);
	}

	{
		Point2f point1(5.f, 5.f);
		Point2f point2(5.f, 5.f);
		Point2f point3(10.f, 10.f);

		Point2f result1 = point1 * point1;
		Point2f result2 = point1 * point2;
		Point2f result3 = point1 * point3;

		Point2f expectedResult1(25.f, 25.f);
		Point2f expectedResult2(25.f, 25.f);
		Point2f expectedResult3(50.f, 50.f);

		TEST_VALIDATE(result1, expectedResult1);
		TEST_VALIDATE(result2, expectedResult2);
		TEST_VALIDATE(result3, expectedResult3);
	}

	// Subtraction
	{
		Point2i point1(5, 5);
		Point2i point2(5, 5);
		Point2i point3(10, 10);

		Point2i result1 = point1 - point1;
		Point2i result2 = point1 - point2;
		Point2i result3 = point1 - point3;

		Point2i expectedResult1(0, 0);
		Point2i expectedResult2(0, 0);
		Point2i expectedResult3(-5, -5);

		TEST_VALIDATE(result1, expectedResult1);
		TEST_VALIDATE(result2, expectedResult2);
		TEST_VALIDATE(result3, expectedResult3);
	}

	{
		Point2f point1(5.f, 5.f);
		Point2f point2(5.f, 5.f);
		Point2f point3(10.f, 10.f);

		Point2f result1 = point1 - point1;
		Point2f result2 = point1 - point2;
		Point2f result3 = point1 - point3;

		Point2f expectedResult1(0.f, 0.f);
		Point2f expectedResult2(0.f, 0.f);
		Point2f expectedResult3(-5.f, -5.f);

		TEST_VALIDATE(result1, expectedResult1);
		TEST_VALIDATE(result2, expectedResult2);
		TEST_VALIDATE(result3, expectedResult3);
	}

	// Division
	{
		Point2i point1(5, 5);
		Point2i point2(5, 5);
		Point2i point3(10, 10);

		Point2i result1 = point1 / point1;
		Point2i result2 = point1 / point2;
		Point2i result3 = point1 / point3;
		Point2i result4 = point3 / point1;

		Point2i expectedResult1(1, 1);
		Point2i expectedResult2(1, 1);
		Point2i expectedResult3(0, 0);
		Point2i expectedResult4(2, 2);

		TEST_VALIDATE(result1, expectedResult1);
		TEST_VALIDATE(result2, expectedResult2);
		TEST_VALIDATE(result3, expectedResult3);
		TEST_VALIDATE(result4, expectedResult4);
	}

	{
		Point2f point1(5.f, 5.f);
		Point2f point2(5.f, 5.f);
		Point2f point3(10.f, 10.f);

		Point2f result1 = point1 / point1;
		Point2f result2 = point1 / point2;
		Point2f result3 = point1 / point3;

		Point2f expectedResult1(1.f, 1.f);
		Point2f expectedResult2(1.f, 1.f);
		Point2f expectedResult3(0.5f, 0.5f);

		TEST_VALIDATE(result1, expectedResult1);
		TEST_VALIDATE(result2, expectedResult2);
		TEST_VALIDATE(result3, expectedResult3);
	}
}
TEST_FUNCTION_END_MODULE(Utils, Point_Arithmetics)
#pragma endregion

#pragma region Rectangle
TEST_FUNCTION_BEGIN_MODULE(Utils, Rectangle_IsOverlapping)
{
	Rect4f rect1(0.f, 0.f, 5.f, 5.f);
	Rect4f rect2(2.5f, 2.5f, 5.f, 5.f);
	Rect4f rect3(2.5f, 2.5f, 1.f, 1.f);
	Rect4f rect4(6.f, 6.f, 1.f, 1.f);
	Rect4f rect5(5.f, 5.f, 1.f, 1.f);

	TEST_VALIDATE(rect1.IsOverlapping(rect1), true);
	TEST_VALIDATE(rect1.IsOverlapping(rect2), true);
	TEST_VALIDATE(rect1.IsOverlapping(rect3), true);
	TEST_VALIDATE(rect1.IsOverlapping(rect4), false);
	TEST_VALIDATE(rect1.IsOverlapping(rect5), true);
	TEST_VALIDATE(rect4.IsOverlapping(rect4), true);
	TEST_VALIDATE(rect4.IsOverlapping(rect3), false);
	TEST_VALIDATE(rect4.IsOverlapping(rect2), true);
	TEST_VALIDATE(rect5.IsOverlapping(rect4), true);
}
TEST_FUNCTION_END_MODULE(Utils, Rectangle_IsOverlapping)

TEST_FUNCTION_BEGIN_MODULE(Utils, Rectangle_IsInside)
{
	Rect4f rect1(0.f, 0.f, 5.f, 5.f);
	Rect4f rect2(2.5f, 2.5f, 5.f, 5.f);

	Point2f point1(2.5f, 2.5f);
	Point2f point2(0.f, 0.f);
	Point2f point3(5.f, 5.f);
	Point2f point4(7.5f, 7.5f);

	TEST_VALIDATE(rect1.IsInside(point1), true);
	TEST_VALIDATE(rect1.IsInside(point2), true);
	TEST_VALIDATE(rect1.IsInside(point3), true);
	TEST_VALIDATE(rect1.IsInside(point4), false);
	TEST_VALIDATE(rect2.IsInside(point1), true);
	TEST_VALIDATE(rect2.IsInside(point2), false);
	TEST_VALIDATE(rect2.IsInside(point3), true);
	TEST_VALIDATE(rect2.IsInside(point4), true);
}
TEST_FUNCTION_END_MODULE(Utils, Rectangle_IsInside)
#pragma endregion

#pragma region String
TEST_FUNCTION_BEGIN_MODULE(Utils, String_Conversion)
{
	std::string testString = "Hello World!";
	std::wstring testWideString = L"Hello World!";

	TEST_VALIDATE(Utils::StringToWideString(testString) == testWideString, true);
	TEST_VALIDATE(Utils::WideStringToString(testWideString) == testString, true);
}
TEST_FUNCTION_END_MODULE(Utils, String_Conversion)
#pragma endregion