#include <iostream>
#include <cmath>

class Solution {
public:
    static bool solve(int target) {
        int small_num = 0;
        int large_num = ceil(sqrt(target));
        while (small_num < large_num) {
            int square_sum = small_num * small_num + large_num * large_num;
            if (square_sum == target) {
                std::cout << small_num << "," << large_num << std::endl;
                return true;
            } else if (target < square_sum) {
                --large_num;
            } else {
                ++small_num;
            }

        }
        return false;
    }
};

int main() {
    int target = 36 + 9;

    bool has_solve = Solution::solve(target);
    std::cout << (has_solve ? "solve" : "not solve") << std::endl;
    return 0;
}