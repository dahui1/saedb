#pragma once
#include <functional>
#include <string>

using ServiceFunc = std::function<bool(const std::string&, std::string&)>;

struct RpcServer
{
    static RpcServer* CreateServer(int port = 32001, int threads = 20);

    RpcServer() = default;
    RpcServer(const RpcServer&) = delete;
    RpcServer& operator=(const RpcServer&) = delete;
    virtual ~RpcServer() = default;

    virtual void addMethod(const std::string& name, ServiceFunc func) = 0;
    virtual void run() = 0;
};
