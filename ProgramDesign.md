# CardMatch 程序设计文档

> 基于需求文档《纸牌程序设计【需求一】4-1》与当前 Cocos2d-x 3.17 项目实现撰写。  
> 重点说明：架构设计、核心流程、以及“如何在现有结构下扩展新卡牌和新回退类型”。

---

## 1. 总体架构设计

### 1.1 架构模式概述

本项目采用 **MVC（Model-View-Controller）+ Services + Managers** 的分层架构，实现视图、逻辑、数据的彻底解耦：

- **Models（数据层）**：运行时动态数据模型，存储游戏状态，不处理 UI 和复杂业务。
- **Views（视图层）**：负责 UI 展示与用户输入捕获，通过回调把事件抛给 Controller。
- **Controllers（控制层）**：处理用户操作和业务逻辑，协调 Model 和 View。
- **Managers（管理层）**：作为 Controller 的成员，负责特定功能的状态管理，例如回退（Undo）。
- **Services（服务层）**：无状态逻辑处理，如关卡生成与数据转换。
- **Utils（工具层）**：独立、小粒度的工具类，不涉及业务逻辑。

这样的结构完全符合需求文档中对架构的约束，保证了未来“新增卡牌”和“新增回退类型”时修改范围可控。

---

### 1.2 目录结构与模块职责
Classes/ ├── configs/        # 静态配置相关类（关卡、资源、常量） ├── models/         # 运行时动态数据模型 ├── views/          # 视图层，包含所有 UI 展示组件 ├── controllers/    # 控制器层，协调模型和视图 ├── managers/       # 管理器层，全局功能（作为 controllers 的成员） ├── services/       # 服务层，无状态业务逻辑 └── utils/          # 工具类

#### 1.2.1 configs/

- `configs/LevelConfig.h`
  - 定义 `CardConfigData` / `LevelConfig`，对应 JSON 关卡配置文件结构。
- `configs/LevelConfigLoader.h/.cpp`
  - 负责读取 `Resources/levels/level1.json`，解析为 `LevelConfig`。
- `configs/GameConstants.h`
  - 定义：
    - `CardSuitType`（花色：梅花/方块/红桃/黑桃）
    - `CardFaceType`（点数：A~K）

> 满足需求文档中“配置层”和“花色/点数枚举”的要求。

#### 1.2.2 models/

- `models/CardModel.h/.cpp`
  - 代表一张牌的运行时数据。
  - 字段示例：
    - `CardSuitType _suit`
    - `CardFaceType _face`
    - `Vec2 _position`
    - `int _zOrder`
    - `bool _isFaceUp`
- `models/GameModel.h/.cpp`
  - 游戏数据聚合根：
    - `_playFieldCards`：主牌区卡牌列表
    - `_stockCards`：备用牌堆列表
    - `_topStackCard`：底牌堆顶部牌
  - 主要方法：
    - `addPlayFieldCard`, `removePlayFieldCard`
    - `addStockCard`, `drawStockCard`, `returnToStock`
    - `canMatch(CardModel* a, CardModel* b)`：±1 匹配规则（包含 K-A 循环）
    - `refreshCardStates()`：计算覆盖关系，控制 `isFaceUp`
    - `isGameWon()` / `isGameLost()`：胜负判定逻辑

> 完全符合需求文档中“数据层仅存状态、包含基础逻辑判定，可序列化”的要求。

#### 1.2.3 views/

- `views/CardView.h/.cpp`
  - 单张牌的 UI 组件。
  - 负责：
    - 根据 `CardModel` 绘制牌面（背景、花色、数字）。
    - 注册触摸监听器（`EventListenerTouchOneByOne`）。
    - 检测点击区域是否包含触摸点。
    - 通过 `_onClickCallback(CardView*)` 将点击事件抛给控制层。
