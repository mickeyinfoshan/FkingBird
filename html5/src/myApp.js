/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2008-2010 Ricardo Quesada
 Copyright (c) 2011      Zynga Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

var MyLayer = cc.Layer.extend({
    isMouseDown:false,
    helloImg:null,
    helloLabel:null,
    circle:null,
    sprite:null,
    bird:null,
    upBarrelArray:[],
    downBarrelArray:[],
    pointLineArray:[],
    point:0,
    GROUND_HEIGHT:0,
    WIDTH:0,
    BARREL_WIDTH:0,
    BIRD_WIDTH:0,
    init:function () {

        //////////////////////////////
        // 1. super init first
        this._super();

        /////////////////////////////
        // 2. add a menu item with "X" image, which is clicked to quit the program
        //    you may modify it.
        // ask director the window size
        var size = cc.Director.getInstance().getWinSize();
        this.setTouchEnabled(true);
        this.WIDTH = 70 * size.height / 640;
        this.GROUND_HEIGHT = 120* size.height / 640;
        this.BARREL_WIDTH = 60;
        this.BIRD_WIDTH = 30;
        // add a "close" icon to exit the progress. it's an autorelease object
        var closeItem = cc.MenuItemImage.create(
            s_CloseNormal,
            s_CloseSelected,
            function () {
                cc.log("close");
            },this);
        closeItem.setAnchorPoint(0.5, 0.5);

        var menu = cc.Menu.create(closeItem);
        menu.setPosition(0, 0);
        this.addChild(menu, 1);
        closeItem.setPosition(size.width - 20, 20);

        /////////////////////////////
        // 3. add your codes below...
        // add a label shows "Hello World"
        // create and initialize a label
        this.helloLabel = cc.LabelTTF.create("Tap to Start", "Impact", 42);
        // position the label on the center of the screen
        this.helloLabel.setPosition(size.width / 2, size.height * 3 / 4);
        // add the label as a child to this layer
        this.addChild(this.helloLabel, 5);

        // add "Helloworld" splash screen"
        this.sprite = cc.Sprite.create(s_HelloWorld);
        this.sprite.setAnchorPoint(0.5, 0.5);
        this.sprite.setPosition(size.width / 2, size.height / 2);
        this.sprite.setScale(size.height/this.sprite.getContentSize().height);
        this.sprite.setOpacity(100);
        this.addChild(this.sprite, 0);

        this.bird = new Bird();
        this.bird.init();
        this.addChild(this.bird,8);
        scaleSize(this.bird,this.BIRD_WIDTH,this.BIRD_WIDTH);

        this.moveGround(0);
       //schedules
       this.schedule(this.addBarrels,2);
       this.schedule(this.isCrashedByUpBarrel);
       this.schedule(this.isCrashedByDownBarrel);
       this.schedule(this.isCrashedByPointLine);
       this.schedule(this.moveGround,2);
       this.schedule(this.isCrashedByGround);
    
        cc.Director.getInstance().pause();
    },
    onTouchesBegan:function(touch,event){
            this.bird.moveUp();
            if(cc.Director.getInstance().isPaused()){
                cc.Director.getInstance().resume();
                this.helloLabel.setString("0");
            }
            return true;
    },
    addBarrels:function(dt){
        var size = cc.Director.getInstance().getWinSize();
        var moveDoneSelector = function(sender){
            var sprite = sender;
            this.removeChild(sprite);
            if(sprite.tag == 1){
                this.upBarrelArray.shift();
            }
            else if(sprite.tag==2){
                this.downBarrelArray.shift();
            }
            else if(sprite.tag==0){
                this.pointLineArray.shift();
            }
        };
        var moveDone = cc.CallFunc.create(moveDoneSelector,this);

        var wholeHeight = size.height - this.GROUND_HEIGHT;
        var middleLine = wholeHeight/2.0 + this.GROUND_HEIGHT;
        var departure = Math.random() * (wholeHeight/2 - this.WIDTH);
        if(Math.round(Math.random()*10)%2==0){
            departure = 0 - departure;
        }
        middleLine = middleLine + departure;
        var upY = middleLine + this.WIDTH;
        var downY = middleLine - this.WIDTH;

        var upBarrel = cc.Sprite.create("barrel.jpg");
        upBarrel.tag = 1;
        upBarrel.setAnchorPoint(0.5,0);
        upBarrel.setPosition(size.width,upY);
        var uw =upBarrel.getBoundingBox().getWidth();
        scaleSize(upBarrel,this.BARREL_WIDTH,size.height - this.GROUND_HEIGHT);                                            ///////////////////////////////////////////////////////////
        //console.log("up: "+upBarrel.getBoundingBox().getHeight());
        this.upBarrelArray.push(upBarrel);
        var upMoveLeft = cc.MoveTo.create(2,cc.p(0,upY));
        var upMoveSeq = cc.Sequence.create(upMoveLeft,moveDone);
        this.addChild(upBarrel);
     

        var downMoveLeft = cc.MoveTo.create(2,cc.p(0,downY));
        var downBarrel = cc.Sprite.create("barrel.jpg");
        downBarrel.tag = 2;
        downBarrel.setAnchorPoint(0.5,1);
        downBarrel.setPosition(size.width,downY);
        //console.log("down: "+downBarrel.getBoundingBox().getHeight());
        this.downBarrelArray.push(downBarrel);
        scaleSize(downBarrel,this.BARREL_WIDTH,size.height - this.GROUND_HEIGHT);
        this.addChild(downBarrel);
        var downMoveSeq = cc.Sequence.create(downMoveLeft,moveDone);

        upBarrel.runAction(upMoveSeq);
        downBarrel.runAction(downMoveSeq);

        var pointLine = cc.Sprite.create("pointLine.png");
        pointLine.setAnchorPoint(0.5,0);
        pointLine.setVisible(false);
        pointLine.setPosition(size.width,downY);
        pointLine.tag = 0;
        this.pointLineArray.push(pointLine);
        scaleSize(pointLine,10,2*this.WIDTH);
        this.addChild(pointLine);
        var pointLineMoveLeft = cc.MoveTo.create(2,cc.p(0,downY));
        var pointLineMoveSeq = cc.Sequence.create(pointLineMoveLeft,moveDone);
        pointLine.runAction(pointLineMoveSeq);

    },
    isCrashedByUpBarrel:function(dt){
        var i;
        for(i=0;i<this.upBarrelArray.length;i++){
            if(this.bird.isCrashedByUpBarrel(this.upBarrelArray[i])){
                this.gameOverHandler();
                break;
            }
                
        }
    },
    isCrashedByDownBarrel:function(dt){
        var i;
        for(i=0;i<this.downBarrelArray.length;i++){
            if(this.bird.isCrashedByDownBarrel(this.downBarrelArray[i])){
                this.gameOverHandler();  
                break; 
            }
        }
    },
    isCrashedByPointLine:function(dt){
        var i;
        for(i=0;i<this.pointLineArray.length;i++){
            if(this.bird.isCrashedByPointLine(this.pointLineArray[0])){
                this.removeChild(this.pointLineArray[0]);
                this.pointLineArray.shift();
                this.point++;
                this.helloLabel.setString(this.point);
            }
        }
    },
    moveGround:function(dt){
        //alert("ground");
        var size = cc.Director.getInstance().getWinSize();
        var ground = cc.Sprite.create("ground.jpg");
        ground.tag = 4;
        ground.setAnchorPoint(0,0);
        ground.setPosition(0,0);
        scaleSize(ground,size.width*2,this.GROUND_HEIGHT);
        this.addChild(ground);
        var moveDoneSelector = function(sender){
            var sprite = sender;
        };
        var moveDone = cc.CallFunc.create(moveDoneSelector,this);
        var des_x = (-1) * size.width;
        var moveLeft = cc.MoveTo.create(2,cc.p(des_x,0));
        var groundSeq = cc.Sequence.create(moveLeft,moveDone);
        ground.runAction(groundSeq);
    },
    gameOverHandler:function(){
        var size = cc.Director.getInstance().getWinSize();

        if(!localStorage["fkingBird_bestPoint"]){
            localStorage["fkingBird_bestPoint"] = 0;
        }
        var best = localStorage["fkingBird_bestPoint"];
        if(this.point > best){
            localStorage["fkingBird_bestPoint"] = this.point;
        }
        this.unscheduleAllCallbacks(); 
        this.bird.unscheduleAllCallbacks();
        //this.helloLabel.setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER);
        this.helloLabel.setHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER);
        this.helloLabel.setString("Game Over\nYour Point: "+this.point+"\nBest: "
            +localStorage["fkingBird_bestPoint"]);
        this.helloLabel.setPosition(size.width/2,size.height/3*2);

        var restart = cc.MenuItemImage.create(
            "restart.png",
            "restart.png",
            function () {
                cc.Director.getInstance().replaceScene(new MyScene());
            },this);
        restart.setAnchorPoint(0.5, 0.5);
        restart.tag = 5;
        scaleSize(restart,100,100);
        var menu_ = cc.Menu.create(restart);
        menu_.setPosition(size.width/2,size.height/3);
        this.addChild(menu_, 1);
        //restart.setPosition(size.width/2,size.height/2);
    },
    isCrashedByGround:function(dt){
        if(this.bird.getPositionY()<=this.GROUND_HEIGHT){
            this.gameOverHandler();
        }
    }
});

