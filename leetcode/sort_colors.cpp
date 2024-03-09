#include <iostream>
#include <vector>

using namespace std;

class Solution1 {
public:
    static void sortColors(vector<int> &nums) {
        int size = nums.size();
        if (size < 2) {
            return;
        }

        // all in [0, zero) = 0
        // all in [zero, i) = 1
        // all in [two, len - 1] = 2

        int zero = 0;
        int two = size;
        int i = 0;
        while (i < two) {
            if (nums[i] == 0) {
                swap(nums[zero], nums[i]);
                zero++;
                i++;
            } else if (nums[i] == 1) {
                i++;
            } else {
                two--;
                swap(nums[i], nums[two]);
            }
        }
    }
};

class Solution2 {
public:
    static void sortColors(vector<int> &nums) {
        int size = nums.size();
        if (size < 2) {
            return;
        }

        // all in [0, zero] = 0
        // all in (zero, i) = 1
        // all in (two, len - 1] = 2

        int zero = -1;
        int two = size - 1;
        int i = 0;

        while (i <= two) {
            if (nums[i] == 0) {
                zero++;
                swap(nums[zero], nums[i]);
                i++;
            } else if (nums[i] == 1) {
                i++;
            } else {
                swap(nums[i], nums[two]);
                two--;
            }
        }
    }
};

int main() {
    vector<int> nums = {2, 0, 2, 1, 1, 0};
    Solution1::sortColors(nums);
    for (auto num: nums) {
        cout << num << " ";
    }
    cout << endl;

    vector<int> nums2 = {2, 0, 2, 1, 1, 0};
    Solution2::sortColors(nums2);
    for (auto num: nums2) {
        cout << num << " ";
    }
    cout << endl;
}