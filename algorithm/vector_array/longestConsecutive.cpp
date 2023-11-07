#include <vector>
#include <unordered_map>
#include <iostream>
/**
 * 了解了。既然我们目前还在第一天，那么困难难度的题目将仍旧是关于数组的。以下是一个困难难度的数组题目：

 * **题目描述**:
 * 给定一个未排序的整数数组，找出最长连续序列的长度。要求算法的时间复杂度为 O(n)。
 *
 * **示例**:
 * 输入: [100, 4, 200, 1, 3, 2]
 * 输出: 4
 * 解释: 最长的连续元素序列是 [1, 2, 3, 4]。因此它的长度为 4。
 *
 * 这个问题要求在线性时间复杂度内解决，这意味着不能简单地对数组排序。一个可能的解法是使用哈希表来记录数组中每个元素是否已经被访问过，然后通过迭代数组中的每个元素，去探索它可能存在的连续序列，并更新最大长度。尝试这个问题，并让我知道你的进展。如果有需要，我可以提供更详细的算法描述和代码。
 * @param nums
 * @return
 */
// Function to find the longest consecutive sequence
int longestConsecutive(std::vector<int>& nums) {
    std::unordered_map<int, bool> num_presence;
    // Mark all numbers as not visited
    for (int num : nums) {
        num_presence[num] = false;
    }

    int longest_streak = 0;

    // Iterate through all numbers
    for (int num : nums) {
        // If the number is not visited
        if (!num_presence[num]) {
            num_presence[num] = true;
            int current_streak = 1;

            // Check for the consecutive numbers on the right
            for (int current_num = num + 1; num_presence.find(current_num) != num_presence.end(); ++current_num) {
                num_presence[current_num] = true;
                current_streak++;
            }

            // Check for the consecutive numbers on the left
            for (int current_num = num - 1; num_presence.find(current_num) != num_presence.end(); --current_num) {
                num_presence[current_num] = true;
                current_streak++;
            }

            // Update the longest streak found
            longest_streak = std::max(longest_streak, current_streak);
        }
    }

    return longest_streak;
}

// Example usage
int main() {
    std::vector<int> nums = {100, 4, 200, 1, 3, 2};
    std::cout << "The length of the longest consecutive sequence is " << longestConsecutive(nums) << std::endl;
    return 0;
}
