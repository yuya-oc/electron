// Copyright (c) 2016 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "atom/browser/api/atom_api_desktop_notification.h"

#include "atom/common/node_includes.h"
#include "native_mate/dictionary.h"

namespace atom {

namespace api {

Notification::Notification(v8::Isolate* isolate) :
  desktop_notification_(DesktopNotification::Create()) {
  Init(isolate);
  desktop_notification_->AddObserver(this);
}

Notification::~Notification() {
}

void Notification::OnClick() {
  Emit("click");
}

void Notification::OnDismiss() {
  Emit("dismiss");
}

void Notification::OnFail() {
  Emit("fail");
}

// static
mate::Handle<Notification> Notification::CreateHandle(v8::Isolate* isolate) {
  return mate::CreateHandle(isolate, new Notification(isolate));
}

v8::Local<v8::Value> Notification::Create(const std::string& title,
                                          mate::Arguments* args) {
  auto notification = Notification::CreateHandle(args->isolate());
  DesktopNotificationOption option;
  if (args->Length() > 1) {
    mate::Dictionary dic;
    args->GetNext(&dic);
    dic.Get("body", &option.body);
    dic.Get("tag", &option.tag);
    dic.Get("icon", &option.icon);
    dic.Get("silent", &option.silent);
  }
  notification.get()->desktop_notification_->Show(title, option);
  return notification.ToV8();
}

// static
void Notification::BuildPrototype(v8::Isolate* isolate,
                                  v8::Local<v8::FunctionTemplate> prototype) {
  prototype->SetClassName(mate::StringToV8(isolate, "DesktopNotification"));
  mate::ObjectTemplateBuilder(isolate, prototype->PrototypeTemplate())
    .MakeDestroyable()
    .SetMethod("close", &Notification::Close);
}

void Notification::Close() {
  desktop_notification_->Close();
}

}  // namespace api

}  // namespace atom

namespace {

void Initialize(v8::Local<v8::Object> exports, v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context, void* priv) {
  v8::Isolate* isolate = context->GetIsolate();
  mate::Dictionary dict(isolate, exports);
  dict.Set("DesktopNotification",
           atom::api::Notification::GetConstructor(isolate)->GetFunction());
  dict.SetMethod("create", &atom::api::Notification::Create);
}

}  // namespace

NODE_MODULE_CONTEXT_AWARE_BUILTIN(atom_common_desktop_notification, Initialize);
