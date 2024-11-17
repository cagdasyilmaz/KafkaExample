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

#include "../include/Logger.h"

void initLogger(const std::string& name, const unsigned int port)
{
    google::InitGoogleLogging(name.c_str());

    // Define log directory path based on the program name
    std::string logDir = "../KafkaLogs/" + name;

    // Set log directory
    FLAGS_log_dir = logDir;

    // Check if directory exists, if not, create it
    if (!std::filesystem::exists(logDir)) {
        std::filesystem::create_directories(logDir);  // Recursively create directories
    }

    // Rotate log file after reaching 1MB
    FLAGS_max_log_size = 1;  // Log file size limit in MB

    // Flush logs immediately after writing
    FLAGS_logbufsecs = 0;    // No buffer, immediate flush

    FLAGS_logtostdout = false;   // Disable default logging
    FLAGS_log_prefix = false;    // Disable default prefix since we're using a custom sink
    FLAGS_log_utc_time = false;  // Disable UTC time in the log file
    FLAGS_timestamp_in_logfile_name = true;  // Enable timestamp in the log file name
    FLAGS_log_prefix = true;                 // Enable log prefix

    // Add the custom log sink
    auto* customSink = new CustomLogSink("127.0.0.1", port);
    customSink->start();  // Start the monitoring thread after construction
    google::AddLogSink(customSink);
}

CustomLogSink::CustomLogSink(const std::string& udpAddress, unsigned short udpPort)
    : running(false), udpLogger(std::make_unique<UDPLogger>(udpAddress, udpPort)) {}

CustomLogSink::~CustomLogSink(){
    running = false;
    if (sizeMonitorThread.joinable()) sizeMonitorThread.join();
}

void CustomLogSink::start() {
    running = true;   // Initialize the running flag here and start the thread
    sizeMonitorThread = std::thread(&CustomLogSink::checkAndCompressLog, this);
    LOG(INFO) << "Logger started!" << std::endl;
}

void CustomLogSink::send(google::LogSeverity severity, const char* full_filename, const char* base_filename,
                         int line, const google::LogMessageTime& log_message_time, const char* message, size_t message_len)
{
    // Extract the timestamp information from LogMessageTime
    const std::tm& tm_time = log_message_time.tm();
    const long microseconds = log_message_time.usec();  // Get the microseconds part

    // Format the log timestamp including microseconds
    char buffer[30];
    strftime(buffer, 30, "%Y/%m/%d %H:%M:%S", &tm_time);  // Format time up to seconds

    // Determine log level
    std::string log_level;
    switch (severity) {
        case google::INFO:
            log_level = "I";
            break;
        case google::WARNING:
            log_level = "W";
            break;
        case google::ERROR:
            log_level = "E";
            break;
        case google::FATAL:
            log_level = "F";
            break;
        default:
            break;
    }

    std::ostringstream logDate, logSeverity;
    logDate << "[" << buffer << "." << std::setw(6) << std::setfill('0') << microseconds << "]";
    logSeverity << log_level;

    log.Clear();
    log.set_log_date(logDate.str());
    log.set_log_severity(logSeverity.str());
    log.set_log_message(std::string(message, message_len));

    // Serialize the Protobuf log message to a string
    std::string serializedLog;
    if (!log.SerializeToString(&serializedLog)) {
        LOG(ERROR) << "Failed to serialize log message" << std::endl;
        return;
    }

    // Send log to UDP if it is enabled
    if (udpLogger) {
        udpLogger->sendLog(serializedLog);
    }

    // Print log message with colorized log level and microsecond precision
    /*std::cout << "[" << buffer << "." << std::setw(6) << std::setfill('0') << microseconds << "] "
              << color_code << "[" << log_level << "]" << RESET_COLOR << " "
              << std::string(message, message_len) << std::endl;*/
}

void CustomLogSink::checkAndCompressLog() const {
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(30));
        for (const auto& entry : std::filesystem::directory_iterator(FLAGS_log_dir)) {
            if (entry.path().extension() == ".txt" && std::filesystem::file_size(entry) >= FLAGS_max_log_size * 1024 * 1024) {
                // Remove .txt extension from filename
                std::filesystem::path renamedPath = entry.path();
                renamedPath.replace_extension("");  // Remove the .txt extension

                try {
                    std::filesystem::rename(entry.path(), renamedPath);  // Rename to remove .txt
                    std::string command = "bzip2 --force --best " + renamedPath.string();    // --best aliases are primarily for GNU gzip compatibility

                    if (int result = std::system(command.c_str()); result != 0) {
                        LOG(ERROR) << "Error: Compression of " << renamedPath << " failed with error code " << result;
                    }
                } catch (const std::filesystem::filesystem_error& e) {
                    LOG(ERROR) << "Error renaming file " << entry.path() << ": " << e.what();
                }
            }
        }
    }
}