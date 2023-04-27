#include <iostream>
#include <fstream>
#include <ctime>

enum LogLevel {ERROR, WARNING, INFO};

class AbstractLogger {
public:
    virtual void log(const std::string& message) = 0;
    virtual ~AbstractLogger() {}
};

class FileLogger : public AbstractLogger {
private:
    std::ofstream logFile;
    std::string filePath;
    int maxSize;
public:
    FileLogger(const std::string& filePath, int maxSize) : filePath(filePath), maxSize(maxSize) {
        logFile.open(filePath, std::ios::app);
    }

    virtual void log(const std::string& message) override {
        if (logFile.tellp() >= maxSize) {
            rollOver();
        }

        std::time_t t = std::time(0);
        std::tm* now = std::localtime(&t);
        char timeBuf[100];
        std::strftime(timeBuf, sizeof(timeBuf), "[%F %T] ", now);
        logFile << timeBuf << message << std::endl;
        logFile.flush();
    }

    void rollOver() {
        logFile.close();
        std::string backupFile = filePath + ".bak";
        std::rename(filePath.c_str(), backupFile.c_str());
        logFile.open(filePath, std::ios::app);
    }
};

class ConsoleLogger : public AbstractLogger {
public:
    virtual void log(const std::string& message) override {
        std::time_t t = std::time(0);
        std::tm* now = std::localtime(&t);
        char timeBuf[100];
        std::strftime(timeBuf, sizeof(timeBuf), "[%F %T] ", now);
        std::cout << timeBuf << message << std::endl;
    }
};

class NullLogger : public AbstractLogger {
public:
    virtual void log(const std::string& message) override {}
};

class Logger {
private:
    AbstractLogger* logger;
public:
    Logger(LogLevel level, const std::string& filePath = "", int maxSize = 0) {
        if (level == WARNING || level == ERROR) {
            logger = new FileLogger(filePath, maxSize);
        } else if (level == INFO) {
            logger = new ConsoleLogger();
        } else {
            logger = new NullLogger();
        }
    }

    ~Logger() {
        delete logger;
    }

    void log(const std::string& message) {
        logger->log(message);
    }
};

#define LOG_ERROR(x) Logger(ERROR, "error.log", 1024*1024).log(x)
#define LOG_WARNING(x) Logger(WARNING, "warning.log", 1024*1024).log(x)
#define LOG_INFO(x) Logger(INFO).log(x)
