#include "HelloWorldScene.h"
#include <cstdlib>
#include <ctime>
using namespace cocos2d;
//È¥ÄãÂè±ÆµÄsetContentSize 
CCScene* HelloWorld::scene()
{
    CCScene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = CCScene::create();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        HelloWorld *layer = HelloWorld::create();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    bool bRet = false;
    do 
    {
        //////////////////////////////////////////////////////////////////////////
        // super init first
        //////////////////////////////////////////////////////////////////////////

        CC_BREAK_IF(! CCLayer::init());

        //////////////////////////////////////////////////////////////////////////
        // add your codes below...
        //////////////////////////////////////////////////////////////////////////
		CCSize size = CCDirector::sharedDirector()->getWinSize();
		CCDirector::sharedDirector()->setDepthTest(false);
        // 1. Add a menu item with "X" image, which is clicked to quit the program.

        // Create a "close" menu item with close icon, it's an auto release object.

        CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
            "CloseNormal.png",
            "CloseSelected.png",
            this,
            menu_selector(HelloWorld::menuCloseCallback));
        CC_BREAK_IF(! pCloseItem);
		CCSize pcSize = pCloseItem->boundingBox().size;
		pCloseItem->setScale(40.0/320*size.width/pcSize.width);
        // Place the menu item bottom-right conner.
        pCloseItem->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - 20, CCDirector::sharedDirector()->getWinSize().height - 20));

        // Create a menu with the "close" menu item, it's an auto release object.
        CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
        pMenu->setPosition(CCPointZero);
        CC_BREAK_IF(! pMenu);

        // Add the menu to HelloWorld layer as a child layer.
        this->addChild(pMenu, 1);

        // 2. Add a label shows "Hello World".

        // Create a label and initialize with string "Hello World".
		pLabel = CCLabelTTF::create("Hello World", "Arial", 42);
        CC_BREAK_IF(! pLabel);

        // Get window size and place the label upper. 
        
        pLabel->setPosition(ccp(size.width / 2, size.height*3/4));

        // Add the label to HelloWorld layer as a child layer.
        this->addChild(pLabel, 1);

        // 3. Add add a splash screen, show the cocos2d splash image.
        CCSprite* pSprite = CCSprite::create("HelloWorld.jpg");
        CC_BREAK_IF(! pSprite);

        // Place the sprite on the center of the screen

		CCSize psSize = pSprite->boundingBox().size;
		pSprite->setScaleX(size.width/psSize.width);
		pSprite->setScaleY(size.height/psSize.height);
		pSprite->setAnchorPoint(ccp(0,1));
		pSprite->setPosition(ccp(0, size.height));
        // Add the sprite to HelloWorld layer as a child layer.
		pSprite->setOpacity(100);
        this->addChild(pSprite, 0);
		CCLOG("anchorPoint: (%03f,%03f)",pSprite->getAnchorPoint().x,pSprite->getAnchorPoint().y);
		CCLOG("boundingBox:%05f contentsize: %05f",pSprite->boundingBox().size.width,pSprite->getContentSize().width);
		//set the label touchable
		this->setTouchEnabled(true);

		//init const

		DOWN_A = 800 * size.height / 640;
		UP_V = 400 * size.height / 640;
		WIDTH = 70 * size.height / 640;
		LAND_HEIGHT = 120* size.height / 640;
		//init score and v;
		score = new int(0);
		v = new float(0);

		//init arrays
		barrelArray = new CCArray;
		pointLineArray = new CCArray;

		//init the bird
		bird = CCSprite::create("bird.png");

		CCSize bSize = bird->boundingBox().size;
		//bird->setAnchorPoint(ccp(0,0));
		bird->setScaleX(30.0/320*size.width/bSize.width);
		bird->setScaleY(30.0/480.0*size.height/bSize.height);
		bird->setZOrder(6);
		bird->setPosition(ccp(size.width/4,size.height/2));
		
		this->addChild(bird);

		CCSprite * bg = CCSprite::create("ground.jpg");
		CCSize bgSize = bg->boundingBox().size;
		bg->setScaleX(size.width*2/bgSize.width);
		bg->setScaleY(LAND_HEIGHT/bgSize.height);
		bg->setAnchorPoint(ccp(0,0));
		bg->setPosition(ccp(0,0));
		bg->setZOrder(9);
		this->addChild(bg);
		moveGround(0);
		pLabel->setString("Tap to Start");
        bRet = true;
    } while (0);

	this->schedule(schedule_selector(HelloWorld::mySchedule));
	this->schedule(schedule_selector(HelloWorld::addBarrel),1.2);
	this->schedule(schedule_selector(HelloWorld::moveGround),2);
	//init random seed
	srand(time(0));
	CCDirector::sharedDirector()->pause();
    return bRet;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    // "close" menu item clicked
    CCDirector::sharedDirector()->end();
}

