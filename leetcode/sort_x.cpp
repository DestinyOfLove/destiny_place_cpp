#include <iostream>

using namespace std;

int mySqrt(int x) {
    if (x == 0) return 0;

    int left = 1, right = x;
    while (left <= right) {
        long long mid = left + (right - left) / 2; // 使用 long long 防止溢出
        long long sq = mid * mid;
        if (sq == x) {
            return mid;
        } else if (sq < x) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return right; // 当 left > right 时，right 是最后一个满足 sq <= x 的数
}

int main() {
    cout << "Test Case 1: sqrt(4) = " << mySqrt(4) << endl; // 应输出 2
    cout << "Test Case 2: sqrt(8) = " << mySqrt(8) << endl; // 应输出 2，因为 2*2=4 < 8, 但 3*3=9 > 8
    cout << "Test Case 3: sqrt(1) = " << mySqrt(1) << endl; // 应输出 1
    return 0;
}
