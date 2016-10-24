// Copyright (c) 2016 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "atom/browser/ui/desktop_notification.h"

namespace atom {

// static
bool DesktopNotification::initialized_;

DesktopNotification::DesktopNotification() {
  if (!initialized_) {
    initialized_ = Initialize();
  }
}

DesktopNotification::~DesktopNotification() {
}

// static
DesktopNotification* DesktopNotification::Create() {
  return new DesktopNotification();
}

void DesktopNotification::NotifyClick() {
  FOR_EACH_OBSERVER(DesktopNotificationObserver, observers_, OnClick());
}

void DesktopNotification::NotifyDismiss() {
  FOR_EACH_OBSERVER(DesktopNotificationObserver, observers_, OnDismiss());
}

void DesktopNotification::NotifyFail() {
  FOR_EACH_OBSERVER(DesktopNotificationObserver, observers_, OnFail());
}


}  // namespace atom
