#pragma once
#include "Core\CubeySystem.h"
#include <queue>
#include <string>

#define LOGDEBUG(a) LoggingSystem::Log(a, LOGTYPE::DEBUG)
#define LOGWARNING(a) LoggingSystem::Log(a, LOGTYPE::WARNING)
#define LOGERROR(a) LoggingSystem::Log(a, LOGTYPE::ERR)

enum LOGTYPE
{
    DEBUG,
    WARNING,
    ERR
};

class LoggingSystem : public CubeySystem
{
public:
    LoggingSystem();
    ~LoggingSystem();
    void Update(float dt);
    static void Log(std::string message, LOGTYPE warningLevel);

private:
    static std::queue<std::string> messageQueue;
    static std::queue<LOGTYPE> warningLevelQueue;
    float fpsTimer = 0.0f;
};
