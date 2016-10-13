// Copyright(c) 2016 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "atom/browser/ui/desktop_notification.h"

#include <ShObjIdl.h>
#include <wrl/wrappers/corewrappers.h>

#include "atom/browser/browser.h"
#include "base/strings/utf_string_conversions.h"
#include "common/application_info.h"

namespace atom {

using ABI::Windows::Data::Xml::Dom::IXmlAttribute;
using ABI::Windows::Data::Xml::Dom::IXmlDocument;
using ABI::Windows::Data::Xml::Dom::IXmlElement;
using ABI::Windows::Data::Xml::Dom::IXmlNamedNodeMap;
using ABI::Windows::Data::Xml::Dom::IXmlNodeList;
using ABI::Windows::Data::Xml::Dom::IXmlNode;
using ABI::Windows::Data::Xml::Dom::IXmlText;
using ABI::Windows::UI::Notifications::IToastNotification;
using ABI::Windows::UI::Notifications::IToastNotificationFactory;
using ABI::Windows::UI::Notifications::IToastNotificationManagerStatics;
using ABI::Windows::UI::Notifications::IToastNotifier;
using Microsoft::WRL::ComPtr;
using Microsoft::WRL::Wrappers::HString;
using Microsoft::WRL::Wrappers::HStringReference;

// static
ComPtr<IToastNotificationManagerStatics> DesktopNotification::toast_manager_;
ComPtr<IToastNotifier> DesktopNotification::toast_notifier_;

// static
bool DesktopNotification::Initialize() {
  HStringReference toast_manager_str(
    RuntimeClass_Windows_UI_Notifications_ToastNotificationManager);
  auto ret = Windows::Foundation::GetActivationFactory(toast_manager_str.Get(),
                                                       &toast_manager_);
  if (FAILED(ret)) {
    return false;
  }

  HString app_id;
  app_id.Set(Browser::Get()->GetAppUserModelID());

  return SUCCEEDED(
    toast_manager_->CreateToastNotifierWithId(app_id.Get(), &toast_notifier_));
}

bool DesktopNotification::Show(const std::string& title,
                               const DesktopNotificationOption& option) {
  ComPtr<IXmlDocument> toast_xml;
  if (FAILED(GetToastXml(toast_manager_.Get(),
                         base::UTF8ToUTF16(title),
                         base::UTF8ToUTF16(option.body),
                         base::UTF8ToUTF16(option.icon),
                         option.silent,
                         &toast_xml))) {
    return false;
  }

  ComPtr<IToastNotificationFactory> toast_factory;
  if (FAILED(Windows::Foundation::GetActivationFactory(
    HStringReference(
      RuntimeClass_Windows_UI_Notifications_ToastNotification).Get(),
    &toast_factory))) {
    return false;
  }

  if (FAILED(toast_factory->CreateToastNotification(
    toast_xml.Get(), &toast_notification_))) {
    return false;
  }
  SetupCallbacks(this);
  toast_notifier_->Show(toast_notification_.Get());

  return true;
}

void DesktopNotification::Close() {
  toast_notifier_->Hide(toast_notification_.Get());
}

// static
bool DesktopNotification::SetupCallbacks(DesktopNotification* notification) {
  auto handler = Microsoft::WRL::Make<ToastEventHandler>(notification);
  EventRegistrationToken activated_token;
  notification->toast_notification_->
    add_Activated(handler.Get(), &activated_token);
  EventRegistrationToken dismissed_token;
  notification->toast_notification_->
    add_Dismissed(handler.Get(), &dismissed_token);
  EventRegistrationToken failed_token;
  notification->toast_notification_->
    add_Failed(handler.Get(), &failed_token);
  return true;
}

// static
bool DesktopNotification::GetToastXml(
  IToastNotificationManagerStatics* toastManager,
  const std::wstring& title,
  const std::wstring& msg,
  const std::wstring& icon_path,
  bool silent,
  IXmlDocument** toast_xml) {
  ABI::Windows::UI::Notifications::ToastTemplateType template_type;
  if (title.empty() || msg.empty()) {
    // Single line toast.
    template_type = icon_path.empty() ?
      ABI::Windows::UI::Notifications::ToastTemplateType_ToastText01 :
      ABI::Windows::UI::Notifications::ToastTemplateType_ToastImageAndText01;
    if (FAILED(toast_manager_->GetTemplateContent(template_type, toast_xml)))
      return false;
    if (!SetXmlText(*toast_xml, title.empty() ? msg : title))
      return false;
  } else {
    // Title and body toast.
    template_type = icon_path.empty() ?
      ABI::Windows::UI::Notifications::ToastTemplateType_ToastText02 :
      ABI::Windows::UI::Notifications::ToastTemplateType_ToastImageAndText02;
    if (FAILED(toastManager->GetTemplateContent(template_type, toast_xml)))
      return false;
    if (!SetXmlText(*toast_xml, title, msg))
      return false;
  }

  // Configure the toast's notification sound
  if (silent) {
    if (FAILED(SetXmlAudioSilent(*toast_xml))) {
      return false;
    }
  }

  // Configure the toast's image
  if (!icon_path.empty()) {
    return SetXmlImage(*toast_xml, icon_path);
  }

  return true;
}

// static
bool DesktopNotification::SetXmlAudioSilent(
  IXmlDocument* doc) {
  HStringReference tag(L"toast");

  ComPtr<IXmlNodeList> node_list;
  if (FAILED(doc->GetElementsByTagName(tag.Get(), &node_list)))
    return false;

  ComPtr<IXmlNode> root;
  if (FAILED(node_list->Item(0, &root)))
    return false;

  ComPtr<IXmlElement> audio_element;
  HStringReference audio_str(L"audio");
  if (FAILED(doc->CreateElement(audio_str.Get(), &audio_element)))
    return false;

  ComPtr<IXmlNode> audio_node_tmp;
  if (FAILED(audio_element.As(&audio_node_tmp)))
    return false;

  // Append audio node to toast xml
  ComPtr<IXmlNode> audio_node;
  if (FAILED(root->AppendChild(audio_node_tmp.Get(), &audio_node)))
    return false;

  // Create silent attribute
  ComPtr<IXmlNamedNodeMap> attributes;
  if (FAILED(audio_node->get_Attributes(&attributes)))
    return false;

  ComPtr<IXmlAttribute> silent_attribute;
  HStringReference silent_str(L"silent");
  if (FAILED(doc->CreateAttribute(silent_str.Get(), &silent_attribute)))
    return false;

  ComPtr<IXmlNode> silent_attribute_node;
  if (FAILED(silent_attribute.As(&silent_attribute_node)))
    return false;

  // Set silent attribute to true
  HStringReference silent_value(L"true");

  ComPtr<IXmlText> silent_text;
  if (FAILED(doc->CreateTextNode(silent_value.Get(), &silent_text)))
    return false;

  ComPtr<IXmlNode> silent_node;
  if (FAILED(silent_text.As(&silent_node)))
    return false;

  ComPtr<IXmlNode> child_node;
  if (FAILED(silent_attribute_node->
    AppendChild(silent_node.Get(), &child_node)))
    return false;

  ComPtr<IXmlNode> silent_attribute_pnode;
  return SUCCEEDED(attributes.Get()->
    SetNamedItem(silent_attribute_node.Get(), &silent_attribute_pnode));
}

// static
bool DesktopNotification::SetXmlText(
  IXmlDocument* doc, const std::wstring& text) {
  HString tag;
  ComPtr<IXmlNodeList> node_list;
  if (!GetTextNodeList(&tag, doc, &node_list, 1))
    return false;

  ComPtr<IXmlNode> node;
  if (FAILED(node_list->Item(0, &node)))
    return false;

  return AppendTextToXml(doc, node.Get(), text);
}

bool DesktopNotification::SetXmlText(
  IXmlDocument* doc, const std::wstring& title, const std::wstring& body) {
  HString tag;
  ComPtr<IXmlNodeList> node_list;
  if (!GetTextNodeList(&tag, doc, &node_list, 2))
    return false;

  ComPtr<IXmlNode> node;
  if (FAILED(node_list->Item(0, &node)))
    return false;

  if (!AppendTextToXml(doc, node.Get(), title))
    return false;

  if (FAILED(node_list->Item(1, &node)))
    return false;

  return AppendTextToXml(doc, node.Get(), body);
}

// static
bool DesktopNotification::SetXmlImage(
  IXmlDocument* doc, const std::wstring& icon_path) {
  HStringReference tag(L"image");
  ComPtr<IXmlNodeList> node_list;
  if (FAILED(doc->GetElementsByTagName(tag.Get(), &node_list)))
    return false;

  ComPtr<IXmlNode> image_node;
  if (FAILED(node_list->Item(0, &image_node)))
    return false;

  ComPtr<IXmlNamedNodeMap> attrs;
  if (FAILED(image_node->get_Attributes(&attrs)))
    return false;

  HStringReference src(L"src");
  ComPtr<IXmlNode> src_attr;
  if (FAILED(attrs->GetNamedItem(src.Get(), &src_attr)))
    return false;

  HStringReference img_path(icon_path.c_str());
  ComPtr<IXmlText> src_text;
  if (FAILED(doc->CreateTextNode(img_path.Get(), &src_text)))
    return false;

  ComPtr<IXmlNode> src_node;
  if (FAILED(src_text.As(&src_node)))
    return false;

  ComPtr<IXmlNode> child_node;
  return SUCCEEDED(src_attr->AppendChild(src_node.Get(), &child_node));
}

// static
bool DesktopNotification::GetTextNodeList(HString *tag,
                                          IXmlDocument* doc,
                                          IXmlNodeList** node_list,
                                          uint32_t req_length) {
  tag->Set(L"text");

  if (FAILED(doc->GetElementsByTagName(tag->Get(), node_list)))
    return false;

  uint32_t node_length;
  if (FAILED((*node_list)->get_Length(&node_length)))
    return false;

  return node_length >= req_length;
}

// static
bool DesktopNotification::AppendTextToXml(IXmlDocument* doc,
                                          IXmlNode* node,
                                          const std::wstring& text) {
  HStringReference str(text.c_str());

  ComPtr<IXmlText> xml_text;
  if (FAILED(doc->CreateTextNode(str.Get(), &xml_text)))
    return false;

  ComPtr<IXmlNode> text_node;
  if (FAILED(xml_text.As(&text_node)))
    return false;

  ComPtr<IXmlNode> append_node;
  return SUCCEEDED(node->AppendChild(text_node.Get(), &append_node));
}

/*
 * Toast Event Handler
 */
ToastEventHandler::ToastEventHandler(DesktopNotification* notification)
  : notification_(notification) {
}

ToastEventHandler::~ToastEventHandler() {
}

IFACEMETHODIMP ToastEventHandler::Invoke(
  IToastNotification* sender, IInspectable* args) {
  notification_->NotifyClick();
  return S_OK;
}

IFACEMETHODIMP ToastEventHandler::Invoke(
  IToastNotification* sender,
  ABI::Windows::UI::Notifications::IToastDismissedEventArgs* e) {
  notification_->NotifyDismiss();
  return S_OK;
}

IFACEMETHODIMP ToastEventHandler::Invoke(
  IToastNotification* sender,
  ABI::Windows::UI::Notifications::IToastFailedEventArgs* e) {
  notification_->NotifyFail();
  return S_OK;
}

}  // namespace atom
