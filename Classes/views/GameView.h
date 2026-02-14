// --------------------------------------------------------
// 文件名: Classes/views/GameView.h
// --------------------------------------------------------
#ifndef __GAME_VIEW_H__
#define __GAME_VIEW_H__

#include "cocos2d.h"
#include "models/CardModel.h"
#include <functional>

/**
 * @class GameView
 * @brief 游戏主界面视图层
 * 
 * 职责：
 * - 管理所有UI元素的显示和布局
 * - 提供底牌/备用牌的位置查询接口
 * - 通过回调接口与 Controller 交互
 * 
 * 设计原则：
 * - 不持有 Model 数据，只接收外部传入的数据进行显示
 * - 所有用户交互通过回调转发给 Controller
 */
class GameView : public cocos2d::Layer {
public:
    /**
     * @brief 创建 GameView 实例
     * @return 自动释放的 GameView 指针
     */
    static GameView* create();
    bool init() override;
    ~GameView();

    /**
     * @brief 获取主牌区图层
     * @return 主牌区的 Node 节点
     */
    cocos2d::Node* getPlayFieldLayer() { return _playFieldLayer; }

    /**
     * @brief 获取底牌堆的世界坐标
     * @return 底牌堆中心点的世界坐标
     */
    cocos2d::Vec2 getStackPosition();
    
    /**
     * @brief 获取备用牌堆的世界坐标
     * @return 备用牌堆中心点的世界坐标
     */
    cocos2d::Vec2 getStockPosition();

    /**
     * @brief 更新底牌堆显示
     * @param topCard 当前底牌，传 nullptr 表示清空
     */
    void updateStackView(CardModel* topCard);
    
    /**
     * @brief 更新备用牌堆显示
     * @param count 剩余备用牌数量
     */
    void updateStockView(int count);

    /**
     * @brief 设置撤销按钮回调
     * @param callback 点击撤销按钮时触发的回调函数
     */
    void setUndoCallback(const std::function<void()>& callback);
    
    /**
     * @brief 设置备用牌点击回调
     * @param callback 点击备用牌时触发的回调函数
     */
    void setStockCallback(const std::function<void()>& callback);
    
    /**
     * @brief 设置重新开始按钮回调
     * @param callback 点击重新开始按钮时触发的回调函数
     */
    void setRestartCallback(const std::function<void()>& callback);
    
    /**
     * @brief 设置撤销按钮启用状态
     * @param enabled true=启用，false=禁用（灰化显示）
     */
    void setUndoButtonEnabled(bool enabled);

    /**
     * @brief 显示游戏结束界面
     * @param win true=胜利，false=失败
     */
    void showGameOverUI(bool win);

private:
    // ===================== 初始化相关 =====================
    
    void setupBackground();
    void setupBottomPanel();
    void setupPlayField();
    void setupStackArea();
    void setupStockPile();
    void setupUndoButton();
    
    // 备用牌堆初始化子函数
    void createStockPileSprites();
    void setupStockTouchListener();
    void setupStockCountLabel();
    
    // ✅ 新增：触摸处理方法
    /**
     * @brief 备用牌堆触摸开始处理
     * @param t 触摸对象
     * @param e 事件对象
     * @return 是否处理该触摸
     */
    bool onStockTouchBegan(cocos2d::Touch* t, cocos2d::Event* e);
    
    /**
     * @brief 备用牌堆触摸结束处理
     * @param t 触摸对象
     * @param e 事件对象
     */
    void onStockTouchEnded(cocos2d::Touch* t, cocos2d::Event* e);
    
    /**
     * @brief 计算备用牌堆的触摸区域
     * @return 触摸区域矩形
     */
    cocos2d::Rect calculateStockTouchBounds() const;
    
    // ===================== 游戏结束界面相关 =====================
    
    cocos2d::LayerColor* createGameOverMask();
    void addGameOverTitle(cocos2d::LayerColor* mask, bool win);
    void addGameOverRestartButton(cocos2d::LayerColor* mask);
    void playGameOverAnimation(cocos2d::LayerColor* mask);

    // ===================== 成员变量 =====================
    
    cocos2d::Node* _playFieldLayer;           ///< 主牌区图层
    cocos2d::Node* _stackNode;                ///< 底牌堆节点
    cocos2d::Node* _stockPileNode;            ///< 备用牌堆节点
    cocos2d::Sprite* _stockSprite;            ///< 备用牌顶部精灵
    cocos2d::Label* _stockCountLabel;         ///< 备用牌数量标签
    cocos2d::Node* _undoBtnNode;              ///< 撤销按钮节点
    
    std::function<void()> _undoCallback;      ///< 撤销按钮回调
    std::function<void()> _stockCallback;     ///< 备用牌点击回调
    std::function<void()> _restartCallback;   ///< 重新开始回调
};

#endif