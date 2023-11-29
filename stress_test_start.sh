#########################################################################
# File Name:    stress_test_start.sh
# Author:        程序员Carl
# 微信公众号:    代码随想录
# Created Time: Wed Jan 30 20:05:15 2019
#########################################################################
#!/bin/bash
set -e  # 让脚本在任何错误发生时立即退出

echo "Compiling..."
g++ stress-test/stress_test.cpp -o ./bin/stress --std=c++11 -pthread
echo "Compilation successful."

echo "Running..."
./bin/stress
echo "Script execution completed."
