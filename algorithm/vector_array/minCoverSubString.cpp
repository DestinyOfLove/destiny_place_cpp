/**
 * * 了解您的需求。下面是一个难度较高的算法问题替换题目三：
 *
 * **题目3: 最小覆盖子串** - 给你一个字符串 `s` 和一个字符集合 `t`，请你找出 `s` 中的最小子串，使得这个子串包含 `t` 中所有字符。如果 `s` 中不存在这样的子串，则返回空字符串 `""`。如果存在这样的子串，我们保证它是唯一的答案。
 *
 * 这个问题是一个典型的滑动窗口问题，需要巧妙地运用双指针技术来减少不必要的搜索，是一个难度较大的问题。您可以开始尝试解决这个问题。如果您需要帮助，我可以为您提供解题的提示或者思路。
 */

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <climits>
#include <iostream>

using namespace std;

// 函数minWindow用于寻找字符串s中包含字符集合t的所有字符的最小子串
string minWindow(string s, unordered_set<char>& t) {
    // need存储t中字符及其出现的次数，window存储当前窗口中相应字符的计数
    unordered_map<char, int> window;

    int left = 0, right = 0; // 双指针，表示当前窗口的左右边界
    int valid = 0; // valid变量表示窗口中满足t字符集的字符数量
    int start = 0, len = INT_MAX; // start和len用来记录最小子串的起始索引和长度

    while (right < s.size()) { // 开始滑动窗口
        char c = s[right]; // c是即将移入窗口的字符
        right++; // 右移窗口

        // 进行窗口内数据的一系列更新
        if (t.count(c)) {
            window[c]++;
            // 只有当窗口中的某个字符数量第一次达到1时，才算一个有效字符
            if (window[c] == 1) {
                valid++;
            }
        }

        // 判断左侧窗口是否要收缩
        while (valid == t.size()) {
            // 更新最小覆盖子串
            if (right - left < len) {
                start = left;
                len = right - left;
            }

            // d是即将移出窗口的字符
            char d = s[left];
            left++; // 左移窗口

            // 进行窗口内数据的一系列更新
            if (t.count(d)) {
                window[d]--;
                // 只有当窗口中的某个字符数量减少到0时，才算失去一个有效字符
                if (window[d] == 0) {
                    valid--;
                }
            }
        }
    }

    // 返回最小覆盖子串
    return len == INT_MAX ? "" : s.substr(start, len);
}

int main() {
    string s = "ADOBECODEBANC";
    unordered_set<char> t;
    t.insert('A');
    t.insert('B');
    t.insert('C');
    string result = minWindow(s, t);
    cout << "The minimum window substring is: " << result << endl;
    return 0;
}
