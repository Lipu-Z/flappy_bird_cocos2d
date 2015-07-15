#include "WatchOutScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;

Scene* WatchOut::createScene() {
    auto scene = Scene::create();
    
    auto layer = WatchOut::create();

    scene->addChild(layer);

    return scene;
}

bool WatchOut::init() {
	if ( !Layer::init() )
    {
        return false;
    }
	//���plist�������
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("watchout.plist", "watchout.png");
	//Ԥ������Ч�ļ�
	preloadMusic();
	//��ʼ��View
	setupView();
	return true;
}

void WatchOut::setupView() {
	//���ڴ�С
	Size winSize = Director::getInstance()->getVisibleSize();

	//�Ѵ��ڷ�Ϊ5�ȷ�
	float winSize_5 = winSize.height / 5;
	/*
	�ϲ���ռ3��
	-1������ռ1����
	*/
	float bgTopHeight = (winSize_5 * 3) - 1;
	//�²���ռ2�ȷ�
	float bgBottomHeight =  winSize_5 * 2;
	//������ɫ
	auto bgTop = LayerColor::create(color_default, winSize.width, bgTopHeight);
	bgTop->setPosition(Vec2(0, bgBottomHeight + 1));
	bgTop->setTag(tag_bg_top);
	bgTop->setAnchorPoint(Vec2::ZERO);
	this->addChild(bgTop, 1);
	//�м��ɫ��
	auto lineMiddle = LayerColor::create(Color4B::BLACK, winSize.width, 1);
	lineMiddle->setPosition(Vec2(0, bgBottomHeight));
	this->addChild(lineMiddle, 0);
	//�ײ���ɫ
	auto bgBottom = LayerColor::create(color_default_b, winSize.width, bgBottomHeight);
	bgBottom->setPosition(Vec2::ZERO);
	bgBottom->setAnchorPoint(Vec2::ZERO);
	bgBottom->setTag(tag_bg_bottom);
	this->addChild(bgBottom, 0);

	auto cache = SpriteFrameCache::getInstance();
	//�÷ֱ���
	auto scoreBg = Sprite::createWithSpriteFrame(cache->getSpriteFrameByName("score_bg.png"));
	scoreBg->setPosition(Vec2(bgTop->getContentSize().width / 2, bgTop->getPositionY() + bgTop->getContentSize().height / 2));
	scoreBg->setScale(0.6, 0.6);
	scoreBg->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	scoreBg->setTag(tag_score_bg);
	this->addChild(scoreBg, 1);
	//�÷�Label
	auto label = LabelTTF::create("0", "Arial", 100);
	label->setPosition(Vec2(scoreBg->getContentSize().width / 2, scoreBg->getContentSize().height / 2));
	label->setColor(color_default_cc3);
	label->setTag(tag_score_label);
	scoreBg->addChild(label);

	//��ʼ��Ϸ��ť
	auto startMenuItem = MenuItemImage::create();
	startMenuItem->setNormalSpriteFrame(cache->getSpriteFrameByName("start_game.png"));
	startMenuItem->setSelectedSpriteFrame(cache->getSpriteFrameByName("start_game.png"));
	startMenuItem->setPosition(Vec2(bgBottom->getContentSize().width / 2, bgBottom->getContentSize().height / 2));
	startMenuItem->setCallback(CC_CALLBACK_1(WatchOut::startGame, this));
	startMenuItem->setScale(0.8, 0.8);
	auto menu = Menu::create(startMenuItem, NULL);
    menu->setPosition(Vec2::ZERO);
	menu->setTag(tag_start_game);
    this->addChild(menu, 2);

	//���ԲԲ
	auto yuanyuan = Sprite::createWithSpriteFrame(cache->getSpriteFrameByName("yuanyuan.png"));
	yuanyuan->setPosition(Vec2(2, 2));
	yuanyuan->setAnchorPoint(Vec2::ZERO);
	yuanyuan->setTag(tag_yuanyuan);
	yuanyuan->setVisible(false);
	bgTop->addChild(yuanyuan);
	//�ұ߷���
	auto fangfang = Sprite::createWithSpriteFrame(cache->getSpriteFrameByName("fangfang.png"));
	fangfang->setPosition(Vec2(winSize.width - fangfang->getContentSize().width - 2, 2));
	fangfang->setAnchorPoint(Vec2::ZERO);
	fangfang->setTag(tag_fangfang);
	fangfang->setVisible(false);
	bgTop->addChild(fangfang);

	//��ʼ����Ծ�߶�
	_jumpHeight =  yuanyuan->getContentSize().height * 0.9;
	//��ʼ����Ϸ״̬
	_gameState = GAME_STATE_START;
	//��ʼ����Ϸ��ʼ�ٶȣ�����/ԲԲ�ܵ��ٶȣ�
	_runSpeed = 2.0f;
	//��ʼ������
	_score = 0;

	//����ÿ֡����
	scheduleUpdate();

	//�����Ļ�����¼�
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(WatchOut::onTouchBegan, this);
	dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void WatchOut::startGame(Ref* pSender) {
	//������Ч
	SimpleAudioEngine::getInstance()->playEffect(music_button_touch);

	//��ö�������
	auto top = this->getChildByTag(tag_bg_top);
	//��÷���/ԲԲ
	Sprite* yuanyuan = (Sprite*)top->getChildByTag(tag_yuanyuan);
	Sprite* fangfang = (Sprite*)top->getChildByTag(tag_fangfang);

	/* ���ò��� */
	_runSpeed = 2.0f;
	_score = 0;
	auto scoreBg = this->getChildByTag(tag_score_bg);
	//����Label
	LabelTTF* scoreLabel = (LabelTTF*)scoreBg->getChildByTag(tag_score_label);
	scoreLabel->setString(StringUtils::format("%i", 0));
	//������ɫ
	LayerColor* topBg = (LayerColor*)this->getChildByTag(tag_bg_top);
	//�ײ���ɫ
	LayerColor* bottomBg = (LayerColor*)this->getChildByTag(tag_bg_bottom);
	topBg->setColor(ccc3(205, 225, 235));
	bottomBg->setColor(ccc3(63, 88, 100));
	scoreLabel->setColor(color_default_cc3);

	//����/ԲԲ����
	resetRunner();
	
	auto startGame = this->getChildByTag(tag_start_game);
	//����[��ʼ��Ϸ]��ť
	startGame->setVisible(false);
}

void WatchOut::update(float dt) {
	auto top = this->getChildByTag(tag_bg_top);
	Sprite* yuanyuan = (Sprite*)top->getChildByTag(tag_yuanyuan);
	Sprite* fangfang = (Sprite*)top->getChildByTag(tag_fangfang);

	if(_gameState == GAME_STATE_PLAYING) {
		//�ƶ�����/ԲԲ
		yuanyuan->setPositionX(yuanyuan->getPositionX() + _runSpeed);
		fangfang->setPositionX(fangfang->getPositionX() - _runSpeed);

		auto yyBox = yuanyuan->getBoundingBox();
		auto ffBox = fangfang->getBoundingBox();

		//��ײ���
		if(yyBox.intersectsRect(ffBox)) {
			_gameState = GAME_STATE_OVER;
			SimpleAudioEngine::getInstance()->playEffect(music_player_die);
			auto startGame = this->getChildByTag(tag_start_game);
			startGame->setVisible(true);

			/* ��ըЧ�� */
			auto pExpVec = Vec2::ZERO;
			auto pExpColor = Color4F::BLUE;
			auto pExpTextureRect = Rect::ZERO;
			//���ݵ�ǰTAG��������ը����
			if(_currentTag == yuanyuan->getTag()) {
				yuanyuan->setVisible(false);
				pExpVec = Vec2(
					yuanyuan->getPositionX() + yuanyuan->getContentSize().width / 4,
					yuanyuan->getPositionY() + yuanyuan->getContentSize().height / 4);
				pExpColor = Color4F(57, 99, 207, 255);
				pExpTextureRect = Rect(2,2,9,9);
			}else {
				fangfang->setVisible(false);
				pExpVec = Vec2(
					fangfang->getPositionX() + fangfang->getContentSize().width / 4,
					fangfang->getPositionY() + fangfang->getContentSize().height / 4);
				pExpColor = Color4F(212, 74, 123, 255);
				pExpTextureRect = Rect(13,2,9,9);
			}
			auto pExplostion = ParticleExplosion::create();
			pExplostion->setStartSize(3);
			pExplostion->setStartColor(pExpColor);
			pExplostion->setEndColor(pExplostion->getStartColor());
			pExplostion->setLife(0.1);
			pExplostion->setTotalParticles(100);
			pExplostion->setRotationIsDir(false);
			pExplostion->setAutoRemoveOnFinish(true);
			pExplostion->setPosition(pExpVec);
			pExplostion->setTextureWithRect(Director::getInstance()->getTextureCache()->addImage("watchout.png"), pExpTextureRect);
			pExplostion->setScale(0.5);
			top->addChild(pExplostion, 10);
		}else if(ffBox.getMaxX() < yyBox.getMinX() && 
			ffBox.getMinY() == 2 && 
			yyBox.getMinY() == 2) {//���û����ײ && ����/ԲԲ��û������Ծ״̬
			_gameState = GAME_STATE_RESET; //��Ϸ����
			SimpleAudioEngine::getInstance()->playEffect(music_score);
			yuanyuan->setVisible(false);
			fangfang->setVisible(false);
			_score ++;

			//����Label�ı�
			auto scoreBg = this->getChildByTag(tag_score_bg);
			LabelTTF* scoreLabel = (LabelTTF*)scoreBg->getChildByTag(tag_score_label);
			scoreLabel->setString(StringUtils::format("%i", _score));

			//��������(�ȼ�����)���ٶ����ӣ�������ɫ�ı�
			if(_score % 6 == 0) {
				_runSpeed += 0.5;
				int bgLevel = _score / 6;
				LayerColor* topBg = (LayerColor*)this->getChildByTag(tag_bg_top);
				LayerColor* bottomBg = (LayerColor*)this->getChildByTag(tag_bg_bottom);
				if(bgLevel == 1) {
					topBg->setColor(color_level_1);
					bottomBg->setColor(color_level_1_b);
					scoreLabel->setColor(color_level_1_cc3);
				}else if(bgLevel == 2) {
					topBg->setColor(color_level_2);
					bottomBg->setColor(color_level_2_b);
					scoreLabel->setColor(color_level_2_cc3);
				}
			}
		}
	}else if(_gameState == GAME_STATE_RESET) {
		resetRunner();
	}
}

bool WatchOut::onTouchBegan(Touch* touch, Event  *event) {
	auto top = this->getChildByTag(tag_bg_top);
	Sprite* yuanyuan = (Sprite*)top->getChildByTag(tag_yuanyuan);
	Sprite* fangfang = (Sprite*)top->getChildByTag(tag_fangfang);

	if(_gameState != GAME_STATE_PLAYING) {
		return true;
	}
	SimpleAudioEngine::getInstance()->playEffect(music_player_jump);
	Size winSize = Director::getInstance()->getVisibleSize();
	auto location = touch->getLocation();
	//�жϴ�����λ�ã���/�ң�
	if(location.x >= winSize.width / 2) {
		//��Ծ
		auto jumpAction = fangfang->getActionByTag(tag_fangfangAction);
		if(!jumpAction) {
			fangfang->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("fangfang_jump.png"));
			jumpAction = Sequence::create(JumpBy::create(0.5, Vec2(-80,0), _jumpHeight, 1),
				CallFunc::create(CC_CALLBACK_0(WatchOut::callbackRunAction, this, fangfang)),
				nullptr);
			jumpAction->setTag(tag_fangfangAction);
			fangfang->runAction(jumpAction);
		}else if(jumpAction->isDone()) { //��һ�ε����ʱ��ֻ����Ծ��ɲ���Ч
			fangfang->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("fangfang_jump.png"));
			fangfang->runAction(jumpAction);
		}
	}else {
		auto jumpAction = yuanyuan->getActionByTag(tag_yuanyuanAction);
		if(!jumpAction) {
			yuanyuan->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("yuanyuan_jump.png"));
			jumpAction = Sequence::create(JumpBy::create(0.5, Vec2(80,0), _jumpHeight, 1),
				CallFunc::create(CC_CALLBACK_0(WatchOut::callbackRunAction, this, yuanyuan)),
				nullptr);
			jumpAction->setTag(tag_yuanyuanAction);
			yuanyuan->runAction(jumpAction);
		}else if(jumpAction->isDone()) {
			yuanyuan->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("yuanyuan_jump.png"));
			yuanyuan->runAction(jumpAction);
		}
	}
	return true;
}

