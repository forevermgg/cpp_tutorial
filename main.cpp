#include "DynamicLoopCheck.h"
// 动态变量/函数返回值都支持
uint64_t getDynamicN() { return 700000000; }

int main() {
    auto N = getDynamicN();
    // 循环前校验，7亿次直接告警+打栈
    CHECK_LOOP_DYNAMIC_SIZE(N, "业务-数据同步循环");

    for(int i=0; i<N; i++){
        // 业务逻辑
    }
    return 0;
}