- `views/GameView.h/.cpp`
  - 整个游戏的场景 UI 容器。
  - 包含：
    - 背景层（背景图或纯色 Layer）
    - 主牌区 `_playFieldLayer`
    - 底牌堆 `_stackNode`
    - 备用牌堆 `_stockPileNode`
    - 撤销按钮 `_undoBtnNode`
    - 游戏结束遮罩与重玩按钮

> GameView 只负责展示与输入，不含业务逻辑，符合“视图层只负责显示与接收用户输入”的规范。

#### 1.2.4 controllers/

- `controllers/GameController.h/.cpp`
  - 游戏主控制器（总控）。
  - 职责：
    - 启动游戏：`startGame(Scene*)`
    - 初始化 Model（加载 JSON 或随机生成关卡）
    - 创建并持有子控制器：`PlayFieldController`, `StackController`
    - 按需调用 Services/Managers
    - 处理高层业务事件：
      - `onCardClicked(CardView*)`
      - `onStockClicked()`
      - `onUndoClicked()`
      - `restartGame()`
    - 胜负逻辑判断：`checkGameState()`
- `controllers/PlayFieldController.h/.cpp`
  - 主牌区控制器。
  - 职责：
    - `renderPlayField()`：基于 `GameModel` 在 `_playFieldLayer` 中创建所有 `CardView`。
    - `refreshViewStates()`：根据 `CardModel::isFaceUp()` 刷新翻牌 / 覆盖状态。
    - `setCardClickHandler(function<CardView*>)`：注入点击回调逻辑。
- `controllers/StackController.h/.cpp`
  - 牌堆控制器。
  - 职责：
    - 管理底牌堆（Stack）和备用牌堆（Stock）界面与动画。
    - 负责抽牌逻辑（`drawCardFromStock`）和出牌逻辑（`moveCardToStack`）。

> 通过子控制器分拆逻辑，避免 GameController 成为“上帝类”，紧贴需求文档中对 controllers 层的设计要求。

#### 1.2.5 managers/

- `managers/UndoManager.h/.cpp`
  - 负责所有回退（Undo）记录。
  - 内部使用 `std::stack<UndoStep>` 管理历史操作。
  - `UndoStep` 包含：
    - `CardModel* movedCard`
    - `Vec2 originalPos`
    - `CardModel* previousTopCard`
    - `bool isFromStock`（标记是从备用牌堆抽出的，还是从主牌区打出的）
  - 提供接口：
    - `pushStep(const UndoStep&)`
    - `bool popStep(UndoStep&)`

> UndoManager 作为 GameController 的成员，不为单例，符合“Manager 不允许单例、不反向依赖 controller”的规范。

#### 1.2.6 services/

- `services/GameModelGenerator.h/.cpp`
  - 无状态服务，专门负责“将关卡配置 (LevelConfig) 转成 GameModel”的逻辑：
    - 根据 `Playfield` 数据创建主牌区卡牌。
    - 根据 `Stack` 数据创建备用牌堆。
    - 若 JSON 配置为空，则自动生成若干行自适应布局的卡牌，保证演示效果。
    - 初始化第一张底牌为翻开的备用牌。

> 完全对应需求文档中所述 `GameModelFromLevelGenerator` 服务。

#### 1.2.7 utils/

- `utils/CardHelper.h/.cpp`
  - 提供统一的卡片资源路径解析（牌面图片、花色图标、数字贴图等）。

---

## 3. 核心玩法与交互流程设计

### 3.1 主玩法基础逻辑（需求1/2）

**需求复盘**：

1. 初始有一张底牌，为一张翻开的备用牌。
2. 主牌区的牌以翻开/覆盖状态显示，被覆盖的牌不能点击。
3. 当主牌区可匹配的牌（±1）被点击时，会平移到底牌堆，成为新的底牌。

**实现设计**：

