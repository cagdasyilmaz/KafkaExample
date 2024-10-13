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

#include <kafka/AdminClient.h>
#include <kafka/AdminClientConfig.h>

#include <iostream>

int main(int argc, char* argv[])
{
    const kafka::clients::admin::AdminClientConfig props
            {{
                     { kafka::clients::Config::BOOTSTRAP_SERVERS, { "127.0.0.2:9092" } },
                     { kafka::clients::Config::CLIENT_ID, { "1"} },
                     { kafka::clients::Config::LOG_LEVEL, {"7"}},
             }};

    kafka::clients::admin::AdminClient adminClient(props);
    std::cout << "[" << kafka::utility::getCurrentTime() << "] " << adminClient.name() << " started" << std::endl;

    // Fetch metadata, -- timeout
    std::cout << "[" << kafka::utility::getCurrentTime() << "] will ListTopics" << std::endl;
    {
        auto fetchResult = adminClient.fetchBrokerMetadata("what", std::chrono::seconds(1));
        std::cout << "[" << kafka::utility::getCurrentTime() << "] FetchMetadata: result[" << (fetchResult ? fetchResult->toString() : "NA") << "]" << std::endl;
    }

    // Create Topics, -- timeout
    std::cout << "[" << kafka::utility::getCurrentTime() << "] will CreateTopics" << std::endl;
    {
        auto createResult = adminClient.createTopics({"what"}, 2, 1, kafka::Properties(), std::chrono::seconds(3));
        std::cout << "[" << kafka::utility::getCurrentTime() << "] createTopics: result[" << createResult.error.message() << "]" << std::endl;
    }

   kafka::clients::admin::ListTopicsResult listTopicsResult = adminClient.listTopics();

    {
        int i = 1;
        for (const auto& topic: listTopicsResult.topics) {
            std::cout << "topic[" <<  i << "]: " << topic << std::endl;
            ++i;
        }
    }

    {
        // Delete Topics, -- timeout
        std::cout << "[" << kafka::utility::getCurrentTime() << "] will DeleteTopics" << std::endl;
        {
            auto deleteResult = adminClient.deleteTopics({"what"}, std::chrono::seconds(5));
            std::cout << "[" << kafka::utility::getCurrentTime() << "] DeleteTopics: result[" << deleteResult.error.message() << "]" << std::endl;
        }
    }

    {
        int i = 1;
        for (const auto& topic: listTopicsResult.topics) {
            std::cout << "topic[" <<  i << "]: " << topic << std::endl;
            ++i;
        }
    }

    return 0;
}