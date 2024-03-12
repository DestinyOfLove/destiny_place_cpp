
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Solution {
public:
    vector<int> diffWaysToCompute(string input) {
        vector<int> ways;
        for (int i = 0; i < input.size(); i++) {
            char c = input[i];
            // 当字符是运算符时，执行分治
            if (c == '+' || c == '-' || c == '*') {
                // 分别计算左边和右边的结果集
                vector<int> left = diffWaysToCompute(input.substr(0, i));
                vector<int> right = diffWaysToCompute(input.substr(i + 1));
                // 结合左右结果集
                for (int l: left) {
                    for (int r: right) {
                        if (c == '+') {
                            ways.push_back(l + r);
                        } else if (c == '-') {
                            ways.push_back(l - r);
                        } else if (c == '*') {
                            ways.push_back(l * r);
                        }
                    }
                }
            }
        }
        // 处理全数字字符串的情况
        if (ways.empty()) {
            ways.push_back(stoi(input));
        }
        return ways;
    }
};

int main() {
    Solution solution;
    string expression = "2-1-1";
    vector<int> result = solution.diffWaysToCompute(expression);
    cout << "Possible results: ";
    for (int num: result) {
        cout << num << " ";
    }
    cout << endl;
    return 0;
}