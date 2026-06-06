#pragma once

#include <algorithm>
#include <utility>
#include <vector>

namespace NBenchmark
{

template<typename T>
void BubbleSortClassic(std::vector<T>& arr)
{
    size_t n = arr.size();
    for (size_t i = 0; i < n - 1; ++i)
    {
        for (size_t j = 0; j < n - 1; ++j)
        {
            if (arr[j] > arr[j + 1])
            {
                std::swap(arr[j], arr[j + 1]);
            }
        }
    }
}

template<typename T>
void BubbleSortOptimized(std::vector<T>& arr)
{
    const size_t n = arr.size();
    for (size_t i = 0; i + 1 < n; ++i)
    {
        bool swapped = false;
        for (size_t j = 0; j + 1 < n - i; ++j)
        {
            if (arr[j] > arr[j + 1])
            {
                std::swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped)
        {
            break;
        }
    }
}

template<typename T>
void InsertionSort(std::vector<T>& arr)
{
    for (size_t i = 1; i < arr.size(); ++i)
    {
        T key = std::move(arr[i]);
        size_t j = i;
        while (j > 0 && arr[j - 1] > key)
        {
            arr[j] = std::move(arr[j - 1]);
            --j;
        }
        arr[j] = std::move(key);
    }
}

template<typename T>
void StdSort(std::vector<T>& arr)
{
    std::sort(arr.begin(), arr.end());
}

} // namespace NBenchmark
