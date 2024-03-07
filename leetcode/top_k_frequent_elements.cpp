#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <functional>

class Solution {
public:
    static std::vector<int> topKFrequent(std::vector<int> &nums, int k) {
        // 用哈希表统计每个元素出现的次数
        std::unordered_map<int, int> num2freq;
        for (int num: nums) {
            ++num2freq[num];
        }

        // 定义一个最小堆来存储频率及其对应的元素
        // 堆中的比较是基于频率的
        auto order_by_freq = [&num2freq](int lft_num, int rht_num) {
            return num2freq[lft_num] > num2freq[rht_num];
        };
//        std::priority_queue<int, std::vector<int>, std::function<bool(int, int)> > pq(comp_function);
        std::priority_queue<int, std::vector<int>, decltype(order_by_freq)> pq(order_by_freq);              /* 小根堆，最小的在上面 */

        // 维护一个大小为 k 的最小堆
        for (auto &entry: num2freq) {
            pq.push(entry.first);
            if (pq.size() > k) {
                pq.pop();
            }
        }

        // 从最小堆中取出所有元素
        std::vector<int> topK;
        while (!pq.empty()) {
            topK.push_back(pq.top());
            pq.pop();
        }

        return topK;
    }
};

// 主函数
int main() {
    std::vector<int> nums = {1, 1, 1, 2, 2, 3};
    int k = 2;
    Solution solution;
    std::vector<int> result = solution.topKFrequent(nums, k);

    std::cout << "Top " << k << " frequent elements are: ";
    for (int num: result) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    return 0;
}
