#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

int main(int argc, char **argv) {
    if (argc < 2) return 0;
    int N = atoi(argv[1]);
    vector<int> vector1(N, 1);
    vector<int> vector2(N, 1);
    int s = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            s += vector1[i] * vector2[j];

    cout << s << endl;

}
