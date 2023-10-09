#include <functional>
#include <iostream>


using namespace std;

double f(double *x, int n, std::function<double(double)> &function) {
  if (n == 2)
    return (function(x[1]) - function(x[0])) / (x[1] - x[0]);

  return (f(&x[1], n - 1, function) - f(x, n - 1, function)) /
         (x[n - 1] - x[0]);
}

int main() {
  int n;
  cin >> n;

  vector<double> a(n+1);
  vector<double> b(n+1);
  vector<double> c(n+1);
  vector<double> f(n+1);

  for (int i = 2; i <= n; ++i) {
    cin >> a[i];
    a[i] *= -1;
  }
  for (int i = 1; i <= n-1; ++i) {
    cin >> b[i];
    b[i] *= -1;
  }

  for (int i = 1; i <= n; ++i) {
    cin >> c[i];
  }

  for (int i = 1; i <= n; ++i) {
    cin >> f[i];
  }

  vector<double> alpha(n+1);
  vector<double> beta(n+1);

  alpha[1] = b[1]/c[1];
  beta[1] = f[1]/c[1];

  for (int i = 2; i <= n; ++i) {
    alpha[i] = b[i] / (c[i] - a[i] * alpha[i-1]);
    beta[i] = (f[i] + a[i] * beta[i-1]) / (c[i] - a[i] * alpha[i-1]);
  }

  vector<double> x(n+1);
  x[n] = beta[n];
  for (int i = n-1; i >= 1; --i) {
    x[i] = alpha[i] * x[i+1] + beta[i];
  }

  for (int i = 1; i <= n; ++i) {
    cout << x[i] << " ";
  }
}
