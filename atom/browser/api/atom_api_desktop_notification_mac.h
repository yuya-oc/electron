// Copyright (c) 2016 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_API_ATOM_API_DESKTOP_NOTIFICATION_MAC_H_
#define ATOM_BROWSER_API_ATOM_API_DESKTOP_NOTIFICATION_MAC_H_

#include "atom/browser/api/atom_api_desktop_notification.h"

namespace atom {

namespace api {

class NotificationMac : public Notification {
protected:
 NotificationMac(v8::Isolate* isolate);

private:
 friend class Notification;

 DISALLOW_COPY_AND_ASSIGN(NotificationMac);
};

}  // namespace api

}  // namespace atom

#endif  // ATOM_BROWSER_API_ATOM_API_DESKTOP_NOTIFICATION_MAC_H_
