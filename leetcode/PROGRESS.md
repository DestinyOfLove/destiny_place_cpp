# LeetCode 练习进度跟踪

## 📊 总体统计

- **已完成总数**: 2/65
- **当前阶段**: 第一阶段 - 基础数据结构补强
- **目标**: 系统化掌握主要算法模式

## 🎯 阶段进度

### 第一阶段：基础数据结构补强 (2/20)

#### 链表专题 (2/6)
- [x] 206. Reverse Linked List (Easy)
- [x] 21. Merge Two Sorted Lists (Easy) 
- [ ] 141. Linked List Cycle (Easy)
- [ ] 19. Remove Nth Node From End (Medium)
- [ ] 2. Add Two Numbers (Medium)
- [ ] 23. Merge k Sorted Lists (Hard)

#### 二叉树基础 (0/8)
- [ ] 144. Binary Tree Preorder Traversal (Easy)
- [ ] 94. Binary Tree Inorder Traversal (Easy)
- [ ] 145. Binary Tree Postorder Traversal (Easy)
- [ ] 102. Binary Tree Level Order Traversal (Medium)
- [ ] 104. Maximum Depth of Binary Tree (Easy)
- [ ] 226. Invert Binary Tree (Easy)
- [ ] 98. Validate Binary Search Tree (Medium)
- [ ] 236. Lowest Common Ancestor (Medium)

#### 栈和队列 (0/6)
- [ ] 20. Valid Parentheses (Easy)
- [ ] 155. Min Stack (Easy)
- [ ] 232. Implement Queue using Stacks (Easy)
- [ ] 394. Decode String (Medium)
- [ ] 739. Daily Temperatures (Medium)
- [ ] 84. Largest Rectangle in Histogram (Hard)

### 第二阶段：核心算法模式 (0/25)

#### 滑动窗口 (0/6)
- [ ] 3. Longest Substring Without Repeating Characters (Medium)
- [ ] 76. Minimum Window Substring (Hard)
- [ ] 438. Find All Anagrams in a String (Medium)
- [ ] 567. Permutation in String (Medium)
- [ ] 209. Minimum Size Subarray Sum (Medium)
- [ ] 424. Longest Repeating Character Replacement (Medium)

#### 回溯算法 (0/7)
- [ ] 46. Permutations (Medium)
- [ ] 78. Subsets (Medium)
- [ ] 39. Combination Sum (Medium)
- [ ] 22. Generate Parentheses (Medium)
- [ ] 79. Word Search (Medium)
- [ ] 51. N-Queens (Hard)
- [ ] 37. Sudoku Solver (Hard)

#### 动态规划扩展 (0/12)
- [ ] 70. Climbing Stairs (Easy)
- [ ] 198. House Robber (Medium)
- [ ] 322. Coin Change (Medium)
- [ ] 300. Longest Increasing Subsequence (Medium)
- [ ] 1143. Longest Common Subsequence (Medium)
- [ ] 72. Edit Distance (Hard)
- [ ] 152. Maximum Product Subarray (Medium)
- [ ] 121. Best Time to Buy and Sell Stock (Easy)
- [ ] 309. Best Time to Buy and Sell Stock with Cooldown (Medium)
- [ ] 416. Partition Equal Subset Sum (Medium)
- [ ] 1049. Last Stone Weight II (Medium)
- [ ] 312. Burst Balloons (Hard)

### 第三阶段：高级算法与优化 (0/20)

#### 图算法进阶 (0/8)
- [ ] 207. Course Schedule (Medium)
- [ ] 210. Course Schedule II (Medium)
- [ ] 133. Clone Graph (Medium)
- [ ] 695. Max Area of Island (Medium)
- [ ] 547. Number of Provinces (Medium)
- [ ] 787. Cheapest Flights Within K Stops (Medium)
- [ ] 743. Network Delay Time (Medium)
- [ ] 269. Alien Dictionary (Hard)

#### 高级数据结构 (0/7)
- [ ] 208. Implement Trie (Medium)
- [ ] 211. Design Add and Search Words Data Structure (Medium)
- [ ] 295. Find Median from Data Stream (Hard)
- [ ] 146. LRU Cache (Medium)
- [ ] 460. LFU Cache (Hard)
- [ ] 355. Design Twitter (Medium)
- [ ] 297. Serialize and Deserialize Binary Tree (Hard)

#### 数学与位操作 (0/5)
- [ ] 191. Number of 1 Bits (Easy)
- [ ] 338. Counting Bits (Easy)
- [ ] 136. Single Number (Easy)
- [ ] 371. Sum of Two Integers (Medium)
- [ ] 287. Find the Duplicate Number (Medium)

## 🏆 已完成题目 (2)

### 链表专题 (2)
- [x] 206. Reverse Linked List (l206_reverse_linked_list.cpp)
- [x] 21. Merge Two Sorted Lists (l21_merge_two_sorted_lists.cpp)

## 📝 练习日志

### 最近完成
<!-- 在这里记录每天的练习情况 -->

#### 2025-08-17
- ✅ 206. Reverse Linked List - 使用三指针技巧迭代反转
- ✅ 21. Merge Two Sorted Lists - 使用 dummy head 技巧合并有序链表

### 遇到的难点和总结
<!-- 记录学习过程中的思考和总结 -->

#### 📌 206. Reverse Linked List
**实现要点**：
- 使用三指针（prev, curr, next）迭代反转
- 时间复杂度 O(n)，空间复杂度 O(1)
- 无错误，一次通过 ✅

#### 📌 21. Merge Two Sorted Lists  
**初次实现的错误**：
1. **❌ 缺少 curr 指针前进**
   - 问题：在 while 循环中链接节点后，忘记更新 `curr = curr->next`
   - 后果：所有节点都链接到同一个位置，导致链表构建错误
   - 修正：每次链接后必须移动 curr 指针

2. **❌ 错误修改 dummy_head**
   - 问题：试图在循环中修改 dummy_head 指针（`dummy_head = curr`）
   - 原因：误解了 dummy head 的作用
   - 修正：dummy_head 应保持不变，最后返回 `dummy_head->next`

3. **❌ 无用的 is_first 标志**
   - 问题：添加了不必要的 is_first 标志和相关逻辑
   - 修正：使用 dummy head 后不需要特殊处理第一个节点

**关键学习点**：
- **Dummy Head 模式**：创建哨兵节点简化边界处理
- **正确的链表构建模式**：`curr->next = node; curr = curr->next;`
- **指针管理**：dummy_head 保持不变，curr 用于遍历

## 🎯 当前目标
- [x] 206. Reverse Linked List ✅
- [x] 21. Merge Two Sorted Lists ✅
- [ ] 下一题：141. Linked List Cycle