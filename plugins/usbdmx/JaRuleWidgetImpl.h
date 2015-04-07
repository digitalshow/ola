/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * JaRuleWidgetImpl.h
 * The implementation of the Ja Rule Widget.
 * Copyright (C) 2015 Simon Newton
 */

#ifndef PLUGINS_USBDMX_JARULEWIDGETIMPL_H_
#define PLUGINS_USBDMX_JARULEWIDGETIMPL_H_

#include <ola/rdm/DiscoveryAgent.h>
#include <ola/rdm/QueueingRDMController.h>
#include <ola/rdm/RDMCommand.h>
#include <ola/rdm/RDMControllerInterface.h>
#include <ola/rdm/UID.h>
#include <ola/util/SequenceNumber.h>

#include "plugins/usbdmx/JaRuleEndpoint.h"
#include "plugins/usbdmx/JaRuleWidget.h"
#include "plugins/usbdmx/LibUsbAdaptor.h"

namespace ola {
namespace plugin {
namespace usbdmx {

/**
 * @brief The internal implementation of a JaRuleWidget.
 */
class JaRuleWidgetImpl : public ola::rdm::DiscoveryTargetInterface,
                         public ola::rdm::DiscoverableRDMControllerInterface,
                         public JaRuleEndpoint::MessageHandlerInterface {
 public:
  /**
   * @brief Create a new JaRuleWidgetImpl.
   * @param ss The SelectServer to run the RDM callbacks on.
   * @param adaptor The LibUsbAdaptor to use.
   * @param device the libusb_device for the Ja Rule widget.
   * @param controller_uid The UID of the controller. This is used for DUB &
   *   Mute / Unmute messages.
   */
  JaRuleWidgetImpl(ola::io::SelectServerInterface *ss,
                   AsyncronousLibUsbAdaptor *adaptor,
                   libusb_device *device,
                   const ola::rdm::UID &controller_uid);

  ~JaRuleWidgetImpl();

  /**
   * @brief Initialize the widget.
   * @returns true if the USB device was claimed correctly.
   */
  bool Init();

  // From DiscoverableRDMControllerInterface
  void RunFullDiscovery(ola::rdm::RDMDiscoveryCallback *callback);
  void RunIncrementalDiscovery(ola::rdm::RDMDiscoveryCallback *callback);
  void SendRDMRequest(const ola::rdm::RDMRequest *request,
                      ola::rdm::RDMCallback *on_complete);

  // From DiscoveryTargetInterface
  void MuteDevice(const ola::rdm::UID &target,
                  MuteDeviceCallback *mute_complete);
  void UnMuteAll(UnMuteDeviceCallback *unmute_complete);
  void Branch(const ola::rdm::UID &lower,
              const ola::rdm::UID &upper,
              BranchCallback *branch_complete);

  // From MessageHandlerInterface
  void NewMessage(const Message &message);

  /**
   * @brief Send DMX data from this widget
   * @param buffer The DmxBuffer containing the data to send.
   * @returns true if the data was sent, false otherwise.
   */
  bool SendDMX(const DmxBuffer &buffer);

  /**
   * @brief Send a reset message to the hardware widget.
   */
  void ResetDevice();

 private:
  typedef enum {
    RC_OK,
    RC_UNKNOWN,
    RC_BUFFER_FULL,
    RC_BAD_PARAM,
    RC_TX_ERROR,
    RC_RX_TIMEOUT
  } JaRuleReturnCode;

  JaRuleEndpoint m_endpoint;
  ola::rdm::DiscoveryAgent m_discovery_agent;
  const ola::rdm::UID m_our_uid;
  ola::SequenceNumber<uint8_t> m_transaction_number;
  ola::rdm::RDMCallback *m_rdm_callback;
  const ola::rdm::RDMRequest *m_rdm_request;
  MuteDeviceCallback *m_mute_callback;
  UnMuteDeviceCallback *m_unmute_callback;
  BranchCallback *m_branch_callback;
  ola::rdm::UIDSet m_uids;

  void PrintAck(const Message &message);
  void HandleDUBResponse(const Message &message);
  void HandleRDM(const Message &message);
  void HandleRDMResponse(const Message &message);
  ola::rdm::RDMResponse* UnpackRDMResponse(
      const ola::rdm::RDMRequest *request,
      const uint8_t *data,
      unsigned int length,
      ola::rdm::rdm_response_code *response_code);
  void DiscoveryComplete(ola::rdm::RDMDiscoveryCallback *callback,
                         OLA_UNUSED bool ok,
                         const ola::rdm::UIDSet &uids);

  DISALLOW_COPY_AND_ASSIGN(JaRuleWidgetImpl);
};
}  // namespace usbdmx
}  // namespace plugin
}  // namespace ola
#endif  // PLUGINS_USBDMX_JARULEWIDGETIMPL_H_
