// --------------------------------------------------------
// 文件名: Classes/views/GameView.cpp
// --------------------------------------------------------
#include "GameView.h"
#include "utils/CardHelper.h"
#include "views/CardView.h"

USING_NS_CC;

GameView* GameView::create() {
    GameView* view = new (std::nothrow) GameView();
    if (view && view->init()) {
        view->autorelease();
        return view;
    }
    CC_SAFE_DELETE(view);
    return nullptr;
}

bool GameView::init() {
    if (!Layer::init()) return false;

    // 拆分为多个小函数
    setupBackground();
    setupBottomPanel();
    setupPlayField();
    setupStackArea();
    setupStockPile();
    setupUndoButton();

    return true;
}

// ===================== 私有初始化函数 =====================

void GameView::setupBackground() {
    auto director = Director::getInstance();
    Size visibleSize = director->getVisibleSize();
    Vec2 origin = director->getVisibleOrigin();

    auto bg = Sprite::create("kitty.jpg");
    if (bg) {
        bg->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
        float sx = visibleSize.width / bg->getContentSize().width;
        float sy = visibleSize.height / bg->getContentSize().height;
        bg->setScale(std::max(sx, sy));
        this->addChild(bg, -10);
    }
    else {
        auto layer = LayerColor::create(Color4B(255, 240, 245, 255));
        this->addChild(layer, -10);
    }
}

void GameView::setupBottomPanel() {
    auto director = Director::getInstance();
    Size visibleSize = director->getVisibleSize();
    Vec2 origin = director->getVisibleOrigin();

    // 底部操作区 (580px)
    float bottomHeight = 580.0f;
    auto bottomPanel = LayerColor::create(Color4B(255, 255, 255, 180), visibleSize.width, bottomHeight);
    bottomPanel->setPosition(origin.x, origin.y);
    this->addChild(bottomPanel, 0);

    // 顶部分割线
    auto line = LayerColor::create(Color4B(200, 200, 200, 255), visibleSize.width, 2);
    line->setPosition(origin.x, origin.y + bottomHeight);
    this->addChild(line, 1);
}

void GameView::setupPlayField() {
    auto director = Director::getInstance();
    Vec2 origin = director->getVisibleOrigin();

    _playFieldLayer = Node::create();
    _playFieldLayer->setPosition(origin);
    this->addChild(_playFieldLayer, 10);
}

void GameView::setupStackArea() {
    auto director = Director::getInstance();
    Size visibleSize = director->getVisibleSize();
    Vec2 origin = director->getVisibleOrigin();

    float bottomHeight = 580.0f;
    float centerX = origin.x + visibleSize.width / 2.0f;
    float bottomCenterY = origin.y + bottomHeight / 2.0f;

    // 底牌堆 (中心)
    _stackNode = Node::create();
    _stackNode->setPosition(Vec2(centerX, bottomCenterY));
    this->addChild(_stackNode, 5);

    // 底牌底座
    auto stackBase = Sprite::create();
    stackBase->setTextureRect(Rect(0, 0, 150, 210));
    stackBase->setColor(Color3B::BLACK);
    stackBase->setOpacity(15);
    stackBase->setScale(3.0f);  
    _stackNode->addChild(stackBase, -1);
}

void GameView::setupStockPile() {
    createStockPileSprites();      // 创建牌背堆叠
    setupStockTouchListener();     // 设置触摸监听
    setupStockCountLabel();        // 设置数量标签
}

void GameView::createStockPileSprites() {
    auto director = Director::getInstance();
    Size visibleSize = director->getVisibleSize();
    Vec2 origin = director->getVisibleOrigin();

    float bottomHeight = 580.0f;
    float centerX = origin.x + visibleSize.width / 2.0f;
    float bottomCenterY = origin.y + bottomHeight / 2.0f;
    float stockStartX = centerX - 420.0f;

    // 备用牌堆容器
    _stockPileNode = Node::create();
    _stockPileNode->setPosition(Vec2(stockStartX, bottomCenterY));
    this->addChild(_stockPileNode, 5);

    // 创建牌背堆叠效果
    for (int i = 0; i < 5; i++) {
        auto sp = Sprite::create(CardHelper::getCardBasePath());
        if (!sp) { 
            sp = Sprite::create(); 
            sp->setTextureRect(Rect(0, 0, 150, 210)); 
        }

        sp->setScale(3.0f); 
        sp->setPosition(Vec2(i * 10.0f, 0));

        if (i == 4) {
            _stockSprite = sp;
            sp->setColor(Color3B::WHITE);
        }
        else {
            int gray = 200 + i * 10;
            sp->setColor(Color3B(gray, gray, gray));
        }

        _stockPileNode->addChild(sp, i);
    }
}

