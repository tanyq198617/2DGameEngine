
var RootWidget = hoolai.gui.GUIManager.getRootWidget();

//var progressBar = new hoolai.gui.ProgressBar(RootWidget, {x:100, y:0, width:200, height:30}, 1<<1);
//progressBar.setBackgroundImage("at01.jpg");
//progressBar.setForegroundImage("1.jpg", {x:0.5, y:0.5, width:0.1, height:0.1});
//progressBar.setRange(0, 100);
//progressBar.value = 100;

hoolai.log("testLabel===begin");
//===创建对象
var Label = new hoolai.gui.Label(RootWidget);
Label.userInteractionEnabled = true;
//Label = new hoolai.gui.Label(RootWidget, {x:200, y:0, width:100, height:50});
//===设置图片
//Label.setNormalBackgroundImage(hoolai.Texture.getTexture("HLAlertViewBackground.png"));
//Label.setSpriteFrame(hoolai.SpriteFrame.getSpriteFrame("test.json", "a01.png"));
//===设置属性
Label.frame = {x:220, y:0, width:100, height:50};
Label.backgroundColor = {r:0.78, g:0.86, b:0.96, a:0.6};
//===设置对齐方式
Label.textAlign = hoolai.TextAlignmentType.HLeft;
//===设置亮度--0表示没有亮度，1表示全亮度
//Label.brightness = 0.1;
Label.text = "Label->Drag Me";
Label.fontSize = 16;
//Label.textColorTL = ;
//Label.textColorBL = ;
//Label.textColorTR = ;
//Label.textColorBR = ;
//===设置描虚边
Label.textStroke = true;
//===设置出体字
//Label.bold = true;
//===设置描虚边颜色
Label.textStrokeColor = {r:1, g:0.1, b:0.1, a:1};
//Label.setText("Label.setText");
//Label.setTextColor({r:1, g:0, b:0, a:1});
Label.setTextColor(100,255,0,255);
Label.setTouchEventListener(handleTouch);
hoolai.log("testLabel===end");
Label.setDraggingEventListener(function(view, event, dragSource) {
                               hoolai.log("draggingevent " + event.type);
                               });

hoolai.log("testLabel_1===begin");
var Label_1 = new hoolai.gui.Label(RootWidget, {x:110, y:170, width:100, height:50});
Label_1.userInteractionEnabled = true;
Label_1.backgroundColor = {r:0.78, g:0.86, b:0.96, a:0.6};
Label_1.text = "Label->Drop Here";
Label_1.setTextColor(100,255,0,255);
Label_1.setDraggingEventListener(handleDragEvent);
hoolai.log("testLabel_1===end");

var widget = new hoolai.gui.Widget();
var testview = new hoolai.gui.View(widget, {x:0, y:0, width:20, height:20});
testview.backgroundColor = {r:1, g:0, b:0, a:1};
testview.userInteractionEnabled = true;
testview.setTouchEventListener(function(){
                                hoolai.gui.GUIManager.getRootWidget().dismissHUDWidget();
                               });
hoolai.gui.GUIManager.getRootWidget().showHUDWidget(widget);

function handleTouch(view, touch)
{
    hoolai.log("handleTouch===begin");
    ViewDND = new hoolai.gui.ViewDND();
    ViewDND.setDraggingView(view);
    ViewDND.doDragDrop(touch.touches[0].location);
    hoolai.log("handleTouch===end");
}

