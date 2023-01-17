
TestViewController = hoolai.gui.ViewController.extend({
	init:function () {
		this._super();
		
		this._widget.backgroundColor = hoolai.color4(1.0, 0.0, 0.0, 1.0);
		hoolai.gui.GUIManager.getRootWidget().addSubview(this._widget);
		var button = new hoolai.gui.Button(this._widget, hoolai.rect(0, 0, 100, 50));
		button.backgroundColor = hoolai.color4(0.0, 1.0, 0.0, 1.0);
		button.normalTitle = "close";
		button.setOnButtonClickListener(this, function () {
			this._widget.removeFromParent(true);
			this.onClose(true);
		});
	}
});
