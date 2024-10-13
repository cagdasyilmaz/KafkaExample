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

#include "../include/KafkaConsumerWrapper.h"

KafkaConsumerWrapper::KafkaConsumerWrapper(const std::string& _topic, const Properties& _props)
        : topic(_topic), consumer(_props)
{
}

void KafkaConsumerWrapper::start()
{
    // Subscribe to the topic
    consumer.subscribe({topic});

    // Poll to assign partitions
    consumer.poll(std::chrono::milliseconds(100));

    // Get assigned partitions
    std::set<TopicPartition> partitions = consumer.assignment();

    for (const TopicPartition& partition : partitions) {
        LOG(INFO) << "topic: " << partition.first << ", partition: " << partition.second;
    }

    // Seek to the beginning of all partitions
    consumer.seekToBeginning(partitions);

    // Start the polling loop
    runPollingLoop();
}

void KafkaConsumerWrapper::stop()
{
    consumer.close();
}

void KafkaConsumerWrapper::runPollingLoop()
{
    // Create an empty Protocol Buffer message
    Vocabulary vocabulary;

    while(running) {
        // Poll messages from Kafka brokers
        auto records = consumer.poll(std::chrono::milliseconds(100));

        for (const auto& record : records) {
            if (!record.error()) {
                processRecord(record, vocabulary);
            } else {
                std::cerr << record.toString() << std::endl;
            }
        }
    }

    // When stopped, close the consumer gracefully
    stop();
}

void KafkaConsumerWrapper::processRecord(const ConsumerRecord& record, Vocabulary& vocabulary)
{
    LOG(INFO) << "Got a new message...";
    LOG(INFO) << "    Topic         : " << record.topic();
    LOG(INFO) << "    Partition     : " << record.partition();
    LOG(INFO) << "    Offset        : " << record.offset();
    LOG(INFO) << "    Timestamp     : " << record.timestamp().toString();
    LOG(INFO) << "    Headers       : " << toString(record.headers());
    LOG(INFO) << "    Key           : [" << record.key().toString() << "]";

    const ConstBuffer& valueBuffer = record.value();
    if (valueBuffer.data() != nullptr && valueBuffer.size() > 0) {
        if (vocabulary.ParseFromArray(valueBuffer.data(), valueBuffer.size())) {
            LOG(INFO) << "Deserialized Protocol Buffer message:";

            LOG(INFO) << "word                 :" << vocabulary.word();
            for(const auto& synonym : vocabulary.synonyms_related_words()) {
                LOG(INFO) << "  id                 :" << synonym.id();
                LOG(INFO) << "  synonym            :" << synonym.synonym();
                LOG(INFO) << "  turkish_meaning    :" << synonym.turkish_meaning();
                LOG(INFO) << "  sample_sentence(s) :" << synonym.sample_sentences();
            }
            LOG(INFO) << "macmillan_url        :" << vocabulary.macmillan_url();
            LOG(INFO) << "youglish_url         :" << vocabulary.youglish_url();
            LOG(INFO) << "reverso_url          :" << vocabulary.reverso_url();

            std::string json_string;
            google::protobuf::util::JsonPrintOptions options;
            options.add_whitespace = false;
            options.always_print_enums_as_ints = true;
            options.preserve_proto_field_names = true;

            auto start = std::chrono::high_resolution_clock::now();
            if(auto result = MessageToJsonString(vocabulary, &json_string, options); result.ok()) {
                std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
                LOG(INFO) << "Message to JSON string transformation time: " << elapsed.count();
                LOG(INFO) << "json: " << json_string;
            }
            else
                LOG(ERROR) << "Error construction JSON: " << result.message();

        } else {
            LOG(ERROR) << "Failed to parse the message!";
        }
    } else {
        LOG(ERROR) << "Received empty or null message!";
    }
}