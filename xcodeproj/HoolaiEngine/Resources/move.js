
var touchLocation;

this.onTouches = function(type, touches) {
    if (type == hoolai.TouchEventType.TouchesBegin) {
        touchLocation = touches[0].location;
    } else if (type == hoolai.TouchEventType.TouchesMove) {
        var pos = mEntity.position;
        var diff = {x:0, y:0};
        diff.x = touches[0].location.x - touchLocation.x;
        diff.y = touches[0].location.y - touchLocation.y;
        pos.x += diff.x;
        pos.y += diff.y;
        mEntity.position = pos;
        touchLocation = touches[0].location;
    }
    return true;
}