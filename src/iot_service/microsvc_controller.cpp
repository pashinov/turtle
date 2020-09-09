// project includes
#include <iot_service/lion_connector.hpp>
#include <iot_service/microsvc_controller.hpp>
#include <iot_service/task_manager.hpp>
#include <utils/config.hpp>
#include <utils/logger.hpp>

// 3rdparty
#include <nlohmann/json.hpp>

// proto
#include <lion.pb.h>
#include <phoenix.pb.h>

namespace iot_service
{
    microsvc_controller::microsvc_controller()
        : alive_(false)
        , task_manager_(std::make_unique<task_manager>(io_service_))
        , phoenix_connector_(std::make_unique<phoenix_connector>())
    {

    }

    void microsvc_controller::start()
    {
        alive_ = true;

        pub_thread_ = std::thread([this]() {
            std::unique_ptr<lion_connector> lion_conn = std::make_unique<lion_connector>();
            lion_conn->connect(CONFIG()->connector.lion.zmq_req.addr);

            phoenix_connector_->publisher_instance()->bind(CONFIG()->connector.phoenix.zmq_pub.addr);

            // TODO: need to implement sync mechanism to wait for sub client connection
            std::this_thread::sleep_for(std::chrono::seconds(10));

            std::function<void()> uptime = [this, &lion_conn]() {
                if (lion_conn->is_connected())
                {
                    auto * sys_info = new lion_protocol::SysInfo();
                    sys_info->set_type(lion_protocol::SysInfoType::UPTIME);
                    auto * resource_type = new lion_protocol::ResourceType();
                    resource_type->set_allocated_sysinfo(sys_info);

                    lion_protocol::Request request;
                    request.set_command(lion_protocol::GET);
                    request.set_allocated_resource(resource_type);

                    lion_conn->send("uptime", request.SerializeAsString());

                    std::string identity;
                    std::string buffer;
                    lion_conn->recv(identity, buffer);

                    lion_protocol::Response response;
                    if (!response.ParseFromString(std::string(static_cast<char*>(buffer.data()), buffer.size())))
                    {
                        LOG_ERROR(LOGGER(CONFIG()->application.name), "Failed to parse received protobuf response from Lion");
                        return;
                    }

                    if (response.status() == lion_protocol::FAIL)
                    {
                        LOG_ERROR(LOGGER(CONFIG()->application.name), "Received response from Lion has a wrong status");
                        return;
                    }

                    phoenix_proto::message msg;
                    msg.set_topic("sysinfo/uptime");
                    msg.set_payload(response.payload().sval());

                    phoenix_connector_->publisher_instance()->publish(CONFIG()->connector.phoenix.zmq_pub.topic, msg.SerializeAsString());
                }
            };

            std::function<void()> boot_time = [this, &lion_conn]() {
                if (lion_conn->is_connected())
                {
                    auto * sys_info = new lion_protocol::SysInfo();
                    sys_info->set_type(lion_protocol::SysInfoType::BOOT_TIME);
                    auto * resource_type = new lion_protocol::ResourceType();
                    resource_type->set_allocated_sysinfo(sys_info);

                    lion_protocol::Request request;
                    request.set_command(lion_protocol::GET);
                    request.set_allocated_resource(resource_type);

                    lion_conn->send("boot_time", request.SerializeAsString());

                    std::string identity;
                    std::string buffer;
                    lion_conn->recv(identity, buffer);

                    lion_protocol::Response response;
                    if (!response.ParseFromString(std::string(static_cast<char*>(buffer.data()), buffer.size())))
                    {
                        LOG_ERROR(LOGGER(CONFIG()->application.name), "Failed to parse received protobuf response from Lion");
                        return;
                    }

                    if (response.status() == lion_protocol::FAIL)
                    {
                        LOG_ERROR(LOGGER(CONFIG()->application.name), "Received response from Lion has a wrong status");
                        return;
                    }

                    phoenix_proto::message msg;
                    msg.set_topic("sysinfo/boot_time");
                    msg.set_payload(response.payload().sval());

                    phoenix_connector_->publisher_instance()->publish(CONFIG()->connector.phoenix.zmq_pub.topic, msg.SerializeAsString());
                }
            };

            std::function<void()> temperature = [this, &lion_conn]() {
                if (lion_conn->is_connected())
                {
                    auto * sys_info = new lion_protocol::SysInfo();
                    sys_info->set_type(lion_protocol::SysInfoType::TEMPERATURE);
                    auto * resource_type = new lion_protocol::ResourceType();
                    resource_type->set_allocated_sysinfo(sys_info);

                    lion_protocol::Request request;
                    request.set_command(lion_protocol::GET);
                    request.set_allocated_resource(resource_type);

                    lion_conn->send("temperature", request.SerializeAsString());

                    std::string identity;
                    std::string buffer;
                    lion_conn->recv(identity, buffer);

                    lion_protocol::Response response;
                    if (!response.ParseFromString(std::string(static_cast<char*>(buffer.data()), buffer.size())))
                    {
                        LOG_ERROR(LOGGER(CONFIG()->application.name), "Failed to parse received protobuf response from Lion");
                        return;
                    }

                    if (response.status() == lion_protocol::FAIL)
                    {
                        LOG_ERROR(LOGGER(CONFIG()->application.name), "Received response from Lion has a wrong status");
                        return;
                    }

                    phoenix_proto::message msg;
                    msg.set_topic("sysinfo/temperature");
                    msg.set_payload(response.payload().sval());

                    phoenix_connector_->publisher_instance()->publish(CONFIG()->connector.phoenix.zmq_pub.topic, msg.SerializeAsString());
                }
            };

            std::function<void()> cpu_info = [this, &lion_conn]() {
                if (lion_conn->is_connected())
                {
                    auto * sys_info = new lion_protocol::SysInfo();
                    sys_info->set_type(lion_protocol::SysInfoType::CPU_INFO);
                    auto * resource_type = new lion_protocol::ResourceType();
                    resource_type->set_allocated_sysinfo(sys_info);

                    lion_protocol::Request request;
                    request.set_command(lion_protocol::GET);
                    request.set_allocated_resource(resource_type);

                    lion_conn->send("cpu_info", request.SerializeAsString());

                    std::string identity;
                    std::string buffer;
                    lion_conn->recv(identity, buffer);

                    lion_protocol::Response response;
                    if (!response.ParseFromString(std::string(static_cast<char*>(buffer.data()), buffer.size())))
                    {
                        LOG_ERROR(LOGGER(CONFIG()->application.name), "Failed to parse received protobuf response from Lion");
                        return;
                    }

                    if (response.status() == lion_protocol::FAIL)
                    {
                        LOG_ERROR(LOGGER(CONFIG()->application.name), "Received response from Lion has a wrong status");
                        return;
                    }

                    phoenix_proto::message msg;
                    msg.set_topic("sysinfo/cpu_info");
                    msg.set_payload(response.payload().sval());

                    phoenix_connector_->publisher_instance()->publish(CONFIG()->connector.phoenix.zmq_pub.topic, msg.SerializeAsString());
                }
            };

            std::function<void()> os_info = [this, &lion_conn]() {
                if (lion_conn->is_connected())
                {
                    auto * sys_info = new lion_protocol::SysInfo();
                    sys_info->set_type(lion_protocol::SysInfoType::OS_INFO);
                    auto * resource_type = new lion_protocol::ResourceType();
                    resource_type->set_allocated_sysinfo(sys_info);

                    lion_protocol::Request request;
                    request.set_command(lion_protocol::GET);
                    request.set_allocated_resource(resource_type);

                    lion_conn->send("os_info", request.SerializeAsString());

                    std::string identity;
                    std::string buffer;
                    lion_conn->recv(identity, buffer);

                    lion_protocol::Response response;
                    if (!response.ParseFromString(std::string(static_cast<char*>(buffer.data()), buffer.size())))
                    {
                        LOG_ERROR(LOGGER(CONFIG()->application.name), "Failed to parse received protobuf response from Lion");
                        return;
                    }

                    if (response.status() == lion_protocol::FAIL)
                    {
                        LOG_ERROR(LOGGER(CONFIG()->application.name), "Received response from Lion has a wrong status");
                        return;
                    }

                    phoenix_proto::message msg;
                    msg.set_topic("sysinfo/os_info");
                    msg.set_payload(response.payload().sval());

                    phoenix_connector_->publisher_instance()->publish(CONFIG()->connector.phoenix.zmq_pub.topic, msg.SerializeAsString());
                }
            };

            std::function<void()> disk_info = [this, &lion_conn]() {
                if (lion_conn->is_connected())
                {
                    auto * sys_info = new lion_protocol::SysInfo();
                    sys_info->set_type(lion_protocol::SysInfoType::DISK_INFO);
                    auto * resource_type = new lion_protocol::ResourceType();
                    resource_type->set_allocated_sysinfo(sys_info);

                    lion_protocol::Request request;
                    request.set_command(lion_protocol::GET);
                    request.set_allocated_resource(resource_type);

                    lion_conn->send("disk_info", request.SerializeAsString());

                    std::string identity;
                    std::string buffer;
                    lion_conn->recv(identity, buffer);

                    lion_protocol::Response response;
                    if (!response.ParseFromString(std::string(static_cast<char*>(buffer.data()), buffer.size())))
                    {
                        LOG_ERROR(LOGGER(CONFIG()->application.name), "Failed to parse received protobuf response from Lion");
                        return;
                    }

                    if (response.status() == lion_protocol::FAIL)
                    {
                        LOG_ERROR(LOGGER(CONFIG()->application.name), "Received response from Lion has a wrong status");
                        return;
                    }

                    phoenix_proto::message msg;
                    msg.set_topic("sysinfo/disk_info");
                    msg.set_payload(response.payload().sval());

                    phoenix_connector_->publisher_instance()->publish(CONFIG()->connector.phoenix.zmq_pub.topic, msg.SerializeAsString());
                }
            };

            task_manager_->add_task(cpu_info);
            task_manager_->add_task(os_info);
            task_manager_->add_task(disk_info);
            task_manager_->add_task(uptime, std::chrono::minutes(10));
            task_manager_->add_task(boot_time, std::chrono::minutes(10));
            task_manager_->add_task(temperature, std::chrono::minutes(10));

            task_manager_->start();

            io_service_.run();
        });

        sub_thread_ = std::thread([this]() {
            phoenix_connector_->subscriber_instance()->connect(CONFIG()->connector.phoenix.zmq_sub.addr);
            phoenix_connector_->subscriber_instance()->subscribe(CONFIG()->connector.phoenix.zmq_sub.topic);
            phoenix_connector_->subscriber_instance()->polling_loop(alive_, CONFIG()->connector.phoenix.zmq_sub.timeout);
        });

        sub_thread_handler_ = std::thread([this]() {
            std::unique_ptr<lion_connector> lion_conn = std::make_unique<lion_connector>();
            lion_conn->connect(CONFIG()->connector.lion.zmq_req.addr);

            while (alive_)
            {
                int timeout_ms = 1000;
                auto data = phoenix_connector_->subscriber_instance()->pop_for(timeout_ms);
                if (data.has_value())
                {
                    std::string topic; std::string payload;
                    std::tie(topic, payload) = data.value();
                    if (topic == "management")
                    {
                        nlohmann::json jpayload = nlohmann::json::parse(payload);
                        if (jpayload["type"].get<std::string>() == "shutdown" && jpayload["value"].get<bool>())
                        {
                            auto * power = new lion_protocol::Power;
                            power->set_type(lion_protocol::PowerType::SHUTDOWN);
                            auto * resource_type = new lion_protocol::ResourceType();
                            resource_type->set_allocated_power(power);

                            lion_protocol::Request request;
                            request.set_command(lion_protocol::SET);
                            request.set_allocated_resource(resource_type);

                            lion_conn->send("shutdown", request.SerializeAsString());

                            std::string identity;
                            std::string buffer;
                            lion_conn->recv(identity, buffer);

                            lion_protocol::Response response;
                            if (!response.ParseFromString(std::string(static_cast<char*>(buffer.data()), buffer.size())))
                            {
                                LOG_ERROR(LOGGER(CONFIG()->application.name), "Failed to parse received protobuf response from Lion");
                                return;
                            }

                            if (response.status() == lion_protocol::FAIL)
                            {
                                LOG_ERROR(LOGGER(CONFIG()->application.name), "Received response from Lion has a wrong status");
                                return;
                            }
                        }
                        else if (jpayload["type"].get<std::string>() == "reboot" && jpayload["value"].get<bool>())
                        {
                            auto * power = new lion_protocol::Power;
                            power->set_type(lion_protocol::PowerType::REBOOT);
                            auto * resource_type = new lion_protocol::ResourceType();
                            resource_type->set_allocated_power(power);

                            lion_protocol::Request request;
                            request.set_command(lion_protocol::SET);
                            request.set_allocated_resource(resource_type);

                            lion_conn->send("reboot", request.SerializeAsString());

                            std::string identity;
                            std::string buffer;
                            lion_conn->recv(identity, buffer);

                            lion_protocol::Response response;
                            if (!response.ParseFromString(std::string(static_cast<char*>(buffer.data()), buffer.size())))
                            {
                                LOG_ERROR(LOGGER(CONFIG()->application.name), "Failed to parse received protobuf response from Lion");
                                return;
                            }

                            if (response.status() == lion_protocol::FAIL)
                            {
                                LOG_ERROR(LOGGER(CONFIG()->application.name), "Received response from Lion has a wrong status");
                                return;
                            }
                        }
                        else
                        {
                            LOG_WARN(LOGGER(CONFIG()->application.name), "Unknown received IoT command: {}", jpayload["id"].get<std::string>());
                        }
                    }
                }
            }
        });
    }

    void microsvc_controller::stop()
    {
        alive_ = false;
        task_manager_->stop();
        io_service_.stop();
        pub_thread_.join();
        sub_thread_.join();
        sub_thread_handler_.join();
    }
}
