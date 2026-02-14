#ifndef __CARD_VIEW_H__
#define __CARD_VIEW_H__

#include "cocos2d.h"
#include "models/CardModel.h"

/**
 * @class CardView
 * @brief 单张卡牌的视图类
 * 
 * 职责：
 * - 负责单张卡牌的视觉呈现（底座、花色、点数）
 * - 处理卡牌的翻面动画和效果
 * - 处理卡牌的触摸事件并通过回调通知上层
 * 
 * 使用场景：
 * 由 PlayFieldController 和 StackController 创建
 * 在游戏区、出牌堆显示卡牌
 * 
 * 设计原则：
 * - 只负责视图显示，不处理业务逻辑
 * - 通过回调函数将用户交互传递给 Controller
 * - 持有 CardModel 用于获取卡牌属性
 */
class CardView : public cocos2d::Node {
public:
    CardView();
    virtual ~CardView();

    /**
     * @brief 创建卡牌视图
     * @param cardModel 卡牌数据模型
     * @return 自动释放的卡牌视图指针，失败返回 nullptr
     */
    static CardView* createWithCardModel(CardModel* cardModel);
    
    /**
     * @brief 初始化卡牌视图
     * @param cardModel 卡牌数据模型
     * @return 初始化成功返回 true
     */
    bool initWithCardModel(CardModel* cardModel);

    /**
     * @brief 翻转卡牌
     * @param showFront true=显示正面（显示花色点数），false=显示背面
     */
    void flip(bool showFront);
    
    /**
     * @brief 设置卡牌位置
     * @param position 目标位置
     */
    void setCardPosition(const cocos2d::Vec2& position);

    /**
     * @brief 获取卡牌数据模型
     * @return 卡牌模型指针
     */
    CardModel* getCardModel() const { return _cardModel; }

    /**
     * @brief 设置点击回调函数
     * @param callback 点击时触发的回调，参数为当前 CardView
     */
    void setOnClickCallback(const std::function<void(CardView*)>& callback);

private:
    /**
     * @brief 触摸开始事件处理
     * @param touch 触摸对象
     * @param event 事件对象
     * @return 是否处理该触摸事件
     */
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    
    /**
     * @brief 触摸结束事件处理
     * @param touch 触摸对象
     * @param event 事件对象
     */
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    cocos2d::Sprite* _cardBase;      ///< 卡牌基础底座
    cocos2d::Sprite* _suitSprite;    ///< 花色图标
    cocos2d::Sprite* _numberTop;     ///< 上方的点数标识
    cocos2d::Sprite* _numberBottom;  ///< 下方的点数标识（预留）
    CardModel* _cardModel;           ///< 卡牌数据模型

    std::function<void(CardView*)> _onClickCallback;  ///< 点击回调函数
};

#endif