#include <iostream>
#include <vector>

class Solution {
public:
    int findMin(std::vector<int> &nums) {
        int left = 0;
        int right = nums.size() - 1;
        while (left < right) {
            int mid = left + (right - left) / 2;
            if (nums[mid] > nums[right]) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        return nums[left];
    }
};

int main() {
    Solution solution;
    std::vector<int> nums = {4, 5, 6, 7, 0, 1, 2};
    int min = solution.findMin(nums);
    std::cout << "The minimum number in the array is: " << min << std::endl;
    return 0;
}
