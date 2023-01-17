var total_rows = 3;
var total_columns = 3;
var unitsize = 24;
var time = 0;
var map = new Array();
var unitsTimes = new Array();
var nowUnitNum;
var totalScore = 0;
var textures = new Array();
var nowTex = 0;
var pause = true;

function getIndex(colum, row) {
    return row*total_columns + colum;
}

function clearMap() {
    
}

function initUnitPoint() {
  
}

function newGame() {
    for (i = 0; i < total_rows*total_columns; i++) {
        unitsTimes[i] = 0;
        unitsTimes[i] =  Math.round((Math.random()*10));

    }
    clearMap();
    initUnitPoint();
    totalScore = 0;
}
function addScore(thiz) {
    totalScore += 100;
    thiz.getEntityManager().getEntity("score").text = totalScore;
}

function isGameOver() {
    if(game_time>1000)
    {
        return true;
    }
    
    return false;
}


this.onStart = function() {
    this.setUserInteractionEnabled(true);
    var entMgr = this.getEntityManager();
    var scoretxt = this.getEntityManager().createEntity("scorelabel", "HLTransform2DComponent", "HLTextTTFComponent");
    var pos = {x:270, y:440};
    scoretxt.position = pos;
    scoretxt.text = "score";
    this.addChild(scoretxt);
    
    var scorenum = this.getEntityManager().createEntity("score", "HLTransform2DComponent", "HLTextTTFComponent");
    pos.x = 270;
    pos.y = 425;
    scorenum.position = pos;
    scorenum.text = "0";
    this.addChild(scorenum);
   
    for (i = 0; i < total_rows; i++) {
        for (j = 0; j < total_columns; j++) {
            var entity = this.getEntityManager().createEntity("block"+getIndex(j, i), "HLTransform2DComponent", "HLSpriteComponent");
            var pos2 = {x:0, y:1};

            entity.anchorPoint = pos2;
            //calculate the cell space
            var cellSpaceW=(320/(total_columns+1));
            var cellSpaceH=(480/(total_rows+1));
            pos.x = cellSpaceW*(j+1);
            pos.y = cellSpaceH*(i+1);
            entity.position = pos;
            var size = {width:unitsize, height:unitsize};

            entity.size = size;
            entity.visible = true;
            
//            EntityManager.getEntity("score").text = cellSpaceW;

            //            entity.texture = new hoolai.Texture2D("red.png");
            this.addChild(entity);
        }
    }
    
    textures[0] = new hoolai.Texture2D("blue.png");
    textures[1] = new hoolai.Texture2D("cyan.png");
    textures[2] = new hoolai.Texture2D("green.png");
    textures[3] = new hoolai.Texture2D("magenta.png");
    textures[4] = new hoolai.Texture2D("orange.png");
    textures[5] = new hoolai.Texture2D("red.png");
    textures[6] = new hoolai.Texture2D("yellow.png");
    
    pause = false;
    newGame();
}

function setGameOver() {
    var gameOver = false;
   
    if (gameOver) {
        pause = true;
    }
}

this.onUpdate = function(delta) {
    if (pause) {
        return;
    }
    time+=delta;
    if (time >= 0.5) {

        time = 0;
        nowTex = Math.round((Math.random()*6));
        for (i = 0; i < total_columns; i++)
        {
            for (j = 0; j < total_columns; j++)
            {
                var index = getIndex(0 + j, 0 + i);
                var entity = this.getEntityManager().getEntity("block"+index);
                entity.texture = textures[nowTex];
                var nowUnitX=0;
                var nowUnitY=0;

                //显示或不显示
                if (unitsTimes[getIndex(nowUnitX + j, nowUnitY + i)] > 10)
                {
                    unitsTimes[getIndex(nowUnitX + j, nowUnitY + i)] =0;
                }else if (unitsTimes[getIndex(nowUnitX + j, nowUnitY + i)] > 8)
                {
                    unitsTimes[getIndex(nowUnitX + j, nowUnitY + i)] ++;
                    entity.visible = true;
                }else{
                    unitsTimes[getIndex(nowUnitX + j, nowUnitY + i)] ++;
                    entity.visible = false;
                }
            }
        }
       
        //addScore();
    }
 
}
function isInPointInRect(_pointX,_pointY,rx,ry,rw,rh)
{
    if(_pointX>rx&&_pointX<(rx+rw)&&_pointY>ry&&_pointY<(ry+rh))
    {
        return true;
    }
    
    return false;
}

this.onTouches = function(type, touches) {
    if (pause) {
        return;
    }
//    hoolai.log("2222:");

    if (type == hoolai.TouchEventType.touchesBegin) {
        for (i = 0; i < total_columns; i++) {
            for (j = 0; j < total_columns; j++) {
                var index = getIndex(0 + j, 0 + i);
                var entity = this.getEntityManager().getEntity("block"+index);
//                hoolai.log("@1__"+entity.position.x);
//                hoolai.log("@1__"+entity.position.x);
//                hoolai.log("x:");

                var _x = entity.position.x;
                var _y = entity.position.y;
                var _w = entity.textureRect.width;
                var _h = entity.textureRect.height;
                hoolai.log(entity.name);
                hoolai.log("x:"+_x);
                hoolai.log("y:"+_y);
                hoolai.log("w:"+_w);
                hoolai.log("h:"+_h);

//                entity.textureRect._w
                if(isInPointInRect(touches[0].location.x,touches[0].location.y,_x,_y,_w,_h) )
                {
                    hoolai.log("PointIsInRect!");

                    addScore();
                }
            }
        }
        
    }
}

this.onMouseEvent = function(type, mouseEvent) {
    if (pause) {
        return;
    }
    if (type == hoolai.MouseEventType.mouseDown) {
        hoolai.log("mouseX:"+mouseEvent.location.x);
        hoolai.log("mouseY:"+mouseEvent.location.y);
        for (i = 0; i < total_columns; i++) {
            for (j = 0; j < total_columns; j++) {
                var index = getIndex(0 + j, 0 + i);
                var entity = this.getEntityManager().getEntity("block"+index);
                //                hoolai.log("@1__"+entity.position.x);
                //                hoolai.log("@1__"+entity.position.x);
                //                hoolai.log("x:");
                
                var _x = entity.position.x;
                var _y = entity.position.y;
                var _w = entity.textureRect.width;
                var _h = entity.textureRect.height;
                hoolai.log(entity.name);
                hoolai.log("x:"+_x);
                hoolai.log("y:"+_y);
                hoolai.log("w:"+_w);
                hoolai.log("h:"+_h);
                //                entity.textureRect._w
                if(entity.visible&&isInPointInRect(mouseEvent.location.x,mouseEvent.location.y,_x,_y,_w,_h) )
                {
                    hoolai.log("PointIsInRect!");
                    
                    addScore(this);
                }
            }
        }
    }
}
