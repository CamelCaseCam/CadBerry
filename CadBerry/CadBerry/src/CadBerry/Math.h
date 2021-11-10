#pragma once

namespace CDB
{
	struct Vec2f
	{
		Vec2f(float X, float Y) : x(X), y(Y) {}
		float x, y;
	};

	struct Vec2i
	{
		Vec2i(int X, int Y) : x(X), y(Y) {}
		int x, y;
	};

	struct Vec2d
	{
		Vec2d(double X, double Y) : x(X), y(Y) {}
		double x, y;
	};

	template<typename T>
	struct Vec2
	{
		Vec2(T X, T Y) : x(X), y(Y) {}
		T x, y;
	};

	struct Vec3f
	{
		Vec3f(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
		float x, y, z;
	};

	struct Vec3i
	{
		Vec3i(int X, int Y, int Z) : x(X), y(Y), z(Z) {}
		int x, y, z;
	};

	struct Vec3d
	{
		Vec3d(double X, double Y, double Z) : x(X), y(Y), z(Z) {}
		double x, y, z;
	};

	template<typename T>
	struct Vec3
	{
		Vec3(T X, T Y, T Z) : x(X), y(Y), z(Z) {}
		T x, y, z;
	};
}