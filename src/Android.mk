LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := hoolaiengine_static

LOCAL_MODULE_FILENAME := libHoolaiEngine

LOCAL_SRC_FILES := \
application/HLApplication.cpp  \
application/HLDirector2D.cpp  \
base/HLAutoreleasePool.cpp  \
base/HLNotificationCenter.cpp  \
base/HLObject.cpp  \
common/bmfont/HLBMFont.cpp  \
common/HLAsyncGLTask.cpp  \
common/HLClassRegister.cpp  \
common/HLFont/HLFont.cpp  \
common/HLGeometry.cpp  \
common/HLProfileUtil.cpp  \
common/HLStringUtil.cpp  \
common/HLUtil.cpp \
common/md5.cpp  \
core/components/HLBatchComponent.cpp  \
core/components/HLBlendFuncComponent.cpp  \
core/components/HLBMFontComponent.cpp  \
core/components/HLCamera2DComponent.cpp  \
core/components/HLClippingComponent.cpp  \
core/components/HLColorAdvanceComponent.cpp  \
core/components/HLColorComponent.cpp  \
core/components/HLGUIViewComponent.cpp  \
core/components/HLParticleComponent.cpp  \
core/components/HLParticleExampleComponent.cpp  \
core/components/HLPolygonHitTestComponent.cpp  \
core/components/HLScissorComponent.cpp  \
core/components/HLShaderComponent.cpp  \
core/components/HLSpriteComponent.cpp  \
core/components/HLTransform2DComponent.cpp  \
core/components/skeleton/HLBone.cpp  \
core/components/skeleton/HLBoneAnimation.cpp  \
core/components/skeleton/HLSkeletonComponent.cpp  \
core/components/skeleton/HLTween.cpp  \
core/components/skeleton/skeleton.pb.cpp  \
core/components/HLPrimitiveComponent.cpp  \
core/eventdispatcher/HLTouchDispatcher.cpp  \
core/HLAnimateSystem.cpp  \
core/HLEntitySystem.cpp  \
core/HLResourceManager.cpp  \
core/HLSceneManager.cpp  \
core/HLSpriteFrame.cpp  \
core/HLTimer.cpp  \
core/utils/HLFileUtil.cpp  \
glhelpers/HLDrawingPrimitives.cpp  \
glhelpers/HLGLConfiguration.cpp  \
glhelpers/HLGLMatrixStack.cpp  \
glhelpers/HLGLStateCache.cpp  \
glhelpers/HLProgram.cpp  \
glhelpers/HLGaussianBlurProgram.cpp  \
gui/guifile.pb.cpp  \
gui/HLAlertView.cpp  \
gui/HLButton.cpp  \
gui/HLEditBox.cpp  \
gui/HLGUIManager.cpp  \
gui/HLImageView.cpp  \
gui/HLLabel.cpp  \
gui/HLProgressIndicator.cpp  \
gui/HLProgressRadialView.cpp  \
gui/HLSceneView.cpp  \
gui/HLScrollView.cpp  \
gui/HLTableView.cpp  \
gui/HLTableViewCell.cpp  \
gui/HLToast.cpp  \
gui/HLToolTip.cpp  \
gui/HLView.cpp  \
gui/HLViewDND.cpp  \
gui/HLViewLoader.cpp  \
gui/HLWidget.cpp  \
gui/RichLabel/HLRichLabel.cpp  \
gui/RichLabel/HLStyledNode.cpp  \
gui/RichLabel/HLStyledTextParser.cpp  \
gui/RichLabel/HLXHTMLParser.cpp  \
jsscripting/JSComponent.cpp  \
jsscripting/JSScriptingCore.cpp  \
jsscripting/jswrappers/gui/JSAlertView.cpp  \
jsscripting/jswrappers/gui/JSButton.cpp  \
jsscripting/jswrappers/gui/JSEditBox.cpp  \
jsscripting/jswrappers/gui/JSGUIManager.cpp  \
jsscripting/jswrappers/gui/JSImageView.cpp  \
jsscripting/jswrappers/gui/JSLabel.cpp  \
jsscripting/jswrappers/gui/JSProgressBar.cpp  \
jsscripting/jswrappers/gui/JSProgressRadialView.cpp  \
jsscripting/jswrappers/gui/JSRichLabel.cpp  \
jsscripting/jswrappers/gui/JSSceneView.cpp  \
jsscripting/jswrappers/gui/JSScrollView.cpp \
jsscripting/jswrappers/gui/JSToast.cpp  \
jsscripting/jswrappers/gui/JSToolTip.cpp  \
jsscripting/jswrappers/gui/JSView.cpp  \
jsscripting/jswrappers/gui/JSViewDND.cpp  \
jsscripting/jswrappers/gui/JSViewLoader.cpp  \
jsscripting/jswrappers/gui/JSWidget.cpp  \
jsscripting/jswrappers/JSAnimationSystem.cpp  \
jsscripting/jswrappers/JSConversions.cpp  \
jsscripting/jswrappers/JSCPPWrapper.cpp  \
jsscripting/jswrappers/JSDirector2D.cpp  \
jsscripting/jswrappers/JSEntitySystem.cpp  \
jsscripting/jswrappers/JSNotificationCenter.cpp  \
jsscripting/jswrappers/JSSceneManager.cpp  \
jsscripting/jswrappers/JSSpriteFrame.cpp  \
jsscripting/jswrappers/JSTexture.cpp  \
jsscripting/jswrappers/JSTimer.cpp \
jsscripting/jswrappers/JSApplication.cpp \
jsscripting/jswrappers/JSResourceManager.cpp \
jsscripting/jswrappers/JSUserDefaults.cpp \
jsscripting/jswrappers/JSTouchDispatcher.cpp \
jsscripting/jswrappers/network/JSHttpRequest.cpp \
network/HLHttpDownloadManager.cpp \
network/HLHttpNetwork.cpp \
network/HLHttpRequest.cpp \
network/HLReachability.cpp \
textures/HLImageJPEGCodec.cpp \
textures/HLImagePNGCodec.cpp \
textures/HLImagePVRCodec.cpp \
textures/HLImageWEBPCodec.cpp \
textures/HLImageETC1Codec.cpp \
textures/HLRenderTexture.cpp \
textures/HLTexture.cpp \
../libs/kazmath/src/aabb.c \
../libs/kazmath/src/mat3.c \
../libs/kazmath/src/mat4.c \
../libs/kazmath/src/neon_matrix_impl.c \
../libs/kazmath/src/plane.c \
../libs/kazmath/src/quaternion.c \
../libs/kazmath/src/ray2.c \
../libs/kazmath/src/utility.c \
../libs/kazmath/src/vec2.c \
../libs/kazmath/src/vec3.c \
../libs/kazmath/src/vec4.c \
../libs/kazmath/src/GL/mat4stack.c \
../libs/kazmath/src/GL/matrix.c \
../libs/OpenThreads/src/common/Atomic.cpp \
../libs/OpenThreads/src/common/Version.cpp \
../libs/OpenThreads/src/pthreads/PThread.cpp \
../libs/OpenThreads/src/pthreads/PThreadBarrier.cpp \
../libs/OpenThreads/src/pthreads/PThreadCondition.cpp \
../libs/OpenThreads/src/pthreads/PThreadMutex.cpp \
android/HLApplication_android.cpp \
android/HLUserDefaults.cpp \
android/HLApkTools.cpp \
android/HLResourceManager_android.cpp \
android/HLAsyncGLTask.cpp \
android/jni/IMEJni.cpp \
android/jni/Java_com_hoolai_engine_HLRenderer.cpp \
android/jni/Java_com_hoolai_engine_HLHelper.cpp \
android/jni/Java_com_hoolai_engine_HLNotification.cpp \
android/jni/JniHelper.cpp \
android/jni/TouchesJni.cpp 


LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include/android \
			   $(LOCAL_PATH)/../libs/kazmath/include \
			   $(LOCAL_PATH)/../libs/json \
			   $(LOCAL_PATH)/../libs/OpenThreads/include 		   

LOCAL_C_INCLUDES :=$(LOCAL_PATH)/../include/android \
		   	$(LOCAL_PATH)/../libs/kazmath/include \
		   	$(LOCAL_PATH)/../libs/json \
			$(LOCAL_PATH)/../libs/OpenThreads/include \
		   	$(LOCAL_PATH)/android

LOCAL_LDLIBS := -lGLESv2 \
                -llog \
                -lz \
                -landroid \
                -lEGL

LOCAL_EXPORT_LDLIBS := -lGLESv2 \
                       -llog \
                       -lz \
                       -landroid \
                       -lEGL
                       
LOCAL_WHOLE_STATIC_LIBRARIES := libpng_static
LOCAL_WHOLE_STATIC_LIBRARIES += libjpeg_static
LOCAL_WHOLE_STATIC_LIBRARIES += libwebp_static
LOCAL_WHOLE_STATIC_LIBRARIES += libjson
LOCAL_WHOLE_STATIC_LIBRARIES += libcurl_static
LOCAL_WHOLE_STATIC_LIBRARIES += libprotobuf_lite_static


include $(BUILD_STATIC_LIBRARY)

$(call import-module,curl/android)
$(call import-module,protobuf)
$(call import-module,jpeg/android)
$(call import-module,png/android)
$(call import-module,webp/android)
