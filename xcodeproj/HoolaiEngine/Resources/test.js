
var total_rows = 21;
var total_columns = 10;
var unitsize = 24;
var time = 0;
var units = new Array();
var map = new Array();
var unitsMap = new Array();
var nowUnitNum;
var nowUnitX;
var nowUnitY;
var totalScore = 0;
var textures = new Array();
var nowTex = 0;
var dirty = false;

var pause = true;

for(i = 0; i < 19; i++) {
    units[i] = new Array();
    for(j = 0; j < 16; j++) {
        units[i][j] = 0;
    }
}
units[0][1] = 1;  // ■
units[0][4] = 1;  //■■■
units[0][5] = 1;  //
units[0][6] = 1;  //

units[1][0] = 1;  //■
units[1][4] = 1;  //■■
units[1][5] = 1;  //■
units[1][8] = 1;  //

units[2][1] = 1;  // ■
units[2][4] = 1;  //■■
units[2][5] = 1;  // ■
units[2][9] = 1;  //

units[3][0] = 1;  //■■■
units[3][1] = 1;  // ■
units[3][2] = 1;  //
units[3][5] = 1;  //

units[4][0] = 1;  //■
units[4][4] = 1;  //■
units[4][8] = 1;  //■■
units[4][9] = 1;  //

units[5][2] = 1;  //  ■
units[5][4] = 1;  //■■■
units[5][5] = 1;  //
units[5][6] = 1;  //

units[6][0] = 1;  //■■
units[6][1] = 1;  // ■
units[6][5] = 1;  // ■
units[6][9] = 1;  //

units[7][0] = 1;  //■■■
units[7][1] = 1;  //■
units[7][2] = 1;  //
units[7][4] = 1;  //

units[8][1] = 1;  // ■
units[8][5] = 1;  // ■
units[8][8] = 1;  //■■
units[8][9] = 1;  //

units[9][0] = 1;  //■■■
units[9][1] = 1;  //  ■
units[9][2] = 1;  //
units[9][6] = 1;  //

units[10][0] = 1; //■■
units[10][1] = 1; //■
units[10][4] = 1; //■
units[10][8] = 1; //

units[11][0] = 1; //■
units[11][4] = 1; //■■■
units[11][5] = 1; //
units[11][6] = 1; //

units[12][1] = 1; // ■■
units[12][2] = 1; //■■
units[12][4] = 1; //
units[12][5] = 1; //

units[13][0] = 1; //■
units[13][4] = 1; //■■
units[13][5] = 1; // ■
units[13][9] = 1; //

units[14][0] = 1; //■■
units[14][1] = 1; // ■■
units[14][5] = 1; //
units[14][6] = 1; //

units[15][1] = 1; // ■
units[15][4] = 1; //■■
units[15][5] = 1; //■
units[15][8] = 1; //

units[16][0] = 1; //■
units[16][4] = 1; //■
units[16][8] = 1; //■
units[16][12] = 1;//■

units[17][0] = 1; //■■■■
units[17][1] = 1; //
units[17][2] = 1; //
units[17][3] = 1; //

units[18][0] = 1; //■■
units[18][1] = 1; //■■
units[18][4] = 1; //
units[18][5] = 1; //

function getIndex(colum, row) {
    return row*total_columns + colum;
}

function clearMap() {
    for (i = 0; i < total_rows*total_columns; i++) {
        map[i] = 0;
    }
}

function initUnitPoint() {
    nowUnitNum = Math.round((Math.random()*18));
    if (nowUnitNum < 4) {
        nowTex = 1;
    } else if (nowUnitNum < 8) {
        nowTex = 2;
    } else if (nowUnitNum < 12) {
        nowTex = 3;
    } else if (nowUnitNum < 16) {
        nowTex = 4;
    } else if (nowUnitNum < 18) {
        nowTex = 5;
    } else {
        nowTex = 6;
    }
    nowUnitX = 5;
    nowUnitY = 0;
}

