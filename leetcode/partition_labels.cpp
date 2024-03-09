#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

vector<int> partitionLabels(string s) {
    unordered_map<char, int> last_indices;
    vector<int> partition_lengths;

    // Step 1: 记录每个字母最后一次出现的位置
    for (int i = 0; i < s.size(); ++i) {
        last_indices[s[i]] = i;
    }

    // Step 2: 遍历字符串，确定片段长度
    int start = 0, end = 0;
    for (int i = 0; i < s.size(); ++i) {
        end = max(end, last_indices[s[i]]); // Step 3: 更新片段的最远边界
        if (i == end) {                     // Step 4: 达到片段的结束位置
            partition_lengths.push_back(i - start + 1);
            start = i + 1;
        }
    }

    return partition_lengths;
}

int main() {
    string s = "ababcbacadefegdehijhklij";
    vector<int> partitions = partitionLabels(s);

    for (int len: partitions) {
        cout << len << " ";
    }
    cout << endl;

    return 0;
}
