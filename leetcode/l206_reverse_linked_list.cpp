#include <iostream>
#include <vector>

/*
Problem: 206. Reverse Linked List

Given the head of a singly linked list, reverse the list and return the reversed list.

Example 1:
Input: head = [1,2,3,4,5]
Output: [5,4,3,2,1]

Example 2:
Input: head = [1,2]
Output: [2,1]

Example 3:
Input: head = []
Output: []

Constraints:
- The number of nodes in the list is the range [0, 5000].
- -5000 <= Node.val <= 5000

Follow up: A linked list can be reversed either iteratively or recursively. Could you implement both?
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
    ListNode* reverseList(ListNode* head) {
        ListNode* prev = nullptr;
        ListNode* curr = head;
        while (curr != nullptr) {
            // 保存下一个，然后局部反转
            ListNode* next = curr->next;
            curr->next = prev;
            prev = curr;
            curr = next;
        }
        return prev;
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
    while (head) {
        std::cout << head->val;
        if (head->next) std::cout << " -> ";
        head = head->next;
    }
    std::cout << std::endl;
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

    // Test Case 1: [1,2,3,4,5] -> [5,4,3,2,1]
    {
        std::vector<int> input = {1, 2, 3, 4, 5};
        ListNode* head = createList(input);

        std::cout << "Original: ";
        printList(head);

        ListNode* reversed = solution.reverseList(head);

        std::cout << "Reversed: ";
        printList(reversed);

        deleteList(reversed);
    }

    // Test Case 2: [1,2] -> [2,1]
    {
        std::vector<int> input = {1, 2};
        ListNode* head = createList(input);

        std::cout << "\nOriginal: ";
        printList(head);

        ListNode* reversed = solution.reverseList(head);

        std::cout << "Reversed: ";
        printList(reversed);

        deleteList(reversed);
    }

    // Test Case 3: [] -> []
    {
        ListNode* head = nullptr;

        std::cout << "\nOriginal: []" << std::endl;

        ListNode* reversed = solution.reverseList(head);

        std::cout << "Reversed: ";
        if (reversed == nullptr) {
            std::cout << "[]" << std::endl;
        } else {
            printList(reversed);
        }

        deleteList(reversed);
    }

    return 0;
}