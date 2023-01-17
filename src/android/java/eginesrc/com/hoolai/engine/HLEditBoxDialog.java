package com.hoolai.engine;

import android.app.Dialog;
import android.content.Context;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.text.InputFilter;
import android.text.InputType;
import android.text.method.DigitsKeyListener;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;

public class HLEditBoxDialog extends Dialog {
	// ===========================================================
	// Constants
	// ===========================================================

	/**
	 * The user is allowed to enter any text, including line breaks.
	 */
	private final int kEditBoxInputModeAny = 0;

	private final int kEditBoxInputModeAscii = 1;

	/**
	 * The user is allowed to enter an e-mail address.
	 */
	private final int kEditBoxInputModeEmailAddr = 2;

	/**
	 * The user is allowed to enter an integer value.
	 */
	private final int kEditBoxInputModeNumeric = 3;

	private final int kEditBoxInputModeNumberPad = 4;

	/**
	 * The user is allowed to enter a phone number.
	 */
	private final int kEditBoxInputModePhoneNumber = 5;

	/**
	 * The user is allowed to enter a URL.
	 */
	private final int kEditBoxInputModeUrl = 6;

	/**
	 * The user is allowed to enter a real number value. This extends
	 * kEditBoxInputModeNumeric by allowing a decimal point.
	 */
	private final int kEditBoxInputModeDecimal = 7;

	/**
	 * Indicates that the text entered is confidential data that should be
	 * obscured whenever possible. This implies EDIT_BOX_INPUT_FLAG_SENSITIVE.
	 */
	private final int kEditBoxInputFlagPassword = 0;

	/**
	 * Indicates that the text entered is sensitive data that the implementation
	 * must never store into a dictionary or table for use in predictive,
	 * auto-completing, or other accelerated input schemes. A credit card number
	 * is an example of sensitive data.
	 */
	private final int kEditBoxInputFlagSensitive = 1;

	/**
	 * This flag is a hint to the implementation that during text editing, the
	 * initial letter of each word should be capitalized.
	 */
	private final int kEditBoxInputFlagInitialCapsWord = 2;

	/**
	 * This flag is a hint to the implementation that during text editing, the
	 * initial letter of each sentence should be capitalized.
	 */
	private final int kEditBoxInputFlagInitialCapsSentence = 3;

	/**
	 * Capitalize all characters automatically.
	 */
	private final int kEditBoxInputFlagInitialCapsAllCharacters = 4;

	private final int kKeyboardReturnTypeDefault = 0;
	private final int kKeyboardReturnTypeNext = 1;
	private final int kKeyboardReturnTypeDone = 2;
	private final int kKeyboardReturnTypeSend = 3;
	private final int kKeyboardReturnTypeSearch = 4;
	private final int kKeyboardReturnTypeGo = 5;

	// ===========================================================
	// Fields
	// ===========================================================

	private EditText mInputEditText;
	private TextView mTextViewTitle;

	private final String mTitle;
	private final String mMessage;
	private final String mDigitsFilter;
	private final int mInputMode;
	private final int mInputFlag;
	private final int mReturnType;
	private final int mMaxLength;

	private int mInputFlagConstraints;
	private int mInputModeContraints;
	private boolean mIsMultiline;

	// ===========================================================
	// Constructors
	// ===========================================================

	public HLEditBoxDialog(final Context pContext, final String pTitle,
			final String pMessage, final int pInputMode, final int pInputFlag,
			final int pReturnType, final int pMaxLength,
			final boolean multiline, final String digitsFilter) {
		super(pContext, android.R.style.Theme_Translucent_NoTitleBar_Fullscreen);
		// super(context, R.style.Theme_Translucent);

		this.mTitle = pTitle;
		this.mMessage = pMessage;
		this.mInputMode = pInputMode;
		this.mInputFlag = pInputFlag;
		this.mReturnType = pReturnType;
		this.mMaxLength = pMaxLength;
		this.mIsMultiline = multiline;
		this.mDigitsFilter = digitsFilter;
	}

	@Override
	protected void onCreate(final Bundle pSavedInstanceState) {
		super.onCreate(pSavedInstanceState);

		this.getWindow().setBackgroundDrawable(new ColorDrawable(0x80000000));

		final LinearLayout layout = new LinearLayout(this.getContext());
		layout.setOrientation(LinearLayout.VERTICAL);

		final LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
				ViewGroup.LayoutParams.MATCH_PARENT,
				ViewGroup.LayoutParams.MATCH_PARENT);

