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

#pragma once

#include <glog/logging.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <atomic>

void initLogger(const std::string& name);

class CustomLogSink : public google::LogSink {
public:
    CustomLogSink(const std::string& udpAddress, unsigned short udpPort);
    ~CustomLogSink() override;

    // Start the thread after the object is fully constructed
    void start();

    void send(google::LogSeverity severity, const char* full_filename, const char* base_filename, int line,
                         const google::LogMessageTime& log_message_time, const char* message, size_t message_len) override;

private:
    std::atomic<bool> running;
    std::thread sizeMonitorThread;
    void checkAndCompressLog() const;
};