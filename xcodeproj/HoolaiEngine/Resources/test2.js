var view = new hoolai.gui.View(hoolai.gui.GUIManager.getRootWidget());
var timer = new hoolai.Timer(0.1, true);
timer.setDelegate(this, function(){
                  hoolai.log("test2");
                  var pos = view.position;
                  maybeGC();
                  hoolai.log("====");
                  });


function fun(num)
{
    if (num == 0 || num == 1)
    {
        return num;
    }
    else
    {
        return num+fun(num-1);
    }
}

hoolai.log(fun(1000000)+"");