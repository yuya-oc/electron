#include "atom/browser/ui/electron_notification_center_delegate.h"
#include "atom/browser/ui/desktop_notification_mac.h"

@implementation ElectronNotificationCenterDelegate

- (void)userNotificationCenter:(NSUserNotificationCenter*)center
       didActivateNotification:(NSUserNotification *)notif {
    auto notification = atom::DesktopNotificationMac::GetNotification(notif);
    if (notification){
        notification->NotifyClick();
      }
    [center removeDeliveredNotification:notif];
}

- (BOOL)userNotificationCenter:(NSUserNotificationCenter*)center
     shouldPresentNotification:(NSUserNotification*)notification {
    // Display notifications even if the app is active.
    return YES;
}

@end
