#include <Geode/Geode.hpp>
#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

using namespace geode::prelude;
using cocos2d::extension::CCScale9Sprite;

#define $ $modify

class $(LevelSearchLayer) {
	bool init(int i) {
		LevelSearchLayer::init(i);
		auto mod = Mod::get();

		if (mod->getSettingValue<bool>("search_layer")) {
			auto col = mod->getSettingValue<ccColor3B>("color");

			auto changeOpaq = [=](char const* name, unsigned char opac) {
				auto spr = static_cast<CCScale9Sprite*>(getChildByID(name));
				if (spr) {
					spr->setColor(ccc3(0, 0, 0));
					spr->setOpacity(opac);
				} else {
					log::error("Failed to find sprite: {}", name);
				}
			};

			changeOpaq("length-filters-bg", 104);
			changeOpaq("difficulty-filters-bg", 101);
			changeOpaq("quick-search-bg", 101);
			changeOpaq("level-search-bg", 105);
			changeOpaq("level-search-bar-bg", 80);
		}

		return true;
	}
};

class $(CCSprite) {
	static CCSprite* create(char const* name) {
		auto sprite = CCSprite::create(name);

		if (strcmp(name, "GJ_gradientBG.png") == 0) {
			auto mod = Mod::get();
			auto col = mod->getSettingValue<ccColor3B>("color");

			if (mod->getSettingValue<bool>("use_image")) {
				auto img = mod->getSettingValue<ghc::filesystem::path>("image");

				auto spr2 = CCSprite::create(img.string().c_str());
				if (spr2)
					sprite = spr2;
				else
					log::error("Failed to load image: {}", img.string().c_str());
			}

			Loader::get()->queueInMainThread([=] {
				if (sprite->getColor() == ccc3(0, 102, 255))
					sprite->setColor(col);
			});

		}

		return sprite;
	}
};



