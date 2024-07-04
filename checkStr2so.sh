#!/bin/bash

# 使用方法: ./check_string_in_so.sh <so文件路径> <要查找的字符串>

if [ $# -ne 2 ]; then
    echo "使用方法: $0 <so文件路径> <要查找的字符串>"
    exit 1
fi

SO_FILE=$1
SEARCH_STRING=$2

# 检查.so文件是否存在
if [ ! -f "$SO_FILE" ]; then
    echo "错误: 文件 '$SO_FILE' 不存在"
    exit 1
fi

# 使用nm查找字符串
nm_result=$(nm -C "$SO_FILE" | grep -i "$SEARCH_STRING")

if [ -n "$nm_result" ]; then
    echo "使用nm找到字符串 '$SEARCH_STRING':"
    echo "$nm_result"
else
    echo "使用nm未找到字符串 '$SEARCH_STRING'"
fi

# 使用objdump查找字符串
objdump_result=$(objdump -s -j .rodata "$SO_FILE" | grep -i "$SEARCH_STRING")

if [ -n "$objdump_result" ]; then
    echo "使用objdump在.rodata段中找到字符串 '$SEARCH_STRING':"
    echo "$objdump_result"
else
    echo "使用objdump在.rodata段中未找到字符串 '$SEARCH_STRING'"
fi

# 如果两种方法都没找到，给出警告
if [ -z "$nm_result" ] && [ -z "$objdump_result" ]; then
    echo "警告: 字符串 '$SEARCH_STRING' 未在 $SO_FILE 中找到"
    echo "这可能意味着该字符串未被编译进共享库，或者可能被优化掉了"
fi
