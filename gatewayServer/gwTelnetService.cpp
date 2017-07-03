#include "gatewayServer/gwTelnetService.h"
#include "gatewayServer/gwServer.h"

gwTelnetService::gwTelnetService()
{
}

gwTelnetService::~gwTelnetService()
{
}

const DWORD gwTelnetService::getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const
{
    return Tidy::nStringTool::copyStr(headerBuffer, "Telnet ", bufferSize);
}

Tidy::nLogger* gwTelnetService::getLogger() const
{
    return server->getLogger();
}

bool gwTelnetService::initService()
{
    return true;
}

void gwTelnetService::finalService()
{
}

