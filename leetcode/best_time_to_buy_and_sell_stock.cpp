#include <iostream>
#include <vector>
#include <climits> // 包含 INT_MAX

using namespace std;

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
