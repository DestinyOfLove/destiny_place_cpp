#include <iostream>
#include <vector>

using namespace std;

/**
 * 遇到两个数字 nums[i-1] nums[i]，在出现递减的时候，有两种选择
 * 1. nums[i] = nums[i-1]
 * 2. nums[i-1] = nums[i]
 * 我们尽可能使用策略2，因为使用策略1的话，会将后面的数字提高，更加不利于保持非递减的性质。
 * 但是策略2将原先保证递减的子数列的最后一个数字变小了，存在破坏非递减性质的可能。因此需要看一下，如果nums[i-2] <= nums[i]的时候，才允许应用策略2。否则没办法只能策略1
 */

bool checkPossibility(vector<int> &nums) {
    bool modified = false;  // 记录是否已修改过元素
    for (int i = 1; i < nums.size(); ++i) {
        if (nums[i] < nums[i - 1]) {
            if (modified) {
                return false;  // 已经修改过一次了，无法再次修改
            }
            if (i == 1 || nums[i - 2] <= nums[i]) {
                nums[i - 1] = nums[i];  // 应用策略2
            } else {
                nums[i] = nums[i - 1];  // 应用策略1
            }
            modified = true;  // 记录已修改过元素
        }
    }
    return true;
}

int main() {
    vector<int> nums1 = {4, 2, 3};
    cout << "Test Case 1: " << (checkPossibility(nums1) ? "True" : "False") << endl; // 应输出 True

    vector<int> nums2 = {4, 2, 1};
    cout << "Test Case 2: " << (checkPossibility(nums2) ? "True" : "False") << endl; // 应输出 False

    return 0;
}
