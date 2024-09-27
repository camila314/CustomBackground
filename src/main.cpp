#include <Geode/Geode.hpp>
#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/modify/CCScale9Sprite.hpp>

using namespace geode::prelude;
using cocos2d::extension::CCScale9Sprite;
using cocos2d::CCSprite;
class $modify(CCScale9Sprite) {
	struct Fields {
		bool m_hasFixed = false;
	};

	void visit() {
		CCScale9Sprite::visit();

		if (!m_fields->m_hasFixed) {
			fixSprites();
			m_fields->m_hasFixed = true;
		}
	}

	void transformSprite(CCSprite* sprite, CCSize transform, int idx) {
		auto tRect = sprite->getTextureRect();

		tRect.size -= transform;

		if (idx == 0)
			sprite->setPosition(sprite->getPosition() + transform);
		else if (idx == 1 && (transform.width > 0 || transform.height > 0))
			sprite->setVisible(false);
		else if (idx == 2)
			tRect.origin += transform;

		sprite->setTextureRect(tRect);
	}

	void fixSprites() {
		CCSprite* sprites[3][3] = {
			{_topLeft, _top, _topRight},
			{_left, _centre, _right},
			{_bottomLeft, _bottom, _bottomRight}
		};

		auto halfSize = getContentSize() / 2;

		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				auto sprite = sprites[i][j];

				if (sprite == nullptr)
					return;

				auto tRect = sprite->getTextureRect();
				auto diff = tRect.size - halfSize;
				diff = CCSize(fmax(0, diff.width), fmax(0, diff.height));

				transformSprite(sprite, CCSize(diff.width, 0), j);
				transformSprite(sprite, CCSize(0, diff.height), i);
			}
		}
	}
};

class $modify(EditLevelLayer) {
	bool init(GJGameLevel* gl) {
		EditLevelLayer::init(gl);

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

			changeOpaq("level-name-background", 105);
			changeOpaq("description-background", 85);

			auto inp = static_cast<CCTextInputNode*>(getChildByID("description-input"));
			if (inp) {
				col = ccc3(
					std::min(col.r + 30, 255),
					std::min(col.g + 30, 255),
					std::min(col.b + 30, 255)
				);

				Loader::get()->queueInMainThread([=]() {
					inp->m_placeholderColor = col;

					if (inp->getString() == "")
						inp->m_textArea->colorAllLabels(col);
				});
			} else {
				log::error("Failed to find node: description-input");
			}

		}

		return true;
	}
};

class $modify(LevelSearchLayer) {
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

class $modify(CCSprite) {
	static CCSprite* create(char const* name) {
		auto sprite = CCSprite::create(name);

		if (strcmp(name, "GJ_gradientBG.png") == 0) {
			auto mod = Mod::get();
			auto col = mod->getSettingValue<ccColor3B>("color");

			if (mod->getSettingValue<bool>("use_image")) {
				auto img = mod->getSettingValue<std::filesystem::path>("image");

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