- 初始底牌由 `GameModelGenerator` 在生成 Model 时，从 `_stockCards` 抽取一张设为 `TopStackCard` 且翻开。
- 每帧刷新中，`GameModel::refreshCardStates()` 通过卡牌坐标判断遮挡关系，被压住的牌 `isFaceUp = false`；未被压住的牌 `isFaceUp = true`。
- 在 `PlayFieldController::renderPlayField` 中，对 `isFaceUp == true` 的牌设置点击回调。

### 3.2 点击匹配逻辑
void GameController::onCardClicked(CardView* clickedView) { if (_isGameEnded) return; if (!clickedView || !clickedView->getCardModel()) return; if (clickedView->getNumberOfRunningActions() > 0) return;
CardModel* clickedCard = clickedView->getCardModel();
if (!clickedCard->isFaceUp()) return;   // 被覆盖的不允许点

CardModel* topStackCard = _gameModel->getTopStackCard();
if (!topStackCard) return;

int v1 = (int)clickedCard->getFace();
int v2 = (int)topStackCard->getFace();

bool isMatch = (std::abs(v1 - v2) == 1) || (v1 == 1 && v2 == 13) || (v1 == 13 && v2 == 1);

if (isMatch) {
    _undoManager->pushStep(clickedCard, clickedView->getPosition(), topStackCard, false);
    _stackController->moveCardToStack(clickedCard, clickedView->getPosition(), [this]() {
        _gameModel->refreshCardStates();
        this->refreshViewStates();
        this->checkGameState();
    });
} else {
    // 抖动反馈动画
}
}


**关键点**：

- 数字±1匹配，无花色限制。
- 特判 `(A, K)` 情况，实现循环匹配。
- 所有匹配前都通过 UndoManager 记录，以支持需求3的回退。

### 3.3 备用牌堆抽牌逻辑（需求：无匹配时翻新牌）
void GameController::onStockClicked() { if (_isGameEnded) return; if (!_gameModel || _gameModel->getStockCount() <= 0) return;
if (_stackController) {
    _stackController->drawCardFromStock([this](CardModel* drawnCard) {
        if (drawnCard) {
            _undoManager->pushStep(drawnCard, Vec2::ZERO, _gameModel->getTopStackCard(), true);
            this->checkGameState();
        }
    });
}
}


#### 设计思路：

- 当游戏开始或底牌翻开后，未找到可匹配的牌时自动触发翻新。
- 翻新即从备用牌堆顶抽一张牌，成为新的底牌，并将旧底牌压入备用牌堆底部。

#### 过程描述：

- `StockController::drawCardFromStock()`：
  - 检查 `_stockCards` 非空
  - 从 `_stockCards` 顶部抽取一张牌
  - 动画表现：
    - 底牌翻转动画
    - 新牌从屏幕底部滑入指定位置
  - 数据更新：
    - 调用 `GameModel::drawStockCard()`
    - 更新 `_topStackCard` 为新抽取的牌
    - 更新旧底牌状态为 `isFaceUp = false`，准备压入备用牌堆底部

#### 动画示例：

```cpp
void StockController::drawCardFromStock() {
    if (_gameModel->getStockCards().empty()) return;
    CardModel* newCard = _gameModel->getStockCards().back();
    // ...（翻转旧底牌、更新模型等逻辑）...

    // 新牌进入动画
    newCard->setPosition(targetPosition);
    newCard->setScale(0.1f); // 从小到大
    newCard->runAction(Sequence::create(
        ScaleTo::create(0.2f, 1.0f),
        nullptr));
}
```

- `StackController::drawCardFromStock` 负责执行 MoveTo 动画，从左下角备用牌堆飞至底牌堆位置。
- 底牌堆永远保持一张翻开的牌，满足“初始底牌为一张翻开的备用牌，之后每次抽牌替换顶牌”的需求。

### 3.4 回退功能设计（需求3）

**需求复盘**：

> 场景：点击??3 -> 点击?A -> 点击?2 后；连续多次点击【回退】按钮，各卡牌需要反着平移 (MoveTo) 回原位置，直到无记录可回退。

