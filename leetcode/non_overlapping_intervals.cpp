
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

// 定义比较间隔结束时间的函数
bool compareEnds(const vector<int> &a, const vector<int> &b) {
    return a[1] < b[1];
}

class Solution {
public:
    // 重命名函数以更好地描述其功能
    int removeOverlappingIntervals(vector<vector<int>> &intervals) {
        // 检查输入是否为空
        if (intervals.empty()) return 0;

        // 对间隔按结束时间排序
        sort(intervals.begin(), intervals.end(), compareEnds);

        // 初始化计数器和当前非重叠间隔的结束时间
        int nonOverlapCount = 1;
        int lastNonOverlapEnd = intervals[0][1];

        // 遍历所有间隔
        for (int i = 1; i < intervals.size(); i++) {
            // 如果当前间隔开始于或晚于上一个非重叠间隔的结束，则是新的非重叠间隔
            if (intervals[i][0] >= lastNonOverlapEnd) {
                nonOverlapCount++;
                // 更新最后一个非重叠间隔的结束时间
                lastNonOverlapEnd = intervals[i][1];
            }
        }

        // 返回需要移除的重叠间隔数量
        return intervals.size() - nonOverlapCount;
    }
};

int main() {
    Solution s;
    vector<vector<int>> intervals = {{1, 2},
                                     {2, 3},
                                     {3, 4},
                                     {1, 3}};
    cout << s.removeOverlappingIntervals(intervals) << endl;

    return 0;
}
