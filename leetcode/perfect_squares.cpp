#include <iostream>
#include <vector>
#include <queue>

using namespace std;

class Solution {
public:
    int numSquares(int n) {
        if (n <= 0) return 0;

        // 用于记录某个节点是否被访问过
        vector<bool> visited(n + 1, false);
        queue<int> q;
        q.push(0); // 从节点 0 开始
        int path_len = 0; // 记录从 0 到 n 的路径长度

        while (!q.empty()) {
            int size = q.size();
            path_len++;
            for (int i = 0; i < size; ++i) {
                int current_path_sum = q.front();
                q.pop();
                // 尝试所有可能的完全平方数
                for (int num = 1; num * num <= n; ++num) {
                    int path_sum = current_path_sum + num * num;
                    if (path_sum == n) {
                        // 找到 n
                        return path_len;
                    }
                    if (path_sum > n) {
                        // 超过 n 跳过
                        break;
                    }
                    if (!visited[path_sum]) {
                        // 未访问过，加入 BFS 队列
                        visited[path_sum] = true;
                        q.push(path_sum);
                    }
                }
            }
        }
        return path_len;
    }
};

int main() {
    Solution solver;
    int n = 4;
    cout << "The least number of perfect squares for " << n << " is: " << solver.numSquares(n) << endl;
    // Expected Output: The least number of perfect squares for 12 is: 3 (4 + 4 + 4)
    return 0;
}
