// --------------------------------------------------------
// 文件名: Classes/controllers/GameController.cpp
// --------------------------------------------------------
#include "GameController.h"
#include "configs/LevelConfigLoader.h"
#include "services/GameModelGenerator.h"
#include "services/LevelGenerator.h"
#include "utils/CardHelper.h"
#include <cmath>

USING_NS_CC;

GameController::GameController() 
    : _gameModel(nullptr)
    , _gameView(nullptr)
    , _undoManager(nullptr)
    , _playFieldController(nullptr)
    , _stackController(nullptr)
    , _isGameEnded(false) {}

GameController::~GameController() {
    CC_SAFE_RELEASE(_gameModel);
    CC_SAFE_RELEASE(_playFieldController);
    CC_SAFE_RELEASE(_stackController);
    if (_undoManager) delete _undoManager;
}

GameController* GameController::create() {
    GameController* controller = new GameController();
    if (controller && controller->init()) {
        controller->autorelease();
        return controller;
    }
    CC_SAFE_DELETE(controller);
    return nullptr;
}

bool GameController::init() {
    _gameModel = nullptr;
    _gameView = nullptr;
    _undoManager = new UndoManager();
    _playFieldController = nullptr;
    _stackController = nullptr;
    _isGameEnded = false;
    return true;
}

void GameController::startGame(cocos2d::Scene* scene) {
    _gameView = GameView::create();
    if (_gameView) {
        scene->addChild(_gameView);
        _gameView->setUndoCallback(CC_CALLBACK_0(GameController::onUndoClicked, this));
        _gameView->setStockCallback(CC_CALLBACK_0(GameController::onStockClicked, this));
        _gameView->setRestartCallback(CC_CALLBACK_0(GameController::restartGame, this));
    }

    generateTestLevel();
    createSubControllers();
    renderScene();
}

void GameController::createSubControllers() {
    CC_SAFE_RELEASE(_playFieldController);
    CC_SAFE_RELEASE(_stackController);

    _playFieldController = PlayFieldController::create(_gameModel, _gameView);
    _stackController = StackController::create(_gameModel, _gameView);

    if (_playFieldController) {
        _playFieldController->retain();
        _playFieldController->setCardClickHandler(CC_CALLBACK_1(GameController::onCardClicked, this));
    }
    if (_stackController) {
        _stackController->retain();
    }
}

void GameController::generateTestLevel() {
    CC_SAFE_RELEASE(_gameModel);
    
    _gameModel = GameModel::create();
    if (!_gameModel) return;
    _gameModel->retain();

    LevelConfig config = LevelConfigLoader::loadLevelConfig("level1.json");

    if (config.playfieldCards.empty()) {
        CCLOG("Config empty. Using default pyramid layout.");
        config = LevelGenerator::generateDefaultPyramid();  // ? 使用新服务
    }

    GameModelGenerator::generate(_gameModel, config);
    _gameModel->refreshCardStates();
}

void GameController::renderScene() {
    if (!_gameView || !_gameModel) return;

    if (_playFieldController) {
        _playFieldController->renderPlayField();
    }

    if (_stackController) {
        _stackController->updateStackView();
        _stackController->updateStockView();
    }
}

void GameController::refreshViewStates() {
    if (_playFieldController) {
        _playFieldController->refreshViewStates();
    }
}

void GameController::checkGameState() {
    if (_isGameEnded) return;

    if (_gameModel->isGameWon()) {
        _isGameEnded = true;
        CCLOG("GAME WON!");
        _gameView->showGameOverUI(true);
    }
    else if (_gameModel->isGameLost()) {
        _isGameEnded = true;
        CCLOG("GAME LOST - NO MOVES!");
        _gameView->showGameOverUI(false);
    }
}

void GameController::restartGame() {
    CCLOG("Restarting game...");
    
    _isGameEnded = false;
    _undoManager->clear();
    
    generateTestLevel();
    
    if (_playFieldController) {
        _playFieldController->setGameModel(_gameModel);
    }
    if (_stackController) {
        _stackController->setGameModel(_gameModel);
    }
    
    renderScene();
}

void GameController::onCardClicked(CardView* clickedView) {
    if (_isGameEnded) return;
    if (!clickedView || !clickedView->getCardModel()) return;
    if (clickedView->getNumberOfRunningActions() > 0) return;

    CardModel* clickedCard = clickedView->getCardModel();
    if (!clickedCard->isFaceUp()) return;

    CardModel* topStackCard = _gameModel->getTopStackCard();
    if (!topStackCard) return;

    if (isCardMatchable(clickedCard, topStackCard)) {
        handleMatchedCard(clickedCard, clickedView);
    }
    else {
        playMismatchAnimation(clickedView);
    }
}

bool GameController::isCardMatchable(CardModel* card1, CardModel* card2) {
    int v1 = (int)card1->getFace();
    int v2 = (int)card2->getFace();
    return (std::abs(v1 - v2) == 1) || (v1 == 1 && v2 == 13) || (v1 == 13 && v2 == 1);
}

void GameController::handleMatchedCard(CardModel* clickedCard, CardView* clickedView) {
    CardModel* topStackCard = _gameModel->getTopStackCard();
    
    // 记录撤销步骤
    _undoManager->pushStep(clickedCard, clickedView->getPosition(), topStackCard, false);

    // 执行动画
    if (_stackController) {
        _stackController->moveCardToStack(clickedCard, clickedView->getPosition(), [this]() {
            this->_gameModel->refreshCardStates();
            this->refreshViewStates();
            this->checkGameState();
        });
    }
}

void GameController::playMismatchAnimation(CardView* cardView) {
    cardView->runAction(Sequence::create(
        MoveBy::create(0.05f, Vec2(10, 0)),
        MoveBy::create(0.05f, Vec2(-20, 0)),
        MoveBy::create(0.05f, Vec2(10, 0)), nullptr));
}

void GameController::onStockClicked() {
    if (_isGameEnded) return;
    if (!_gameModel || _gameModel->getStockCount() <= 0) return;

    if (_stackController) {
        _stackController->drawCardFromStock([this](CardModel* drawnCard) {
            if (drawnCard) {
                _undoManager->pushStep(drawnCard, Vec2::ZERO, _gameModel->getTopStackCard(), true);
                this->checkGameState();
            }
        });
    }
}

void GameController::onUndoClicked() {
    if (_isGameEnded) return;

    UndoStep step;
    if (!_undoManager->popStep(step)) return;

    // 恢复旧的栈顶
    _gameModel->setTopStackCard(step.previousTopCard);
    if (_stackController) {
        _stackController->updateStackView();
    }

    if (step.isFromStock) {
        handleUndoFromStock(step);
    }
    else {
        handleUndoFromPlayField(step);
    }

    CC_SAFE_RELEASE(step.movedCard);
    CC_SAFE_RELEASE(step.previousTopCard);
}

void GameController::handleUndoFromStock(const UndoStep& step) {
    if (_stackController) {
        _stackController->undoDrawCard(step.movedCard);
    }
}

void GameController::handleUndoFromPlayField(const UndoStep& step) {
    if (_stackController) {
        _stackController->undoMoveToStack(step.movedCard, step.originalPos, [this]() {
            this->_gameModel->refreshCardStates();
            this->refreshViewStates();
        });
    }
}