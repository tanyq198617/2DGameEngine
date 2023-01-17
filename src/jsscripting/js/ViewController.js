
hoolai.gui.ViewController = hoolai.Class.extend({
	init:function () {
		this._widget = new hoolai.gui.Widget();
		this._widget.setOnCloseListener(this, this.onClose);
		addGCRootObject(this);
		
		if (arguments.length > 0 && typeof(arguments[0])=="string") {
			this._view = hoolai.gui.ViewLoader.loadView(arguments[0], this, this._widget);
		}
	},
	onClose:function (clean) {
		if (clean) {
			removeGCRootObject(this);
			maybeGC();
		}
	}
});
