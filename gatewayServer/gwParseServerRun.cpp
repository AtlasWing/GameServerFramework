#include "protocol/serverRunCommand.h"
#include "game/nGameServer.h"

#include "gatewayServer/gwServer.h"

bool gwServer::parseServerRunCommand(Tidy::nServerConn* serverConn, const protocol::base::ServerRunCommandBase *nullCommand, const unsigned int len)
{
    using namespace protocol::ServerRun;
    switch(nullCommand->param)
    {
    }

    return Game::nGameServer::parseServerRunCommand(serverConn, nullCommand, len);
}