//function handleDragEvent(view, event, dragSource, ret)
function handleDragEvent(view, event, dragSource)
{
    hoolai.log("handleDragEvent===begin " + event);
    var label = view;
    //hoolai.log("event=="+event);
    //hoolai.log("event.type=="+event.type);
    //hoolai.log("dragSource=="+dragSource);
    if (event.type == hoolai.ViewDraggingEventType.Entered)
    {
        view.backgroundColor = {r:1, g:0, b:0, a:1};
    }
    else if (event.type == hoolai.ViewDraggingEventType.Exited)
    {
        view.backgroundColor = {r:0.78, g:0.86, b:0.96, a:0.6};
    }
    else if (event.type == hoolai.ViewDraggingEventType.Dropped)
    {
        view.backgroundColor = {r:1, g:0.5, b:0, a:1};
        var label2 = dragSource.getSourceView();
        label.setText(label2.getText());
    }
    else if (event.type == hoolai.ViewDraggingEventType.Accepted)
    {
    }
    else if (event.type == hoolai.ViewDraggingEventType.Canceled)
    {
    }
    hoolai.log("handleDragEvent===end");
    return true;
}
//
//function UTF8ArrToStr (aBytes) {
//    
//    var sView = "";
//    
//    for (var nPart, nLen = aBytes.length, nIdx = 0; nIdx < nLen; nIdx++) {
//        nPart = aBytes[nIdx];
//        sView += String.fromCharCode(
//                                     nPart > 251 && nPart < 254 && nIdx + 5 < nLen ? /* six bytes */
//                                     /* (nPart - 252 << 32) is not possible in ECMAScript! So...: */
//                                     (nPart - 252) * 1073741824 + (aBytes[++nIdx] - 128 << 24) + (aBytes[++nIdx] - 128 << 18) + (aBytes[++nIdx] - 128 << 12) + (aBytes[++nIdx] - 128 << 6) + aBytes[++nIdx] - 128
//                                     : nPart > 247 && nPart < 252 && nIdx + 4 < nLen ? /* five bytes */
//                                     (nPart - 248 << 24) + (aBytes[++nIdx] - 128 << 18) + (aBytes[++nIdx] - 128 << 12) + (aBytes[++nIdx] - 128 << 6) + aBytes[++nIdx] - 128
//                                     : nPart > 239 && nPart < 248 && nIdx + 3 < nLen ? /* four bytes */
//                                     (nPart - 240 << 18) + (aBytes[++nIdx] - 128 << 12) + (aBytes[++nIdx] - 128 << 6) + aBytes[++nIdx] - 128
//                                     : nPart > 223 && nPart < 240 && nIdx + 2 < nLen ? /* three bytes */
//                                     (nPart - 224 << 12) + (aBytes[++nIdx] - 128 << 6) + aBytes[++nIdx] - 128
//                                     : nPart > 191 && nPart < 224 && nIdx + 1 < nLen ? /* two bytes */
//                                     (nPart - 192 << 6) + aBytes[++nIdx] - 128
//                                     : /* nPart < 127 ? */ /* one byte */
//                                     nPart
//                                     );
//    }
//    
//    return sView;
//    
//}

//var request = new hoolai.HttpRequest("http://www.baidu.com");
//request.setResponseHandler(function(req, code, ret){var arr = new Uint8Array(ret); hoolai.log(UTF8ArrToStr(arr));});
//request.startRequest();

var sceneMgr = new hoolai.SceneManager();
//{
//    var numberOfParticles = 150;
//    var entity = sceneMgr.getEntityManager().createEntity("entity", "HLTransform2DComponent");
//    entity.position = {x: 100, y: 100};
//    entity.addParticleComponent(numberOfParticles);
//    entity.setProperty("texture", hoolai.Texture.getTexture("particle-smoke.png"));
//    entity.setProperty("duration", -1);
//    entity.setProperty("emitterType", 0);
//    entity.setProperty("gravity", {x:-10, y:0});
//    entity.setProperty("radialAcceleration", 0);
//    entity.setProperty("radialAccelVariance", 0);
//    entity.setProperty("speed", 20);
//    entity.setProperty("speedVariance", 5);
//    entity.setProperty("angle", 90);
//    entity.setProperty("angleVariance", 10);
//    entity.setProperty("posVariance", {x:2, y:1});
//    entity.setProperty("lifespan", 3);
//    entity.setProperty("lifespanVariance", 0.25);
//    entity.setProperty("startSize", 5.0);
//    entity.setProperty("startSizeVariance", 2.0);
//    entity.setProperty("endSize", 15.0);
//    entity.setProperty("endSizeVariance", 4.0);
//    entity.setProperty("emissionRate", numberOfParticles/3);
//    entity.setProperty("startColor", {r:0.8, g:0.8, b:0.8, a:1.0});
//    entity.setProperty("startColorVariance", {r:0.01, g:0.01, b:0.01, a:0.0});
//    entity.setProperty("finishColor", {r:0.0, g:0.0, b:0.0, a:1.0});
//    entity.setProperty("finishColorVariance", {r:0.0, g:0.0, b:0.0, a:0.0});
//    entity.setBlendAdditive(false);
//    
//    sceneMgr.addEntity(entity);
//}
var entity = sceneMgr.getEntityManager().createEntity("entity", "HLTransform2DComponent", "HLSpriteComponent", "HLColorAdvanceComponent");
entity.position = {x: 100, y: 10};
entity.texture = hoolai.Texture.getTexture("tree.png");
entity.size = {width: 205, height: 500};
hoolai.TweenSystem.startAnimations("Die", 5.0);
hoolai.TweenSystem.setAnimationDidStopCallback(this, function(){
                                               hoolai.log("aaaaaa");
                                               });
