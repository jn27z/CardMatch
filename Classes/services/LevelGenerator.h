#ifndef __LEVEL_GENERATOR_H__
#define __LEVEL_GENERATOR_H__

#include "configs/LevelConfig.h"

/**
 * @class LevelGenerator
 * @brief 关卡配置生成服务
 * 
 * 职责：
 * - 生成默认的金字塔布局
 * - 生成备用牌堆配置
 * - 提供测试关卡的快速生成
 * 
 * 使用场景：
 * 当配置文件不存在时，由 GameController 调用生成默认关卡
 * 
 * 设计原则：
 * - Services层：提供静态方法，不持有数据
 * - 只负责配置生成，不处理业务逻辑
 */
class LevelGenerator {
public:
    /**
     * @brief 生成默认的金字塔布局配置
     * @return 关卡配置对象
     */
    static LevelConfig generateDefaultPyramid();
    
private:
    /**
     * @brief 生成金字塔的游戏区卡牌
     * @param config 输出的配置对象
     */
    static void generatePyramidCards(LevelConfig& config);
    
    /**
     * @brief 生成备用牌堆
     * @param config 输出的配置对象
     * @param count 备用牌数量
     */
    static void generateStockCards(LevelConfig& config, int count);
    
    /**
     * @brief 生成随机卡牌数据
     * @param x X坐标
     * @param y Y坐标
     * @return 卡牌配置数据
     */
    static CardConfigData createRandomCard(float x, float y);
};

#endif