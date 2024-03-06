#include <iostream>

#include <vector>

class Solution {
public:
    static void solve(const std::vector<int> &array, int target) {
        /* 定义两个指针，前后遍历 */
        if (array.empty()) {
            return;
        }
        int lft_idx = 0;
        int rht_idx = static_cast<int>(array.size() - 1);

        while (lft_idx < rht_idx) {
            int sum = array[lft_idx] + array[rht_idx];
            if (sum == target) {
                std::cout << lft_idx << "," << rht_idx;
                return;
            } else if (sum > target) {
                --rht_idx;
            } else {
                ++lft_idx;
            }
        }
    }
};

int main() {
    std::vector<int> array = {1, 2, 3, 4, 77, 99};
    int target = 6;

    Solution::solve(array, target);
    return 0;
}