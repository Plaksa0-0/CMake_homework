#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../src/Timer.h"
#include "../src/Sorting.h"

std::vector<int> GenerateIntVector(size_t size, int minVal, int maxVal)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(minVal, maxVal);

    std::vector<int> result(size);
    for (auto& val : result)
    {
        val = dist(gen);
    }

    return result;
}

std::vector<double> GenerateDoubleVector(size_t size, double minVal, double maxVal)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(minVal, maxVal);

    std::vector<double> result(size);
    for (auto& val : result)
    {
        val = dist(gen);
    }

    return result;
}

std::vector<std::string> GenerateStringVector(size_t size, int minStringSize, int maxStringSize)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> len_dist(minStringSize, maxStringSize);
    std::uniform_int_distribution<> char_dist(97, 122);

    std::vector<std::string> result(size);
    for (auto& str : result)
    {
        size_t len = len_dist(gen);
        str.resize(len);
        for (auto& c : str)
        {
            c = char_dist(gen);
        }
    }

    return result;
}

std::ofstream* g_resultsFile = nullptr;

class CTestCase
{
public:
    explicit CTestCase(const std::string& name, std::function<int()> test) :
        m_name(name),
        m_test(std::move(test))
    {
    }

    const std::string& GetName() const
    {
        return m_name;
    }

    int operator()() const
    {
        return m_test();
    }

private:
    const std::string m_name;
    const std::function<int()> m_test;
};


class CFixture
{
public:
    void AddTest(const CTestCase& test)
    {
        m_tests.emplace_back(std::move(test));
    }

    void Run()
    {
        std::ofstream resultsFile("results.txt");
        g_resultsFile = &resultsFile;

        const auto now = std::chrono::system_clock::now();
        const auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf{};
#ifdef _WIN32
        localtime_s(&tm_buf, &time);
#else
        localtime_r(&time, &tm_buf);
#endif
        resultsFile << "Benchmark run: " << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << "\n\n";

        std::cout << "Running " << m_tests.size() << " tests..." << std::endl;

        for (const auto& test : m_tests)
        {
            try
            {
                std::cout << "--------------------------------" << std::endl;
                std::cout << "Test " << test.GetName() << std::endl;
                std::cout << "--------------------------------" << std::endl;

                resultsFile << "Test: " << test.GetName() << "\n";

                const auto result = test();
                if (result == 0)
                {
                    m_passed++;
                    std::cout << "[PASS] Test " << test.GetName() << std::endl;
                }
                else
                {
                    m_failed++;
                    std::cout << "[FAIL] Test " << test.GetName() << ": " << result << std::endl;
                    resultsFile << "  Status: FAIL (" << result << ")\n";
                }

                resultsFile << "\n";
            }
            catch (const std::exception& e)
            {
                m_failed++;
                std::cout << "[FAIL] Test " << test.GetName() << ": " << e.what() << std::endl;
                resultsFile << "  Status: FAIL (" << e.what() << ")\n\n";
            }
        }

        std::cout << "Results: " << m_passed << " passed, " << m_failed << " failed\n";
        resultsFile << "Results: " << m_passed << " passed, " << m_failed << " failed\n";

        g_resultsFile = nullptr;
    }

private:
    std::vector<CTestCase> m_tests;
    int m_passed = 0;
    int m_failed = 0;
};

template<typename T, typename SortFn>
int RunSingleSort(const std::vector<T>& source, const char* name, SortFn sortFn)
{
    auto vec = source;
    NBenchmark::CTimer timer(name);
    sortFn(vec);
    const long long us = timer.GetElapsedMicroseconds();
    const bool ok = std::is_sorted(vec.begin(), vec.end());

    if (g_resultsFile)
    {
        *g_resultsFile << "  " << name << ": " << us << " us, " << (ok ? "PASS" : "FAIL") << "\n";
    }

    return ok ? 0 : 1;
}

template<typename T, typename Generator>
int RunSortBenchmark(Generator gen)
{
    const auto data = gen();
    int result = 0;

    result |= RunSingleSort<T>(data, "BubbleSortClassic", [](std::vector<T>& v) {
        NBenchmark::BubbleSortClassic(v);
    });
    result |= RunSingleSort<T>(data, "BubbleSortOptimized", [](std::vector<T>& v) {
        NBenchmark::BubbleSortOptimized(v);
    });
    result |= RunSingleSort<T>(data, "InsertionSort", [](std::vector<T>& v) {
        NBenchmark::InsertionSort(v);
    });
    result |= RunSingleSort<T>(data, "StdSort", [](std::vector<T>& v) {
        NBenchmark::StdSort(v);
    });

    return result;
}

int Test1()
{
    return RunSortBenchmark<int>([] { return GenerateIntVector(1000, 0, 1000); });
}

int Test2()
{
    return RunSortBenchmark<int>([] { return GenerateIntVector(5000, 0, 1000); });
}

int Test3()
{
    return RunSortBenchmark<int>([] { return GenerateIntVector(100, 10'000'000, 100'000'000); });
}

int Test4()
{
    return RunSortBenchmark<std::string>([] { return GenerateStringVector(100, 5, 15); });
}

int Test5()
{
    return RunSortBenchmark<std::string>([] { return GenerateStringVector(500, 3, 20); });
}

int Test6()
{
    return RunSortBenchmark<std::string>([] { return GenerateStringVector(50, 10, 50); });
}

int main()
{
    CFixture f;

    f.AddTest(CTestCase("Test1 (size: 1000, minVal: 0, maxVal: 1000)", Test1));
    f.AddTest(CTestCase("Test2 (size: 5000, minVal: 0, maxVal: 1000)", Test2));
    f.AddTest(CTestCase("Test3 (size: 100, minVal: 10'000'000, maxVal: 100'000'000)", Test3));
    f.AddTest(CTestCase("Test4 (string, size: 100, minLen: 5, maxLen: 15)", Test4));
    f.AddTest(CTestCase("Test5 (string, size: 500, minLen: 3, maxLen: 20)", Test5));
    f.AddTest(CTestCase("Test6 (string, size: 50, minLen: 10, maxLen: 50)", Test6));

    f.Run();

    return 0;
}
