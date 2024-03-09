#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <queue>

class SolutionWithSort {
public:
// 对输入字符串s按照字符出现频率从高到低排序
    static std::string frequencySort(const std::string& str) {
        // 使用unordered_map统计每个字符的出现次数
        std::unordered_map<char, int> char2freq;
        for (char c: str) {
            char2freq[c]++;
        }

        // 将统计结果转换为vector，便于排序
        std::vector<std::pair<char, int>> char_sorted_by_freq(char2freq.begin(), char2freq.end());

        // 按照字符出现频率降序排列
        std::sort(char_sorted_by_freq.begin(), char_sorted_by_freq.end(),
                  [](const std::pair<char, int> &a, const std::pair<char, int> &b) {
                      return a.second > b.second;
                  });

        // 根据排序后的字符及其频率构建结果字符串
        std::string result;
        for (const auto &pair: char_sorted_by_freq) {
            for (int i = 0; i < pair.second; ++i) {
                result += pair.first;
            }
        }

        return result;
    }

};

class SolutionWithPriorityQueue {
public:
    static std::string frequencySort(const std::string& str) {
        // 使用unordered_map统计每个字符的出现次数
        std::unordered_map<char, int> char2freq;
        for (char c: str) {
            char2freq[c]++;
        }
        // 使用优先级队列，按照字符出现频率从高到低排序
        std::priority_queue<std::pair<char, int>, std::vector<std::pair<char, int>>, std::greater<std::pair<char, int>>> pq;
        for (const auto &pair: char2freq) {
            pq.emplace(pair);
        }
        // 根据排序后的字符及其频率构建结果字符串
        std::string result;
        while (!pq.empty()) {
            auto pair = pq.top();
            pq.pop();
            for (int i = 0; i < pair.second; ++i) {
                result += pair.first;
            }
        }
        return result;
    }
};

int main() {
    std::cout << SolutionWithSort::frequencySort("tree") << std::endl;
    std::cout << SolutionWithSort::frequencySort("cccaaa") << std::endl;

    std::cout << SolutionWithPriorityQueue::frequencySort("tree") << std::endl;
    std::cout << SolutionWithPriorityQueue::frequencySort("cccaaa") << std::endl;
}