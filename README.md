# Apache Kafka Client Applications Implemented in C++

In this project, Kafka Client, Kafka Producer and KafkaAdmin Client are demonstratively implemented. To be able to compile the code of those applications, it is assumed that Kafka and Zookeper has been already installed in some server or the PC at which programs will run.

## Producer Program 

In the main folder, `KafkaProducer.cpp` reads `vocabulary` folder which includes demonstrative vocabulary types and loads them to the PC's RAM. Those types are loaded as JSON structure. Program selects a random words from word types structure and makes a transformation from JSON to Protocol Buffers. The serialized data constructed by ProtoBuf library then send to Apache Kafka server. This process are repeated in a specified time period.

    [2024/10/13 23:50:16.178225] [I] ../vocabulary/verbs/verbs.json file loaded to RAM with 5 element(s).
    [2024/10/13 23:50:16.179112] [I] ../vocabulary/advanced_verbs/advanced_verbs.json file loaded to RAM with 5 element(s).
    [2024/10/13 23:50:16.179386] [I] ../vocabulary/phrasal_verbs/phrasal_verbs.json file loaded to RAM with 5 element(s).
    [2024/10/13 23:50:16.179639] [I] ../vocabulary/nouns/nouns.json file loaded to RAM with 5 element(s).
    [2024/10/13 23:50:16.180001] [I] ../vocabulary/advanced_nouns/advanced_nouns.json file loaded to RAM with 5 element(s).
    [2024/10/13 23:50:16.180266] [I] ../vocabulary/adjectives/adjectives.json file loaded to RAM with 5 element(s).
    [2024/10/13 23:50:16.180509] [I] ../vocabulary/advanced_adjectives/advanced_adjectives.json file loaded to RAM with 5 element(s).
    [2024/10/13 23:50:16.180748] [I] ../vocabulary/adverbs/adverbs.json file loaded to RAM with 5 element(s).
    [2024/10/13 23:50:16.181146] [I] ../vocabulary/advanced_adverbs/advanced_adverbs.json file loaded to RAM with 5 element(s).
    [2024/10/13 23:50:16.181558] [I] ../vocabulary/prepositions/prepositions.json file loaded to RAM with 5 element(s).
    [2024/10/13 23:50:16.181814] [I] ../vocabulary/advanced_prepositions/advanced_prepositions.json file loaded to RAM with 5 element(s).
    [2024/10/13 23:50:16.181911] [I] Program loads 11 ".json," files to the RAM.
    [2024/10/13 23:50:22.495944] [I] Message delivered: test-0@0, CreateTime[2024-10-13 23:50:16.220], Persisted
    [2024/10/13 23:50:24.557363] [I] Message delivered: test-0@1, CreateTime[2024-10-13 23:50:24.500], Persisted
    

## Consumer Program

