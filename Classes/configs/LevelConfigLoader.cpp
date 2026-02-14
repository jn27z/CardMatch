#include "LevelConfigLoader.h"
#include "json/document.h"
#include "cocos2d.h"

USING_NS_CC;

namespace {
    /**
     * @brief 解析单个卡牌配置数据
     * @param jsonObj JSON对象
     * @param includePosition 是否包含位置信息
     * @return 卡牌配置数据
     */
    CardConfigData parseCardData(const rapidjson::Value& jsonObj, bool includePosition) {
        CardConfigData data;
        // CardFace 从0开始，需要+1转换为1-13的范围
        data.face = jsonObj["CardFace"].GetInt() + 1;
        data.suit = jsonObj["CardSuit"].GetInt();
        
        if (includePosition && jsonObj.HasMember("Position")) {
            data.x = jsonObj["Position"]["x"].GetFloat();
            data.y = jsonObj["Position"]["y"].GetFloat();
        } else {
            data.x = 0;
            data.y = 0;
        }
        return data;
    }

    /**
     * @brief 解析卡牌数组
     * @param jsonArray JSON数组
     * @param includePosition 是否包含位置信息
     * @return 卡牌配置数据列表
     */
    std::vector<CardConfigData> parseCardArray(const rapidjson::Value& jsonArray, bool includePosition) {
        std::vector<CardConfigData> cards;
        for (rapidjson::SizeType i = 0; i < jsonArray.Size(); i++) {
            cards.push_back(parseCardData(jsonArray[i], includePosition));
        }
        return cards;
    }
}

LevelConfig LevelConfigLoader::loadLevelConfig(const std::string& filename) {
    LevelConfig config;

    // 读取文件内容
    std::string content = FileUtils::getInstance()->getStringFromFile(filename);
    if (content.empty()) {
        CCLOG("Config file not found: %s", filename.c_str());
        return config;
    }

    // 解析JSON
    rapidjson::Document doc;
    doc.Parse(content.c_str());
    if (doc.HasParseError()) {
        CCLOG("JSON parse error in file: %s", filename.c_str());
        return config;
    }

    // 解析游戏区卡牌（包含位置）
    if (doc.HasMember("Playfield") && doc["Playfield"].IsArray()) {
        config.playfieldCards = parseCardArray(doc["Playfield"], true);
    }

    // 解析堆叠区卡牌（不含位置）
    if (doc.HasMember("Stack") && doc["Stack"].IsArray()) {
        config.stackCards = parseCardArray(doc["Stack"], false);
    }

    return config;
}