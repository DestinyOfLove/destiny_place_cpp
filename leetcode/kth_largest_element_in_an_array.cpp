/**
* 题目描述，数组中最大的k个数
*/

#include <iostream>
#include <vector>
#include <queue>

/**
 * 优先队列
 * 时间复杂度O(nlogk)
 */
class SolutionMinHeap {
public:
    static int findKthLargest(std::vector<int> &nums, int k) {
        std::priority_queue<int, std::vector<int>, std::greater<int>> q;
        for (int num: nums) {
            q.push(num);
            if (q.size() > k) {
                q.pop();
            }
        }
        return q.top();
    }
};


/**
 * 排序
 * 时间复杂度O(nlogn)
 */
class SolutionSort {
    public:
        static int findKthLargest(std::vector<int> &nums, int k) {
            std::sort(nums.begin(), nums.end(), std::greater<int>());
            return nums[k - 1];
        }
};


/**
 * 快速选择
 * 时间复杂度O(n)
 */
class SolutionNthElement {
    public:
        static int findKthLargest(std::vector<int> &nums, int k) {
            std::nth_element(nums.begin(), nums.begin() + k - 1, nums.end(), std::greater<int>());
            return nums[k - 1];
        }
};

int main() {
    std::vector<int> nums = {3, 2, 1, 5, 6, 4};
    int k = 2;
    std::cout << SolutionMinHeap::findKthLargest(nums, k) << std::endl;
    std::cout << SolutionSort::findKthLargest(nums, k) << std::endl;
    std::cout << SolutionNthElement::findKthLargest(nums, k) << std::endl;
}