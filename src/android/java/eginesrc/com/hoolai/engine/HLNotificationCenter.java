package com.hoolai.engine;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

/**
 * user notification game engine send message to android Activity and call back
 * game engine
 * 
 * @author huang chonggao
 * 
 */
public class HLNotificationCenter {
	public static interface HLNotificationInterface {
		public void onNotification(String notifyName, Object userInfo);
	}

	private static Map<String, List<HLNotificationInterface>> observers = new HashMap<String, List<HLNotificationInterface>>();

	public static void addObserver(String notifyName,
			HLNotificationInterface observer) {
		if (!observers.containsKey(notifyName)) {
			List<HLNotificationInterface> list = new LinkedList<HLNotificationInterface>();
			list.add(observer);
			observers.put(notifyName, list);
		} else {
			List<HLNotificationInterface> list = observers.get(notifyName);
			if (!list.contains(observer)) {
				list.add(observer);
			}
		}
	}

	public static void removeObserver(String notifyName,
			HLNotificationInterface observer) {
		if (observers.containsKey(notifyName)) {
			List<HLNotificationInterface> list = observers.get(notifyName);
			list.remove(observer);
		}
	}

	public static void removeObserver(HLNotificationInterface observer) {
		for (List<HLNotificationInterface> obs : observers.values()) {
			obs.remove(observer);
		}
	}

	/**
	 * send message use notification
	 * 
	 * @param notiname
	 * @param pdata
	 * @param notiID
	 * @author huang chonggao
	 */
	private static native void nativePostNotification(String notiname,
			Object msg);

	public static void postNotification(final String notiname, final Object msg) {
		// postNotification is thread-safe
//		HLHelper.getsHLHelperListener().runOnGLThread(new Runnable() {
//			@Override
//			public void run() {
				nativePostNotification(notiname, msg);
//			}
//		});
	}

	/**
	 * pop notification to top
	 * 
	 * @param notiname
	 * @param pdata
	 * @param notiID
	 * @author huang chonggao
	 */
	private static void onNotification(final String notifyName, final Object msg) {
		((HLActivity) HLActivity.getContext())
		.runOnJavaMainThread(new Runnable() {

			@Override
			public void run() {
				List<HLNotificationInterface> list = observers
						.get(notifyName);
				if (list != null) {
					for (HLNotificationInterface o : list) {
						o.onNotification(notifyName, msg);
					}
				}
			}
		});
	}
}
