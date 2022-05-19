#pragma once
#include <iostream>
#define LOG_INFO (std::cout << "[INFO] <" << LOG_MODULE_NAME << ">: ")
#define LOG_DEBUG (std::cout << "[DEBUG] <" << LOG_MODULE_NAME << ">: ")
#define LOG_WARNING (std::cout << "[WARNING] <" << LOG_MODULE_NAME << ">: ")
#define LOG_ERROR (std::cout << "[ERROR] <" << LOG_MODULE_NAME << ">: ")
#define LOG_SEVERE (std::cout << "[SEVERE] <" << LOG_MODULE_NAME << ">: ")

