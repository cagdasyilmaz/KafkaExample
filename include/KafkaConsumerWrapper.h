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

#include "Vocabulary.pb.h"
#include "KafkaConsumerConfig.h"

#include <google/protobuf/message.h>
#include <google/protobuf/util/json_util.h>

#include <glog/logging.h>

#include <iostream>
#include <string>
#include <atomic>
#include <csignal>

// Global flag for signal handling
extern std::atomic_bool running;

using namespace kafka;
using namespace kafka::clients::consumer;
using namespace google::protobuf::util;

class KafkaConsumerWrapper {
public:
      KafkaConsumerWrapper(const std::string& _topic, const Properties& _props);

      void start();

      void stop();

private:
    void runPollingLoop();

    static void processRecord(const ConsumerRecord& record, Vocabulary& vocabulary);

    std::string topic;
    Properties props;
    KafkaConsumer consumer;
};
