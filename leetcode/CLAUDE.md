# CLAUDE.md - LeetCode 练习指南

本文档为 Claude Code 提供 LeetCode 练习的标准化流程和规范。

## 📋 标准化练习流程

### 概览
每道题目遵循 5 个阶段的标准流程：准备 → 实现 → 测试 → Review → 记录

---

## 阶段 1：准备阶段 📝

### 目标
生成题目模板，帮助用户理解问题

### Claude 职责
1. **创建题目文件**
   - 命名格式：`l<题号>_<题目名>.cpp`
   - 示例：`l206_reverse_linked_list.cpp`

2. **文件结构**
   ```cpp
   // 题目描述（作为注释）
   // 示例输入输出
   // 约束条件
   
   struct ListNode { ... };  // 必要的数据结构
   
   class Solution {
   public:
       ReturnType functionName(Parameters) {
           // TODO(human): Implement the solution here
       }
   };
   
   // 辅助函数（创建测试数据、打印结果、内存清理）
   // main函数包含所有测试用例
   ```

3. **提供学习引导**
   - 使用 "Learn by Doing" 格式
   - Context: 题目背景和与前题的关联
   - Your Task: 明确指出需要实现的函数
   - Guidance: 提供思路提示，不给出答案

---

## 阶段 2：实现阶段 💻

### 用户职责
- 独立完成代码实现
- 处理边界条件

### Claude 职责
- 等待用户完成
- 不主动提供答案
- 用户卡住时给予适当提示

---

## 阶段 3：测试与调试阶段 🧪

### 编译运行方式

#### 方式 1：CLion（推荐）
- 直接点击运行按钮
- 可执行文件生成在 `cmake-build-debug/leetcode/`

#### 方式 2：命令行编译
```bash
cd leetcode
g++ -std=c++11 -o l<题号>_<题目名> l<题号>_<题目名>.cpp
./l<题号>_<题目名>
```

#### 方式 3：CMake 构建
```bash
cd cmake-build-debug
cmake ..
make l<题号>_<题目名>
./leetcode/l<题号>_<题目名>
```

### Claude 职责
- 帮助解释编译错误
- 不直接指出逻辑错误
- 引导用户自己发现问题

---

## 阶段 4：Review 与学习阶段 🔍

### 触发条件
用户请求："review 代码" 或 "看看问题在哪里"

### Claude Review 模板

```markdown
## 🔍 代码问题分析

### 问题1：[问题名称] ❌
**位置**: 第X行
**错误代码**：
\```cpp
// 错误的代码片段
\```
**问题说明**：为什么这是错误的
**修正方法**：应该如何修改

### 问题2：...

## ✅ 正确的实现
[如果用户已修正，确认正确性]

## 💡 关键学习点
- 核心概念或模式
- 这个错误的常见场景
- 如何避免类似错误
```

### 学习引导原则
- 详细解释错误原因
- 提供正确的思维模式
- 关联到通用编程模式

---

## 阶段 5：记录与提交阶段 📚

### 更新 PROGRESS.md

#### A. 进度统计更新
```markdown
- **已完成总数**: X/65
### 第一阶段：基础数据结构补强 (X/20)
#### 链表专题 (X/6)
- [x] 题号. 题目名 (Easy/Medium/Hard)
```

#### B. 练习日志格式
```markdown
### 最近完成
#### YYYY-MM-DD
- ✅ 题号. 题目名 - 简要说明解法

### 遇到的难点和总结
#### 📌 题号. 题目名
**实现要点**：
- 使用的算法/数据结构
- 时间复杂度 O(?)，空间复杂度 O(?)

**初次实现的错误**：（如果有）
1. **❌ 错误描述**
   - 问题：具体什么错误
   - 后果：导致什么结果
   - 修正：如何修复

**关键学习点**：
- 掌握的新模式/技巧
- 理解的核心概念
```

### Git 提交规范

#### 提交新题目解决方案
```bash
git add leetcode/l<题号>_<题目名>.cpp
git commit -m "feat: implement <题号>. <题目名> solution

- Add [算法类型] solution using [核心技巧]
- Time complexity: O(?), Space complexity: O(?)
- Handle edge cases: [列举]
- Update progress tracking (X/65 completed)

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>"
```

#### 提交文档更新
```bash
git add leetcode/PROGRESS.md
git commit -m "docs: add practice log with error analysis for problem <题号>

- Document errors and fixes
- Add key learning points
- Update progress statistics"
```

---

## 🎯 核心原则

### 1. 学习导向
- **错误是学习机会**：详细记录和分析每个错误
- **理解 > 数量**：深入理解每道题的核心概念
- **模式识别**：帮助用户识别通用解题模式

### 2. 渐进式帮助
- 不直接给答案
- 先提示思路
- Review 时才详细分析
- 记录错误供后续复习

### 3. 系统化记录
- 保持文档格式一致
- 错误分析要具体
- 学习点要可复用
- 便于后续复习回顾

---

## 📊 题目难度进阶

### 链表专题顺序
1. 基础操作：反转、合并
2. 快慢指针：环检测、中点
3. 复杂操作：K个一组反转、重排
4. 综合应用：LRU Cache 等

### 学习曲线管理
- Easy 题目：重点在基础操作熟练度
- Medium 题目：重点在算法思路和优化
- Hard 题目：重点在复杂度分析和边界处理

---

## 🔄 每日流程示例

```
1. Claude 生成题目 → l141_linked_list_cycle.cpp
2. 用户实现代码 → 独立完成
3. 用户运行测试 → 发现有错误
4. 用户请求 review → Claude 分析错误
5. 用户修正代码 → 测试通过
6. Claude 更新文档 → PROGRESS.md 记录错误和学习点
7. Git 提交 → 保存进度
8. 准备下一题 → 循环继续
```

---

## 📝 常见错误模式库

### 链表操作
- 忘记移动指针：`curr = curr->next`
- 错误修改 dummy head
- 未处理空链表情况
- 内存泄漏（忘记 delete）

### 指针操作
- 空指针解引用
- 指针移动越界
- 野指针使用

### 边界条件
- 空输入处理
- 单元素情况
- 越界访问

---

## 🚀 进阶建议

当用户完成一个专题后：
1. 总结该专题的通用模式
2. 对比不同解法的优劣
3. 建议相关的进阶题目
4. 提供专题总结文档

---

本文档持续更新，根据练习过程不断完善。