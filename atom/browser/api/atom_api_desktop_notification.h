// Copyright (c) 2016 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_API_ATOM_API_DESKTOP_NOTIFICATION_H_
#define ATOM_BROWSER_API_ATOM_API_DESKTOP_NOTIFICATION_H_

#include <string>

#include "atom/browser/api/event_emitter.h"
#include "atom/browser/ui/desktop_notification.h"
#include "native_mate/handle.h"

namespace atom {

namespace api {

class Notification :
  public mate::EventEmitter<Notification>, public DesktopNotificationObserver {
 public:
  static mate::Handle<Notification> CreateHandle(v8::Isolate* isolate);
  static v8::Local<v8::Value> Create(const std::string& title,
                                     mate::Arguments* args);
  static void BuildPrototype(v8::Isolate* isolate,
                             v8::Local<v8::FunctionTemplate> prototype);

 protected:
  explicit Notification(v8::Isolate* isolate);
  ~Notification() override;

  // DesktopNotificationObserver overrides.
  void OnClick() override;
  void OnDismiss() override;
  void OnFail() override;

  void Close();

 private:
  std::unique_ptr<
    atom::DesktopNotification> desktop_notification_;

  DISALLOW_COPY_AND_ASSIGN(Notification);
};

}  // namespace api

}  // namespace atom

#endif  // ATOM_BROWSER_API_ATOM_API_DESKTOP_NOTIFICATION_H_
