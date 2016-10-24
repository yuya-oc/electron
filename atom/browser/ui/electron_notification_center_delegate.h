
#ifndef BROWSER_MAC_NOTIFICATION_DELEGATE_H_
#define BROWSER_MAC_NOTIFICATION_DELEGATE_H_

#import <Foundation/Foundation.h>

// NotificationCenterDegelate conflicts with electron/brightray's one.
@interface ElectronNotificationCenterDelegate :
NSObject<NSUserNotificationCenterDelegate> {
}
@end

#endif  // BROWSER_MAC_NOTIFICATION_DELEGATE_H_