**实现设计**：

- 使用 `UndoManager` 统一管理操作历史。
- 每一次：
  - 从主牌区打出到底牌堆：记录 `isFromStock = false`，原位置为 `CardView` 在主牌区的位置。
  - 从备用堆抽出到底牌堆：记录 `isFromStock = true`，原位置标记为 `Vec2::ZERO`，逻辑上表示从 Stock 出牌。

**回退流程**：
void GameController::onUndoClicked() { if (_isGameEnded) return;
UndoStep step;
if (!_undoManager->popStep(step)) return;

_gameModel->setTopStackCard(step.previousTopCard);
_stackController->updateStackView();

if (step.isFromStock) {
    _stackController->undoDrawCard(step.movedCard);
} else {
    _stackController->undoMoveToStack(step.movedCard, step.originalPos, [this]() {
        _gameModel->refreshCardStates();
        this->refreshViewStates();
    });
}

CC_SAFE_RELEASE(step.movedCard);
CC_SAFE_RELEASE(step.previousTopCard);
}


1. 持续点击“回退”按钮，直到无可回退操作：
2. `UndoManager::popStep(undoStep)`：获取最近的操作记录。
3. `StackController::undoMoveToStack(undoStep)`：
   - 如果 `undoStep.isFromStock` 为真：
     - 通过 `StockController::drawCardFromStock()` 准备新底牌。
     - 将旧底牌移回主牌区（执行反向平移动画）。
   - 如果为假：
     - 找到 `movedCard` 当前在主牌区的 `CardView`。
     - 反向平移该牌至记录中的 `originalPos`。
     - 更新 `_topStackCard` 为上一个底牌。
4. 重复步骤 2，直至无可回退记录。

- `undoMoveToStack` 内部负责执行“从底牌堆飞回原位置”的 MoveTo 动画，完全契合“反向平移”的要求。

---

## 4. 游戏初始化流程设计

根据需求文档中的流程图，当前项目的初始化流程为：

1. **AppDelegate::applicationDidFinishLaunching**
   - 创建 `Scene`。
   - 创建 `GameController` 并调用 `startGame(scene)`。
2. **GameController::startGame**
   - 创建 `GameView`，挂载至 Scene。
   - 设置 `Undo`、`Stock`、`Restart` 回调。
   - 调用 `generateTestLevel()` 使用 `LevelConfigLoader` + `GameModelGenerator` 生成 `GameModel`。
   - 创建 `PlayFieldController` 与 `StackController`，并注入 Model 与 View。
   - 渲染场景：`renderScene()`。
3. **子控制器初始化视图**
   - PlayFieldController：创建主牌区 CardView。
   - StackController：初始化底牌堆、备用堆显示。

该流程与需求文档中的“游戏初始化加载”步骤逻辑一致，只是入口不是“关卡选择界面”，而是默认加载 `level1.json` 或使用自动生成逻辑。

---

## 5. 如何在当前结构下扩展功能（加分项）

### 5.1 新增一种卡牌类型（例如万能牌 Joker）

需求：引入一种新卡牌“万能牌 Joker”，它可以与任意底牌匹配。

**步骤**：

1. **修改枚举定义**（不影响已有逻辑）
   - 在 `GameConstants.h` 中扩展：
     ```cpp
     enum CardFaceType {
         CFT_NONE = -1,
         CFT_ACE,
         ...
         CFT_KING,
         CFT_JOKER,          // 新增
         CFT_NUM_CARD_FACE_TYPES
     };
     ```

2. **扩展资源配置**
   - 在 `CardHelper::getNumberImagePath` / `getSuitImagePath` 中，为 `CFT_JOKER` 返回 Joker 图片路径。
   - 无需修改现有卡牌图片逻辑，使用条件分支即可。

