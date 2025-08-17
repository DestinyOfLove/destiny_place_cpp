#include <iostream>
#include <vector>

/*
Problem: 21. Merge Two Sorted Lists

You are given the heads of two sorted linked lists list1 and list2.

Merge the two lists into one sorted list. The list should be made by splicing together
the nodes of the first two lists.

Return the head of the merged linked list.

Example 1:
Input: list1 = [1,2,4], list2 = [1,3,4]
Output: [1,1,2,3,4,4]

Example 2:
Input: list1 = [], list2 = []
Output: []

Example 3:
Input: list1 = [], list2 = [0]
Output: [0]

Constraints:
- The number of nodes in both lists is in the range [0, 50].
- -100 <= Node.val <= 100
- Both list1 and list2 are sorted in non-decreasing order.
*/

/**
 * Definition for singly-linked list.
 */
struct ListNode {
    int val;
    ListNode* next;

    ListNode() : val(0), next(nullptr) {}

    ListNode(int x) : val(x), next(nullptr) {}

    ListNode(int x, ListNode* next) : val(x), next(next) {}
};

class Solution {
public:
    ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
        // 双指针
        // 保存头

        ListNode* dummy_head = new ListNode(0);
        ListNode* curr = dummy_head;
        while (list1 != nullptr && list2 != nullptr) {
            // cmp v1 v2
            if (list1->val <= list2->val) {
                curr->next = list1;
                list1 = list1->next;
            } else {
                curr->next = list2;
                list2 = list2->next;
            }
            curr = curr->next;
        }
        if (list1 != nullptr) {
            curr->next = list1;
        }
        if (list2 != nullptr) {
            curr->next = list2;
        }

        return dummy_head->next;
    }
};

// Helper function to create linked list from vector
ListNode* createList(const std::vector<int>& vals) {
    if (vals.empty()) return nullptr;

    ListNode* head = new ListNode(vals[0]);
    ListNode* curr = head;
    for (int i = 1; i < vals.size(); i++) {
        curr->next = new ListNode(vals[i]);
        curr = curr->next;
    }
    return head;
}

// Helper function to print linked list
void printList(ListNode* head) {
    if (!head) {
        std::cout << "[]";
        return;
    }

    std::cout << "[";
    while (head) {
        std::cout << head->val;
        if (head->next) std::cout << ",";
        head = head->next;
    }
    std::cout << "]";
}

// Helper function to clean up memory
void deleteList(ListNode* head) {
    while (head) {
        ListNode* temp = head;
        head = head->next;
        delete temp;
    }
}

int main() {
    Solution solution;

    // Test Case 1: [1,2,4] + [1,3,4] = [1,1,2,3,4,4]
    {
        std::vector<int> input1 = {1, 2, 4};
        std::vector<int> input2 = {1, 3, 4};
        ListNode* list1 = createList(input1);
        ListNode* list2 = createList(input2);

        std::cout << "Test Case 1:" << std::endl;
        std::cout << "List1: ";
        printList(list1);
        std::cout << std::endl;
        std::cout << "List2: ";
        printList(list2);
        std::cout << std::endl;

        ListNode* merged = solution.mergeTwoLists(list1, list2);

        std::cout << "Merged: ";
        printList(merged);
        std::cout << std::endl << std::endl;

        deleteList(merged);
    }

    // Test Case 2: [] + [] = []
    {
        ListNode* list1 = nullptr;
        ListNode* list2 = nullptr;

        std::cout << "Test Case 2:" << std::endl;
        std::cout << "List1: ";
        printList(list1);
        std::cout << std::endl;
        std::cout << "List2: ";
        printList(list2);
        std::cout << std::endl;

        ListNode* merged = solution.mergeTwoLists(list1, list2);

        std::cout << "Merged: ";
        printList(merged);
        std::cout << std::endl << std::endl;

        deleteList(merged);
    }

    // Test Case 3: [] + [0] = [0]
    {
        std::vector<int> input2 = {0};
        ListNode* list1 = nullptr;
        ListNode* list2 = createList(input2);

        std::cout << "Test Case 3:" << std::endl;
        std::cout << "List1: ";
        printList(list1);
        std::cout << std::endl;
        std::cout << "List2: ";
        printList(list2);
        std::cout << std::endl;

        ListNode* merged = solution.mergeTwoLists(list1, list2);

        std::cout << "Merged: ";
        printList(merged);
        std::cout << std::endl;

        deleteList(merged);
    }

    return 0;
}