#include <iostream>
#include <vector>

class Solution {
public:
    int findMin(std::vector<int> &nums) {
        if (nums.size() == 1) {
            return nums[0];
        }
        int left = 0;
        int max_right_num = nums[nums.size() - 1];
        int right = nums.size() - 1;
        /*
         * 1. 找下界，返回left；
         * 2. 注意是否添加==判断。如果不添加，则会走left = mid + 1，意味着当left == mid == right（找到目标值）时，left会改变，而返回的值正是nums[left]。导致答案错误
         */
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (nums[mid] <= max_right_num) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        return nums[left];
    }
};

int main() {
    Solution solution;
    std::vector<int> nums = {2, 1};
    int min = solution.findMin(nums);
    std::cout << "The minimum number in the array is: " << min << std::endl;
    return 0;
}
