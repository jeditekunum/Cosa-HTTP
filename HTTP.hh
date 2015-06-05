/**
 * @file HTTP.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
 *
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
 * This file is part of the Arduino Che Cosa project.
 */

#ifndef COSA_HTTP_HH
#define COSA_HTTP_HH

#include "Cosa/Types.h"
#include "Cosa/Socket.hh"

class HTTP {
public:
  /** Max length of hostname. */
  static const size_t HOSTNAME_MAX = 32;

  /** Max length of HTTP request. */
  static const size_t REQUEST_MAX = 64;

  /**
   * HTTP server request handler. Should be sub-classed and the
   * virtual member function on_request() should be implemented to
   * produce response to HTTP requests.
   */
  class Server {
  public:
    /**
     * Default constructor.
     */
    Server() : m_sock(NULL) {}

    /**
     * Start server with given socket. Initiates socket for incoming
     * connection-oriented requests (TCP/listen). Returns true if
     * successful otherwise false.
     * @param[in] sock server socket.
     * @return bool.
     */
    bool begin(Socket* sock)
    {
      if (sock == NULL) return (false);
      m_sock = sock;
      return (sock->listen() == 0);
    }

    /**
     * Server loop function; wait for a request for the given time
     * period. Parse incoming requests from client and calls
     * on_request(). The socket is disconnected and reinitialized for
     * incoming requests after the processing the request. Returns
     * zero if successful otherwise a negative error code;
     * timeout(-2) if the time period is exceeded
     * @param[in] ms timeout period (milli-seconds, default BLOCK).
     * @return zero or negative error code.
     */
    int run(uint32_t ms = 0L);

    /**
     * Stop server and close socket. Returns true if successful
     * otherwise false.
     * @return bool.
     */
    bool end()
    {
      if (m_sock == NULL) return (false);
      m_sock->close();
      m_sock = NULL;
      return (true);
    }

    /**
     * Get client address, network address and port.
     * @param[out] addr network address.
     */
    void get_client(INET::addr_t& addr) const
      __attribute__((always_inline))
    {
      m_sock->get_src(addr);
    }

    /**
     * @override
     * Application extension; Should implement the response to the
     * given request (http).
     * @param[in] page iostream for response.
     * @param[in] method http request method string.
     * @param[in] path resource path string.
     * @param[in] query possible query string.
     */
    virtual void on_request(IOStream& page, char* method, char* path, char* query) = 0;

  protected:
    /**
     * Socket connection to server; may be used for attributes parse
     * and for response output stream.
     */
    Socket* m_sock;
  };

  /**
   * HTTP client request handler. Should be sub-classed and the
   * virtual member function on_response() should be implemented to
   * read response to HTTP requests.
   */
  class Client {
  public:
    /**
     * Default constructor.
     */
    Client() : m_sock(NULL) {}

    /**
     * Default destructor. Closes and releases given socket.
     */
    ~Client()
    {
      end();
    }

    /**
     * Start web client with given socket. The socket will be used for
     * all web access until closed with the member function end() or the
     * default destructor. Returns true if successful otherwise false.
     * @param[in] sock connection-less socket (TCP).
     * @return bool.
     */
    bool begin(Socket* sock);

    /**
     * Stop web client and close socket. Returns true if successful
     * otherwise false.
     * @return bool.
     */
    bool end();

    /**
     * Get web page for given url. Wait for at most the given time
     * limit. The virtual member function on_response() is called when
     * the connection has been established and page may be read.
     * Returns zero if successful otherwise negative error code;
     * -2 if a timeout occurs, -3 url parse error.
     * @param[in] url uniform resource locator string.
     * @param[in] ms timeout limit in milli-seconds (Default 5 seconds).
     */
    int get(const char* url, uint32_t ms = 5000L);

    /**
     * @override HTTP::Client
     * Called when a server has been connected and a response is
     * ready to be read.
     * @param[in] hostname network name of host.
     * @param[in] path resource name string.
     */
    virtual void on_response(const char* hostname, const char* path) = 0;

  protected:
    /**
     * Socket connection to client; may be used for response parsing.
     */
    Socket* m_sock;
  };
};

#endif
