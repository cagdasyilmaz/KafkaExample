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

#include "kafka/KafkaConsumer.h"

#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    using namespace kafka;
    using namespace kafka::clients;
    using namespace kafka::clients::consumer;

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <brokers> <topic>\n";
        exit(argc == 1 ? 0 : 1); // NOLINT
    }

    const std::string brokers = argv[1];
    const Topic       topic   = argv[2];

    try {

        // Create configuration object
        const Properties props ({
                                        {"bootstrap.servers",  {brokers}},
                                        {"enable.auto.commit", {"false"}},
                                        {"group.id",{"1"}},
                                        {"client.id", {"1"}},
                                        {"log_level", {"7"}}
                                });

        // Create a consumer instance
        KafkaConsumer consumer(props);

        // Subscribe to topics
        consumer.subscribe({topic});

        auto lastTimeCommitted = std::chrono::steady_clock::now();

        // Read messages from the topic
        std::cout << "% Reading messages from topic: " << topic << std::endl;
        bool allCommitted = true;
        bool running      = true;
        while (running) {
            auto records = consumer.poll(std::chrono::milliseconds(100));
            for (const auto& record: records) {
                // In this example, quit on empty message
                if (record.value().size() == 0) {
                    running = false;
                    break;
                }

                if (!record.error()) {
                    std::cout << "% Got a new message..." << std::endl;
                    std::cout << "    Topic    : " << record.topic() << std::endl;
                    std::cout << "    Partition: " << record.partition() << std::endl;
                    std::cout << "    Offset   : " << record.offset() << std::endl;
                    std::cout << "    Timestamp: " << record.timestamp().toString() << std::endl;
                    std::cout << "    Headers  : " << toString(record.headers()) << std::endl;
                    std::cout << "    Key   [" << record.key().toString() << "]" << std::endl;
                    std::cout << "    Value [" << record.value().toString() << "]" << std::endl;

                    allCommitted = false;
                } else {
                    std::cerr << record.toString() << std::endl;
                }
            }

            if (!allCommitted) {
                auto now = std::chrono::steady_clock::now();
                if (now - lastTimeCommitted > std::chrono::seconds(1)) {
                    // Commit offsets for messages polled
                    std::cout << "% syncCommit offsets: " << utility::getCurrentTime() << std::endl;
                    consumer.commitAsync(); // or commitAsync()

                    lastTimeCommitted = now;
                    allCommitted      = true;
                }
            }
        }

        // consumer.close(); // No explicit close is needed, RAII will take care of it

    } catch (const KafkaException& e) {
        std::cerr << "% Unexpected exception caught: " << e.what() << std::endl;
    }
}

