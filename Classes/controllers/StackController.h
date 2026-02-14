#pragma once

#include "cocos2d.h"
#include "models/GameModel.h"
#include "views/GameView.h"
#include "models/CardModel.h"

/**
 * @class StackController
 * @brief 出牌堆和备用牌堆控制器
 * 
 * 职责：
 * - 管理出牌堆（Stack）的视图更新
 * - 管理备用牌堆（Stock）的视图更新
 * - 处理卡牌移动动画（游戏区 -> 出牌堆、备用牌堆 -> 出牌堆）
 * - 处理撤销操作的卡牌恢复动画
 * 
 * 使用场景：
 * 由 GameController 创建和持有，负责出牌和备用牌相关的所有操作
 * 
 * 设计原则：
 * - 封装所有与出牌堆相关的动画逻辑
 * - 通过回调函数通知 GameController 操作完成
 * - 不直接处理业务逻辑，只负责视图和动画
 */
class StackController : public cocos2d::Ref {
public:
    /**
     * @brief 创建控制器实例
     * @param gameModel 游戏数据模型
     * @param gameView 游戏视图
     * @return 自动释放的控制器指针，失败返回 nullptr
     */
    static StackController* create(GameModel* gameModel, GameView* gameView);

    /**
     * @brief 更新出牌堆视图
     * @details 根据 GameModel 的顶部卡牌更新显示
     */
    void updateStackView();

    /**
     * @brief 更新备用牌堆视图
     * @details 根据 GameModel 的备用牌数量更新显示
     */
    void updateStockView();

    /**
     * @brief 从备用牌堆抽取一张牌
     * @param onComplete 完成回调，参数为抽取的卡牌（失败时为 nullptr）
     * @details 执行从备用牌堆到出牌堆的移动动画，动画完成后更新 GameModel
     */
    void drawCardFromStock(const std::function<void(CardModel*)>& onComplete);

    /**
     * @brief 将游戏区的卡牌移动到出牌堆
     * @param card 要移动的卡牌模型
     * @param fromPos 起始位置（世界坐标）
     * @param onComplete 完成回调
     * @details 执行移动动画，动画完成后从游戏区移除卡牌并更新出牌堆
     */
    void moveCardToStack(CardModel* card, const cocos2d::Vec2& fromPos, 
                         const std::function<void()>& onComplete);

    /**
     * @brief 撤销从备用牌堆抽牌操作
     * @param card 要放回的卡牌
     * @details 将卡牌放回备用牌堆，不执行动画
     */
    void undoDrawCard(CardModel* card);

    /**
     * @brief 撤销从游戏区出牌操作
     * @param card 要恢复的卡牌
     * @param originalPos 原始位置（游戏区坐标）
     * @param onComplete 完成回调
     * @details 执行从出牌堆飞回游戏区的动画
     */
    void undoMoveToStack(CardModel* card, const cocos2d::Vec2& originalPos, 
                         const std::function<void()>& onComplete);

    /**
     * @brief 更新持有的 GameModel 引用
     * @param gameModel 新的游戏模型
     * @note 用于重启游戏时更新模型引用
     */
    void setGameModel(GameModel* gameModel);

private:
    StackController();
    ~StackController();

    /**
     * @brief 初始化控制器
     * @param gameModel 游戏数据模型
     * @param gameView 游戏视图
     * @return 初始化成功返回 true
     */
    bool init(GameModel* gameModel, GameView* gameView);

    GameModel* _gameModel;  ///< 游戏数据模型
    GameView* _gameView;    ///< 游戏视图层
};