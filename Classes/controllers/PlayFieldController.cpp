#include "PlayFieldController.h"

USING_NS_CC;

PlayFieldController::PlayFieldController()
    : _gameModel(nullptr)
    , _gameView(nullptr) {
}

PlayFieldController::~PlayFieldController() {
    CC_SAFE_RELEASE(_gameModel);
    CC_SAFE_RELEASE(_gameView);
}

PlayFieldController* PlayFieldController::create(GameModel* gameModel, GameView* gameView) {
    auto ctrl = new (std::nothrow) PlayFieldController();
    if (ctrl && ctrl->init(gameModel, gameView)) {
        ctrl->autorelease();
        return ctrl;
    }
    CC_SAFE_DELETE(ctrl);
    return nullptr;
}

bool PlayFieldController::init(GameModel* gameModel, GameView* gameView) {
    if (!gameModel || !gameView) return false;

    _gameModel = gameModel;
    _gameView = gameView;

    CC_SAFE_RETAIN(_gameModel);
    CC_SAFE_RETAIN(_gameView);

    return true;
}

void PlayFieldController::setCardClickHandler(const std::function<void(CardView*)>& handler) {
    _cardClickHandler = handler;
}

void PlayFieldController::renderPlayField() {
    if (!_gameView || !_gameModel) return;

    auto playFieldLayer = _gameView->getPlayFieldLayer();
    if (!playFieldLayer) return;

    playFieldLayer->removeAllChildren();

    const auto& cards = _gameModel->getPlayFieldCards();
    for (auto cardModel : cards) {
        auto cardView = CardView::createWithCardModel(cardModel);
        if (!cardView) continue;

        cardView->setPosition(cardModel->getPosition());
        cardView->setLocalZOrder(cardModel->getLocalZOrder());
        cardView->flip(cardModel->isFaceUp());

        if (cardModel->isFaceUp() && _cardClickHandler) {
            cardView->setOnClickCallback(_cardClickHandler);
        }

        playFieldLayer->addChild(cardView);
    }
}

void PlayFieldController::refreshViewStates() {
    if (!_gameView) return;
    auto layer = _gameView->getPlayFieldLayer();
    if (!layer) return;

    auto children = layer->getChildren();
    for (auto node : children) {
        auto view = dynamic_cast<CardView*>(node);
        if (!view || !view->getCardModel()) continue;

        bool isFaceUp = view->getCardModel()->isFaceUp();
        view->flip(isFaceUp);
        if (isFaceUp && _cardClickHandler) {
            view->setOnClickCallback(_cardClickHandler);
        }
        else {
            view->setOnClickCallback(nullptr);
        }
    }
}

void PlayFieldController::setGameModel(GameModel* gameModel) {
    if (_gameModel != gameModel) {
        CC_SAFE_RELEASE(_gameModel);
        _gameModel = gameModel;
        CC_SAFE_RETAIN(_gameModel);
    }
}