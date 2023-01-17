
var test1 =
{
    attr : 123,
    callback : function(finished, obj) {
        if (finished) {
            hoolai.log("callback1 " + this.attr);
            hoolai.TweenSystem.startAnimations("test1", 1, 4, obj);
            hoolai.TweenSystem.setAutoReverse(true);
            obj.position = {x:200, y:200};
            hoolai.TweenSystem.commitAnimations();
        }
    }
}

//this.attr = 222;

var test2 = {
    attr : 1,
    callback : function(finished, obj) {
        if (finished) {
            hoolai.log("callback2 " + this.attr);
            hoolai.TweenSystem.startAnimations("test3", 1, 4, obj);
            hoolai.TweenSystem.setAutoReverse(true);
            obj.rotation = 45;
            hoolai.TweenSystem.commitAnimations();
        }
    }
}

var test3 = function() {
    this.attr = 1;
    this.callback = function(finished, obj) {
        if (finished) {
            hoolai.log("callback2 " + this.attr);
            hoolai.TweenSystem.startAnimations("test3", 1, 4, obj);
            hoolai.TweenSystem.setAutoReverse(true);
            obj.rotation = 45;
            hoolai.TweenSystem.commitAnimations();
        }
    }
}

var test4 = new test3();

this.onStart = function() {
    var entMgr = mEntity.getEntityManager();
    var entity = entMgr.getEntity("gui");
    hoolai.TweenSystem.startAnimations("test", 1, 4, entity);
    hoolai.TweenSystem.setAutoReverse(true);
    entity.position = {x:100, y:300};
    hoolai.TweenSystem.setAnimationDidStopCallback(test1.callback.bind(test1));
    hoolai.TweenSystem.commitAnimations();
    
    hoolai.TweenSystem.startAnimations("test2", 1, 4, entity);
    hoolai.TweenSystem.setAutoReverse(true);
    entity.scale = 2;
    hoolai.TweenSystem.setAnimationDidStopCallback(test4, test4.callback);
    hoolai.TweenSystem.commitAnimations();
}
