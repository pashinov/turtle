//
// Created by apashinov on 04/08/20
//

#pragma once

// system includes
#include <string>

// 3rdparty includes
#include <spdlog/spdlog.h>

namespace utils
{
    struct config_st
    {
        struct Application
        {
            std::string name;
        };

        struct System
        {
            struct Logging
            {
                spdlog::level::level_enum level;
                std::string path;
            };

            Logging logging;
        };

        struct Connector
        {
            struct Lion
            {
                struct ZmqReq
                {
                    std::string addr;
                };

                ZmqReq zmq_req;
            };

            struct Phoenix
            {
                struct ZmqPub
                {
                    std::string addr;
                    std::string topic;
                };

                struct ZmqSub
                {
                    std::string addr;
                    std::string topic;
                    int timeout;
                };

                ZmqPub zmq_pub;
                ZmqSub zmq_sub;
            };

            Lion lion;
            Phoenix phoenix;
        };

        Application application;
        System system;
        Connector connector;
    };
}
