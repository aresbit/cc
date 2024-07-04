#!/bin/bash

# 使用方法: ./check_code_in_so.sh <so文件路径> <函数名> [源文件路径]

if [ $# -lt 2 ]; then
    echo "使用方法: $0 <so文件路径> <函数名> [源文件路径]"
    exit 1
fi

SO_FILE=$1
FUNC_NAME=$2
SOURCE_FILE=${3:-""}

# 检查.so文件是否存在
if [ ! -f "$SO_FILE" ]; then
    echo "错误: 文件 '$SO_FILE' 不存在"
    exit 1
fi

# 使用nm查找函数
nm_result=$(nm -C "$SO_FILE" | grep -i " T $FUNC_NAME")

if [ -n "$nm_result" ]; then
    echo "在符号表中找到函数 '$FUNC_NAME':"
    echo "$nm_result"
    
    # 提取函数地址
    func_addr=$(echo $nm_result | awk '{print $1}')
    
    # 使用objdump查看反汇编代码
    echo "函数 '$FUNC_NAME' 的反汇编代码:"
    objdump -d "$SO_FILE" --start-address=0x$func_addr | grep -A 20 "<$FUNC_NAME>:"
    
    # 如果提供了源文件路径，使用addr2line查找源代码位置
    if [ -n "$SOURCE_FILE" ]; then
        echo "源代码位置:"
        addr2line -e "$SO_FILE" -f -C 0x$func_addr
        
        # 尝试显示相关的源代码行
        line_info=$(addr2line -e "$SO_FILE" -C 0x$func_addr | cut -d: -f2)
        if [ -f "$SOURCE_FILE" ]; then
            echo "相关源代码:"
            sed -n "$((line_info-5)),$((line_info+5))p" "$SOURCE_FILE"
        fi
    fi
else
    echo "警告: 函数 '$FUNC_NAME' 未在 $SO_FILE 中找到"
    echo "这可能意味着该函数未被编译进共享库，或者可能被内联或优化掉了"
fi

# 搜索特定的变量赋值模式
if [ -n "$SOURCE_FILE" ]; then
    echo "搜索 'int abc = func(void);' 模式:"
    objdump -d "$SO_FILE" | grep -B 5 -A 5 "call.*<$FUNC_NAME>" | grep "mov.*%eax"
fi
