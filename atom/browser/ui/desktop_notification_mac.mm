#include "atom/browser/ui/desktop_notification_mac.h"
#include "base/strings/sys_string_conversions.h"
namespace atom {

  //static
    base::scoped_nsobject<ElectronNotificationCenterDelegate> DesktopNotificationMac::notification_center_delegate_;
    //        static
    std::set<DesktopNotificationMac*> DesktopNotificationMac::notifications_;

    
  DesktopNotificationMac::DesktopNotificationMac(){
     notification_.reset([[NSUserNotification alloc] init]);
      notifications_.insert(this);
  }
  DesktopNotificationMac::~DesktopNotificationMac(){
      [NSUserNotificationCenter.defaultUserNotificationCenter
       removeDeliveredNotification:notification_];
      notifications_.erase(this);
  }

  // static
  DesktopNotification* DesktopNotificationMac::Create() {
    return new DesktopNotificationMac();
  }

// static
bool DesktopNotification::Initialize() {
    DesktopNotificationMac::notification_center_delegate_.reset([ElectronNotificationCenterDelegate alloc]);
    NSUserNotificationCenter.defaultUserNotificationCenter.delegate = DesktopNotificationMac::notification_center_delegate_;
  return true;
}

    bool DesktopNotification::Show(const std::string& title,
                                   const DesktopNotificationOption& option){
        return true;
    }

bool DesktopNotificationMac::Show(const std::string& title,
                               const DesktopNotificationOption& option) {
    [notification_ setTitle:base::SysUTF8ToNSString(title)];
    [notification_ setInformativeText:base::SysUTF8ToNSString(option.body)];

/*    if ([notification_ respondsToSelector:@selector(setContentImage:)] &&
        !icon.drawsNothing()) {
        NSImage* image = skia::SkBitmapToNSImageWithColorSpace(
                                                               icon, base::mac::GetGenericRGBColorSpace());
        [notification_ setContentImage:image];
    }*/

    if (option.silent) {
        [notification_ setSoundName:nil];
    } else {
        [notification_ setSoundName:NSUserNotificationDefaultSoundName];
    }

    [NSUserNotificationCenter.defaultUserNotificationCenter
     deliverNotification:notification_];
    return true;
}


void DesktopNotification::Close() {
  return;
}

    // static
    DesktopNotificationMac* DesktopNotificationMac::GetNotification(NSUserNotification* notif){
        for (DesktopNotificationMac* native_notification : notifications_) {
            if ([native_notification->notification() isEqual:notif])
                return native_notification;
        }
        return nullptr;
    }
    
}  // namespace atom
