// --------------------------------------------------------
// 文件名: Classes/views/CardView.cpp
// --------------------------------------------------------
#include "CardView.h"
#include "utils/CardHelper.h"

USING_NS_CC;

CardView::CardView()
    : _cardBase(nullptr), _suitSprite(nullptr), _numberTop(nullptr), _numberBottom(nullptr), _cardModel(nullptr), _onClickCallback(nullptr)
{
}

CardView::~CardView() {
    CC_SAFE_RELEASE(_cardModel);
}

CardView* CardView::createWithCardModel(CardModel* cardModel) {
    CardView* view = new CardView();
    if (view && view->initWithCardModel(cardModel)) {
        view->autorelease();
        return view;
    }
    CC_SAFE_DELETE(view);
    return nullptr;
}

bool CardView::initWithCardModel(CardModel* cardModel) {
    if (!Node::init()) return false;

    CC_SAFE_RELEASE(_cardModel);
    _cardModel = cardModel;
    CC_SAFE_RETAIN(_cardModel);

    this->setAnchorPoint(Vec2(0.5f, 0.5f));
    this->setIgnoreAnchorPointForPosition(false);

    std::string bgPath = CardHelper::getCardBasePath();
    _cardBase = Sprite::create(bgPath);
    if (!_cardBase) {
        _cardBase = Sprite::create();
        _cardBase->setTextureRect(Rect(0, 0, 150, 210));
    }

    // 【修改】统一设置为 3.0 倍
    float cardScale = 3.0f;
    _cardBase->setScale(cardScale);

    Size scaledSize = _cardBase->getContentSize() * cardScale;
    this->setContentSize(scaledSize);

    _cardBase->setPosition(scaledSize.width / 2, scaledSize.height / 2);
    this->addChild(_cardBase);

    // 花色和数字（代码保持不变）
    if (_cardModel) {
        std::string suitPath = CardHelper::getSuitImagePath(_cardModel->getSuit());
        _suitSprite = Sprite::create(suitPath);
        if (_suitSprite) {
            _suitSprite->setPosition(_cardBase->getContentSize().width / 2, _cardBase->getContentSize().height / 2);
            float s = (_cardBase->getContentSize().width / 3.0f) / _suitSprite->getContentSize().width;
            _suitSprite->setScale(s);
            _cardBase->addChild(_suitSprite, 1);
        }

        bool isBlack = (_cardModel->getSuit() == CST_CLUBS || _cardModel->getSuit() == CST_SPADES);

        std::string numPath = CardHelper::getNumberImagePath(_cardModel->getFace(), true, isBlack);
        _numberTop = Sprite::create(numPath);
        if (_numberTop) {
            _numberTop->setPosition(_cardBase->getContentSize().width * 0.15f, _cardBase->getContentSize().height * 0.85f);
            _numberTop->setScale(0.4f);
            _cardBase->addChild(_numberTop, 2);
        }
    }

    this->flip(_cardModel ? _cardModel->isFaceUp() : false);

    // 触摸监听
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(CardView::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(CardView::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void CardView::setOnClickCallback(const std::function<void(CardView*)>& callback) {
    _onClickCallback = callback;
}

bool CardView::onTouchBegan(Touch* touch, Event* event) {
    if (_cardModel && !_cardModel->isFaceUp()) return false;
    if (!_cardBase) return false;

    Vec2 locationInNode = this->convertTouchToNodeSpace(touch);
    Size s = this->getContentSize();
    Rect rect = Rect(0, 0, s.width, s.height);

    if (rect.containsPoint(locationInNode)) {
        // 【修复】按下缩小到 2.85f（标准 3.0f 的 95%）
        _cardBase->setScale(2.85f);
        return true;
    }
    return false;
}

void CardView::onTouchEnded(Touch* touch, Event* event) {
    // 【修复】恢复到标准大小 3.0f
    _cardBase->setScale(3.0f);
    if (_onClickCallback) {
        _onClickCallback(this);
    }
}

void CardView::flip(bool showFront) {
    if (_suitSprite) _suitSprite->setVisible(showFront);
    if (_numberTop) _numberTop->setVisible(showFront);

    if (_cardModel) {
        _cardModel->setFaceUp(showFront);
    }

    if (!showFront) {
        _cardBase->setColor(Color3B(200, 220, 255));
    }
    else {
        _cardBase->setColor(Color3B::WHITE);
    }
}

void CardView::setCardPosition(const cocos2d::Vec2& position) {
    this->setPosition(position);
}