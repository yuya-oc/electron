// Copyright (c) 2016 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_UI_DESKTOP_NOTIFICATION_H_
#define ATOM_BROWSER_UI_DESKTOP_NOTIFICATION_H_

#include <string>

#include "atom/browser/ui/desktop_notification_observer.h"
#include "base/observer_list.h"

#if defined(OS_WIN)
#define _HIDE_GLOBAL_ASYNC_STATUS
#include <windows.ui.notifications.h>
#include <wrl/client.h>
#include <wrl/implements.h>
#include <wrl/wrappers/corewrappers.h>
#endif

namespace atom {

#if defined(OS_WIN)
using ToastActivatedEventHandler = ABI::Windows::Foundation::ITypedEventHandler<
  ABI::Windows::UI::Notifications::ToastNotification*,
  IInspectable*>;
using ToastDismissedEventHandler = ABI::Windows::Foundation::ITypedEventHandler<
  ABI::Windows::UI::Notifications::ToastNotification*,
  ABI::Windows::UI::Notifications::ToastDismissedEventArgs*>;
using ToastFailedEventHandler = ABI::Windows::Foundation::ITypedEventHandler<
  ABI::Windows::UI::Notifications::ToastNotification*,
  ABI::Windows::UI::Notifications::ToastFailedEventArgs*>;
#endif
#if defined(OS_MACOSX)

#endif

class DesktopNotificationOption {
 public:
  std::string body = "";
  std::string tag = "";
  std::string icon = "";
  bool silent = false;
};

class DesktopNotification {
 public:
  static DesktopNotification* Create();
  virtual ~DesktopNotification();

  virtual bool Show(const std::string& title,
            const DesktopNotificationOption& option);

  void AddObserver(DesktopNotificationObserver* obs) {
    observers_.AddObserver(obs);
  }

  void RemoveObserver(DesktopNotificationObserver* obs) {
    observers_.RemoveObserver(obs);
  }

  void NotifyClick();
  void NotifyDismiss();
  void NotifyFail();

  void Close();

 protected:
  DesktopNotification();

 private:
  base::ObserverList<DesktopNotificationObserver> observers_;

  static bool initialized_;
  static bool Initialize();

#if defined(OS_WIN)
  static Microsoft::WRL::ComPtr<
    ABI::Windows::UI::Notifications::IToastNotificationManagerStatics>
    toast_manager_;
  static Microsoft::WRL::ComPtr<
    ABI::Windows::UI::Notifications::IToastNotifier>
    toast_notifier_;
  Microsoft::WRL::ComPtr<ABI::Windows::UI::Notifications::IToastNotification>
    toast_notification_;

  static bool SetupCallbacks(DesktopNotification* notification);
  static bool GetToastXml(
    ABI::Windows::UI::Notifications::IToastNotificationManagerStatics*
      toastManager,
    const std::wstring& title,
    const std::wstring& msg,
    const std::wstring& icon_path,
    bool silent,
    ABI::Windows::Data::Xml::Dom::IXmlDocument** toastXml);
  static bool SetXmlAudioSilent(
    ABI::Windows::Data::Xml::Dom::IXmlDocument* doc);
  static bool SetXmlText(ABI::Windows::Data::Xml::Dom::IXmlDocument* doc,
                         const std::wstring& text);
  static bool SetXmlText(ABI::Windows::Data::Xml::Dom::IXmlDocument* doc,
                         const std::wstring& title,
                         const std::wstring& body);
  static bool SetXmlImage(ABI::Windows::Data::Xml::Dom::IXmlDocument* doc,
                          const std::wstring& icon_path);
  static bool GetTextNodeList(
    Microsoft::WRL::Wrappers::HString* tag,
    ABI::Windows::Data::Xml::Dom::IXmlDocument* doc,
    ABI::Windows::Data::Xml::Dom::IXmlNodeList** nodeList,
    uint32_t reqLength);
  static bool AppendTextToXml(ABI::Windows::Data::Xml::Dom::IXmlDocument* doc,
                              ABI::Windows::Data::Xml::Dom::IXmlNode* node,
                              const std::wstring& text);
#endif

  DISALLOW_COPY_AND_ASSIGN(DesktopNotification);
};

#if defined(OS_WIN)
class ToastEventHandler :
  public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<
    Microsoft::WRL::ClassicCom>,
  ToastActivatedEventHandler,
  ToastDismissedEventHandler,
  ToastFailedEventHandler> {
 public:
  explicit ToastEventHandler(DesktopNotification* notification);
  ~ToastEventHandler();

  IFACEMETHODIMP Invoke(
    ABI::Windows::UI::Notifications::IToastNotification* sender,
    IInspectable* args);
  IFACEMETHODIMP Invoke(
    ABI::Windows::UI::Notifications::IToastNotification* sender,
    ABI::Windows::UI::Notifications::IToastDismissedEventArgs* e);
  IFACEMETHODIMP Invoke(
    ABI::Windows::UI::Notifications::IToastNotification* sender,
    ABI::Windows::UI::Notifications::IToastFailedEventArgs* e);

 private:
  DesktopNotification* notification_;  // weak ref.

  DISALLOW_COPY_AND_ASSIGN(ToastEventHandler);
};
#endif

}  // namespace atom

#endif  // ATOM_BROWSER_UI_DESKTOP_NOTIFICATION_H_
