#include "LevelGenerator.h"
#include <cstdlib>

LevelConfig LevelGenerator::generateDefaultPyramid() {
    LevelConfig config;
    generatePyramidCards(config);
    generateStockCards(config, 24);
    return config;
}

void LevelGenerator::generatePyramidCards(LevelConfig& config) {
    const float kScreenWidth = 1080.0f;
    const float kScreenCenterX = kScreenWidth / 2.0f;
    const float kStartY = 1750.0f;
    const float kCardWidth = 150.0f * 3.0f;
    const float kCardHeight = 210.0f * 3.0f;
    const int kPyramidRows = 5;
    const float kRowVerticalSpacing = kCardHeight * 0.35f;
    const float kCardHorizontalSpacing = kCardWidth * 0.55f;

    for (int row = 0; row < kPyramidRows; row++) {
        int cardsInRow = row + 1;
        float rowWidth = (cardsInRow - 1) * kCardHorizontalSpacing;
        float startX = kScreenCenterX - rowWidth / 2.0f;
        float y = kStartY - row * kRowVerticalSpacing;

        for (int col = 0; col < cardsInRow; col++) {
            float x = startX + col * kCardHorizontalSpacing;
            config.playfieldCards.push_back(createRandomCard(x, y));
        }
    }
}

void LevelGenerator::generateStockCards(LevelConfig& config, int count) {
    for (int i = 0; i < count; i++) {
        config.stackCards.push_back(createRandomCard(0, 0));
    }
}

CardConfigData LevelGenerator::createRandomCard(float x, float y) {
    CardConfigData data;
    data.face = (rand() % 13) + 1;
    data.suit = (rand() % 4);
    data.x = x;
    data.y = y;
    return data;
}