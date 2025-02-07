#include "setup.h"
#include <limits.h>
#include <unistd.h>

// Define global variables

const std::string Number = "-bin8";
const std::string currentDir = GetCurrentWorkingDir();

std::string routerFilePath;
std::string linkFilePath;
std::string aggGroupFilePath;

const std::string conName = "con";
const std::string proName = "pro";
const std::string fowName = "forwarder";
const std::string aggName = "agg";

uint32_t consumerNum = 0;
uint32_t producerNum = 0;
uint32_t forwarderNum = 0;
uint32_t aggregatorNum = 0;
std::string cc = "bbr";
uint16_t basetime = 1000;
uint16_t starttime = 1;
uint16_t stoptime = 500;