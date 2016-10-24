
#ifndef ATOM_BROWSER_UI_DESKTOP_NOTIFICATION_MAC_H_
#define ATOM_BROWSER_UI_DESKTOP_NOTIFICATION_MAC_H_

#import <Foundation/Foundation.h>
#include <set>
#include "atom/browser/ui/desktop_notification.h"
#include "atom/browser/ui/electron_notification_center_delegate.h"
#include "base/mac/scoped_nsobject.h"

namespace atom {
    class DesktopNotificationMac: public DesktopNotification {
    public:
        static DesktopNotification* Create();
        virtual ~DesktopNotificationMac();

        bool Show(const std::string& title,
                  const DesktopNotificationOption& option) override;
//        void Close();
        NSUserNotification* notification() const {return notification_;}
        static DesktopNotificationMac* GetNotification(NSUserNotification* notif);
    protected:
        DesktopNotificationMac();
//        static bool Initialize();
    private:
        friend DesktopNotification;
        base::scoped_nsobject<NSUserNotification> notification_;
        static base::scoped_nsobject<ElectronNotificationCenterDelegate> notification_center_delegate_;
        static std::set<DesktopNotificationMac*> notifications_;
        DISALLOW_COPY_AND_ASSIGN(DesktopNotificationMac);

    };
}  // namespace atom

#endif