var MyScene = cc.Scene.extend({
    onEnter:function () {
        this._super();
        var layer = new MyLayer();
        this.addChild(layer);
        layer.init();
    }
});

var Bird = cc.Sprite.extend({
    v:0,
    g:0,
    upV:0,
    init:function(){
        this._super();
        this.initWithFile(s_bird);
        var size = cc.Director.getInstance().getWinSize();
        this.setPosition(size.width/4,size.height/2);
        this.v = 0;
        this.g = (-1)*800 * size.height / 640;
        this.upV = 280 * size.height / 640;
        this.schedule(this.changeV);
        this.schedule(this.moveDown);
    },
    isCrashed:function(sp){
        var thisRect = this.getBoundingBox();
        var spRect = sp.getBoundingBox();
        ////console.log(spRect.getWidth());
        if(cc.rectIntersectsRect(thisRect,spRect)){
            return true;
        }
    },
    isCrashedByUpBarrel:function(sp){
        if(this.isCrashed(sp)&&sp.tag==1){
            return true;
        }
    },
    isCrashedByDownBarrel:function(sp){
        if(this.isCrashed(sp)&&sp.tag==2){
            return true;
        }
    },
    isCrashedByPointLine:function(sp){
        if(this.isCrashed(sp) && sp.tag==0){
            return true;
        }
    },
    moveDown:function(dt){
        var y = this.getPositionY();
        y = y + this.v * dt;
        this.setPositionY(y);
    },
    changeV:function(dt){
        this.v = this.v + this.g * dt;
    },
    moveUp:function(dt){
        this.v = this.upV;
        return true;
    }
    
});

