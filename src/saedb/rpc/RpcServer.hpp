#pragma once
#include <functional>
#include <string>

namespace sae {
namespace rpc {

using ServiceFunc = std::function<bool(const std::string&, std::string&)>;

template<class T>
struct ServiceBinder {
    ServiceBinder(T& service) : service(service) {
    }

    // I cannot figure out the correct type for f.
    // Let the compiler do the work then.
    template<typename F>
    ServiceFunc operator()(F f) {
        return bind(f, std::ref(service), std::placeholders::_1, std::placeholders::_2);
    }

private:
    T& service;
};

template<class T>
ServiceBinder<T> make_binder(T& service) {
    return ServiceBinder<T>(service);
}

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

} // namespace rpc
} // namespace sae