void GameView::setupStockTouchListener() {
    auto stockListener = EventListenerTouchOneByOne::create();
    stockListener->setSwallowTouches(true);
    stockListener->onTouchBegan = CC_CALLBACK_2(GameView::onStockTouchBegan, this);
    stockListener->onTouchEnded = CC_CALLBACK_2(GameView::onStockTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(stockListener, _stockPileNode);
}

bool GameView::onStockTouchBegan(cocos2d::Touch* t, cocos2d::Event* e) {
    if (!_stockPileNode || !_stockPileNode->isVisible()) return false;

    Vec2 loc = _stockPileNode->convertToNodeSpace(t->getLocation());
    Rect bounds = calculateStockTouchBounds();

    CCLOG("Stock touch at: (%.1f, %.1f), bounds: (%.1f, %.1f, %.1f, %.1f)", 
          loc.x, loc.y, bounds.origin.x, bounds.origin.y, 
          bounds.size.width, bounds.size.height);

    if (bounds.containsPoint(loc)) {
        _stockPileNode->runAction(ScaleTo::create(0.1f, 0.95f));
        return true;
    }
    return false;
}

void GameView::onStockTouchEnded(cocos2d::Touch* t, cocos2d::Event* e) {
    _stockPileNode->runAction(ScaleTo::create(0.1f, 1.0f));
    if (_stockCallback) {
        CCLOG("Stock callback triggered!");
        _stockCallback();
    } else {
        CCLOG("Stock callback is NULL!");
    }
}

cocos2d::Rect GameView::calculateStockTouchBounds() const {
    const float kCardWidth = 150.0f * 3.0f;
    const float kCardHeight = 210.0f * 3.0f;
    const float kTotalWidth = kCardWidth + 4 * 10.0f;
    
    return Rect(-kCardWidth / 2.0f, -kCardHeight / 2.0f, kTotalWidth, kCardHeight);
}

void GameView::setupStockCountLabel() {
    // 数量标签
    auto countBg = Sprite::create();
    countBg->setTextureRect(Rect(0, 0, 60, 40));
    countBg->setColor(Color3B::BLACK);
    countBg->setOpacity(150);
    countBg->setPosition(Vec2(20, -150));
    _stockPileNode->addChild(countBg, 10);

    _stockCountLabel = Label::createWithSystemFont("0", "Arial", 28);
    _stockCountLabel->setPosition(countBg->getContentSize() / 2);
    countBg->addChild(_stockCountLabel);
}

void GameView::setupUndoButton() {
    auto director = Director::getInstance();
    Size visibleSize = director->getVisibleSize();
    Vec2 origin = director->getVisibleOrigin();

    float bottomHeight = 580.0f;
    float centerX = origin.x + visibleSize.width / 2.0f;
    float bottomCenterY = origin.y + bottomHeight / 2.0f;

    _undoBtnNode = Node::create();
    _undoBtnNode->setPosition(Vec2(centerX + 420, bottomCenterY));
    this->addChild(_undoBtnNode, 5);

    auto btnBg = Sprite::create();
    btnBg->setTextureRect(Rect(0, 0, 160, 70));
    btnBg->setColor(Color3B(255, 105, 180));
    _undoBtnNode->addChild(btnBg);

    auto undoLabel = Label::createWithSystemFont("UNDO", "Arial", 36);
    undoLabel->enableBold();
    _undoBtnNode->addChild(undoLabel);

    auto undoListener = EventListenerTouchOneByOne::create();
    undoListener->setSwallowTouches(true);
    undoListener->onTouchBegan = [this, btnBg](Touch* t, Event* e) {
        Vec2 loc = btnBg->convertTouchToNodeSpace(t);
        Rect rect(0, 0, btnBg->getContentSize().width, btnBg->getContentSize().height);
        
        if (rect.containsPoint(loc)) {
            _undoBtnNode->runAction(ScaleTo::create(0.1f, 0.9f));
            return true;
        }
        return false;
    };
    undoListener->onTouchEnded = [this](Touch* t, Event* e) {
        _undoBtnNode->runAction(ScaleTo::create(0.1f, 1.0f));
        if (_undoCallback) _undoCallback();
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(undoListener, btnBg);
}

// ===================== 公共接口函数 =====================

Vec2 GameView::getStackPosition() { 
    return _stackNode->getParent()->convertToWorldSpace(_stackNode->getPosition()); 
}

Vec2 GameView::getStockPosition() { 
    return _stockSprite->getParent()->getParent()->convertToWorldSpace(
        _stockSprite->getParent()->getPosition() + _stockSprite->getPosition()
    ); 
}

void GameView::updateStockView(int count) {
    if (count <= 0) {
        _stockPileNode->setVisible(false);
    }
    else {
        _stockPileNode->setVisible(true);
        _stockCountLabel->setString(std::to_string(count));
        
        // 根据剩余数量动态调整显示的牌背数量
        int visibleCards = std::min(count, 5);
        auto children = _stockPileNode->getChildren();
        for (int i = 0; i < 5; i++) {
            if (i < (int)children.size() && dynamic_cast<Sprite*>(children.at(i))) {
                children.at(i)->setVisible(i < visibleCards);
            }
        }
    }
}

void GameView::setUndoCallback(const std::function<void()>& cb) { _undoCallback = cb; }
void GameView::setStockCallback(const std::function<void()>& cb) { _stockCallback = cb; }
void GameView::setRestartCallback(const std::function<void()>& cb) { _restartCallback = cb; }

void GameView::updateStackView(CardModel* topCard) {
    _stackNode->removeAllChildren();

    // 补回底座
    auto stackBase = Sprite::create();
    stackBase->setTextureRect(Rect(0, 0, 150, 210));
    stackBase->setColor(Color3B::BLACK);
    stackBase->setOpacity(15);
    stackBase->setScale(3.0f);  
    _stackNode->addChild(stackBase, -1);

    if (topCard) {
        auto cardView = CardView::createWithCardModel(topCard);
        cardView->setPosition(Vec2::ZERO);
        cardView->flip(true);
        cardView->setOnClickCallback(nullptr);
        _stackNode->addChild(cardView);
    }
}

void GameView::setUndoButtonEnabled(bool enabled) {
    if (_undoBtnNode) {
        _undoBtnNode->setOpacity(enabled ? 255 : 100);
    }
}

void GameView::showGameOverUI(bool win) {
    auto mask = createGameOverMask();
    addGameOverTitle(mask, win);
    addGameOverRestartButton(mask);
    playGameOverAnimation(mask);
}

// ===================== 游戏结束界面相关 =====================

cocos2d::LayerColor* GameView::createGameOverMask() {
    auto mask = LayerColor::create(Color4B(0, 0, 0, 180));
    this->addChild(mask, 100);

    // 添加触摸监听，阻止点击穿透
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch*, Event*) { return true; };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, mask);

    return mask;
}

