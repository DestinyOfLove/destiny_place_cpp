
#include <iostream>
#include <vector>

using namespace std;

bool canPlaceFlowers(vector<int> &flowerbed, int n) {
    std::size_t len = flowerbed.size();
    for (int i = 0; i < len && n > 0; ++i) {
        if (flowerbed[i] == 0) {
            int prev = (i == 0) ? 0 : flowerbed[i - 1];
            int next = (i == len - 1) ? 0 : flowerbed[i + 1];
            if (prev == 0 && next == 0) {
                flowerbed[i] = 1;
                --n;
            }
        }
    }
    return n <= 0;
}

int main() {
    vector<int> flowerbed1 = {1, 0, 0, 0, 1};
    int n1 = 1;
    cout << "Test Case 1: " << (canPlaceFlowers(flowerbed1, n1) ? "True" : "False") << endl;

    vector<int> flowerbed2 = {1, 0, 0, 0, 1};
    int n2 = 2;
    cout << "Test Case 2: " << (canPlaceFlowers(flowerbed2, n2) ? "True" : "False") << endl;

    return 0;
}
