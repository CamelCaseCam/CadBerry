#pragma once
#include <cdbpch.h>

namespace CDB
{
	template<typename T>
	void swap(T* a, T* b)
	{
		T aCopy = *a;
		*a = *b;
		*b = aCopy;
	}

	template<typename T>
	int partition(std::vector<T>& Vector, int low, int high)
	{
		int pivot = Vector[high];
		int i = low - 1;

		for (int j = low; j <= high - 1; ++j)
		{
			if (Vector[j] <= pivot)
			{
				++i;
				swap(&Vector[i], &Vector[j]);
			}
		}
		swap(&Vector[i + 1], &Vector[high]);
		return i + 1;
	}

	template<typename T>
	void Quicksort(std::vector<T>& Vector) { Quicksort<T>(Vector, 0, Vector.size()); }

	template<typename T>
	void Quicksort(std::vector<T>& Vector, int low, int high)
	{
		if (low < high)
		{
			int pi = partition<T>(Vector, low, high);

			Quicksort<T>(Vector, low, pi - 1);
			Quicksort<T>(Vector, pi + 1, high);
		}
	}
}