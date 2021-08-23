#include <iostream>
#include <ctime>
#define n 20
#define m 30
using namespace std;

double maxMin(double** a)
{
    double min = 0.0, maxMin = 0.0;
#pragma omp parallel for shedule(auto)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            a[i][j] = rand() % 100 + 1;
            if (j == 0 || a[i][j] < min) {
                min = a[i][j];
            }
            cout << a[i][j] << " ";
        }
        if (i == 0 || min > maxMin) {
            maxMin = min;
        }
    }
    return maxMin;
}

int main()
{
    srand((int)time(0));

    double** a = new double*[n];
    for (int i = 0; i < n; i++) {
        a[i] = new double[m];
    }

    cout << endl
         << "MAXmin=" << maxMin(a) << "\n";

    for (int i = 0; i < n; i++) {
        delete[] a[i];
    }
    delete[] a;
    return 0;
}
