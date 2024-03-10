#include <iostream>
#include <vector>
#include <climits> // 包含 INT_MAX

using namespace std;

/**
 * 双指针
 */
int maxProfitDoublePointer(vector<int> &prices) {
    int buy = 0; // 买入指针，表示买入时机的索引
    int sell = 1; // 卖出指针，表示卖出时机的索引
    int max_profit = 0; // 最大利润初始化为0

    while (sell < prices.size()) {
        if (prices[buy] < prices[sell]) {
            // 如果当前卖出价比买入价高，计算此次交易的利润，并更新最大利润
            int profit = prices[sell] - prices[buy];
            max_profit = max(max_profit, profit);
        } else {
            // 如果遇到更低的买入价，则更新买入指针
            buy = sell;
        }
        sell++; // 卖出指针始终向前移动
    }

    return max_profit;
}

/**
 * 直接遍历
 */
int maxProfit(vector<int> &prices) {
    int max_profit = 0;
    int min_price = INT_MAX;

    for (int price: prices) {
        min_price = min(min_price, price);
        max_profit = max(max_profit, price - min_price);
    }

    return max_profit;
}

int main() {
    vector<int> prices1 = {7, 1, 5, 3, 6, 4};
    cout << "Test Case 1: " << maxProfit(prices1) << endl; // 应输出 5

    vector<int> prices2 = {7, 6, 4, 3, 1};
    cout << "Test Case 2: " << maxProfit(prices2) << endl; // 应输出 0

    return 0;
}