In the main folder, `KafkaConsumer.cpp` connects to the Apache Kafka Server with a certain topic name, i.e. `test`, and a certain parition number `0`. After a message produced by `KafkaProducer` program,  `KafkaConsumer` fetches this message and deserialize by employing  ProtoBuf library. The deserialized data is also transformed to the previous JSON format before implemented by `KafkaProducer`.

    [2024/10/13 23:50:22.708845] [I] topic: test, partition: 0
    [2024/10/13 23:50:23.110610] [I] Got a new message...
    [2024/10/13 23:50:23.110756] [I]     Topic         : test
    [2024/10/13 23:50:23.110817] [I]     Partition     : 0
    [2024/10/13 23:50:23.110867] [I]     Offset        : 0
    [2024/10/13 23:50:23.110952] [I]     Timestamp     : CreateTime[2024-10-13 23:50:16.220]
    [2024/10/13 23:50:23.111035] [I]     Headers       : Vocabulary:ProtoBuf
    [2024/10/13 23:50:23.111117] [I]     Key           : [[null]]
    [2024/10/13 23:50:23.111199] [I] Deserialized Protocol Buffer message:
    [2024/10/13 23:50:23.111248] [I] word                 :addictive
    [2024/10/13 23:50:23.111292] [I]   id                 :1
    [2024/10/13 23:50:23.111336] [I]   synonym            :causing addiction
    [2024/10/13 23:50:23.111382] [I]   turkish_meaning    :bağımlılık yapıcı
    [2024/10/13 23:50:23.111428] [I]   sample_sentence(s) :Tobacco is highly <span style='color:red;'>addictive</span>.
    [2024/10/13 23:50:23.111478] [I] macmillan_url        :https://dictionary.cambridge.org/dictionary/english-turkish/addictive
    [2024/10/13 23:50:23.111562] [I] youglish_url         :https://youglish.com/pronounce/addictive/english?
    [2024/10/13 23:50:23.111652] [I] reverso_url          :https://context.reverso.net/translation/english-turkish/addictive
    [2024/10/13 23:50:23.140511] [I] Message to JSON string transformation time: 0.0287804
    [2024/10/13 23:50:23.140669] [I] json: {"word":"addictive","synonyms_related_words":[{"id":1,"synonym":"causing addiction","turkish_meaning":"bağımlılık yapıcı","sample_sentences":"Tobacco is highly \u003cspan style='color:red;'\u003eaddictive\u003c/span\u003e."}],"macmillan_url":"https://dictionary.cambridge.org/dictionary/english-turkish/addictive","youglish_url":"https://youglish.com/pronounce/addictive/english?","reverso_url":"https://context.reverso.net/translation/english-turkish/addictive"}
    [2024/10/13 23:50:24.643221] [I] Got a new message...
    [2024/10/13 23:50:24.643365] [I]     Topic         : test
    [2024/10/13 23:50:24.643419] [I]     Partition     : 0
    [2024/10/13 23:50:24.643466] [I]     Offset        : 1
    [2024/10/13 23:50:24.643512] [I]     Timestamp     : CreateTime[2024-10-13 23:50:24.500]
    [2024/10/13 23:50:24.643579] [I]     Headers       : Vocabulary:ProtoBuf
    [2024/10/13 23:50:24.643656] [I]     Key           : [[null]]
    [2024/10/13 23:50:24.643733] [I] Deserialized Protocol Buffer message:
    [2024/10/13 23:50:24.643784] [I] word                 :accurate
    [2024/10/13 23:50:24.643828] [I]   id                 :1
    [2024/10/13 23:50:24.643870] [I]   synonym            :correct, exact, precise
    [2024/10/13 23:50:24.643980] [I]   turkish_meaning    :doğru, kesin
    [2024/10/13 23:50:24.644024] [I]   sample_sentence(s) :We need <span style='color:red;'>accurate</span> figures to make better estimate of the cost of the project.
    [2024/10/13 23:50:24.644069] [I] macmillan_url        :https://www.macmillandictionary.com/dictionary/british/accurate
    [2024/10/13 23:50:24.644114] [I] youglish_url         :https://youglish.com/pronounce/accurate/english?
    [2024/10/13 23:50:24.644155] [I] reverso_url          :https://context.reverso.net/translation/english-turkish/accurate
    [2024/10/13 23:50:24.644573] [I] Message to JSON string transformation time: 0.000374156
    [2024/10/13 23:50:24.644646] [I] json: {"word":"accurate","synonyms_related_words":[{"id":1,"synonym":"correct, exact, precise","turkish_meaning":"doğru, kesin","sample_sentences":"We need \u003cspan style='color:red;'\u003eaccurate\u003c/span\u003e figures to make better estimate of the cost of the project."}],"macmillan_url":"https://www.macmillandictionary.com/dictionary/british/accurate","youglish_url":"https://youglish.com/pronounce/accurate/english?","reverso_url":"https://context.reverso.net/translation/english-turkish/accurate"}
    

## AdminClient Program

This program basically fulfill the following features which can be handled by AdminClient in Apache Kafka:

1.  List all the topics in a Kafka broker

2. Create the topic with name `what`

3. Delete the topic with name `what`


## Installation / Requirements

- [**modern-cpp-kafka**](https://github.com/morganstanley/modern-cpp-kafka "Github"): A C++ API for Kafka clients (i.e. KafkaProducer, KafkaConsumer, AdminClient) 

**Dependencies** 

1.  The compiler should support C++17
     Or, C++14, but with pre-requirements

2. Need boost headers (for `boost::optional`)

3. For GCC compiler, it needs optimization options (e.g. `-O2`)

4. [librdkafka](https://github.com/confluentinc/librdkafka "librdkafka")   headers and library (only the C part)

5. Also see the requirements from librdkafka
[rapidjson](https://github.com/Tencent/rapidjson "rapidjson") headers: only required by addons/KafkaMetrics.h

- [**nlohmann::json**](https://github.com/nlohmann/json "nlohmann::json"): JSON for Modern C++

- [**protobuf**](https://github.com/protocolbuffers/protobuf "protobuf"): Protocol Buffers - Google's data interchange format (`protoc` compiler should also be installed to compile `.proto` file for C++ programming language) (Please note that: Installed protobuf version should be compatible with the code produced by `protoc` compiler)
- [**glog**](https://github.com/google/glog "glog"): C++ implementation of the Google logging module. (If your glog version different than 0.6.0, then you should change the `CMakeList.txt` file with the your glog version)