#include "HLResourceManager.h"

#include "android/asset_manager.h"
#include "android/asset_manager_jni.h"

#include "jni/JniHelper.h"

#include <stdlib.h>

using namespace std;

AAssetManager* s_assetmanager;

extern "C" {
    JNIEXPORT void JNICALL
    Java_com_hoolai_engine_HLHelper_nativeSetAssetManager(JNIEnv* env,
                                                               jobject thiz,
                                                               jobject java_assetmanager) {
        AAssetManager* assetmanager =
            AAssetManager_fromJava(env, java_assetmanager);
        if (NULL == assetmanager) {
            return;
        }

        s_assetmanager = assetmanager;
    }
}

NS_HL_BEGIN

void HLResourceManager::initResourcePathsNative()
{
    JniMethodInfo t;
    
    if (JniHelper::getStaticMethodInfo(t, "com/hoolai/engine/HLHelper", "getWritablePath", "()Ljava/lang/String;")) {
        jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
        mWritablePath = JniHelper::jstring2string(str);
        t.env->DeleteLocalRef(str);
    }
    addFileSearchPath(mWritablePath.c_str());
}

HLFileData* HLResourceManager::getFileDataInAppBundle(const char* file)
{
	if (s_assetmanager)
	{
		AAsset* asset =
            AAssetManager_open(s_assetmanager,
                               file,
                               AASSET_MODE_UNKNOWN);
        if (!asset)
        	return NULL;
        off_t size = AAsset_getLength(asset);
        unsigned char * buffer = new unsigned char[size];
        int bytesread = AAsset_read(asset, (void*)buffer, size);
        AAsset_close(asset);
        HLFileData* data = new HLFileData();
        data->size = bytesread;
        data->buffer = buffer;
        return data;
	}
	return NULL;
}

bool HLResourceManager::fileInAppBundle(const char* file)
{
	bool ret = false;
	if (s_assetmanager)
	{
		AAsset* aa = AAssetManager_open(s_assetmanager, file, AASSET_MODE_UNKNOWN);
		if (aa)
		{
			ret = true;
			AAsset_close(aa);
		}
	}
	return ret;
}

bool HLResourceManager::copyFileInAssetToWritablePath(const char* file)
{
	if (s_assetmanager)
	{
        AAsset* asset = AAssetManager_open(s_assetmanager, file, AASSET_MODE_UNKNOWN);
        if (!asset)
        {
            AAsset_close(asset);
            return false;
        }
        off_t size = AAsset_getLength(asset);
        unsigned char * buffer = new unsigned char[1024*1024];
        
        std::string wpath = mWritablePath+file;
        FILE* fp = fopen(wpath.c_str(), "wb");
        do
        {
            int bytesread = AAsset_read(asset, (void*)buffer, 1024*1024);
            fwrite(buffer, bytesread, 1, fp);
            size -= bytesread;
        } while (size > 0);
        fclose(fp);
        delete [] buffer;
    }
}

NS_HL_END