		this.mTextViewTitle = new TextView(this.getContext());
		final LinearLayout.LayoutParams textviewParams = new LinearLayout.LayoutParams(
				ViewGroup.LayoutParams.WRAP_CONTENT,
				ViewGroup.LayoutParams.WRAP_CONTENT);
		textviewParams.leftMargin = textviewParams.rightMargin = this
				.convertDipsToPixels(10);
		this.mTextViewTitle.setTextSize(TypedValue.COMPLEX_UNIT_DIP, 20);
		layout.addView(this.mTextViewTitle, textviewParams);

		final LinearLayout layout1 = new LinearLayout(this.getContext());
		layout1.setOrientation(LinearLayout.HORIZONTAL);

		this.mInputEditText = new EditText(this.getContext());
		final LinearLayout.LayoutParams editTextParams = new LinearLayout.LayoutParams(
				ViewGroup.LayoutParams.MATCH_PARENT,
				ViewGroup.LayoutParams.WRAP_CONTENT, 1.f);
		editTextParams.leftMargin = editTextParams.rightMargin = this
				.convertDipsToPixels(10);

		final LinearLayout.LayoutParams btnParams = new LinearLayout.LayoutParams(
				ViewGroup.LayoutParams.WRAP_CONTENT,
				ViewGroup.LayoutParams.WRAP_CONTENT);
		Button btn = new Button(this.getContext());
		btn.setText("完成");
		btn.setOnClickListener(new android.view.View.OnClickListener() {
			public void onClick(View v) {
				HLEditBoxDialog.this.closeKeyboard();
				HLHelper.setEditTextDialogResult(
						HLEditBoxDialog.this.mInputEditText.getText()
								.toString(), false);
				HLEditBoxDialog.this.dismiss();
			}
		});

		layout1.addView(this.mInputEditText, editTextParams);
		layout1.addView(btn, btnParams);

		layout.addView(layout1);

		this.setContentView(layout, layoutParams);

		this.getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

		this.mTextViewTitle.setText(this.mTitle);
		this.mInputEditText.setText(this.mMessage);

		if (this.mDigitsFilter.length() > 0) {
			this.mInputEditText.setKeyListener(DigitsKeyListener
					.getInstance(this.mDigitsFilter));
		}

		int oldImeOptions = EditorInfo.IME_FLAG_NO_EXTRACT_UI;

		// if (this.mInputMode == kEditBoxInputModeAscii) {
		// oldImeOptions |= EditorInfo.IME_FLAG_FORCE_ASCII;
		// }
		switch (this.mInputMode) {
		case kEditBoxInputModeAny:
		case kEditBoxInputModeAscii:
//			oldImeOptions |= EditorInfo.IME_FLAG_FORCE_ASCII;
			this.mInputModeContraints = InputType.TYPE_CLASS_TEXT;
			break;
		case kEditBoxInputModeEmailAddr:
			this.mInputModeContraints = InputType.TYPE_CLASS_TEXT
					| InputType.TYPE_TEXT_VARIATION_EMAIL_ADDRESS;
			break;
		case kEditBoxInputModeNumberPad:
			this.mInputModeContraints = InputType.TYPE_CLASS_NUMBER;
			break;
		case kEditBoxInputModeNumeric:
			this.mInputModeContraints = InputType.TYPE_CLASS_NUMBER
					| InputType.TYPE_NUMBER_FLAG_SIGNED
					| InputType.TYPE_NUMBER_FLAG_DECIMAL;
			break;
		case kEditBoxInputModePhoneNumber:
			this.mInputModeContraints = InputType.TYPE_CLASS_PHONE;
			break;
		case kEditBoxInputModeUrl:
			this.mInputModeContraints = InputType.TYPE_CLASS_TEXT
					| InputType.TYPE_TEXT_VARIATION_URI;
			break;
		case kEditBoxInputModeDecimal:
			this.mInputModeContraints = InputType.TYPE_CLASS_NUMBER
					| InputType.TYPE_NUMBER_FLAG_DECIMAL;
			break;
		default:

			break;
		}

		if (this.mIsMultiline) {
			this.mInputModeContraints |= InputType.TYPE_TEXT_FLAG_MULTI_LINE;
			this.mInputModeContraints |= InputType.TYPE_TEXT_FLAG_IME_MULTI_LINE;
			this.mInputEditText.setGravity(Gravity.TOP);
			this.mInputEditText.setSingleLine(false);
			this.mInputEditText.setLines(3);
		}

