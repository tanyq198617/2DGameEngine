

var __tag = this._tag?this._tag:"HoolaiEngine";

let promise = null;
var globalDebuggee = null;

var gTestGlobals = [];

function TestTabActor(aConnection, aGlobal)
{
  this.conn = aConnection;
  this._global = aGlobal;
  this._threadActor = new ThreadActor(this, this._global);
  this.conn.addActor(this._threadActor);
  this._attached = false;
}

TestTabActor.prototype = {
  constructor: TestTabActor,
  actorPrefix: "TestTabActor",

  get window() {
    return { wrappedJSObject: this._global };
  },
    
  form: function() {
      let response = { actor: this.actorID, title: __tag, url: "http://www.hoolai.com" };
        
        // Walk over tab actors added by extensions and add them to a new ActorPool.
      let actorPool = new ActorPool(this.conn);
        //    this._createExtraActors(DebuggerServer.tabActorFactories, actorPool);
      if (!actorPool.isEmpty()) {
         this._tabActorPool = actorPool;
          this.conn.addActorPool(this._tabActorPool);
      }
        
        //    this._appendExtraActors(response);
        
      return response;
  },

  onAttach: function(aRequest) {
    this._attached = true;
    return { type: "tabAttached", threadActor: this._threadActor.actorID };
  },

  onDetach: function(aRequest) {
    if (!this._attached) {
      return { "error":"wrongState" };
    }
    return { type: "detached" };
  },

  // Hooks for use by TestTabActors.
  addToParentPool: function(aActor) {
    this.conn.addActor(aActor);
  },

  removeFromParentPool: function(aActor) {
    this.conn.removeActor(aActor);
  }
};

TestTabActor.prototype.requestTypes = {
  "attach": TestTabActor.prototype.onAttach,
  "detach": TestTabActor.prototype.onDetach
};

function TestTabList(aConnection) {
  this.conn = aConnection;

  // An array of actors for each global added with
  // DebuggerServer.addTestGlobal.
  this._tabActors = [];

  // A pool mapping those actors' names to the actors.
  this._tabActorPool = new ActorPool(aConnection);

  for (let global of gTestGlobals) {
    let actor = new TestTabActor(aConnection, global);
    actor.selected = false;
    this._tabActors.push(actor);
    this._tabActorPool.addActor(actor);
  }
  if (this._tabActors.length > 0) {
    this._tabActors[0].selected = true;
  }

  aConnection.addActorPool(this._tabActorPool);
}

TestTabList.prototype = {
    constructor: TestTabList,
    getList: function () {
        return promise.resolve([tabActor for (tabActor of this._tabActors)]);
    }
};

this.processInput = function (inputstr) {

    if (!inputstr) {
        return;
    }

    if (inputstr === "connected")
    {

        DebuggerServer.createRootActor = (conn => {
            return new RootActor(conn, { tabList: new TestTabList(conn) });
        });
        DebuggerServer.init(() => true);
        DebuggerServer.openListener(5086);

        if (debuggerServer && debuggerServer.onSocketAccepted)
        {
            var aTransport = {
                host: "127.0.0.1",
                port: 5086,
                openInputStream: function() {
                    return {
                        close: function(){}
                    };
                },
                openOutputStream: function() {
                    return {
                        close: function(){},
                        write: function(){},
                        asyncWait: function(){}
                    };
                },
            };

            debuggerServer.onSocketAccepted(null, aTransport);
        }
        return;
    }

    if (DebuggerServer && DebuggerServer._transport && DebuggerServer._transport.onDataAvailable)
    {
        DebuggerServer._transport.onDataAvailable(inputstr);
    }
};

this._prepareDebugger = function (global) {

    globalDebuggee = global;
    var executeScript = global.executeScript;
    
    executeScript(this, 'scripts_debugger/debugger/DevToolsUtils.js');
    executeScript(this, 'scripts_debugger/debugger/core/promise.js');
    executeScript(this, 'scripts_debugger/debugger/transport.js');
    executeScript(this, 'scripts_debugger/debugger/actors/root.js');
    executeScript(this, 'scripts_debugger/debugger/actors/script.js');
    executeScript(this, 'scripts_debugger/debugger/main.js');
    
    promise = exports;
    gTestGlobals.push(global);
};