//�����������С��
void WatchOut::resetRunner() {
	Size winSize = Director::getInstance()->getVisibleSize();

	auto top = this->getChildByTag(tag_bg_top);
	Sprite* yuanyuan = (Sprite*)top->getChildByTag(tag_yuanyuan);
	Sprite* fangfang = (Sprite*)top->getChildByTag(tag_fangfang);
	yuanyuan->setPosition(Vec2(2, 2));
	fangfang->setPosition(Vec2(winSize.width - fangfang->getContentSize().width - 2, 2));

	//���0-100����� �ж���ż��
	int random = (int)(CCRANDOM_0_1() * 100) % 2;
	if(random == 0) {
		_currentTag = tag_yuanyuan;
		yuanyuan->setScale(0.5);
		fangfang->setScale(1);
	}else {
		_currentTag = tag_fangfang;
		fangfang->setScale(0.5);
		yuanyuan->setScale(1);
	}
	yuanyuan->setVisible(true);
	fangfang->setVisible(true);
	_gameState = GAME_STATE_PLAYING;
}

//��Ծ��Ļص����� ��ԭSprite״̬
void WatchOut::callbackRunAction(Sprite* pTarget) {
	std::string name = pTarget->getTag() == tag_fangfang ? "fangfang.png" : "yuanyuan.png";
	pTarget->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(name));
}

//Ԥ������Ч�ļ�
void WatchOut::preloadMusic() {
	SimpleAudioEngine::getInstance()->preloadEffect(music_button_touch);
	SimpleAudioEngine::getInstance()->preloadEffect(music_player_jump);
	SimpleAudioEngine::getInstance()->preloadEffect(music_score);
	SimpleAudioEngine::getInstance()->preloadEffect(music_player_die);
}