		this.mInputEditText.setInputType(this.mInputModeContraints
				| this.mInputFlagConstraints);

		switch (this.mInputFlag) {
		case kEditBoxInputFlagPassword:
			this.mInputFlagConstraints = InputType.TYPE_CLASS_TEXT
					| InputType.TYPE_TEXT_VARIATION_PASSWORD;
			break;
		default:
			break;
		}

		this.mInputEditText.setInputType(this.mInputFlagConstraints
				| this.mInputModeContraints);

		switch (this.mReturnType) {
		case kKeyboardReturnTypeDefault:
			this.mInputEditText.setImeOptions(oldImeOptions
					| EditorInfo.IME_ACTION_NONE);
			break;
		case kKeyboardReturnTypeNext:
			this.mInputEditText.setImeOptions(oldImeOptions
					| EditorInfo.IME_ACTION_NEXT);
			break;
		case kKeyboardReturnTypeDone:
			this.mInputEditText.setImeOptions(oldImeOptions
					| EditorInfo.IME_ACTION_DONE);
			break;
		case kKeyboardReturnTypeSend:
			this.mInputEditText.setImeOptions(oldImeOptions
					| EditorInfo.IME_ACTION_SEND);
			break;
		case kKeyboardReturnTypeSearch:
			this.mInputEditText.setImeOptions(oldImeOptions
					| EditorInfo.IME_ACTION_SEARCH);
			break;
		case kKeyboardReturnTypeGo:
			this.mInputEditText.setImeOptions(oldImeOptions
					| EditorInfo.IME_ACTION_GO);
			break;
		default:
			this.mInputEditText.setImeOptions(oldImeOptions
					| EditorInfo.IME_ACTION_NONE);
			break;
		}

		if (this.mMaxLength > 0) {
			this.mInputEditText
					.setFilters(new InputFilter[] { new InputFilter.LengthFilter(
							this.mMaxLength) });
		}

		final Handler initHandler = new Handler();
		initHandler.postDelayed(new Runnable() {
			@Override
			public void run() {
				HLEditBoxDialog.this.mInputEditText.requestFocus();
				HLEditBoxDialog.this.mInputEditText
						.setSelection(HLEditBoxDialog.this.mInputEditText
								.length());
				HLEditBoxDialog.this.openKeyboard();
			}
		}, 200);

		this.mInputEditText
				.setOnEditorActionListener(new OnEditorActionListener() {
					@Override
					public boolean onEditorAction(final TextView v,
							final int actionId, final KeyEvent event) {
						/*
						 * If user didn't set keyboard type, this callback will
						 * be invoked twice with 'KeyEvent.ACTION_DOWN' and
						 * 'KeyEvent.ACTION_UP'.
						 */
						if (!mIsMultiline
								&& (actionId != EditorInfo.IME_NULL || (actionId == EditorInfo.IME_NULL
										&& event != null && event.getAction() == KeyEvent.ACTION_DOWN))) {
							HLEditBoxDialog.this.closeKeyboard();
							HLHelper.setEditTextDialogResult(
									HLEditBoxDialog.this.mInputEditText
											.getText().toString(), true);
							HLEditBoxDialog.this.dismiss();
							return true;
						}
						return false;
					}
				});
	}

	// ===========================================================
	// Getter & Setter
	// ===========================================================

	// ===========================================================
	// Methods for/from SuperClass/Interfaces
	// ===========================================================

	// ===========================================================
	// Methods
	// ===========================================================

	private int convertDipsToPixels(final float pDIPs) {
		final float scale = this.getContext().getResources()
				.getDisplayMetrics().density;
		return Math.round(pDIPs * scale);
	}

	@Override
	public void onBackPressed() {
		HLHelper.deactiveTextInput();
		super.onBackPressed();
	}

	private void openKeyboard() {
		final InputMethodManager imm = (InputMethodManager) this.getContext()
				.getSystemService(Context.INPUT_METHOD_SERVICE);
		imm.showSoftInput(this.mInputEditText, 0);
	}

	private void closeKeyboard() {
		final InputMethodManager imm = (InputMethodManager) this.getContext()
				.getSystemService(Context.INPUT_METHOD_SERVICE);
		imm.hideSoftInputFromWindow(this.mInputEditText.getWindowToken(), 0);
	}

	// ===========================================================
	// Inner and Anonymous Classes
	// ===========================================================
}
