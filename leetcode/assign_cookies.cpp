
// 题目描述: 每个孩子有一个满足度（需求），每个饼干有一个大小，只有饼干的大小大于等于一个孩子的满足度，孩子才能获得满足。求解最多可以获得满足的孩子数量。
// 解决方案: 首先对孩子们的需求和饼干的大小进行排序，然后从小到大匹配，尽可能满足更多的孩子。

#include <vector>
#include <algorithm>
#include <iostream>

class Solution {
public:
    // 主要功能函数，找到最多能被满足的孩子数量
    int find_content_children(std::vector<int> &children_needs, std::vector<int> &cookies_sizes) {
        // 对需求和饼干大小进行排序
        std::sort(children_needs.begin(), children_needs.end());
        std::sort(cookies_sizes.begin(), cookies_sizes.end());

        // 初始化计数器和指针
        int satisfied_kids = 0;
        auto child_iter = children_needs.begin();
        auto cookie_iter = cookies_sizes.begin();

        // 匹配并更新计数器
        while (child_iter != children_needs.end() && cookie_iter != cookies_sizes.end()) {
            if (*cookie_iter >= *child_iter) { // 如果当前饼干能满足孩子的需求
                satisfied_kids++; // 增加已满足孩子数量
                child_iter++; // 移动孩子需求指针
            }
            cookie_iter++; // 总之，无论是否满足，都移动饼干指针尝试下一个
        }

        return satisfied_kids; // 返回结果
    }
};


int main() {
    std::vector<int> children_needs = {1, 2, 3};
    std::vector<int> cookies_sizes = {1, 100};
    Solution solution;
    int satisfied_kids = solution.find_content_children(children_needs, cookies_sizes);
    std::cout << satisfied_kids << std::endl;
}