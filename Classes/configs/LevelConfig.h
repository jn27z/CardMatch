#ifndef __LEVEL_CONFIG_H__
#define __LEVEL_CONFIG_H__

#include <vector>

/**
 * @struct CardConfigData
 * @brief 卡牌配置数据
 * 
 * 用于从 JSON 或其他配置源加载卡牌的初始数据
 */
struct CardConfigData {
    int face;   ///< 点数（1-13）
    int suit;   ///< 花色（0-3）
    float x;    ///< X 坐标
    float y;    ///< Y 坐标
};

/**
 * @struct LevelConfig
 * @brief 关卡配置结构
 * 
 * 包含一个完整关卡的所有配置信息
 */
struct LevelConfig {
    std::vector<CardConfigData> playfieldCards;  ///< 游戏区卡牌配置列表
    std::vector<CardConfigData> stackCards;      ///< 备用牌堆卡牌配置列表
};

#endif