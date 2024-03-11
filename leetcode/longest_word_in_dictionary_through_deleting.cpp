#include <iostream>
#include <vector>
#include <string>

using namespace std;

string findLongestWord(string s, vector<string> &dictionary) {
    string longest;
    for (const string &word: dictionary) {
        int i = 0, j = 0;
        while (i < s.length() && j < word.length()) {
            if (s[i] == word[j]) {
                j++;
            }
            i++;
        }
        if (j == word.length()) {
            // 如果当前单词比已知的最长单词还长，或者长度相同但字典序更小，则更新最长单词
            if (word.length() > longest.length() || (word.length() == longest.length() && word < longest)) {
                longest = word;
            }
        }
    }
    return longest;
}

int main() {
    string s = "abpcplea";
    vector<string> dictionary = {"ale", "apple", "monkey", "plea"};
    cout << "The longest word: " << findLongestWord(s, dictionary) << endl; // 应输出 "apple"

    return 0;
}
