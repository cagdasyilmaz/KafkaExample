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

#include "../include/KafkaProducerWrapper.h"

void getVocabulary(Vocabulary &vocabulary) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937_64 mt64{seed};

    std::uniform_int_distribution type_selection(0, static_cast<int>(jsonVocabularyContainer.size() - 1));
    int type_number = type_selection(mt64);
    auto type = mapStringToVocabularyType[VocabularyType(type_number)];

    std::uniform_int_distribution word_selection(0, static_cast<int>(jsonVocabularyContainer[type]().size()-1));
    int word_number = word_selection(mt64);

    auto j_string = nlohmann::to_string(jsonVocabularyContainer[type]().read(word_number));

    // Parse the JSON string into the Protocol Buffers object
    google::protobuf::util::JsonParseOptions options;
    auto status = google::protobuf::util::JsonStringToMessage(j_string, &vocabulary, options);

    if (!status.ok()) {
        LOG(ERROR) << "Error parsing JSON: " << status.ToString();
        //return 1;
    }
}

KafkaProducerWrapper::KafkaProducerWrapper(const std::string& _topic, const kafka::Properties& _props)
        : topic{_topic}, properties{_props} {}

bool KafkaProducerWrapper::sendMessage() const {

    KafkaProducer producer(properties);

    Vocabulary vocabulary;

    getVocabulary(vocabulary);

    // Allocate a byte array to hold the serialized message
    auto message_size = vocabulary.ByteSizeLong();
    std::vector<uint8_t> serialized_message(message_size);

    // Serialize the message to the byte array
    if (!vocabulary.SerializeToArray(serialized_message.data(), message_size)) {
        LOG(ERROR) << "Failed to serialize the message!";
        return false;
    }

    // Create a ProducerRecord
    ProducerRecord record(topic, NullKey, Value(serialized_message.data(), serialized_message.size()));

    // Add headers (optional)
    const std::string headerValue = "ProtoBuf";
    record.headers().emplace_back("Vocabulary", kafka::Header::Value(headerValue.c_str(), headerValue.size()));

    // Prepare delivery callback
    auto deliveryCallback = [](const RecordMetadata& metadata, const Error& error) {
        if (!error) {
            LOG(INFO) << "Message delivered: " << metadata.toString();
        } else {
            LOG(ERROR) << "Message failed to be delivered: " << error.message();
        }
    };

    // Send the message with a callback for delivery confirmation
    //const auto begin = std::chrono::steady_clock::now();
    producer.send(record, deliveryCallback);
    //const std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - begin;
    //std::cout << "Send time is: " << elapsed.count() << " seconds" << std::endl;

    producer.close();

    return true;
}



