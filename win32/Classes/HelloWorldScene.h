#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "SimpleAudioEngine.h"

class HelloWorld : public cocos2d::CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);

    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

	int * score;
	float * v;
	cocos2d::CCSprite * bird;
	int DOWN_A ;
	int UP_V ;
	int WIDTH ;
	int LAND_HEIGHT;
	cocos2d::CCLabelTTF* pLabel;
	//const static int SPEED = 5;

	cocos2d::CCArray * barrelArray;
	cocos2d::CCArray * pointLineArray;

	HelloWorld();
	~HelloWorld();

	void setV(float dt);
	void moveBird(float dt);
	void addBarrel(float dt);
	void isBarrelCrashed(float dt);
	void isPointLineCrashed(float dt);
	void isLandCrashed(float dt);
	void moveGround(float dt);
	void mySchedule(float dt);

	void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
	void spriteMoveFinished(CCNode* sender);
	bool isTwoSpriteCrashed(cocos2d::CCSprite* a,cocos2d::CCSprite* b);
	void PointLineCrashedHandler(cocos2d::CCSprite* pointLine);
	void gameOverHandler();
	void restart(cocos2d::CCObject* pSender);

};

#endif  // __HELLOWORLD_SCENE_H__