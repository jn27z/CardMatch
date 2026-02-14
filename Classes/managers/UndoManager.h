// Classes/managers/UndoManager.h
#ifndef __UNDO_MANAGER_H__
#define __UNDO_MANAGER_H__

#include "cocos2d.h"
#include "models/CardModel.h"
#include <vector>

/**
 * @struct UndoStep
 * @brief 撤销步骤数据结构
 * 
 * 记录一次游戏操作的所有必要信息，用于撤销操作
 */
struct UndoStep {
    CardModel* movedCard;           ///< 被移动的卡牌
    cocos2d::Vec2 originalPos;      ///< 卡牌的原始位置
    CardModel* previousTopCard;     ///< 操作前的出牌堆顶部卡牌
    bool isFromStock;               ///< 是否来自备用牌堆（true=从备用牌抽取，false=从游戏区打出）
};

/**
 * @class UndoManager
 * @brief 撤销操作管理器
 * 
 * 职责：
 * - 记录玩家的每一步操作历史
 * - 提供撤销功能，恢复游戏状态
 * - 管理撤销栈的生命周期
 * 
 * 使用场景：
 * 作为 GameController 的成员变量，由 GameController 创建和持有
 * 每次玩家操作后调用 pushStep 记录
 * 点击撤销按钮时调用 popStep 恢复
 * 
 * 设计原则：
 * - 不实现为单例，作为 Controller 的成员变量
 * - 不反向依赖 Controller
 * - 只负责数据的存储和检索，不处理业务逻辑
 */
class UndoManager {
public:
    UndoManager();
    ~UndoManager();

    /**
     * @brief 记录一步操作
     * @param movedCard 被移动的卡牌
     * @param originalPos 卡牌的原始位置（世界坐标）
     * @param prevTop 操作前的出牌堆顶部卡牌
     * @param fromStock 是否来自备用牌堆
     * @note 该方法会 retain 传入的 CardModel 指针
     */
    void pushStep(CardModel* movedCard, const cocos2d::Vec2& originalPos, 
                  CardModel* prevTop, bool fromStock);

    /**
     * @brief 弹出一步操作
     * @param outStep 输出参数，存储弹出的撤销步骤
     * @return 成功返回 true，栈为空返回 false
     * @note 调用者需要负责 release outStep 中的 CardModel 指针
     */
    bool popStep(UndoStep& outStep);

    /**
     * @brief 清空所有撤销历史
     * @details 释放所有记录的 CardModel 引用
     */
    void clear();
    
    /**
     * @brief 检查是否还有可撤销的操作
     * @return 栈为空返回 true
     */
    bool isEmpty() const { return _undoStack.empty(); }

private:
    std::vector<UndoStep> _undoStack;  ///< 撤销步骤栈
};

#endif