#include "GameModelGenerator.h"
#include "configs/GameConstants.h"

USING_NS_CC;

void GameModelGenerator::generate(GameModel* model, const LevelConfig& config) {
    if (!model) return;

    // 1. 场地卡牌
    int id = 0;
    for (const auto& data : config.playfieldCards) {
        auto card = CardModel::create(id, (CardSuitType)data.suit, (CardFaceType)data.face);
        card->setPosition(Vec2(data.x, data.y));
        
        // 【核心】Z-Order 根据 Y 坐标计算
        // Y 越小（越靠下），Z 序越高（显示在上层，遮挡其他牌）
        card->setLocalZOrder((int)(3000 - data.y));
        
        // 卡牌大小（用于碰撞检测）
        card->setContentSize(Size(150 * 3.5f, 210 * 3.5f));  // 525 x 735
        
        // 初始都为背面，后续由 refreshCardStates 决定
        card->setFaceUp(false);
        
        model->addPlayFieldCard(card);
        id++;
    }

    // 2. 生成备用牌
    for (const auto& data : config.stackCards) {
        auto card = CardModel::create(1000 + id++, (CardSuitType)data.suit, (CardFaceType)data.face);
        card->setFaceUp(false);
        model->addStockCard(card);
    }

    // 3. 初始底牌翻开
    if (model->getStockCount() > 0) {
        CardModel* start = model->drawStockCard();
        start->setFaceUp(true);
        model->setTopStackCard(start);
    }

    // 4. 【关键】根据几何遮挡设置卡牌朝向
    model->refreshCardStates();
}