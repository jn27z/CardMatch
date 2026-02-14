// --------------------------------------------------------
// 文件名: Classes/models/GameModel.cpp
// --------------------------------------------------------
#include "GameModel.h"
#include <cmath>

USING_NS_CC;

GameModel::GameModel() : _topStackCard(nullptr) {}

GameModel::~GameModel() {
    CC_SAFE_RELEASE(_topStackCard);
    _playFieldCards.clear();
    _stockCards.clear();
}

GameModel* GameModel::create() {
    GameModel* model = new (std::nothrow) GameModel();
    if (model && model->init()) {
        model->autorelease();
        return model;
    }
    CC_SAFE_DELETE(model);
    return nullptr;
}

bool GameModel::init() {
    _playFieldCards.clear();
    _stockCards.clear();
    setTopStackCard(nullptr);
    return true;
}

// 基础操作
void GameModel::addPlayFieldCard(CardModel* card) { if (card) _playFieldCards.pushBack(card); }
void GameModel::removePlayFieldCard(CardModel* card) { if (card) _playFieldCards.eraseObject(card); }
void GameModel::setTopStackCard(CardModel* card) {
    if (_topStackCard != card) {
        CC_SAFE_RELEASE(_topStackCard);
        _topStackCard = card;
        CC_SAFE_RETAIN(_topStackCard);
    }
}
void GameModel::addStockCard(CardModel* card) { if (card) _stockCards.pushBack(card); }
CardModel* GameModel::drawStockCard() {
    if (_stockCards.empty()) return nullptr;
    CardModel* card = _stockCards.back();
    card->retain();
    _stockCards.eraseObject(card);
    return card;
}
void GameModel::returnToStock(CardModel* card) { if (card) _stockCards.pushBack(card); }
int GameModel::getStockCount() const { return _stockCards.size(); }

// ---------------------------------------------------------------------
// 【核心算法】几何遮挡检测
// ---------------------------------------------------------------------

bool GameModel::canMatch(CardModel* cardA, CardModel* cardB) {
    if (!cardA || !cardB) return false;
    int v1 = (int)cardA->getFace();
    int v2 = (int)cardB->getFace();
    if (std::abs(v1 - v2) == 1) return true;
    if ((v1 == 1 && v2 == 13) || (v1 == 13 && v2 == 1)) return true;
    return false;
}

bool GameModel::isCardBlocked(CardModel* card, const cocos2d::Vec2& cardPos, 
                               const std::map<CardModel*, cocos2d::Vec2>& allCardsPos) {
    if (!card) return false;
    
    // 当前牌的碰撞矩形（使用 70% 的尺寸，只检测中心区域的重叠）
    Size cardSize = card->getContentSize() * 0.7f;
    Rect cardRect(
        cardPos.x - cardSize.width / 2,
        cardPos.y - cardSize.height / 2,
        cardSize.width,
        cardSize.height
    );

    // 遍历所有其他牌
    for (auto otherCard : _playFieldCards) {
        if (otherCard == card) continue;

        auto it = allCardsPos.find(otherCard);
        if (it == allCardsPos.end()) continue;
        
        Vec2 otherPos = it->second;
        
        // 【关键】只有 Y 坐标更小（更靠下、显示在上层）的牌才能遮挡当前牌
        // 同一行（Y 坐标相同或接近）的牌不会互相遮挡
        if (otherPos.y >= cardPos.y - 10.0f) continue;  // 10px 容差
        
        Size otherSize = otherCard->getContentSize() * 0.7f;
        Rect otherRect(
            otherPos.x - otherSize.width / 2,
            otherPos.y - otherSize.height / 2,
            otherSize.width,
            otherSize.height
        );

        // 矩形相交 = 被遮挡
        if (cardRect.intersectsRect(otherRect)) {
            return true;
        }
    }
    return false;
}

// ===================== 核心函数：刷新所有卡牌状态 =====================
void GameModel::refreshCardStates() {
    if (_playFieldCards.empty()) return;

    // 1. 收集所有卡牌的位置
    std::map<CardModel*, Vec2> allCardsPos;
    for (auto card : _playFieldCards) {
        if (card) {
            allCardsPos[card] = card->getPosition();
        }
    }

    // 2. 遍历所有卡牌，根据遮挡状态设置朝向
    for (auto card : _playFieldCards) {
        if (!card) continue;
        
        bool blocked = isCardBlocked(card, card->getPosition(), allCardsPos);
        
        // 被遮挡 = 背面（不可点击），未被遮挡 = 正面（可点击）
        card->setFaceUp(!blocked);
    }
}

// ===================== 游戏状态判定 =====================
bool GameModel::isGameWon() {
    return _playFieldCards.empty();
}

bool GameModel::isGameLost() {
    if (_playFieldCards.empty()) return false;

    CardModel* topCard = getTopStackCard();
    if (!topCard) return false;

    // 检查是否有翻开的牌能匹配
    for (auto card : _playFieldCards) {
        if (card && card->isFaceUp()) {
            if (canMatch(card, topCard)) {
                return false;
            }
        }
    }

    return _stockCards.empty();
}