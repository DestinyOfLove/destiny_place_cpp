#include <iostream>
#include <vector>

using namespace std;

int singleNonDuplicate(vector<int> &nums) {
    int left = 0, right = nums.size() - 1;
    /*
     * 1. 满足条件的下界;
     * 2. return left
     */
    while (left <= right) {
        int mid = left + (right - left) / 2;
        // 确保mid是偶数，如果为奇数则减一
        if (mid % 2 == 1) {
            mid--;
        }
        // 判断唯一元素在mid的左侧还是右侧
        if (nums[mid] == nums[mid + 1]) {
            left = mid + 2;  // 唯一元素在右侧
        } else {
            right = mid - 1;  // 唯一元素在左侧
        }
    }
    return nums[left];
}

int main() {
    vector<int> nums1 = {1, 1, 2, 3, 3, 4, 4, 8, 8};
    cout << "Test Case 1: " << singleNonDuplicate(nums1) << endl; // 应输出 2

    vector<int> nums2 = {3, 3, 7, 7, 10, 11, 11};
    cout << "Test Case 2: " << singleNonDuplicate(nums2) << endl; // 应输出 10

    return 0;
}
