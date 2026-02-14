// Classes/managers/UndoManager.cpp
#include "UndoManager.h"

USING_NS_CC;

UndoManager::UndoManager() {}

UndoManager::~UndoManager() {
    clear();
}

void UndoManager::pushStep(CardModel* movedCard, const Vec2& originalPos, CardModel* prevTop, bool fromStock) {
    UndoStep step;
    step.movedCard = movedCard;
    step.originalPos = originalPos;
    step.previousTopCard = prevTop;
    step.isFromStock = fromStock;

    // 增加引用计数，防止对象被自动回收
    if (step.movedCard) step.movedCard->retain();
    if (step.previousTopCard) step.previousTopCard->retain();

    _undoStack.push_back(step);
}

bool UndoManager::popStep(UndoStep& outStep) {
    if (_undoStack.empty()) return false;

    outStep = _undoStack.back();
    _undoStack.pop_back();

    // 注意：这里弹出了 step，但里面的对象引用计数还在，需要由调用者（Controller）负责释放或重新管理
    // Manager 只负责从栈中移除，不负责 release，因为 Controller 还需要用这些对象恢复状态

    return true;
}

void UndoManager::clear() {
    for (const auto& step : _undoStack) {
        CC_SAFE_RELEASE(step.movedCard);
        CC_SAFE_RELEASE(step.previousTopCard);
    }
    _undoStack.clear();
}