entity.colorPercent = {
    r : 0,
    g : 0,
    b : 0,
    a : 0
};
hoolai.TweenSystem.commitAnimations();

var entity1 = sceneMgr.getEntityManager().createAnonymousEntity("HLTransform2DComponent", "HLSkeletonComponent", "HLColorComponent");
entity1.skelfile = "test/0.bin";
entity1.playSkeletonAnimation("Run", false, true, 0, 2.5);
entity1.position = {x:100, y:100};
//entity1.rotation = 30;
//entity1.scaleX = -1;
sceneMgr.addEntity(entity1);
//entity.scaleX = 0.6;
//entity.setProperty("skewY", 30);
////entity.skewY = 30;
sceneMgr.addEntity(entity);
//var labelView = new hoolai.gui.View();
//labelView.size = {width:100, height:20};
//var nameLabel = new hoolai.gui.Label(labelView);
//nameLabel.size = {width:205, height:10};
//nameLabel.textStroke = true;
//nameLabel.fontSize = 30;
//nameLabel.textColor = {r:1,g:0,b:0,a:1};
//nameLabel.text = "李";
//var nameEntity = sceneMgr.getEntityManager().createAnonymousEntity("HLTransform2DComponent", "HLGUIViewComponent");
//nameEntity.view = labelView;
//nameEntity.size = {width:205, height:20};
//nameEntity.position = {x: 0, y: 380};
//entity.addChild(nameEntity);
//
//var entity1 = sceneMgr.getEntityManager().createAnonymousEntity("HLTransform2DComponent", "HLSpriteComponent");
//entity1.size = {width: 120, height: 125};
//entity1.position = {x: 48, y: 255};
//entity1.texture = hoolai.Texture.getTexture("1308451173.jpg");
//entity.addChild(entity1, -1);
//
//var testentity = sceneMgr.getEntityManager().createAnonymousEntity("HLTransform2DComponent");
//testentity.position = {x:10, y:10};
//testentity.scale = 0.5;
//sceneMgr.addEntity(testentity);
//var headentity = sceneMgr.getEntityManager().createAnonymousEntity("HLTransform2DComponent",
//                                                                   "HLGUIViewComponent");
//headentity.size = {width: 20, height: 20};
//headentity.userInteractionEnabled = true;
//var headView = new hoolai.gui.View();
//headentity.view = headView;
//headView.userInteractionEnabled = true;
//headView.backgroundColor = {r: 1, g: 0, b:0, a:1};
//headView.setTouchEventListener(function (view,touch) {
//                               hoolai.log("click headview ...........");
//                               headView.backgroundColor = {r: 1, g: 1, b:0, a:1};
//                               });
//var bodyentity = sceneMgr.getEntityManager().createAnonymousEntity("HLTransform2DComponent", "HLSkeletonComponent");
//bodyentity.skelfile = "testren/1.bin";
//bodyentity.playSkeletonAnimation("Idle", false, true, 0, 2.5);
//bodyentity.anchorPoint = bodyentity.getSkeletonRegisterPoint("pos_gd");
//entity1.skeletonAddEntityToBone("body_pos", bodyentity);
//testentity.addChild(headentity);
//
//var primitiveentity = sceneMgr.getEntityManager().createAnonymousEntity("HLTransform2DComponent", "HLColorComponent", "HLPrimitiveComponent");
//primitiveentity.color = {r:1, b:1, g:1, a:1};
//primitiveentity.position = {x: 160, y: 240};
//primitiveentity.setProperty("drawType", 4);
//primitiveentity.setProperty("radius", 60);
//primitiveentity.setProperty("segments", 60);
//sceneMgr.addEntity(primitiveentity);
//
//hoolai.Director2D.runWithSceneManager(sceneMgr);

