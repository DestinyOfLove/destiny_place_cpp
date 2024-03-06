#include <vector>
#include <iostream>

class SolutionWithNthElement {
public:
    static int findNthElement(std::vector<int> &nums, int n) {
        int left = 0, right = nums.size() - 1;
        // 调整 n 以适应 0-based 索引
        n--;

        while (left <= right) {
            int pivotIndex = partition(nums, left, right);
            if (pivotIndex == n) {
                // 找到第 n 个元素
                return nums[pivotIndex];
            } else if (pivotIndex < n) {
                // 继续在右侧查找
                left = pivotIndex + 1;
            } else {
                // 继续在左侧查找
                right = pivotIndex - 1;
            }
        }
        return -1; // 如果 n 超出范围，返回 -1
    }

private:
    // Lomuto 分区方案
    static int partition(std::vector<int> &nums, int left, int right) {
        int pivot = nums[right];
        int i = left;
        for (int j = left; j < right; j++) {
            if (nums[j] < pivot) {
                std::swap(nums[i], nums[j]);
                i++;
            }
        }
        std::swap(nums[i], nums[right]);
        return i;
    }
};

int main() {
    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 30, 44, 0};
    int n = 3;
    int res = SolutionWithNthElement::findNthElement(nums, n);
    std::cout << res << std::endl;
}