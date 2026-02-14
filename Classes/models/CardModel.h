// --------------------------------------------------------
// 文件名: Classes/models/CardModel.h
// --------------------------------------------------------
#ifndef __CARD_MODEL_H__
#define __CARD_MODEL_H__

#include "cocos2d.h"
#include "configs/GameConstants.h"

/**
 * @class CardModel
 * @brief 单张卡牌的数据模型
 * 
 * 职责：
 * - 存储卡牌的基本属性（花色、点数、位置、状态）
 * - 提供属性的访问和修改接口
 * - 支持序列化和反序列化（预留存档功能）
 * 
 * 使用场景：
 * 由 GameModel 持有和管理，表示游戏中的每一张卡牌
 * 被 CardView 引用以获取显示数据
 * 
 * 设计原则：
 * - 纯数据模型，不包含复杂业务逻辑
 * - 继承自 cocos2d::Ref 便于内存管理
 */
class CardModel : public cocos2d::Ref {
public:
    /**
     * @brief 创建卡牌模型
     * @param id 卡牌唯一标识
     * @param suit 花色
     * @param face 点数
     * @return 自动释放的卡牌模型指针，失败返回 nullptr
     */
    static CardModel* create(int id, CardSuitType suit, CardFaceType face);

    /**
     * @brief 获取卡牌ID
     * @return 卡牌唯一标识
     */
    int getId() const { return _id; }
    
    /**
     * @brief 获取卡牌花色
     * @return 花色枚举值
     */
    CardSuitType getSuit() const { return _suit; }
    
    /**
     * @brief 获取卡牌点数
     * @return 点数枚举值
     */
    CardFaceType getFace() const { return _face; }

    /**
     * @brief 获取卡牌位置
     * @return 位置坐标
     */
    cocos2d::Vec2 getPosition() const { return _position; }
    
    /**
     * @brief 设置卡牌位置
     * @param pos 目标位置
     */
    void setPosition(const cocos2d::Vec2& pos) { _position = pos; }

    /**
     * @brief 获取卡牌是否正面朝上
     * @return true=正面朝上，false=背面朝上
     */
    bool isFaceUp() const { return _isFaceUp; }
    
    /**
     * @brief 设置卡牌正反面状态
     * @param faceUp true=正面朝上，false=背面朝上
     */
    void setFaceUp(bool faceUp) { _isFaceUp = faceUp; }

    /**
     * @brief 获取卡牌的渲染层级
     * @return Z-Order 值
     */
    int getLocalZOrder() const { return _localZOrder; }
    
    /**
     * @brief 设置卡牌的渲染层级
     * @param zOrder Z-Order 值（越大越靠前）
     */
    void setLocalZOrder(int zOrder) { _localZOrder = zOrder; }

    /**
     * @brief 获取卡牌的内容尺寸
     * @return 卡牌尺寸
     */
    cocos2d::Size getContentSize() const { return _contentSize; }
    
    /**
     * @brief 设置卡牌的内容尺寸
     * @param size 卡牌尺寸
     */
    void setContentSize(const cocos2d::Size& size) { _contentSize = size; }

    /**
     * @brief 获取卡牌所在的列索引（用于遮挡检测）
     * @return 列索引，-1 表示未分配
     */
    int getColumnIndex() const { return _columnIndex; }
    
    /**
     * @brief 设置卡牌所在的列索引
     * @param column 列索引
     */
    void setColumnIndex(int column) { _columnIndex = column; }

    // ==================== 序列化支持（预留） ====================
    
    /**
     * @brief 序列化为字典
     * @return 包含卡牌数据的字典
     * @note 用于存档功能
     */
    cocos2d::ValueMap serialize() const;
    
    /**
     * @brief 从字典反序列化
     * @param data 包含卡牌数据的字典
     */
    void deserialize(const cocos2d::ValueMap& data);

private:
    CardModel();
    ~CardModel();
    
    /**
     * @brief 初始化卡牌模型
     * @param id 卡牌唯一标识
     * @param suit 花色
     * @param face 点数
     * @return 初始化成功返回 true
     */
    bool init(int id, CardSuitType suit, CardFaceType face);

    int _id;                          ///< 卡牌唯一标识
    CardSuitType _suit;               ///< 花色
    CardFaceType _face;               ///< 点数
    cocos2d::Vec2 _position;          ///< 位置坐标
    bool _isFaceUp;                   ///< 是否正面朝上
    int _localZOrder;                 ///< 渲染层级
    cocos2d::Size _contentSize;       ///< 内容尺寸
    int _columnIndex;                 ///< 列索引（用于遮挡检测）
};

#endif // __CARD_MODEL_H__