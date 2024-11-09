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

#include "include/KafkaConsumerWrapper.h"
#include "include/Logger.h"

std::atomic_bool running = {true};

void stopRunning(int sig) {
    if (sig != SIGINT) return;

    if (running) {
        running = false;
    } else {
        // Restore the signal handler, to avoid being stuck with this handler
        signal(SIGINT, SIG_IGN); // NOLINT
    }
}

int main(int argc, char* argv[]) {

    initLogger("KafkaConsumer");

    // Use Ctrl-C to terminate the program
    signal(SIGINT, stopRunning); // NOLINT

    // Kafka topic and properties
    const Topic topic = "test"; // NOLINT
    Properties props;
    property_settings(props);

    // Create a KafkaConsumerWrapper instance
    KafkaConsumerWrapper consumerWrapper(topic, props);

    // Start the consumer in a polling loop
    consumerWrapper.start();

    google::ShutdownGoogleLogging();
    return 0;
}