function setNextPoint(x, y) {
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (units[nowUnitNum][i*4 + j] == 1) {
                map[getIndex(nowUnitX + j, nowUnitY + i)] = 0;
            }
        }
    }
    
    nowUnitX += x;
    nowUnitY += y;
    
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (units[nowUnitNum][i*4+j] == 1) {
                map[getIndex(nowUnitX + j, nowUnitY + i)] = 1;
            }
        }
    }
}

function newGame() {
    for (i = 0; i < total_rows*total_columns; i++) {
        unitsMap[i] = 0;
    }
    clearMap();
    initUnitPoint();
    setNextPoint(0, 0);
    totalScore = 0;
}

function getNextUnit() {
    var nextUnitNum;
    switch (nowUnitNum) {
        case 3:
            nextUnitNum = 0;
            break;
        case 7:
            nextUnitNum = 4;
            break;
        case 11:
            nextUnitNum = 8;
            break;
        case 13:
            nextUnitNum = 12;
            break;
        case 15:
            nextUnitNum = 14;
            break;
        case 17:
            nextUnitNum = 16;
            break;
        case 18:
            nextUnitNum = 18;
            break;
        default:
            nextUnitNum = nowUnitNum + 1;
            break;
    }
    return nextUnitNum;
}

function canRotate() {
    var nextUnitNum = getNextUnit();
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (units[nextUnitNum][i*4+j] == 1) {
                if ((nowUnitX + j < 0) || (nowUnitX + j >= total_columns) || (nowUnitY + i >= total_rows)) {
                    return false;
                }
                if (unitsMap[getIndex(nowUnitX + j, nowUnitY + i)] == 1) {
                    return false;
                }
            }
        }
    }
    return true;
}

function clearLine() {
    var lineNum = 0;
    var isHave0 = false;
    
    for (i = 0; i < total_rows; i++) {
        isHave0 = false;
        for (j = 0; j < total_columns; j++) {
            if (unitsMap[getIndex(j, i)] == 0) {
                isHave0 = true;
                break;
            }
        }
        if (isHave0 == false) {
            for(m = i; m >= 1; m--) {
                for (n = 0; n < total_columns; n++) {
                    unitsMap[getIndex(n, m)] = unitsMap[getIndex(n, m - 1)];
                }
            }
            lineNum++;
        }
    }
    
    switch (lineNum) {
        case 1:
            totalScore += 1;
            break;
        case 2:
            totalScore += 3;
            break;
        case 3:
            totalScore += 5;
            break;
        case 4:
            totalScore += 8;
            break;
        default:
            break;
    }
}

function canGoDown() {
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (units[nowUnitNum][i*4+j] == 1) {
                if (nowUnitY + i + 1 >= total_rows) {
                    return false;
                }
                if (unitsMap[getIndex(nowUnitX + j, nowUnitY + i + 1)] == 1) {
                    return false;
                }
            }
        }
    }
    return true;
}

function canMoveLeftRight(x, y) {
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (units[nowUnitNum][i*4+j] == 1) {
                if ((nowUnitX + x < 0) || (nowUnitX + x + j + 1 > total_columns)) {
                    return false;
                }
                if (unitsMap[getIndex(nowUnitX + j + x, nowUnitY + i + y)] == 1) {
                    return false;
                }
            }
        }
    }
    return true;
}

function storeUnit() {
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (units[nowUnitNum][i*4+j] == 1) {
                unitsMap[getIndex(nowUnitX + j, nowUnitY + i)] = 1;
            }
        }
    }
}