void HelloWorld::setV(float dt)
{
	*v = (*v) - HelloWorld::DOWN_A * dt;
}

void HelloWorld::moveBird(float dt)
{
	float y = bird->getPositionY();
	bird->setPositionY(y + (*v) * dt);
}

void HelloWorld::mySchedule(float dt)
{
	setV(dt);
	moveBird(dt);
	isBarrelCrashed(dt);
	isPointLineCrashed(dt);
	isLandCrashed(dt);
}

void HelloWorld::addBarrel(float dt)
{
	CCSize size = CCDirector::sharedDirector()->getWinSize();
	
	float whole_height = (size.height - LAND_HEIGHT);
	float middle_line = whole_height/2 + LAND_HEIGHT;
	float departure = rand()%(int)(whole_height/2 - WIDTH) ;
	//departure = 0;
	if(rand()%2)
		departure = 0 - departure;
	middle_line = middle_line + departure;

	float upPointY = middle_line + WIDTH;
	float downPointY = middle_line - WIDTH;

	CCMoveTo * UpMoveLeft = CCMoveTo::create(3,ccp(0 - 100, upPointY));
	CCMoveTo * DownMoveLeft = CCMoveTo::create(3,ccp(0 - 100, downPointY));
	CCFiniteTimeAction* done = CCCallFuncN::create(this,callfuncN_selector(HelloWorld::spriteMoveFinished));
	
	CCSprite * upBarrel = CCSprite::create("barrel.jpg");
	CCSize upBarrelSize = upBarrel->getContentSize();
	float upBarrelWidth = 65.0/320.0*size.width;
	float x_s_up =upBarrelWidth/upBarrelSize.width;
	float y_s_up = size.height/upBarrelSize.height;

	upBarrel->setAnchorPoint(ccp(0.5,0));	
	
	upBarrel->setPosition(ccp(size.width+80 , upPointY));
	
	upBarrel->setTag(1);
	barrelArray->addObject(upBarrel);
	//upBarrel->setContentSize(CCSize(upBarrelWidth,size.height));
	upBarrel->setScaleX(x_s_up);
	upBarrel->setScaleY(y_s_up);
	this->addChild(upBarrel);

	upBarrel->runAction(CCSequence::create(UpMoveLeft,done,NULL));

	CCSprite * downBarrel = CCSprite::create("barrel.jpg");
	
	CCSize downBarrelSize = downBarrel->getContentSize();
	float downBarrelWidth = 65.0/320.0*size.width;
	float x_s_down =downBarrelWidth/downBarrelSize.width;
	float y_s_down = size.height/downBarrelSize.height;
		
	CCLOG("y_s_donw: %05f",y_s_down);
	
	downBarrel->setTag(1);
	barrelArray->addObject(downBarrel);
	downBarrel->setAnchorPoint(ccp(0.5,1));	
	
	
	//downBarrel->setContentSize(CCSizeMake(downBarrelWidth,30));
	downBarrel->setScaleX(x_s_down);
	downBarrel->setScaleY(y_s_down);
	CCLOG("boundingBox:%05f contentsize: %05f",downBarrel->boundingBox().size.width,downBarrel->getContentSize().width);
	downBarrel->setAnchorPoint(ccp(0.5,1));	
	
	//downBarrel->setRotation(180);
	//downBarrel->setAnchorPoint(ccp(0.5,0));	
	downBarrel->setPosition(ccp(size.width+80 , downPointY));
	this->addChild(downBarrel);
	CCLOG("anchorPoint: (%03f,%03f)",downBarrel->getAnchorPoint().x,downBarrel->getAnchorPoint().y);
	downBarrel->runAction(CCSequence::create(DownMoveLeft,done,NULL));


	CCSprite * pointLine = CCSprite::create("pointLine.png");
	CCSize pointLineSize = pointLine->boundingBox().size;
	pointLine->setScaleX(1/pointLineSize.width);
	pointLine->setScaleY(2*WIDTH/pointLineSize.height);
	pointLine->setAnchorPoint(ccp(0.5,0));
	pointLine->setPosition(ccp(size.width + 80,downPointY));
	pointLine->setTag(0);
	pointLine->setVisible(false);
	pointLineArray->addObject(pointLine);
	this->addChild(pointLine);
	CCMoveTo * lineMoveLeft = CCMoveTo::create(3,ccp(0 - 100, downPointY));
	pointLine->runAction(CCSequence::create(lineMoveLeft,done,NULL));
}

