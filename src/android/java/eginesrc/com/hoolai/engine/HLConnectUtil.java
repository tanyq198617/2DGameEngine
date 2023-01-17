package com.hoolai.engine;

import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.provider.Settings;
import android.telephony.TelephonyManager;

/**
 * 网络连接
 * @author huang chonggao
 *在使用ConnectivityManager时，
 *必须在AndroidManifest.xml中添加 "android.permission.ACCESS_NETWORK_STATE"  否则无法获得系统的许可。
 */
public class HLConnectUtil 
{
	/**
	 * 打开系统网络设置界面
	 * @param context
	 * @author huang chonggao
	 */
	static public void openNetSetting(Context context)
	{
		if(context!=null)
			context.startActivity(new Intent(Settings.ACTION_WIRELESS_SETTINGS));//进入无线网络配置界面
		//startActivity(new Intent(Settings.ACTION_WIFI_SETTINGS)); //进入手机中的wifi网络设置界面
	}

	/**
	 *TYPE_MOBILE = 0; 手机网络
	 *TYPE_WIFI = 1; 无线
	 *TYPE_MOBILE_MMS = 2; 彩信协议
	 *TYPE_MOBILE_SUPL = 3; 定位服务通信协议
	 *TYPE_MOBILE_DUN = 4; 蓝牙通信协议
	 *TYPE_MOBILE_HIPRI = 5;
	 *TYPE_WIMAX = 6;  宽带无线
	 *TYPE_BLUETOOTH = 7;  蓝牙
	 *TYPE_DUMMY = 8;
	 *TYPE_ETHERNET = 9; 太网
	 *
	 */
	static public int netWorkType(Context context)
	{
		if(context==null)return -1;
		ConnectivityManager connectMgr = (ConnectivityManager) context
				.getSystemService(Context.CONNECTIVITY_SERVICE);

		NetworkInfo info = connectMgr.getActiveNetworkInfo();
		// 判断网络是否是wifi，在判断之前一定要进行的非空判断，如果没有任何网络
		// 连接info ==null
		if(info!=null)
		{
			int  conecttype =info.getType();
			return conecttype;
		}

		return -1;
	}
	/** 手机网络协议服务类型
	NETWORK_TYPE_UNKNOWN = 0;
    NETWORK_TYPE_GPRS = 1; 2.5G
    NETWORK_TYPE_EDGE = 2; 2.75G
    NETWORK_TYPE_UMTS = 3; 3G
    NETWORK_TYPE_CDMA = 4;  3G
    NETWORK_TYPE_EVDO_0 = 5;
    NETWORK_TYPE_EVDO_A = 6;
    NETWORK_TYPE_1xRTT = 7;
    NETWORK_TYPE_HSDPA = 8; 
    NETWORK_TYPE_HSUPA = 9;
    NETWORK_TYPE_HSPA = 10;
    NETWORK_TYPE_IDEN = 11;
    NETWORK_TYPE_EVDO_B = 12;
    NETWORK_TYPE_LTE = 13;
    NETWORK_TYPE_EHRPD = 14;
    NETWORK_TYPE_HSPAP = 15;
	 */
	static public int mobilNetWorkType(Context context)
	{
		if(context==null)return -1;
		ConnectivityManager connectMgr = (ConnectivityManager) context
				.getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo info = connectMgr.getActiveNetworkInfo();
		// 在判断之前一定要进行的非空判断，如果没有任何网络
		// 连接info ==null
		if(info!=null)
		{
			if(info.getType() == ConnectivityManager.TYPE_MOBILE)
				return info.getSubtype();  

		}

		return -1;
	}
	/**
	 * 取出网络状态
	 * @return  -1无法获取  0无知网络  1 wifi  2 2G网络 3 3G网络 4其他网络
	 * @author huang chonggao
	 */
	static public int getNetWorkType(){
		int linktype=netWorkType(HLActivity.getContext());
		if(linktype==1 || linktype==6)
		{
			return 1;
		}
		if(linktype==0)
		{
			int mobtype= mobilNetWorkType(HLActivity.getContext());
			if(mobtype<1)
				return mobtype;
			else if(mobtype>2)
				return 3;
			else
				return 2;
		}
		else if(linktype >0)
			return 4;
		else
			return -1;
	}
	/**
	 * 打开系统设置界面
	 * 
	 * @author huang chonggao
	 */
	static public void openNetWorkSetting()
	{
		openNetSetting(HLActivity.getContext());
	}
}
