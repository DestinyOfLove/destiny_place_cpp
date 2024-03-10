#include <iostream>
#include <vector>
#include <algorithm> // 为了使用max函数

using namespace std;

/**
 * 如果当前的和<0，则没有必要再使用已有的数组了。与之后的值相加的和一定是更小的
 * @param nums
 * @return
 */

int maxSubArray(vector<int> &nums) {
    int max_sum = nums[0];
    int current_sum = nums[0];

    for (int i = 1; i < nums.size(); ++i) {
        current_sum = max(nums[i], current_sum + nums[i]);
        max_sum = max(max_sum, current_sum);
    }

    return max_sum;
}

int main() {
    vector<int> nums1 = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    cout << "Test Case 1: " << maxSubArray(nums1) << endl; // 应输出 6

    vector<int> nums2 = {1};
    cout << "Test Case 2: " << maxSubArray(nums2) << endl; // 应输出 1

    vector<int> nums3 = {5, 4, -1, 7, 8};
    cout << "Test Case 3: " << maxSubArray(nums3) << endl; // 应输出 23

    return 0;
}
