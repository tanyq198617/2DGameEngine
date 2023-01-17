
function require(p) {
	if ('.js' != p.substr(p.length-3))
	{
		p += ".js";
	}
	var mod = require.modules[p];
	if (!mod) {
		mod = {};
		mod.exports = {};
		mod.require = require.relative(p);
		mod.module = mod;
		mod.define = require.define(mod);
		executeScript(mod, 'scripts/modules/'+p);
		require.register(p, mod);
	}
	return mod.exports;
}

require.modules = {};

require.register = function (path, fn) {
	require.modules[path] = fn;
}

require.relative = function (parent) {
	return function (p) {
		if ('.' != p.substr(0, 1)) {
			return require(p);
		}
		
		var path = parent.split('/')
		, segs = p.split('/');
		path.pop();
		
		for (var i = 0; i < segs.length; i++) {
			var seg = segs[i];
			if ('..' == seg) {
				path.pop();
			}
			else if ('.' != seg) {
				path.push(seg);
			}
		}
		
		return require(path.join('/'));
	}
}

require.define = function (mod) {
	return function (fn) {
		if (typeof(fn) == 'function') {
			fn.call(mod, mod.require, mod.exports, this.module);
		}
		else {
			mod.exports = fn;
		}
	}
}

require.reloadModules = function () {
	require.modules = {};
}

hoolai.color4 = function( r, g, b, a ) {
    return {r:r, g:g, b:b, a:a };
};

hoolai.size = function(w,h) {
    return {width:w, height:h};
};

hoolai.point = function(x, y) {
	return {x:x, y:y};
}

hoolai.rect = function(x,y,w,h) {
    return {x:x, y:y, width:w, height:h};
};

hoolai.Class = function () {};
hoolai.Class.extend = function (prop) {
	var _super = this.prototype;

    // Instantiate a base class (but only create the instance,
    // don't run the init constructor)
    initializing = true;
    var prototype = new this();
    initializing = false;
    fnTest = /xyz/.test(function(){xyz;}) ? /\b_super\b/ : /.*/;

    // Copy the properties over onto the new prototype
    for (var name in prop) {
        // Check if we're overwriting an existing function
        prototype[name] = typeof prop[name] == "function" &&
            typeof _super[name] == "function" && fnTest.test(prop[name]) ?
            (function (name, fn) {
                return function () {
                    var tmp = this._super;

                    // Add a new ._super() method that is the same method
                    // but on the super-class
                    this._super = _super[name];

                    // The method only need to be bound temporarily, so we
                    // remove it when we're done executing
                    var ret = fn.apply(this, arguments);
                    this._super = tmp;

                    return ret;
                };
            })(name, prop[name]) :
            prop[name];
    }

    // The dummy class constructor
    function Class() {
        // All construction is actually done in the init method
        if (!initializing && this.init)
            this.init.apply(this, arguments);
    }

    // Populate our constructed prototype object
    Class.prototype = prototype;

    // Enforce the constructor to be what we expect
    Class.prototype.constructor = Class;

    // And make this class extendable
    Class.extend = arguments.callee;

    return Class;
}
