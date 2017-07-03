#include "protocol/serverRunCommand.h"

#include "dataServer/dtServer.h"

bool dtServer::parseServerRunCommand(Tidy::nServerConn* serverConn, const protocol::base::ServerRunCommandBase *nullCommand, const unsigned int len)
{
    switch(nullCommand->param)
    {
        default:
            {
            }
    }

    return Game::nGameServer::parseServerRunCommand(serverConn, nullCommand, len);
}
