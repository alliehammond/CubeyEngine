#include "EnginePCH.h"
#include "Core\LoggingSystem.h"

std::queue<std::string> LoggingSystem::messageQueue;
std::queue<LOGTYPE> LoggingSystem::warningLevelQueue;

LoggingSystem::LoggingSystem()
{
    AllocConsole();
    FILE *dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);
    
    LOGDEBUG("Logging system initialized!");
}

LoggingSystem::~LoggingSystem()
{ }

void LoggingSystem::Update()
{
    //Print all messages in the queue to the console window
    while(!messageQueue.empty())
    {
        std::string curMessage = messageQueue.front();
        messageQueue.pop();
        LOGTYPE warningLevel = warningLevelQueue.front();
        warningLevelQueue.pop();

        if(warningLevel == LOGTYPE::DEBUG)
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0D);
            auto ret = GetLastError();
            std::cout << "[DEBUG]";
            std::cout << curMessage << std::endl;
        }
        if(warningLevel == LOGTYPE::WARNING)
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x06);
            std::cout << "[WARNING]";
            std::cout << curMessage << std::endl;
        }
        if(warningLevel == LOGTYPE::ERR)
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x04);
            std::cout << "[ERROR]";
            std::cout << curMessage << std::endl;
        }
    }
}

void LoggingSystem::Log(std::string message, LOGTYPE warningLevel)
{
    messageQueue.push(message);
    warningLevelQueue.push(warningLevel);
}