this.exports = function() {

this.onStart = function() {
    hoolai.log(testproperty);
    mEntity.setUserInteractionEnabled(true);
    var entMgr = mEntity.getEntityManager();
//    var scoretxt = entMgr.createEntity("scorelabel", "HLTransform2DComponent", "HLTextTTFComponent");
    var pos = {x:270, y:440};
//    scoretxt.position = pos;
//    scoretxt.text = "score";
//    mEntity.addChild(scoretxt);
//    var scorenum = entMgr.createEntity("score", "HLTransform2DComponent", "HLTextTTFComponent");
//    pos.x = 270;
//    pos.y = 425;
//    scorenum.position = pos;
//    scorenum.text = "0";
//    mEntity.addChild(scorenum);
    textures[0] = new hoolai.Texture2D("blue.png");
    textures[1] = new hoolai.Texture2D("cyan.png");
    textures[2] = new hoolai.Texture2D("green.png");
    textures[3] = new hoolai.Texture2D("magenta.png");
    textures[4] = new hoolai.Texture2D("orange.png");
    textures[5] = new hoolai.Texture2D("red.png");
    textures[6] = new hoolai.Texture2D("yellow.png");
    var size = {width:0, height:0};
    for (i = 0; i < total_rows; i++) {
        for (j = 0; j < total_columns; j++) {
            var entity = entMgr.createEntity("block"+getIndex(j, i), "HLTransform2DComponent", "HLSpriteComponent");
            pos.x = 0;
            pos.y = 1;
            entity.anchorPoint = pos;
            pos.x = j*unitsize;
            pos.y = 480-unitsize*i;
            entity.position = pos;
            hoolai.log("x:"+entity.position.x+", y:"+entity.position.y);
            size.width = size.height = unitsize;
            entity.size = size;
            entity.visible = false;
//            entity.texture = textures[5];
            mEntity.addChild(entity);
        }
    }
    
    pause = false;
    newGame();
}

function setGameOver() {
    var gameOver = false;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (units[nowUnitNum][i*4+j] == 1) {
                if (unitsMap[getIndex(nowUnitX + j, nowUnitY + i)] == 1) {
                    gameOver = true;
                    break;
                }
            }
        }
    }
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
        
        if (!canGoDown()) {
            storeUnit();
            clearLine();
//            mEntity.getEntityManager().getEntity("score").text = totalScore+"";
            initUnitPoint();
            setGameOver();
        } else {
            setNextPoint(0, 1);
        }
        
        dirty = true;
    }
    if (dirty) {
        dirty = false;
        for (i = 0; i < total_columns*total_rows; i++) {
            map[i] = unitsMap[i];
            if(map[i] == 1) {
                var entity = mEntity.getEntityManager().getEntity("block"+i);
                entity.texture = textures[0];
            }
        }
        
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (units[nowUnitNum][i*4+j] == 1) {
                    var index = getIndex(nowUnitX + j, nowUnitY + i);
                    map[index] = 1;
                    var entity = mEntity.getEntityManager().getEntity("block"+index);
                    entity.texture = textures[nowTex];
                }
            }
        }
        
        for (i = 0; i < total_columns*total_rows; i++) {
            var entity = mEntity.getEntityManager().getEntity("block"+i);
            entity.visible = (map[i] == 1);
        }
    }
}

this.onTouches = function(type, touches) {
    if (pause) {
        return false;
    }
    if (type == hoolai.TouchEventType.TouchesBegin) {
        if (touches[0].location.y < 100) {
            while(canGoDown()) {
                setNextPoint(0, 1);
            }
            
        } else if (touches[0].location.x < 100) {
            if (canMoveLeftRight(-1, 0)) {
                setNextPoint(-1, 0);
                dirty = true;
            }
        } else if (touches[0].location.x > 220) {
            if (canMoveLeftRight(1, 0)) {
                setNextPoint(1, 0);
                dirty = true;
            }
        } else {
            if (canRotate()) {
                nowUnitNum = getNextUnit();
                dirty = true;
            }
        }
    }
    return true;
}

this.onMouseEvent = function(type, mouseEvent) {
    if (pause) {
        return false;
    }
    if (type == hoolai.MouseEventType.MouseDown) {
        if (mouseEvent.location.y < 100) {
            while(canGoDown()) {
                setNextPoint(0, 1);
            }
            
        } else if (mouseEvent.location.x < 100) {
            if (canMoveLeftRight(-1, 0)) {
                setNextPoint(-1, 0);
                dirty = true;
            }
        } else if (mouseEvent.location.x > 220) {
            if (canMoveLeftRight(1, 0)) {
                setNextPoint(1, 0);
                dirty = true;
            }
        } else {
            if (canRotate()) {
                nowUnitNum = getNextUnit();
                dirty = true;
            }
        }
    }
    return true;
}
}