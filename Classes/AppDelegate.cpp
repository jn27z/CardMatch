// --------------------------------------------------------
// 文件名: Classes/AppDelegate.cpp
// --------------------------------------------------------
#include "AppDelegate.h"
#include "controllers/GameController.h"  
#include "HelloWorldScene.h"

// 引入 Windows 头文件以使用 OutputDebugStringA
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <windows.h>
#endif
#include <stdio.h>
#include <stdarg.h> // 引入可变参数头文件

// #define USE_AUDIO_ENGINE 1
// #define USE_SIMPLE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif

USING_NS_CC;

// DebugLog 函数实现
void DebugLog(const char* format, ...) {
    char buffer[2048];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // 1. 输出到 Cocos 控制台
    cocos2d::log("%s", buffer);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    // 2. 输出到 Visual Studio 的 "输出" 窗口
    OutputDebugStringA(buffer);
    OutputDebugStringA("\n");
#endif
}

static cocos2d::Size designResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate::AppDelegate()
    : _gameController(nullptr)
{
}

AppDelegate::~AppDelegate() {
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::end();
#endif
    // 释放控制器
    CC_SAFE_RELEASE(_gameController);
}

void AppDelegate::initGLContextAttrs() {
    GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8, 0 };
    GLView::setGLContextAttrs(glContextAttrs);
}

static int register_all_packages() {
    return 0;
}

bool AppDelegate::applicationDidFinishLaunching() {
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        // 设置窗口大小
        glview = GLViewImpl::createWithRect("CardMatch", cocos2d::Rect(0, 0, 1080, 2080), 0.5f);
#else
        glview = GLViewImpl::create("CardMatch");
#endif
        director->setOpenGLView(glview);
    }

    director->setDisplayStats(true);
    director->setAnimationInterval(1.0f / 60);

    // 设置设计分辨率
    glview->setDesignResolutionSize(1080, 2080, ResolutionPolicy::FIXED_WIDTH);
    auto frameSize = glview->getFrameSize();

    if (frameSize.height > mediumResolutionSize.height) {
        director->setContentScaleFactor(MIN(largeResolutionSize.height / designResolutionSize.height,
            largeResolutionSize.width / designResolutionSize.width));
    }
    else if (frameSize.height > smallResolutionSize.height) {
        director->setContentScaleFactor(MIN(mediumResolutionSize.height / designResolutionSize.height,
            mediumResolutionSize.width / designResolutionSize.width));
    }
    else {
        director->setContentScaleFactor(MIN(smallResolutionSize.height / designResolutionSize.height,
            smallResolutionSize.width / designResolutionSize.width));
    }

    register_all_packages();

    // 设置资源路径 - 使用正确的相对路径
    FileUtils::getInstance()->addSearchPath("Resources");
    FileUtils::getInstance()->addSearchPath("res");
    FileUtils::getInstance()->addSearchPath("number");
    FileUtils::getInstance()->addSearchPath("levels");  // 不带 Resources/ 前缀

    DebugLog("=== AppDelegate: 应用启动 ===");

    auto scene = Scene::create();
    DebugLog("=== AppDelegate: 场景已创建 ===");

    // 创建控制器并启动游戏
    _gameController = GameController::create();
    if (_gameController) {
        _gameController->retain(); // 必须 retain，防止被自动释放
        DebugLog("=== AppDelegate: GameController 已创建，调用 startGame ===");
        _gameController->startGame(scene);
    }
    else {
        DebugLog("=== AppDelegate: GameController 创建失败 ===");
        auto defaultScene = HelloWorld::createScene();
        director->runWithScene(defaultScene);
        return false;
    }

    director->runWithScene(scene);
    DebugLog("=== AppDelegate: 场景已运行 ===");

    return true;
}

void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();
#endif
}

void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif
}