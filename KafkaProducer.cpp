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

#include "include/KafkaProducerWrapper.h"
#include "include/JSONDatabaseVocabulary.h"
#include "include/Logger.h"

void stopRunning(int sig) {
    if (sig != SIGINT) return;

    if (running) {
        running = false;
    } else {
        // Restore the signal handler, to avoid being stuck with this handler
        signal(SIGINT, SIG_IGN); // NOLINT
    }
}

int main(int argc, char* argv[])
{
    //initLogger("KafkaProducer");
    initLogger("KafkaProducer");

    initialize_JSON_Vocabulary_Classes();

    // Kafka topic and properties
    const std::string topic = "test";
    Properties props;
    property_settings(props);  // Load properties (e.g., from a config file or environment variables)

    // Create the KafkaProducerWrapper instance
    KafkaProducerWrapper producerWrapper(topic, props);

    while (running) {
        // Send a Protocol Buffer message (with year, month, day)
        if (!producerWrapper.sendMessage()) {
            std::cerr << "Failed to send message" << std::endl;
            return -1;
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    google::ShutdownGoogleLogging();
    return 0;
}