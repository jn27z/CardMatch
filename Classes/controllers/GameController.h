// --------------------------------------------------------
// 文件名: Classes/controllers/GameController.h
// --------------------------------------------------------
#ifndef __GAME_CONTROLLER_H__
#define __GAME_CONTROLLER_H__

#include "cocos2d.h"
#include "models/GameModel.h"
#include "views/GameView.h"
#include "views/CardView.h"
#include "managers/UndoManager.h"
#include "controllers/PlayFieldController.h"
#include "controllers/StackController.h"
#include "configs/LevelConfig.h"

/**
 * @class GameController
 * @brief 游戏主控制器
 * 
 * 职责：
 * - 协调 GameModel 和 GameView 之间的交互
 * - 处理用户输入（点击卡牌、撤销、重新开始）
 * - 管理子控制器（PlayFieldController、StackController）
 * - 管理撤销系统（UndoManager）
 * - 控制游戏流程（开始、结束、重启）
 * 
 * 生命周期：
 * 由 HelloWorldScene 创建并持有，贯穿整个游戏过程
 */
class GameController : public cocos2d::Ref {
public:
    GameController();
    ~GameController();
    
    /**
     * @brief 创建控制器实例
     * @return 自动释放的控制器指针，失败返回 nullptr
     */
    static GameController* create();
    
    /**
     * @brief 初始化控制器
     * @return 初始化成功返回 true
     */
    bool init();

    /**
     * @brief 开始游戏
     * @param scene 游戏场景，View 将被添加到此场景
     */
    void startGame(cocos2d::Scene* scene);
    
    /**
     * @brief 处理主牌区卡牌点击事件
     * @param cardView 被点击的卡牌视图
     */
    void onCardClicked(CardView* cardView);
    
    /**
     * @brief 处理备用牌点击事件
     */
    void onStockClicked();
    
    /**
     * @brief 处理撤销按钮点击事件
     */
    void onUndoClicked();
    
    /**
     * @brief 重新开始游戏
     */
    void restartGame();

private:
    /**
     * @brief 生成测试关卡
     * @details 尝试加载配置文件，失败时使用默认金字塔布局
     */
    void generateTestLevel();
    
    /**
     * @brief 创建子控制器
     */
    void createSubControllers();
    
    /**
     * @brief 渲染场景
     */
    void renderScene();

    /**
     * @brief 刷新卡牌视图状态
     */
    void refreshViewStates();

    /**
     * @brief 检查游戏状态（胜利/失败）
     */
    void checkGameState();
    
    /**
     * @brief 判断两张牌是否可以匹配
     * @param card1 第一张牌
     * @param card2 第二张牌
     * @return 可以匹配返回 true
     */
    bool isCardMatchable(CardModel* card1, CardModel* card2);
    
    /**
     * @brief 处理匹配成功的卡牌
     * @param clickedCard 点击的卡牌模型
     * @param clickedView 点击的卡牌视图
     */
    void handleMatchedCard(CardModel* clickedCard, CardView* clickedView);
    
    /**
     * @brief 播放不匹配的抖动动画
     * @param cardView 要抖动的卡牌视图
     */
    void playMismatchAnimation(CardView* cardView);
    
    /**
     * @brief 处理从备用牌堆的撤销操作
     * @param step 撤销步骤
     */
    void handleUndoFromStock(const UndoStep& step);
    
    /**
     * @brief 处理从游戏区的撤销操作
     * @param step 撤销步骤
     */
    void handleUndoFromPlayField(const UndoStep& step);

    GameModel* _gameModel;                      ///< 游戏数据模型
    GameView* _gameView;                        ///< 游戏视图层
    UndoManager* _undoManager;                  ///< 撤销管理器
    PlayFieldController* _playFieldController;  ///< 游戏区控制器
    StackController* _stackController;          ///< 出牌堆控制器
    bool _isGameEnded;                          ///< 游戏是否已结束
};

#endif