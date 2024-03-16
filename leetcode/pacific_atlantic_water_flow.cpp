#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    vector<vector<int>> P, A, ans;
    int n, m;

    vector<vector<int>> pacificAtlantic(vector<vector<int>> &M) {
        if (M.empty() || M[0].empty()) return {};
        n = M.size(), m = M[0].size();
        P = A = vector<vector<int>>(n, vector<int>(m, 0));
        // 从太平洋和大西洋的边缘开始深度优先搜索
        for (int i = 0; i < n; ++i) {
            dfs(M, P, i, 0);
            dfs(M, A, i, m - 1);
        }
        for (int j = 0; j < m; ++j) {
            dfs(M, P, 0, j);
            dfs(M, A, n - 1, j);
        }
        return ans;
    }

    void dfs(vector<vector<int>> &M, vector<vector<int>> &visited, int i, int j) {
        if (visited[i][j]) return; // 如果已经访问过，则返回
        visited[i][j] = 1; // 标记为已访问
        // 如果该点可以从太平洋和大西洋到达，则加入结果集
        if (P[i][j] && A[i][j]) ans.push_back({i, j});
        // 向四个方向深度优先搜索
        if (i - 1 >= 0 && M[i - 1][j] >= M[i][j]) dfs(M, visited, i - 1, j);
        if (i + 1 < n && M[i + 1][j] >= M[i][j]) dfs(M, visited, i + 1, j);
        if (j - 1 >= 0 && M[i][j - 1] >= M[i][j]) dfs(M, visited, i, j - 1);
        if (j + 1 < m && M[i][j + 1] >= M[i][j]) dfs(M, visited, i, j + 1);
    }
};

int main() {
    Solution solver;
    vector<vector<int>> matrix = {
            {1, 2, 2, 3, 5},
            {3, 2, 3, 4, 4},
            {2, 4, 5, 3, 1},
            {6, 7, 1, 4, 5},
            {5, 1, 1, 2, 4}
    };
    vector<vector<int>> result = solver.pacificAtlantic(matrix);
    for (const auto &cell: result) {
        cout << "[" << cell[0] << "," << cell[1] << "] ";
    }
    cout << endl;
    return 0; // 预期输出结果为可以同时流向两个海洋的单元格坐标
}
