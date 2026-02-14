#ifndef __CARD_HELPER_H__
#define __CARD_HELPER_H__

#include "cocos2d.h"
#include "configs/GameConstants.h"
#include <string>

/**
 * @class CardHelper
 * @brief 卡牌资源路径辅助工具类
 * 
 * 职责：
 * - 提供卡牌图片资源路径的生成方法
 * - 根据卡牌属性（花色、点数）获取对应的资源路径
 * - 提供卡牌背面、底座等通用资源的路径获取
 * 
 * 使用场景：
 * 由 CardView 调用，用于加载卡牌相关的图片资源
 * 
 * 设计原则：
 * - 完全独立，不依赖业务逻辑
 * - 提供静态方法，无状态
 * - 仅处理资源路径的字符串拼接和转换
 */
class CardHelper {
public:
    /**
     * @brief 根据花色和点数获取卡牌正面完整图片路径
     * @param suit 卡牌花色
     * @param face 卡牌点数
     * @return 卡牌图片资源路径
     * @note 当前实现返回占位路径，实际使用时由 CardView 组合多个部分
     */
    static std::string getCardFrameName(CardSuitType suit, CardFaceType face);

    /**
     * @brief 获取卡背图片路径
     * @return 卡背图片资源路径
     */
    static std::string getCardBackFrameName();

    /**
     * @brief 获取卡牌基础底座背景图片路径
     * @return 卡牌底座图片资源路径
     */
    static std::string getCardBasePath();
    
    /**
     * @brief 获取花色图标图片路径
     * @param suit 卡牌花色（梅花、方片、红心、黑桃）
     * @return 花色图标图片资源路径
     */
    static std::string getSuitImagePath(CardSuitType suit);
    
    /**
     * @brief 获取点数数字图片路径
     * @param face 卡牌点数（A, 2-10, J, Q, K）
     * @param isBig 是否为大号数字
     * @param isBlack 是否为黑色（黑桃和梅花为黑色）
     * @return 数字图片资源路径
     */
    static std::string getNumberImagePath(CardFaceType face, bool isBig, bool isBlack);
};

#endif