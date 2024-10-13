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

// Define ANSI color codes
const char* RESET_COLOR = "\033[0m";
const char* RED_COLOR = "\033[31m";    // Red for ERROR
const char* BLUE_COLOR = "\033[34m";   // Blue for INFO
const char* YELLOW_COLOR = "\033[33m"; // Yellow for WARNING
const char* GREEN_COLOR = "\033[32m";  // Green for FATAL

void initLogger(const std::string& name)
{
    google::InitGoogleLogging(name.c_str());

    // Define log directory path based on the program name
    std::string logDir = "../KafkaLogs/" + name;

    // Check if directory exists, if not, create it
    if (!std::filesystem::exists(logDir)) {
        std::filesystem::create_directories(logDir);  // Recursively create directories
    }

    // Set log directory
    FLAGS_log_dir = logDir;

    // Rotate log file after reaching 10MB
    FLAGS_max_log_size = 1;  // Log file size limit in MB

    // Flush logs immediately after writing
    FLAGS_logbufsecs = 0;  // No buffer, immediate flush

    // Disable default logging
    FLAGS_logtostdout = false;
    FLAGS_log_prefix = false;  // Disable default prefix since we're using a custom sink

    // Add the custom log sink
    auto* customSink = new CustomLogSink();  // Allocating manually

    //CustomLogSink* customSink = new CustomLogSink();
    google::AddLogSink(customSink);
}

/*void CustomLogSink::send(google::LogSeverity severity, const char *full_filename, const char *base_filename, int line,
                         const struct tm* tm_time, const char* message, size_t message_len) {
    // Get current time with millisecond precision
    auto now = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

    // Format log with timestamp and milliseconds
    char buffer[30];
    strftime(buffer, 30, "%Y%m%d %H:%M:%S", tm_time);  // Format time up to seconds

    // Determine log level and color
    const char* color_code;
    std::string log_level;
    switch (severity) {
        case google::INFO:
            log_level = "I";
            color_code = BLUE_COLOR;
            break;
        case google::WARNING:
            log_level = "W";
            color_code = YELLOW_COLOR;
            break;
        case google::ERROR:
            log_level = "E";
            color_code = RED_COLOR;
            break;
        case google::FATAL:
            log_level = "F";
            color_code = GREEN_COLOR;
            break;
        default:
            color_code = RESET_COLOR;
    }

    // Print log message with colorized log level
    std::cout << "[" << buffer << "." << std::setw(3) << std::setfill('0') << milliseconds << "] "
              << color_code << "[" << log_level << "]" << RESET_COLOR << " "
              << std::string(message, message_len) << std::endl;
}*/

void CustomLogSink::send(google::LogSeverity severity, const char* full_filename, const char* base_filename,
                         int line, const google::LogMessageTime& log_message_time, const char* message, size_t message_len)
{
    // Extract the timestamp information from LogMessageTime
    const std::tm& tm_time = log_message_time.tm();
    int32_t microseconds = log_message_time.usec();  // Get the microseconds part

    // Format the log timestamp including microseconds
    char buffer[30];
    strftime(buffer, 30, "%Y/%m/%d %H:%M:%S", &tm_time);  // Format time up to seconds

    // Determine log level and color
    const char* color_code;
    std::string log_level;
    switch (severity) {
        case google::INFO:
            log_level = "I";
        color_code = BLUE_COLOR;
        break;
        case google::WARNING:
            log_level = "W";
        color_code = YELLOW_COLOR;
        break;
        case google::ERROR:
            log_level = "E";
        color_code = RED_COLOR;
        break;
        case google::FATAL:
            log_level = "F";
        color_code = GREEN_COLOR;
        break;
        default:
            color_code = RESET_COLOR;
    }

    // Print log message with colorized log level and microsecond precision
    std::cout << "[" << buffer << "." << std::setw(6) << std::setfill('0') << microseconds << "] "
              << color_code << "[" << log_level << "]" << RESET_COLOR << " "
              << std::string(message, message_len) << std::endl;
}
