import sys
import os

def buildandroid():
    os.system('./build_debug.sh');
    os.system('cp obj/local/armeabi/libHoolaiEngine.a ../build/android/armeabi/debug/');
    os.system('./build_release.sh');
    os.system('cp obj/local/armeabi/libHoolaiEngine.a ../build/android/armeabi/release/');

def buildios():
    os.system('xcodebuild -scheme "HoolaiEngineIOSStatic" -configuration Debug -sdk iphoneos8.2  -derivedDataPath ./build build')
    os.system('xcodebuild -scheme "HoolaiEngineIOSStatic" -configuration Debug -sdk iphonesimulator8.2 -derivedDataPath ./build build')
    os.system('xcodebuild -scheme "HoolaiEngineIOSStatic" -configuration Release -sdk iphoneos8.2 -derivedDataPath ./build build')
    os.system('xcodebuild -scheme "HoolaiEngineIOSStatic" -configuration Release -sdk iphonesimulator8.2 -derivedDataPath ./build build')
    os.system('xcodebuild -scheme "HoolaiEngineMacStatic" -configuration Debug -derivedDataPath ./build build')
    os.system('xcodebuild -scheme "HoolaiEngineMacStatic" -configuration Release -derivedDataPath ./build build')
    os.system('strip build/Build/Products/Debug/libHoolaiEngineMacStatic.a')
    os.system('strip build/Build/Products/Release/libHoolaiEngineMacStatic.a')
    os.system('strip build/Build/Products/Debug-iphoneos/libHoolaiEngineIOSStatic.a')
    os.system('strip build/Build/Products/Release-iphoneos/libHoolaiEngineIOSStatic.a')
    os.system('strip build/Build/Products/Debug-iphonesimulator/libHoolaiEngineIOSStatic.a')
    os.system('strip build/Build/Products/Release-iphonesimulator/libHoolaiEngineIOSStatic.a')
    os.system('cp build/Build/Products/Debug/libHoolaiEngineMacStatic.a ../build/mac/debug/libHoolaiEngine.a')
    os.system('cp build/Build/Products/Release/libHoolaiEngineMacStatic.a ../build/mac/release/libHoolaiEngine.a')
    os.system('lipo -create build/Build/Products/Debug-iphoneos/libHoolaiEngineIOSStatic.a build/Build/Products/Debug-iphonesimulator/libHoolaiEngineIOSStatic.a -o ../build/ios/debug/libHoolaiEngine.a')
    os.system('lipo -create build/Build/Products/Release-iphoneos/libHoolaiEngineIOSStatic.a build/Build/Products/Release-iphonesimulator/libHoolaiEngineIOSStatic.a -o ../build/ios/release/libHoolaiEngine.a')

if __name__=='__main__':
    reload(sys)
    sys.setdefaultencoding('utf-8')
    os.system('./copyheaders_android.sh');
    os.system('./copyheaders_ios.sh');
    os.chdir('android_helloworld')
    buildandroid()
    os.chdir('../xcodeproj')
    buildios()

