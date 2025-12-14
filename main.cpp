#include <iostream>
#include <cmath>

// ====================================================================
// 命名空间：ImageTransform
// 用于封装所有常量和数据结构，防止全局命名冲突。
// ====================================================================
namespace ImageTransform {

    // --- 原始图片几何参数 (常量) ---
    const double L_ORIGINAL = 103.0;        // 原始图片边长
    const double D_ORIGINAL = 68.0;         // 原始灰色大圆的直径 (作为缩放基准)

    // 原始圆心位置 (相对于 103x103 图像左上角 (0, 0) 的偏移)
    const double CENTER_X_ORIGINAL = 63.0;
    const double CENTER_Y_ORIGINAL = 62.5;

    // --- 结果数据结构 ---
    struct TransformResult {
        double scaleFactor;       // 缩放比例
        double scaledLength;      // 缩放后的图片边长
        double imageDrawX;        // 绘制起始 X 坐标
        double imageDrawY;        // 绘制起始 Y 坐标
        double imageDrawEndX;     // 绘制结束 X 坐标
        double imageDrawEndY;     // 绘制结束 Y 坐标
    };

    /**
     * @brief 计算水滴形图片的缩放比例和绘制位置。
     *
     * @param D_target 目标直径 (GetStrokeWidth())
     * @param start_draw_x 目标重合圆心 X 坐标 (GetStartPointX())
     * @param start_draw_y 目标重合圆心 Y 坐标 (GetStartPointY())
     * @return TransformResult 包含所有计算结果的结构体。
     */
    TransformResult CalculateImageTransform(
            double D_target,
            double start_draw_x,
            double start_draw_y
    ) {
        TransformResult results = {}; // 初始化结果结构体
        // 2. 计算缩放因子 (Scale Factor)
        results.scaleFactor = D_target / D_ORIGINAL;
        // 3. 计算缩放后的图案圆心偏移量
        double center_x_scaled = CENTER_X_ORIGINAL * results.scaleFactor;
        double center_y_scaled = CENTER_Y_ORIGINAL * results.scaleFactor;

        // 缩放后的图片边长
        results.scaledLength = L_ORIGINAL * results.scaleFactor;

        // 4. 计算最终绘制起点 (Image Draw Start)
        // 绘制起点 = 目标重合圆心 - 缩放后的图案圆心偏移
        results.imageDrawX = start_draw_x - center_x_scaled;
        results.imageDrawY = start_draw_y - center_y_scaled;

        // 5. 缩放后的图片终点坐标
        results.imageDrawEndX = results.imageDrawX + results.scaledLength;
        results.imageDrawEndY = results.imageDrawY + results.scaledLength;

        return results;
    }

    /**
     * @brief 打印计算结果到控制台。
     */
    void PrintResults(const TransformResult& results, double D_target, double start_draw_x, double start_draw_y) {
        double center_x_scaled = CENTER_X_ORIGINAL * results.scaleFactor;
        double center_y_scaled = CENTER_Y_ORIGINAL * results.scaleFactor;

        std::cout << "--- 水滴形图片缩放与定位结果 ---" << std::endl;
        std::cout << "目标直径 D_target: " << D_target << std::endl;
        std::cout << "目标重合圆心位置 (Start Point): (" << start_draw_x << ", " << start_draw_y << ")" << std::endl;
        std::cout << "--------------------------------------" << std::endl;

        std::cout << "1. 图片需要缩放 (ScaleFactor): " << results.scaleFactor << std::endl;
        std::cout << "   缩放后图片边长 L_scaled: " << results.scaledLength << std::endl;

        std::cout << "\n2. 缩放后的图片在目标坐标系中的绘制起点 (左上角)：" << std::endl;
        std::cout << "   X 坐标 (image_draw_x): " << results.imageDrawX << std::endl;
        std::cout << "   Y 坐标 (image_draw_y): " << results.imageDrawY << std::endl;

        std::cout << "\n--- 辅助验证信息 ---" << std::endl;
        std::cout << "缩放后图案圆心偏移 (Scaled Center Offset): (" << center_x_scaled << ", " << center_y_scaled << ")" << std::endl;
        std::cout << "绘制终点 (Draw End): (" << results.imageDrawEndX << ", " << results.imageDrawEndY << ")" << std::endl;
        // 验证: 绘制起点 + 缩放偏移 应等于 目标圆心
        std::cout << "验证: 绘制起点 X (" << results.imageDrawX << ") + 偏移 (" << center_x_scaled << ") = " << results.imageDrawX + center_x_scaled
                  << " (目标 X: " << start_draw_x << ")" << std::endl;
    }

} // namespace ImageTransform

// ====================================================================
// 外部函数模拟 (用于测试)
// ====================================================================

double GetStrokeWidth() {
    return 20.0;
}

double GetStartPointX() {
    return 190.0;
}

double GetStartPointY() {
    return 290.0;
}

// ====================================================================
// Main 函数
// ====================================================================

int main() {
    using namespace ImageTransform;

    // 1. 获取输入值
    double D_target = GetStrokeWidth();
    double start_draw_x = GetStartPointX();
    double start_draw_y = GetStartPointY();

    // 2. 执行计算
    TransformResult results = CalculateImageTransform(D_target, start_draw_x, start_draw_y);

    // 3. 打印结果
    PrintResults(results, D_target, start_draw_x, start_draw_y);

    // 4. 示例：如何使用返回的结果结构体
    std::cout << "\n--- 如何使用返回的结果 ---" << std::endl;
    std::cout << "缩放比例: " << results.scaleFactor << std::endl;
    std::cout << "绘制区域: Start(" << results.imageDrawX << ", " << results.imageDrawY
              << ") End(" << results.imageDrawEndX << ", " << results.imageDrawEndY << ")" << std::endl;

    return 0;
}