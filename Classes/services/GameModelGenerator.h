#ifndef __GAME_MODEL_GENERATOR_H__
#define __GAME_MODEL_GENERATOR_H__

#include "models/GameModel.h"
#include "configs/LevelConfig.h"

/**
 * @class GameModelGenerator
 * @brief 游戏模型生成器
 * 
 * 职责：
 * - 根据关卡配置生成 GameModel 的初始数据
 * - 创建游戏区卡牌、备用牌堆
 * - 初始化出牌堆的第一张牌
 * - 设置卡牌的位置、层级、尺寸等属性
 * 
 * 使用场景：
 * 由 GameController 调用，用于初始化游戏数据
 * 
 * 设计原则：
 * - 提供静态方法，不持有数据
 * - 不管理数据生命周期，只负责数据加工
 * - 完全独立，可单独测试
 */
class GameModelGenerator {
public:
    /**
     * @brief 根据配置生成游戏模型数据
     * @param model 要填充的游戏模型
     * @param config 关卡配置数据
     * @details 创建所有卡牌对象，设置初始状态，并调用 refreshCardStates 更新遮挡关系
     */
    static void generate(GameModel* model, const LevelConfig& config);
};

#endif