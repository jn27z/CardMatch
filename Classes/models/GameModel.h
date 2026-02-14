// --------------------------------------------------------
// 文件名: Classes/models/GameModel.h
// --------------------------------------------------------
#ifndef __GAME_MODEL_H__
#define __GAME_MODEL_H__

#include "cocos2d.h"
#include "CardModel.h"
#include <map> // 需要引入 map

/**
 * @class GameModel
 * @brief 游戏核心数据模型
 * 
 * 职责：
 * - 管理主牌区、备用牌堆、底牌堆的数据
 * - 提供几何遮挡检测算法（isCardBlocked）
 * - 判断游戏胜负条件
 * - 支持序列化/反序列化（存档功能）
 * 
 * 使用场景：
 * 由 GameController 创建和持有，所有卡牌状态变更通过此类管理
 */
class GameModel : public cocos2d::Ref {
public:
    GameModel();
    ~GameModel();
    
    /**
     * @brief 创建 GameModel 实例
     * @return 自动释放的 GameModel 指针，失败返回 nullptr
     */
    static GameModel* create();
    
    /**
     * @brief 初始化游戏模型
     * @return 初始化成功返回 true
     */
    bool init();
    
    /**
     * @brief 刷新所有卡牌的翻开/覆盖状态
     * @details 根据几何遮挡算法，计算每张牌是否被其他牌遮挡
     *          被遮挡的牌设为背面，未遮挡的牌翻开
     */
    void refreshCardStates();
    
    /**
     * @brief 添加卡牌到主牌区
     * @param card 要添加的卡牌
     */
    void addPlayFieldCard(CardModel* card);
    
    // --- 主牌区 ---
    cocos2d::Vector<CardModel*> getPlayFieldCards() const { return _playFieldCards; }
    void removePlayFieldCard(CardModel* card);

    // --- 底牌堆 ---
    void setTopStackCard(CardModel* card);
    CardModel* getTopStackCard() const { return _topStackCard; }

    // --- 备用牌堆 ---
    void addStockCard(CardModel* card);
    /**
     * @brief 从备用牌堆抽取一张牌
     * @return 抽取的卡牌，若牌堆为空返回 nullptr
     * @note 调用方需要手动 release()
     */
    CardModel* drawStockCard();
    void returnToStock(CardModel* card);
    int getStockCount() const;

    // --- 【核心】遮挡检测 ---
    /**
     * 检查某张牌是否被其他牌遮挡
     * @param card 目标牌
     * @param cardPos 目标牌的坐标
     * @param allCardsPos 所有牌的坐标映射
     * @return true=被遮挡(不可点), false=自由(可点)
     */
    bool isCardBlocked(CardModel* card, const cocos2d::Vec2& cardPos, const std::map<CardModel*, cocos2d::Vec2>& allCardsPos);

    // 胜负判定
    /**
     * @brief 判断游戏是否胜利
     * @return 主牌区为空时返回 true
     */
    bool isGameWon();
    
    /**
     * @brief 判断游戏是否失败
     * @return 无可点击的匹配牌且备用牌用完时返回 true
     */
    bool isGameLost();

private:
    bool canMatch(CardModel* cardA, CardModel* cardB);

    cocos2d::Vector<CardModel*> _playFieldCards;
    cocos2d::Vector<CardModel*> _stockCards;
    CardModel* _topStackCard;
};

#endif