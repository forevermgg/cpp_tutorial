#pragma once
#include <iostream>
#include <atomic>
#include <string>
#include <chrono>
#include <cstdlib>
#include <cstdint>
#include <execinfo.h>
#include <mutex>

// 全局配置：可动态调整，支持从配置中心拉取
namespace LoopMonitorConfig {
    // 默认告警阈值：100万次（可改，根据业务调整）
    std::atomic<uint64_t> LOOP_WARN_THRESHOLD = 1000000;
    // 同进程仅首次超标告警（防刷屏，线上推荐）
    std::atomic<bool> WARN_ONCE_PER_PROCESS = true;
    // 是否开启栈回溯（测试/预发开，线上可关，减少开销）
    bool ENABLE_STACK_TRACE = true;
    // 是否允许熔断（超标直接终止循环，线上谨慎开启）
    bool ENABLE_LOOP_BREAK = false;
}

// 打印函数调用栈（精准定位超标循环）
inline void printLoopStackTrace() {
    if (!LoopMonitorConfig::ENABLE_STACK_TRACE) return;

    void* callstack[16];
    int frameNum = backtrace(callstack, 16);
    char** funcNames = backtrace_symbols(callstack, frameNum);
    if (!funcNames) return;

    std::cerr << "\n===== LOOP OVERFLOW STACK TRACE =====" << std::endl;
    for (int i = 0; i < frameNum; ++i) {
        std::cerr << "[" << i << "] " << funcNames[i] << std::endl;
    }
    std::cerr << "=====================================\n" << std::endl;
    free(funcNames);
}

// 线程安全的告警器（避免多线程重复刷屏）
inline void loopWarn(const std::string& loopName, uint64_t loopSize) {
    static std::mutex warnMutex;
    std::lock_guard<std::mutex> lock(warnMutex);

    if (LoopMonitorConfig::WARN_ONCE_PER_PROCESS) {
        auto now = std::chrono::system_clock::now();
        auto nowT = std::chrono::system_clock::to_time_t(now);
        std::cerr << "[DYNAMIC_LOOP_WARN] " << ctime(&nowT);
        std::cerr << "LoopName: " << loopName << std::endl;
        std::cerr << "DynamicCount: " << loopSize << " | Threshold: "
                  << LoopMonitorConfig::LOOP_WARN_THRESHOLD << std::endl;

        printLoopStackTrace();
        LoopMonitorConfig::WARN_ONCE_PER_PROCESS = false;
    }
}

/**
 * 1. 循环前校验（推荐优先用）
 * 适配：已知动态循环上限N（变量/函数返回值都可）
 * 作用：提前校验N，超标直接告警，避免无效循环
 */
#define CHECK_LOOP_DYNAMIC_SIZE(N, LOOP_NAME) \
do { \
    const auto loopSize = static_cast<uint64_t>(N); \
    if (loopSize > LoopMonitorConfig::LOOP_WARN_THRESHOLD.load()) { \
        loopWarn(LOOP_NAME, loopSize); \
        if (LoopMonitorConfig::ENABLE_LOOP_BREAK) { \
            std::cerr << "[LOOP_BREAK] 触发熔断，终止循环" << std::endl; \
            break; \
        } \
    } \
} while(0)

/**
 * 2. 循环内计数校验（兜底用）
 * 适配：未知循环上限、N嵌套过深无法提前获取
 * 作用：实时计数，防N异常/步长异常导致循环失控
 */
#define LOOP_DYNAMIC_COUNT_CHECK(CNT_VAR, LOOP_NAME) \
do { \
    if (++(CNT_VAR) > LoopMonitorConfig::LOOP_WARN_THRESHOLD.load()) { \
        loopWarn(LOOP_NAME, CNT_VAR); \
        if (LoopMonitorConfig::ENABLE_LOOP_BREAK) { \
            std::cerr << "[LOOP_BREAK] 计数超标，强制终止" << std::endl; \
            break; \
        } \
    } \
} while(0)

/**
 * 3. 阈值动态调整接口（运行时可改，无需重启）
 * 用法：setLoopWarnThreshold(5000000); // 调整阈值为500万
 */
inline void setLoopWarnThreshold(uint64_t newThreshold) {
    LoopMonitorConfig::LOOP_WARN_THRESHOLD.store(newThreshold);
    std::cerr << "[LOOP_CONFIG] 阈值已更新为: " << newThreshold << std::endl;
}

/**
 * 4. 重置告警标记（测试环境复用）
 * 用法：resetLoopWarnFlag(); // 重置后可再次触发告警
 */
inline void resetLoopWarnFlag() {
    LoopMonitorConfig::WARN_ONCE_PER_PROCESS = true;
}