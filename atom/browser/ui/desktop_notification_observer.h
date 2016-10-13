// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_UI_DESKTOP_NOTIFICATION_OBSERVER_H_
#define ATOM_BROWSER_UI_DESKTOP_NOTIFICATION_OBSERVER_H_

namespace atom {

class DesktopNotificationObserver {
 public:
  virtual void OnClick() {}
  virtual void OnDismiss() {}
  virtual void OnFail() {}

 protected:
  virtual ~DesktopNotificationObserver() {}
};

}  // namespace atom

#endif  // ATOM_BROWSER_UI_DESKTOP_NOTIFICATION_OBSERVER_H_