void HelloWorld::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
{
	*v = UP_V;
	if(CCDirector::sharedDirector()->isPaused())
	{
		CCDirector::sharedDirector()->resume();
		pLabel->setPositionY(CCDirector::sharedDirector()->getWinSize().height - 50);
		pLabel->setString("0");
	}
}

void HelloWorld::spriteMoveFinished(CCNode* sender)
{
	CCSprite* sp = (CCSprite*) sender;
	this->removeChild(sp,true);

	if(sp->getTag()==0)
	{
		if(pointLineArray->count()>0)
			pointLineArray->removeObject(sp);
	}
	if(sp->getTag()==1)
	{
		if(barrelArray->count()>0)
			barrelArray->removeObject(sp);
	}
}

HelloWorld::HelloWorld()
{
	pointLineArray = NULL;
	barrelArray = NULL;
}

HelloWorld::~HelloWorld()
{
	if(barrelArray)
	{
		barrelArray->release();
		barrelArray = NULL;
	}
	if(pointLineArray)
	{
		pointLineArray->release();
		pointLineArray=NULL;
	}

	delete HelloWorld::score;
	delete HelloWorld::v;
}

bool HelloWorld::isTwoSpriteCrashed(CCSprite* a,CCSprite * b)
{
	
	CCRect ar,br;
	/*double a_x,a_y;
	a_x = a->getPositionX() - a->getContentSize().width/2;
	a_y  =a->getPositionY() - a->getContentSize().height/2;
	ar = CCRectMake(a_x,a_y,a->getContentSize().width,a->getContentSize().height);
	double b_x,b_y;
	double b_width,b_height;
	b_width = b->getContentSize().width;
	b_height = b->getContentSize().height;
	b_x = b->getPositionX()-b_width/2;
	b_y = b->getPositionY()-b_height/2;
	if(b->getAnchorPoint().y==0)
	{
		b_y = b->getPositionY();
	}
	else if(b->getAnchorPoint().y==1)
	{
		b_y = b->getPositionY() - b_height;
	}
	br = CCRectMake(b_x,b_y,b_width,b_height);

	return ar.intersectsRect(br);*/
	ar = a->boundingBox();
	br = b->boundingBox();
	return ar.intersectsRect(br);

}

void HelloWorld::isBarrelCrashed(float dt)
{
	bool groundCrashed = bird->getPositionY() < LAND_HEIGHT;
	for(int i = 0;i<barrelArray->count();i++)
	{
		CCSprite * barrel = (CCSprite*)(barrelArray->objectAtIndex(i));
		if(isTwoSpriteCrashed(bird,barrel))
		{
			gameOverHandler();
			break;
		}
	}
}

