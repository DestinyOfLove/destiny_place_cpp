#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class Solution {
public:
    static int findMinArrowShots(vector<vector<int>> &points) {
        if (points.empty()) return 0;

        // 对气球按结束坐标进行升序排序
        sort(points.begin(), points.end(), compareByEnd);

        int count = 1, end = points[0][1];
        for (size_t i = 1; i < points.size(); ++i) {
            if (points[i][0] > end) {
                ++count;
                end = points[i][1];
            }
        }
        return count;
    }

private:
    static bool compareByEnd(const vector<int> &a, const vector<int> &b) {
        return a[1] < b[1];
    }
};

int main() {
    vector<vector<int>> points = {
            {10, 16},
            {2,  8},
            {1,  6},
            {7,  12}
    };
    // 使用类名调用静态成员函数
    cout << Solution::findMinArrowShots(points) << endl;
    return 0;
}
