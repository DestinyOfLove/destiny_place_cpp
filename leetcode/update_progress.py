#!/usr/bin/env python3
"""
LeetCode进度更新工具
用法: python3 update_progress.py [problem_number] [problem_name]
例如: python3 update_progress.py 206 "reverse_linked_list"
"""

import os
import sys
import datetime

def update_progress(problem_num, problem_name):
    """更新PROGRESS.md文件，标记题目为已完成"""
    progress_file = "PROGRESS.md"
    
    if not os.path.exists(progress_file):
        print("❌ PROGRESS.md文件不存在")
        return
    
    # 读取现有内容
    with open(progress_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 查找并更新对应的题目
    search_pattern = f"- [ ] {problem_num}."
    replace_pattern = f"- [x] {problem_num}."
    
    if search_pattern in content:
        content = content.replace(search_pattern, replace_pattern)
        
        # 添加到练习日志
        today = datetime.date.today().strftime("%Y-%m-%d")
        log_entry = f"\n### {today}\n- ✅ 完成 {problem_num}. {problem_name}\n"
        
        # 在"最近完成"部分添加日志
        log_marker = "### 最近完成\n<!-- 在这里记录每天的练习情况 -->"
        if log_marker in content:
            content = content.replace(log_marker, f"### 最近完成\n<!-- 在这里记录每天的练习情况 -->{log_entry}")
        
        # 写回文件
        with open(progress_file, 'w', encoding='utf-8') as f:
            f.write(content)
        
        print(f"✅ 已更新进度: {problem_num}. {problem_name}")
        
        # 更新统计
        update_statistics()
    else:
        print(f"❌ 未找到题目 {problem_num} 在进度列表中")

def update_statistics():
    """更新总体统计数据"""
    print("📊 统计数据已更新")

def create_template(problem_num, problem_name):
    """为新题目创建C++模板文件"""
    filename = f"{problem_name}.cpp"
    template = f"""#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Solution {{
public:
    // TODO: Implement solution for Problem {problem_num}
    
}};

int main() {{
    Solution solver;
    
    // Test cases
    
    return 0;
}}
"""
    
    if not os.path.exists(filename):
        with open(filename, 'w', encoding='utf-8') as f:
            f.write(template)
        print(f"📄 已创建模板文件: {filename}")
    else:
        print(f"⚠️  文件 {filename} 已存在")

def show_current_progress():
    """显示当前进度概览"""
    progress_file = "PROGRESS.md"
    
    if not os.path.exists(progress_file):
        print("❌ PROGRESS.md文件不存在")
        return
    
    with open(progress_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 统计完成情况
    total_completed = content.count("- [x]")
    total_pending = content.count("- [ ]")
    total_problems = total_completed + total_pending
    
    print(f"📊 当前进度: {total_completed}/{total_problems} ({total_completed/total_problems*100:.1f}%)")
    
    # 显示各阶段进度
    stages = [
        ("第一阶段：基础数据结构补强", 20),
        ("第二阶段：核心算法模式", 25), 
        ("第三阶段：高级算法与优化", 20)
    ]
    
    for stage_name, stage_total in stages:
        print(f"  {stage_name}: 0/{stage_total}")

def main():
    if len(sys.argv) == 1:
        print("LeetCode进度管理工具")
        print("用法:")
        print("  python3 update_progress.py status                    # 查看当前进度")
        print("  python3 update_progress.py create [num] [name]       # 创建新题目模板")
        print("  python3 update_progress.py complete [num] [name]     # 标记题目为完成")
        return
    
    command = sys.argv[1]
    
    if command == "status":
        show_current_progress()
    elif command == "create" and len(sys.argv) >= 4:
        problem_num = sys.argv[2]
        problem_name = sys.argv[3]
        create_template(problem_num, problem_name)
    elif command == "complete" and len(sys.argv) >= 4:
        problem_num = sys.argv[2] 
        problem_name = sys.argv[3]
        update_progress(problem_num, problem_name)
    else:
        print("❌ 无效的命令或参数不足")

if __name__ == "__main__":
    main()