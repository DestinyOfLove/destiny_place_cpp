# include <vector>
# include <iostream>

class Solution {
public:
    /**
     * 寻找目标值在给定有序数组中的有效范围，返回一个包含范围起始和结束位置的数组。
     * 如果目标值不存在于数组中，返回{-1, -1}。
     *
     * @param nums 一个有序整数数组
     * @param target 要查找的目标值
     * @return 一个包含范围起始和结束位置的数组，如{0, 2}表示目标值在数组中的范围是[0, 2]。
     */
    std::vector<int> searchRange(const std::vector<int> &nums, int target) {
        int leftIndex = binarySearch(nums, target, true);
        int rightIndex = binarySearch(nums, target, false);

        if (leftIndex <= rightIndex && rightIndex < nums.size() && nums[leftIndex] == nums[rightIndex]) {
            return {leftIndex, rightIndex};
        } else {
            return {-1, -1};
        }
    }

private:
    /**
     * 使用二分查找法找到目标值在有序数组中的位置。
     * 如果`isFirst`为true，返回第一个等于`target`的元素的索引。
     * 如果`isFirst`为false，返回最后一个等于`target`的元素的索引。
     *
     * @param nums 一个已排序的整数数组
     * @param target 要查找的目标值
     * @param isFirst 如果为true，查找第一个匹配项；否则，查找最后一个匹配项
     * @return 如果找到目标值，返回其索引；否则，返回插入点（不影响排序的插入位置）
     */
    static int binarySearch(const std::vector<int> &nums, int target, bool isFirst) {
        int left = 0;
        int right = nums.size() - 1;
        int middle;

        while (left <= right) {
            middle = left + (right - left) / 2;

            if (nums[middle] == target) {
                if (isFirst) {
                    // 如果isFirst为true，继续在左半边查找
                    right = middle - 1;
                } else {
                    // 如果isFirst为false，继续在右半边查找
                    left = middle + 1;
                }
            } else if (nums[middle] > target) {
                right = middle - 1;
            } else {
                left = middle + 1;
            }
        }

        return isFirst ? left : right;
    }
};

int main() {
    std::vector<int> nums = {5, 7, 7, 8, 8, 10};
    Solution solution;
    auto result = solution.searchRange(nums, 8);
    for (auto i: result) {
        std::cout << i << " ";
    }
}
