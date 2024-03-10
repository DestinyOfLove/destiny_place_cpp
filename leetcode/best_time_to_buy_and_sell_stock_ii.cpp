#include <iostream>
#include <vector>

using namespace std;

/**
 * 思路：
 * 此问题可以用贪心算法来解决。因为允许多次交易，我们可以在每一次股票价格上升期间买入并在之后卖出，从而获得利润。我们不需要关心每次交易的具体时间点，只需要关心价格的变化趋势。
 * 贪心算法的关键点在于，只要今天的价格比昨天高，我们就假设今天卖出，在昨天买入，这样可以收集到所有的正利润。这样做的原因是，假设股票在连续几天上涨，那么最终的最大利润等于每天的利润之和。
 */
int maxProfit(vector<int> &prices) {
    int max_profit = 0;
    for (int i = 1; i < prices.size(); ++i) {
        if (prices[i] > prices[i - 1]) {
            max_profit += prices[i] - prices[i - 1];  // 只要今天的价格比昨天高，就计算利润
        }
    }
    return max_profit;
}

int main() {
    vector<int> prices1 = {7, 1, 5, 3, 6, 4};
    cout << "Test Case 1: " << maxProfit(prices1) << endl; // 应输出 7

    vector<int> prices2 = {1, 2, 3, 4, 5};
    cout << "Test Case 2: " << maxProfit(prices2) << endl; // 应输出 4

    vector<int> prices3 = {7, 6, 4, 3, 1};
    cout << "Test Case 3: " << maxProfit(prices3) << endl; // 应输出 0

    return 0;
}