void GameView::addGameOverTitle(cocos2d::LayerColor* mask, bool win) {
    auto director = Director::getInstance();
    Size visibleSize = director->getVisibleSize();
    Vec2 origin = director->getVisibleOrigin();

    // 主标题
    std::string titleText = win ? "VICTORY!" : "NO MOVES!";
    auto label = Label::createWithSystemFont(titleText, "Arial", 80);
    label->enableBold();
    label->setColor(win ? Color3B::YELLOW : Color3B(200, 200, 200));
    label->setPosition(origin.x + visibleSize.width / 2, 
                       origin.y + visibleSize.height / 2 + 100);
    label->setTag(101); // 用于动画
    mask->addChild(label);

    // 副标题
    std::string subText = win ? "Congratulations!" : "Try Again?";
    auto subLabel = Label::createWithSystemFont(subText, "Arial", 36);
    subLabel->setColor(Color3B::WHITE);
    subLabel->setPosition(origin.x + visibleSize.width / 2, 
                          origin.y + visibleSize.height / 2);
    mask->addChild(subLabel);
}

void GameView::addGameOverRestartButton(cocos2d::LayerColor* mask) {
    auto director = Director::getInstance();
    Size visibleSize = director->getVisibleSize();
    Vec2 origin = director->getVisibleOrigin();

    // 重玩按钮背景
    auto restartBg = Sprite::create();
    restartBg->setTextureRect(Rect(0, 0, 280, 80));
    restartBg->setColor(Color3B(255, 105, 180));
    restartBg->setPosition(origin.x + visibleSize.width / 2, 
                           origin.y + visibleSize.height / 2 - 120);
    mask->addChild(restartBg);

    // 按钮文字
    auto restartLabel = Label::createWithSystemFont("Play Again", "Arial", 40);
    restartLabel->enableBold();
    restartLabel->setPosition(restartBg->getContentSize() / 2);
    restartBg->addChild(restartLabel);

    // 按钮点击事件
    auto restartListener = EventListenerTouchOneByOne::create();
    restartListener->setSwallowTouches(true);
    
    restartListener->onTouchBegan = [restartBg](Touch* t, Event* e) {
        Vec2 loc = restartBg->convertTouchToNodeSpace(t);
        Rect rect(0, 0, restartBg->getContentSize().width, 
                        restartBg->getContentSize().height);
        if (rect.containsPoint(loc)) {
            restartBg->runAction(ScaleTo::create(0.1f, 0.9f));
            return true;
        }
        return false;
    };
    
    restartListener->onTouchEnded = [this, mask, restartBg](Touch* t, Event* e) {
        restartBg->runAction(ScaleTo::create(0.1f, 1.0f));
        if (_restartCallback) {
            _restartCallback();
        }
        mask->removeFromParent();
    };
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(restartListener, restartBg);
}

void GameView::playGameOverAnimation(cocos2d::LayerColor* mask) {
    // 遮罩淡入
    mask->setOpacity(0);
    mask->runAction(FadeTo::create(0.3f, 180));

    // 标题缩放弹出
    auto label = mask->getChildByTag(101);
    if (label) {
        label->setScale(0);
        label->runAction(Sequence::create(
            DelayTime::create(0.1f),
            EaseBackOut::create(ScaleTo::create(0.4f, 1.0f)),
            nullptr
        ));
    }
}

GameView::~GameView() {
    // Cocos2d-x 会自动管理所有 child nodes
    // 无需手动清理
}