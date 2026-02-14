#include "CardHelper.h"

// 获取卡牌基础背景路径
std::string CardHelper::getCardBasePath() {
    return "card_general.png";
}

// 获取花色图片路径
std::string CardHelper::getSuitImagePath(CardSuitType suit) {
    std::string suitStr;
    switch (suit) {
    case CST_CLUBS: suitStr = "club"; break;       // 梅花
    case CST_DIAMONDS: suitStr = "diamond"; break; // 方块
    case CST_HEARTS: suitStr = "heart"; break;     // 红桃
    case CST_SPADES: suitStr = "spade"; break;     // 黑桃
    default: return "suits/club.png";
    }
    return "suits/" + suitStr + ".png";
}

// 获取数字图片路径
std::string CardHelper::getNumberImagePath(CardFaceType face, bool isBig, bool isBlack) {
    std::string faceStr;
    int faceNum = (int)face;

    // 将枚举转为对应的文字
    switch (faceNum) {
    case 1: faceStr = "A"; break;
    case 11: faceStr = "J"; break;
    case 12: faceStr = "Q"; break;
    case 13: faceStr = "K"; break;
    default: faceStr = std::to_string(faceNum); break;
    }

    // 确定颜色和大小
    std::string sizePrefix = isBig ? "big_" : "small_";
    std::string colorPrefix = isBlack ? "black_" : "red_";

    return "number/" + sizePrefix + colorPrefix + faceStr + ".png";
}

// 主要方法：获取卡牌完整路径
std::string CardHelper::getCardFrameName(CardSuitType suit, CardFaceType face) {
    // 这里返回一个占位符，实际需要在CardView中组合多个精灵
    return "card_general.png";
}

// 获取卡背图片路径
std::string CardHelper::getCardBackFrameName() {
    return "card_general.png";
}