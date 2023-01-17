exports.ViewLoader = hoolai.gui.ViewLoader;
exports.GUIManager = hoolai.gui.GUIManager;
exports.View = hoolai.gui.View;
exports.Widget = hoolai.gui.Widget;
exports.ImageView = hoolai.gui.ImageView;
exports.Button = hoolai.gui.Button;
exports.Label = hoolai.gui.Label;
exports.RichLabel = hoolai.gui.RichLabel;
exports.Toast = hoolai.gui.Toast;
exports.AlertView = hoolai.gui.AlertView;
exports.ScrollView = hoolai.gui.ScrollView;
exports.ToolTip = hoolai.gui.ToolTip;
exports.EditBox = hoolai.gui.EditBox;
exports.ViewDND = hoolai.gui.ViewDND;
exports.ProgressBar = hoolai.gui.ProgressBar;
exports.ProgressRadialView = hoolai.gui.ProgressRadialView;
exports.SceneView = hoolai.gui.SceneView;
exports.ViewController = hoolai.Class.extend({
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