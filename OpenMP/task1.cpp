#include <iostream>
#include <math.h>
#define eps 0.0000001
using namespace std;

double f(double x)
{
    return 4.0 / (1 + x * x);
}

double parallelProgram2(double a, double b)
{

    double I = eps + 1, I1 = 0;
#pragma omp parallel for shared(a, b, h, n) reduction(+ : Integral1)
    for (int N = 2; (N <= 4) || (fabs(I1 - I) > eps); N *= 2) {
        double h, sum2 = 0, sum4 = 0, sum = 0;
        h = (b - a) / (2 * N);
        for (int i = 1; i <= 2 * N - 1; i += 2) {
            sum4 += f(a + h * i);
            sum2 += f(a + h * (i + 1));
        }
        sum = f(a) + 4 * sum4 + 2 * sum2 - f(b);
        I = I1;
        I1 = (h / 3) * sum;
    }
    return I1;
}

double parallelProgram1(double a, double b)
{

    double I = eps + 1, I1 = 0;
#pragma omp parallel for 
    for (int N = 2; (N <= 4) || (fabs(I1 - I) > eps); N *= 2) {
        double h, sum2 = 0, sum4 = 0, sum = 0;
        h = (b - a) / (2 * N);
        for (int i = 1; i <= 2 * N - 1; i += 2) {
            sum4 += f(a + h * i);
            sum2 += f(a + h * (i + 1));
        }
        sum = f(a) + 4 * sum4 + 2 * sum2 - f(b);
        I = I1;
        I1 = (h / 3) * sum;
    }
    return I1;
}

double sequentVersion(double a, double b)
{

    double I = eps + 1, I1 = 0;
    for (int N = 2; (N <= 4) || (fabs(I1 - I) > eps); N *= 2) {
        double h, sum2 = 0, sum4 = 0, sum = 0;
        h = (b - a) / (2 * N);
        for (int i = 1; i <= 2 * N - 1; i += 2) {
            sum4 += f(a + h * i);
            sum2 += f(a + h * (i + 1));
        }
        sum = f(a) + 4 * sum4 + 2 * sum2 - f(b);
        I = I1;
        I1 = (h / 3) * sum;
    }
    return I1;
}

int main()
{
    double a = 0, b = 1;

    cout << "sequential program: " << sequentVersion(a, b) << endl
        << "first parallel version: " << parallelProgram1(a, b) << endl
        << "second parallel version: " << parallelProgram2(a, b) << endl;

    return 0;
}