//set the Scale based on 320*480
function scaleSize(sp,desWidth,desHeight){
    var spSize = {
        width:sp.getBoundingBox().getWidth(),
        height:sp.getBoundingBox().getHeight()
    };
    sp.getBoundingBox().setWidth(desWidth);
    sp.getBoundingBox().setHeight(desHeight);
    var size = cc.Director.getInstance().getWinSize();
    var x_s;
    var y_s;
    if(spSize.width!=0&&spSize.height!=0){
        x_s = desWidth / 320 * size.width /spSize.width;
        y_s = desHeight / 480 * size.height / spSize.height;
        
    }else if(sp.tag==1||sp.tag==2){
         x_s = desWidth / 320 * size.width /156;
         y_s = desHeight / 480 * size.height / 322;
        
    }else if(sp.tag==0){
         x_s = desWidth / 320 * size.width /17;
         y_s = desHeight / 480 * size.height / 625;
    }
    else if(sp.tag==4){
        x_s = desWidth / 320 * size.width /989;
        y_s = desHeight / 480 * size.height / 161;
    }else if(sp.tag==5){
        x_s = desWidth / 320 * size.width / 150;
        y_s = desHeight / 480 * size.height / 150;
    }
    sp.setScaleX(x_s);
    sp.setScaleY(y_s);
}