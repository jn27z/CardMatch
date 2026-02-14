// Classes/configs/LevelConfigLoader.h
#ifndef __LEVEL_CONFIG_LOADER_H__
#define __LEVEL_CONFIG_LOADER_H__

#include "configs/LevelConfig.h"
#include <string>

/**
 * @class LevelConfigLoader
 * @brief 关卡配置加载器
 * 
 * 职责：
 * - 从 JSON 文件加载关卡配置
 * - 解析卡牌数据（花色、点数、位置）
 * - 转换为 LevelConfig 结构体
 * 
 * 使用场景：
 * 由 GameController 调用，用于加载预设关卡
 * 
 * 设计原则：
 * - 提供静态方法，不持有数据
 * - 只负责文件解析，不处理业务逻辑
 * - 解析失败返回空配置
 */
class LevelConfigLoader {
public:
    /**
     * @brief 加载关卡配置文件
     * @param filename 配置文件名（相对于资源目录）
     * @return 关卡配置对象，文件不存在或解析失败返回空配置
     */
    static LevelConfig loadLevelConfig(const std::string& filename);
};

#endif