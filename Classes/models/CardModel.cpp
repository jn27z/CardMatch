// --------------------------------------------------------
// 文件名: Classes/models/CardModel.cpp
// --------------------------------------------------------
#include "CardModel.h"

USING_NS_CC;

CardModel::CardModel() 
    : _id(-1)
    , _suit(CST_NONE)
    , _face(CFT_NONE)
    , _isFaceUp(false)
    , _columnIndex(-1)
    , _localZOrder(0) {
}

CardModel::~CardModel() {
    // Cocos2d-x 会自动管理内存
}

CardModel* CardModel::create(int id, CardSuitType suit, CardFaceType face) {
    CardModel* model = new (std::nothrow) CardModel();
    if (model && model->init(id, suit, face)) {
        model->autorelease();
        return model;
    }
    CC_SAFE_DELETE(model);
    return nullptr;
}

bool CardModel::init(int id, CardSuitType suit, CardFaceType face) {
    _id = id;
    _suit = suit;
    _face = face;
    _isFaceUp = false;
    _columnIndex = -1;
    _position = Vec2::ZERO;
    _localZOrder = 0;      
    _contentSize = Size::ZERO; 
    return true;
}

cocos2d::ValueMap CardModel::serialize() const {
    ValueMap data;
    data["id"] = _id;
    data["suit"] = (int)_suit;
    data["face"] = (int)_face;
    data["x"] = _position.x;
    data["y"] = _position.y;
    data["faceUp"] = _isFaceUp;
    data["zOrder"] = _localZOrder;
    data["column"] = _columnIndex;
    return data;
}

void CardModel::deserialize(const cocos2d::ValueMap& data) {
    _id = data.at("id").asInt();
    _suit = (CardSuitType)data.at("suit").asInt();
    _face = (CardFaceType)data.at("face").asInt();
    _position.x = data.at("x").asFloat();
    _position.y = data.at("y").asFloat();
    _isFaceUp = data.at("faceUp").asBool();
    _localZOrder = data.at("zOrder").asInt();
    _columnIndex = data.at("column").asInt();
}