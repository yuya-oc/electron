// Copyright (c) 2016 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "atom/browser/api/atom_api_desktop_notification.h"
#include "atom/browser/api/atom_api_desktop_notification_mac.h"
#include "atom/browser/ui/desktop_notification_mac.h"

#include "atom/common/node_includes.h"
#include "native_mate/dictionary.h"

namespace atom {

namespace api {

  NotificationMac::NotificationMac(v8::Isolate* isolate)
      : Notification(isolate, DesktopNotificationMac::Create()) {
  }

  // static
  mate::Handle<Notification> Notification::CreateHandle(v8::Isolate* isolate) {
    return mate::CreateHandle<Notification>(isolate, new NotificationMac(isolate));
  }

}  // namespace api
}  // namespace atom
