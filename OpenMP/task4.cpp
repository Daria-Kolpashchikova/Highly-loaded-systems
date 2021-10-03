#include "omp.h"
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

using namespace std;

int countFreq(string& pat, string& txt)
{
    int M = pat.length();
    int N = txt.length();
    int res = 0;
    double real_time = 0;
    auto t_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i <= N - M; i++)
    {
        int j;
        for (j = 0; j < M; j++)
            if (txt[i + j] != pat[j])
                break;
        if (j == M)
        {
            res++;
            j = 0;
        }
    }
    auto t_end = std::chrono::high_resolution_clock::now();

    real_time = std::chrono::duration<double, std::milli>(t_end - t_start).count();

    std::cout << "sequential program time: " << real_time << " ms" << std::endl;
    return res;
}

int countFreqPar(string& pat, string& txt, int threads)
{
    int M = pat.length();
    int N = txt.length();
    int res = 0;

    omp_set_num_threads(threads);
    double real_time;
    auto t_start = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
    for (int i = 0; i <= N - M; i++)
    {
        int j;
        for (j = 0; j < M; j++)
            if (txt[i + j] != pat[j])
                break;
        if (j == M)
        {
            res++;
            j = 0;
        }
    }
    auto t_end = std::chrono::high_resolution_clock::now();
    real_time = std::chrono::duration<double, std::milli>(t_end - t_start).count();
    cout << "parallel program time: " << real_time << " ms" << std::endl;
    cout << "number of threads: " << threads << std::endl;
    return res;
}

string readFile(string path) {
    std::string line;

    std::ifstream in(path); 
    if (in.is_open())
    {
        while (getline(in, line))
        {
        }
    }
    in.close();

    return line;
}

int main() {
    string path, pattern;

    cout << "enter path to file: ";
    cin >> path;
    string txt = readFile(path);

    cout << "enter pattern: ";
    cin >> pattern;
    cout << "result of the sequent programm: " << countFreq(pattern, txt) << endl << endl;
    cout << "result of the parallel programm: " << countFreqPar(pattern, txt, 4);
}
