#include <iostream>

// Assume that version 4 is the first bad version
bool isBadVersion(int version) {
    return version >= 4;
}

class Solution {
public:
    int firstBadVersion(int n) {
        int left = 1;
        int right = n;
        while (left < right) {
            int mid = left + (right - left) / 2;
            if (isBadVersion(mid)) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        return left;
    }
};

int main() {
    Solution solution;
    int n = 5;
    int first_bad = solution.firstBadVersion(n);
    std::cout << "The first bad version is: " << first_bad << std::endl;
    return 0;
}