void HelloWorld::isPointLineCrashed(float dt)
{
	for(int i = 0;i<pointLineArray->count();i++)
	{
		CCSprite * pointLine = (CCSprite*)(pointLineArray->objectAtIndex(i));
		if(isTwoSpriteCrashed(bird,pointLine))
		{
			PointLineCrashedHandler(pointLine);
		}
	}
}

void HelloWorld::PointLineCrashedHandler(CCSprite* pointLine)
{
	(*score) = (*score) + 1;
	char * str = new char[5];
	sprintf(str,"%01d",*score);
	pLabel->setString(str);
	delete str;

	pointLineArray->removeObject(pointLine);
	this->removeChild(pointLine);
}

void HelloWorld::gameOverHandler()
{
	
			int best = CCUserDefault::sharedUserDefault()->getIntegerForKey("bestScore",0);
			if((*score)>best)
			{
				best = *score;
				CCUserDefault::sharedUserDefault()->setIntegerForKey("bestScore",best);
			}
			//CCDirector::sharedDirector()->pause();
			//CCDirector::sharedDirector()->stopAnimation();
			this->unscheduleAllSelectors();
			CCSize size= CCDirector::sharedDirector()->getWinSize();
			this->unscheduleUpdate();
			char * best_c = new char[40];
			sprintf(best_c,"Game Over\n\nYour Score: %01d\nBest: %01d",*score,best);
			pLabel->setString(best_c);
			pLabel->setZOrder(8);
			
			pLabel->setPositionY(size.height/3*2);

			CCSprite * back = CCSprite::create("labelBackground.jpg");
			CCSize backSize = back->boundingBox().size;
			back->setScaleX(size.width*7/8/backSize.width);
			back->setScaleY(pLabel->boundingBox().size.height/backSize.height);
			back->setPosition(ccp(size.width/2,size.height/3*2));
			back->setZOrder(7);	
			back->setOpacity(200);
			this->addChild(back);

			CCMenuItemImage* again = CCMenuItemImage::create("restart.png","restart.png",this,menu_selector(HelloWorld::restart));
			CCSize againSize = again->boundingBox().size;
			again->setScaleX(size.width/3.0/againSize.width);
			again->setScaleY(size.height/6.0/againSize.height);
			again->setPosition(ccp(0,0));
			CCMenu* aMenu = CCMenu::create(again,NULL);
			aMenu->setPosition(size.width/2,size.height/3);
			this->addChild(aMenu);

}

void HelloWorld::isLandCrashed(float dt)
{
	if(bird->boundingBox().getMinY()<LAND_HEIGHT)
	{
		gameOverHandler();
	}
}

void HelloWorld::restart(CCObject* pSender)
{
	//CCDirector::sharedDirector()->startAnimation();
	CCDirector::sharedDirector()->replaceScene(this->scene());
	//CCDirector::sharedDirector()->startAnimation();
	//CCDirector::sharedDirector()->resume();
	//CCDirector::sharedDirector()->popScene();
	//CCDirector::sharedDirector()->runWithScene(this->scene());
}

void HelloWorld::moveGround(float dt)
{
		CCSize size = CCDirector::sharedDirector()->getWinSize();
		CCSprite * bg = CCSprite::create("ground.jpg");
		CCSize bgSize = bg->boundingBox().size;
		bg->setScaleX(size.width*2/bgSize.width);
		bg->setScaleY(LAND_HEIGHT/bgSize.height);
		bg->setAnchorPoint(ccp(0,0));
		bg->setPosition(ccp(0,0));
		bg->setZOrder(9);
		this->addChild(bg);
		CCFiniteTimeAction* done = CCCallFuncN::create(this,callfuncN_selector(HelloWorld::spriteMoveFinished));
		bgSize = bg->boundingBox().size;
		float desX = (-1)*(bgSize.width)/2;
		bg->runAction(CCSequence::create(CCMoveTo::create(2,ccp(desX,0)),done,NULL));
		
}