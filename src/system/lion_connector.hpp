//
// Created by Alexey Pashinov on 05/09/20
//

#pragma once

// system includes
#include <mutex>

// 3rdparty includes
#include <zmq.hpp>

namespace lion
{
    class lion_connector
    {
    public:
        lion_connector();
        ~lion_connector() = default;

        void connect(const std::string& addr);
        void send(const std::string& identity, const std::string& msg);
        void recv(std::string& identity, std::string& msg);
        void close();
    private:
        std::mutex mutex_;
        zmq::context_t zctx_;
        zmq::socket_t  zsock_;
    };
}