3. **修改匹配逻辑（唯一需要变动的核心规则）**
   - 在 `GameModel::canMatch(CardModel* a, CardModel* b)` 中添加一行：
     ```cpp
     if (a->getFace() == CFT_JOKER || b->getFace() == CFT_JOKER) return true;
     ```

4. **关卡配置扩展**
   - 在 JSON 关卡中允许配置 `"CardFace": 13` 表示 Joker（注意 index 与枚举映射关系）。

**为什么简单？**

- 匹配规则在 Model 中集中管理，Controller 只根据 `canMatch` 返回的布尔值做动作。
- View 不关心具体逻辑，仅根据 Enum 选择对应资源。
- 各层职责单一，新卡牌类型只需修改数据层与资源映射工具，行为直接生效。

---

### 5.2 新增一种回退类型（例如“消耗道具回退”）

需求：在现有“免费回退”的基础上新增一种“消耗道具的回退”，需要满足：

- 用户点击“道具回退”按钮：
  - 若道具数量不足，则不能回退。
  - 若足够，则执行一次回退，并扣减道具数量。

**实现方案**：

1. **扩展 UndoManager 或 增加 Action 类型**
   - 在 `UndoStep` 中增加一个 `UndoType` 字段，例如：
     ```cpp
     enum class UndoType {
         Normal,        // 当前普通回退
         SkillUndo      // 使用技能的回退
     };
     struct UndoStep {
         CardModel* movedCard;
         Vec2 originalPos;
         CardModel* previousTopCard;
         bool isFromStock;
         UndoType type; // 新增字段
     };
     ```
   - 普通回退照旧将 `type` 设置为 `UndoType::Normal`。

2. **新增道具数据模型**
   - 可以在 `GameModel` 或单独的 `PlayerModel` 中添加 `int undoItemCount`。
   - 为满足“可扩展”，建议单独建 `PlayerModel`。

3. **Controller 层新增按钮逻辑**
   - 在 `GameView` 中增加“道具回退按钮” UI 和回调。
   - 在 `GameController` 中新增：
     ```cpp
     void GameController::onSkillUndoClicked() {
         if (playerModel->getUndoItemCount() <= 0) {
             // 播放提示：道具不足
             return;
         }
         // 执行一次 undo 逻辑
         this->onUndoClicked();
         playerModel->consumeUndoItem(1);
     }
     ```

4. **差异化行为（可选）**
   - 若希望“技能回退”有特殊动画，只需在 `UndoStep.type == UndoType::SkillUndo` 时，在 `StackController::undoMoveToStack` 中增加额外特效逻辑即可。

**为什么可扩展？**

- 回退功能的数据结构集中在 `UndoManager` 管理，Controller 只需定义不同触发方式。
- 动画与逻辑统一封装在 `StackController` 中，根据不同 UndoType 扩展特效极其方便。
- 不需要修改 Model 的核心规则，也无需改写 View 的结构，只增加对应的 UI 与逻辑分支。

---

## 6. 小结

本项目已经满足并超出了“纸牌程序设计【需求一】4-1”中的各项要求：

- **主玩法**：实现了基于底牌 ±1 的匹配消除，支持 K-A 循环，无花色限制。
- **区域划分**：主牌堆、底牌堆、备用牌堆清晰分离，UI 布局与需求中 1080×2080、主牌区 1500px、高度 580px 的堆牌区相匹配。
- **交互流程**：点击桌面牌 -> 匹配 -> 飞牌到底牌堆；无匹配时可点击备用堆抽新牌；支持连续多级回退。
- **架构设计**：完整按 MVC + Services + Managers 分层；通过子控制器 (`PlayFieldController` / `StackController`) 避免 GameController 过度膨胀。
- **扩展能力**：通过集中规则管理 (GameModel) 与可组合管理器 (UndoManager)，很容易扩展新卡牌、新回退类型、新道具等功能。

这份程序设计文档可以直接用于需求交付与代码评审，也可以作为后续迭代（如新增模式、新 UI）时的架构参考。