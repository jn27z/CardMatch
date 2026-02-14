#include "StackController.h"
#include "views/CardView.h"

USING_NS_CC;

StackController::StackController()
    : _gameModel(nullptr)
    , _gameView(nullptr) {
}

StackController::~StackController() {
    CC_SAFE_RELEASE(_gameModel);
    CC_SAFE_RELEASE(_gameView);
}

StackController* StackController::create(GameModel* gameModel, GameView* gameView) {
    auto ctrl = new (std::nothrow) StackController();
    if (ctrl && ctrl->init(gameModel, gameView)) {
        ctrl->autorelease();
        return ctrl;
    }
    CC_SAFE_DELETE(ctrl);
    return nullptr;
}

bool StackController::init(GameModel* gameModel, GameView* gameView) {
    if (!gameModel || !gameView) return false;

    _gameModel = gameModel;
    _gameView = gameView;

    CC_SAFE_RETAIN(_gameModel);
    CC_SAFE_RETAIN(_gameView);
    return true;
}

void StackController::updateStackView() {
    if (!_gameView || !_gameModel) return;
    _gameView->updateStackView(_gameModel->getTopStackCard());
}

void StackController::updateStockView() {
    if (!_gameView || !_gameModel) return;
    _gameView->updateStockView(_gameModel->getStockCount());
}

void StackController::drawCardFromStock(const std::function<void(CardModel*)>& onComplete) {
    if (!_gameModel || _gameModel->getStockCount() <= 0) {
        if (onComplete) onComplete(nullptr);
        return;
    }

    CardModel* drawnCard = _gameModel->drawStockCard();
    if (!drawnCard) {
        if (onComplete) onComplete(nullptr);
        return;
    }
    drawnCard->setFaceUp(true);

    auto cardView = CardView::createWithCardModel(drawnCard);
    Vec2 startPos = _gameView->getPlayFieldLayer()->convertToNodeSpace(_gameView->getStockPosition());
    Vec2 endPos = _gameView->getPlayFieldLayer()->convertToNodeSpace(_gameView->getStackPosition());

    cardView->setPosition(startPos);
    cardView->flip(true);
    _gameView->getPlayFieldLayer()->addChild(cardView, 2000);

    auto move = MoveTo::create(0.4f, endPos);
    auto callback = CallFunc::create([this, drawnCard, cardView, onComplete]() {
        this->_gameModel->setTopStackCard(drawnCard);
        this->updateStackView();
        this->updateStockView();
        cardView->removeFromParent();
        
        if (onComplete) onComplete(drawnCard);
        drawnCard->release();
    });
    
    cardView->runAction(Sequence::create(move, callback, nullptr));
}

void StackController::moveCardToStack(CardModel* card, const Vec2& fromPos, 
                                      const std::function<void()>& onComplete) {
    if (!card || !_gameView) {
        if (onComplete) onComplete();
        return;
    }

    card->retain();

    auto playFieldLayer = _gameView->getPlayFieldLayer();
    if (!playFieldLayer) {
        card->release();
        if (onComplete) onComplete();
        return;
    }

    CardView* cardView = nullptr;
    for (auto node : playFieldLayer->getChildren()) {
        auto view = dynamic_cast<CardView*>(node);
        if (view && view->getCardModel() == card) {
            cardView = view;
            break;
        }
    }

    if (!cardView) {
        card->release();
        if (onComplete) onComplete();
        return;
    }

    cardView->setLocalZOrder(2000);
    Vec2 targetPos = playFieldLayer->convertToNodeSpace(_gameView->getStackPosition());

    auto move = MoveTo::create(0.4f, targetPos);
    auto callback = CallFuncN::create([this, card, onComplete](Node* sender) {
        CardView* viewToRemove = dynamic_cast<CardView*>(sender);
        
        this->_gameModel->removePlayFieldCard(card);
        this->_gameModel->setTopStackCard(card);
        this->updateStackView();
        
        if (viewToRemove) viewToRemove->removeFromParent();
        
        if (onComplete) onComplete();
        card->release();
    });
    
    cardView->runAction(Sequence::create(move, callback, nullptr));
}

void StackController::undoDrawCard(CardModel* card) {
    if (!card || !_gameModel) return;
    
    _gameModel->returnToStock(card);
    updateStockView();
}

void StackController::undoMoveToStack(CardModel* card, const Vec2& originalPos, 
                                      const std::function<void()>& onComplete) {
    if (!card || !_gameView) {
        if (onComplete) onComplete();
        return;
    }

    _gameModel->addPlayFieldCard(card);

    auto cardView = CardView::createWithCardModel(card);
    Vec2 startPos = _gameView->getPlayFieldLayer()->convertToNodeSpace(_gameView->getStackPosition());
    cardView->setPosition(startPos);
    cardView->setLocalZOrder(2000);
    cardView->flip(true);
    _gameView->getPlayFieldLayer()->addChild(cardView);

    auto move = MoveTo::create(0.3f, originalPos);
    auto callback = CallFunc::create([this, cardView, card, onComplete]() {
        cardView->setLocalZOrder(card->getLocalZOrder());
        
        if (onComplete) onComplete();
    });
    cardView->runAction(Sequence::create(move, callback, nullptr));
}

void StackController::setGameModel(GameModel* gameModel) {
    if (_gameModel != gameModel) {
        CC_SAFE_RELEASE(_gameModel);
        _gameModel = gameModel;
        CC_SAFE_RETAIN(_gameModel);
    }
}