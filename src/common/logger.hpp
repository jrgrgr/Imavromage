#pragma once

#include <format>
#include <iostream>
#include <unordered_map>
#include <string>
#include <print>

enum class LOG_LEVEL{
    NONE,
    DEBG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

const std::unordered_map<LOG_LEVEL, std::string> log_level_to_str {
    { LOG_LEVEL::DEBG, "[DEBUG] "},
    { LOG_LEVEL::INFO, "[INFO] " },
    { LOG_LEVEL::WARNING, "[WARNING] " },
    { LOG_LEVEL::ERROR, "[ERROR] " },
    { LOG_LEVEL::CRITICAL, "[CRITICAL] " }
};

class Logger {
    public:
        static LOG_LEVEL level;

        template <typename... Args>
        inline static void log(LOG_LEVEL lvl, const std::string &msg, Args&&... args) {
            if (lvl > level)
            	return;

            auto formated_msg = std::vformat(msg, std::make_format_args(args...));
            std::println( (lvl >= LOG_LEVEL::ERROR) ? std::cerr : std::cout,
            	"{}{}", log_level_to_str.at(lvl), std::vformat(msg, std::make_format_args(args...)));
        }

};
