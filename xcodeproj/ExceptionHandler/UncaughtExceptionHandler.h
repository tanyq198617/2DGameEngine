#import <UIKit/UIKit.h>

@interface UncaughtExceptionHandler : NSObject
{
    BOOL dismissed;
}

@end

#ifdef __cplusplus
extern "C"{
#endif
void InstallUncaughtExceptionHandler();
#ifdef __cplusplus
}
#endif