//var scrollview = new hoolai.gui.ScrollView(RootWidget, {x:0, y:0, width:100, height:200});
//scrollview.setBounceable(false);
//scrollview.userInteractionEnabled = true;
//scrollview.setContentSize({width: 300, height: 300});
//scrollview.setDirection(2);
//hoolai.log(""+scrollview.getContentSize().width);
////scrollview.direction = 2;
//scrollview.contentSize = {width: 300, height: 300};
//scrollview.backgroundColor = {r:1, g:1, b:1, a:1};
//var subView = new hoolai.gui.View(scrollview, {x:0, y:0, width:100, height:100});
//subView.backgroundColor = {r:0.5, g:0.5, b:0.5, a:1};
//

//var button = new hoolai.gui.Button(RootWidget, {x:10, y:300, width:100, height:30});
//button.backgroundColor = {r:1, g:0, b:0, a:1};
//button.normalTitle = "Button";
//button.zoomOnTouchDown = true;
//hoolai.gui.AlertView.setDarkTheme();
//button.setOnButtonClickListener(function(){
//                                    var alert = new hoolai.gui.AlertView("title", "content", "Cancel", "OK");
//                                    alert.show();
//                                });
////
//var imageView = new hoolai.gui.ImageView(RootWidget, {x:50, y:50, width: 50, height: 50});
//imageView.setImage("jingjidating_btn_jingjichang_kuaisuhaishi.png");
////imageView.glowRadius = 4;
////imageView.glowColor = {r:1, g:1, b:0, a:0.8};
//

var view = new hoolai.gui.View(RootWidget, {x:50, y:50, width:50, height:50});
view.backgroundColor = {r:1, g:1, b:1, a:1};
var testprogressview = new hoolai.gui.ProgressRadialView(RootWidget, {x:50, y:50, width:50, height:50}, 0);
//testprogressview.setImage("jingjidating_btn_jingjichang_kuaisuhaishi.png");
//testprogressview.setCenterPoint({x:0.6, y:0.6});
//testprogressview.setStartAngle(90);
//testprogressview.setEndAngle(0);
//testprogressview.progress = 0.5;
var progress = 1;
var timer = new hoolai.Timer(0.1, true);
timer.setDelegate(function(){
                  progress -= 0.1;
                  if (progress < 0)
                    progress = 1;
                  testprogressview.progress = progress;
                  });

//hoolai.NotificationCenter.defaultCenter().addObserver("notifytest", {
//                                                      onNotification:function(message){
//                                                      hoolai.log("onNotification " + message.name);
//                                                      }
//                                                      });

var label = new hoolai.gui.Label(RootWidget, {x: 10, y:10, width:100, height:10});
label.text = "";
var imageView = new hoolai.gui.ImageView(label, {x:0, y:10, width: 48.5, height:62.5});
imageView.setImage("at01.jpg");
//var timer = new hoolai.Timer(0.1, true);
//var step = 0.05;
//timer.setDelegate(function() {
//                  var brightness = imageView.brightness;
//                  brightness += step;
//                  if (brightness >= 1.3) {
//                  brightness = 1.3;
//                  step = -0.05;
//                  }
//                  if (brightness <= 1.0) {
//                  brightness = 1.0;
//                  step = 0.05;
//                  }
//                  imageView.brightness = brightness;
//                  });


hoolai.gui.View.startAnimations("anim", 1.0, 0);
imageView.rotation = 360;
hoolai.gui.View.commitAnimations();


//var count = 0;
//var timer = new hoolai.Timer(0.1, true);
//timer.setDelegate(llllll);
//
//function llllll() {
//    count++;
//    var toast = new hoolai.gui.Toast("你好");
//    toast.show();
//    if (count == 20) {
//        count = 0;
//        timer.cancel();
//        timer = null;
//        
//        timer = new hoolai.Timer(5, false);
//        timer.setDelegate(llllll2);
//    }
//};
//
//function llllll2() {
//    hoolai.log("hhhh");
//    var toast = new hoolai.gui.Toast("djdkddjdkdjdkd");
//    toast.show();
//};
