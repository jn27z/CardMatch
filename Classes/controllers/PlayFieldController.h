#pragma once

#include "cocos2d.h"
#include "models/GameModel.h"
#include "views/GameView.h"
#include "views/CardView.h"

/**
 * @class PlayFieldController
 * @brief 游戏主区域控制器
 * 
 * 职责：
 * - 负责游戏区卡牌的渲染和布局显示
 * - 管理卡牌视图的状态更新（翻面、点击事件）
 * - 作为 GameController 的子控制器，专注于游戏区的视图管理
 * 
 * 使用场景：
 * 由 GameController 创建和持有，负责游戏区域的所有视图操作
 * 
 * 交互方式：
 * - 从 GameModel 读取卡牌数据
 * - 通过 GameView 操作视图层
 * - 通过回调函数将点击事件委托给 GameController 处理业务逻辑
 */
class PlayFieldController : public cocos2d::Ref {
public:
    /**
     * @brief 创建控制器实例
     * @param gameModel 游戏数据模型
     * @param gameView 游戏视图
     * @return 自动释放的控制器指针，失败返回 nullptr
     */
    static PlayFieldController* create(GameModel* gameModel, GameView* gameView);

    /**
     * @brief 渲染游戏区的所有卡牌
     * @details 清空现有视图，根据 GameModel 中的卡牌数据重新创建所有 CardView
     */
    void renderPlayField();

    /**
     * @brief 刷新所有卡牌视图的状态
     * @details 根据 CardModel 的 faceUp 状态更新视图的翻面和点击事件
     */
    void refreshViewStates();

    /**
     * @brief 设置卡牌点击处理回调
     * @param handler 点击回调函数，由 GameController 提供
     */
    void setCardClickHandler(const std::function<void(CardView*)>& handler);
    
    /**
     * @brief 更新持有的 GameModel 引用
     * @param gameModel 新的游戏模型
     * @note 用于重启游戏时更新模型引用
     */
    void setGameModel(GameModel* gameModel);

private:
    PlayFieldController();
    ~PlayFieldController();

    /**
     * @brief 初始化控制器
     * @param gameModel 游戏数据模型
     * @param gameView 游戏视图
     * @return 初始化成功返回 true
     */
    bool init(GameModel* gameModel, GameView* gameView);

    GameModel* _gameModel;                              ///< 游戏数据模型
    GameView* _gameView;                                ///< 游戏视图层
    std::function<void(CardView*)> _cardClickHandler;   ///< 卡牌点击回调，由 GameController 提供
};