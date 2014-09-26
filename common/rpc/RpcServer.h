/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * RpcServer.h
 * A generic RPC server.
 * Copyright (C) 2014 Simon Newton
 */

#ifndef COMMON_RPC_RPCSERVER_H_
#define COMMON_RPC_RPCSERVER_H_

#include <stdint.h>
#include <ola/io/SelectServerInterface.h>
#include <ola/network/TCPSocketFactory.h>

#include <memory>

namespace ola {

class ExportMap;

namespace rpc {

/**
 * @brief An RPC server.
 *
 * The RPCServer starts listening on 127.0.0.0:<listen_port> for new client
 * connections. After accepting a new client connection it calls
 * RpcSessionHandlerInterface::NewClient() on the session_handler. For each RPC
 * it then invokes the correct method from the RpcService object.
 *
 * Finally when each client disconnects, it calls
 * RpcSessionHandlerInterface::ClientRemoved() on the session_handler.
 */
class RpcServer {
 public:
  /**
   * @brief Options for the RpcServer.
   */
  struct Options {
   public:
    /**
     * @brief The TCP port to listen on.
     *
     * If tcp_socket is passed, this option is ignored.
     */
    uint16_t listen_port;

    class ExportMap *export_map; /**< The export map to use for stats. */

    /**
     * @brief The listening TCP socket to wait for clients on.
     *
     * The socket should be in listening mode, i.e. have had
     * TCPAcceptingSocket::Listen() called.
     *
     * Ownership of the socket is transferred to the RpcServer.
     * This overrides the listen_port option.
     */
    ola::network::TCPAcceptingSocket *tcp_socket;

    Options()
      : listen_port(0),
        export_map(NULL),
        tcp_socket(NULL) {
    }
  };

  /**
   * @brief Create a new RpcServer.
   * @param ss The SelectServer to use.
   * @param service The RPCService to expose.
   * @param session_handler the RpcSessionHandlerInterface to use for client
   *   connect / disconnect notifications.
   * @param options Options for the RpcServer.
   * @param export_map the ExportMap to use for stats tracking.
   */
  RpcServer(ola::io::SelectServerInterface *ss,
            class RpcService *service,
            class RpcSessionHandlerInterface *session_handler,
            const Options &options);

  ~RpcServer();

  /**
   * @brief Initialize the RpcServer.
   * @returns true if initialization succeeded, false if it failed.
   */
  bool Init();

  /**
   * @brief Return the address this RpcServer is listening on.
   * @returns The SocketAddress this RpcServer is listening on.
   */
  ola::network::GenericSocketAddress ListenAddress();

 private:
  ola::io::SelectServerInterface *m_ss;
  RpcService *m_service;
  class RpcSessionHandlerInterface *m_session_handler;
  const Options m_options;

  ola::network::TCPSocketFactory m_tcp_socket_factory;
  std::auto_ptr<ola::network::TCPAcceptingSocket> m_accepting_socket;

  void NewTCPConnection(ola::network::TCPSocket *socket);
  void ChannelClosed(ola::network::TCPSocket *socket,
                     class RpcSession *session);

  void CleanupChannel(class RpcChannel *channel,
                      ola::network::TCPSocket *socket);

  static const char K_CLIENT_VAR[];
};
}  // namespace rpc
}  // namespace ola

#endif  // COMMON_RPC_RPCSERVER_H_
