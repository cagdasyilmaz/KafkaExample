/****************************************************************************
* MIT License
 *
 * Copyright (c) 2024 İsmail Çağdaş Yılmaz
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 ****************************************************************************/

#include "../include/UDPServer.h"

// Define ANSI color codes
const char* RESET_COLOR  = "\033[0m";
const char* RED_COLOR    = "\033[31m"; // Red for ERROR
const char* BLUE_COLOR   = "\033[34m"; // Blue for INFO
const char* YELLOW_COLOR = "\033[33m"; // Yellow for WARNING
const char* GREEN_COLOR  = "\033[32m"; // Green for FATAL

UDPServer::UDPServer(const std::string& address, unsigned short port)
    : serverEndpoint(boost::asio::ip::make_address(address), port),
      socket(ioService, serverEndpoint) {
    LOG(INFO) << "UDP Server started at " << address << ":" << port << std::endl;
}

UDPServer::~UDPServer() {
     socket.close();
     LOG(INFO) << "UDP Server stopped." << std::endl;
}

void UDPServer::start() {
     socket.async_receive_from(
         boost::asio::buffer(recvBuffer), remoteEndpoint,
         [this](const boost::system::error_code& error, std::size_t bytesTransferred) {
             handleReceive(error, bytesTransferred);
     });

     ioService.run();
}

void UDPServer::handleReceive(const boost::system::error_code& error, std::size_t bytesTransferred) {
    if (!error) {
        std::string message(recvBuffer.data(), bytesTransferred); // Process the message here without printing it

        // Parse the Protobuf message
        ProtoLog log;
        if (log.ParseFromString(message)) {
            // Determine the color code based on severity
            const char* color_code;
            const std::string& severity = log.log_severity();

            if (severity == "I") {
                color_code = BLUE_COLOR;
            } else if (severity == "W") {
                color_code = YELLOW_COLOR;
            } else if (severity == "E") {
                color_code = RED_COLOR;
            } else if (severity == "F") {
                color_code = GREEN_COLOR;
            } else {
                color_code = RESET_COLOR;
            }

            // Print the log message with appropriate color
            std::cout << log.log_date() << " "
                      << color_code << "[" << severity << "]" << RESET_COLOR << " "
                      << log.log_message() << std::endl;

        } else {
            std::cout << "Failed to parse received log message" << std::endl;
        }

        // Restart async receive
        socket.async_receive_from(
          boost::asio::buffer(recvBuffer), remoteEndpoint,
          [this](const boost::system::error_code& error, std::size_t bytesTransferred) {
              handleReceive(error, bytesTransferred);
        });
    } else {
        LOG(ERROR) << "Error receiving message: " << error.message() << std::endl;
    }
}