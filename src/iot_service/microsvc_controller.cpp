// project includes
#include <iot_service/microsvc_controller.hpp>
#include <iot_service/task_manager.hpp>
#include <system/sysinfo.hpp>
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
            phoenix_connector_->publisher_instance()->bind(CONFIG()->connector.phoenix.zmq_pub.addr);

            // TODO: need to implement sync mechanism to wait for sub client connection
            std::this_thread::sleep_for(std::chrono::seconds(10));

            std::function<void()> arch = [this]() {
                auto arch = sys::sysinfo::get(lion_protocol::SysInfoType::ARCH);
                if (arch.has_value())
                {
                    phoenix_protocol::MqttMessage msg;
                    msg.set_topic("sysinfo/arch");

                    nlohmann::json payload;
                    payload["sysinfo"]["arch"] = std::get<std::string>(arch.value());
                    msg.set_payload(payload.dump());

                    phoenix_connector_->publisher_instance()->publish(CONFIG()->connector.phoenix.zmq_pub.topic, msg.SerializeAsString());
                }
            };

            std::function<void()> os = [this]() {
                auto os = sys::sysinfo::get(lion_protocol::SysInfoType::OS);
                if (os.has_value())
                {
                    phoenix_protocol::MqttMessage msg;
                    msg.set_topic("sysinfo/os");

                    nlohmann::json payload;
                    payload["sysinfo"]["os"] = std::get<std::string>(os.value());
                    msg.set_payload(payload.dump());

                    phoenix_connector_->publisher_instance()->publish(CONFIG()->connector.phoenix.zmq_pub.topic, msg.SerializeAsString());
                }
            };

            std::function<void()> os_release = [this]() {
                auto os_release = sys::sysinfo::get(lion_protocol::SysInfoType::OS_RELEASE);
                if (os_release.has_value())
                {
                    phoenix_protocol::MqttMessage msg;
                    msg.set_topic("sysinfo/os_release");

                    nlohmann::json payload;
                    payload["sysinfo"]["os_release"] = std::get<std::string>(os_release.value());
                    msg.set_payload(payload.dump());

                    phoenix_connector_->publisher_instance()->publish(CONFIG()->connector.phoenix.zmq_pub.topic, msg.SerializeAsString());
                }
            };

            std::function<void()> cpu_num = [this]() {
                auto cpu_num = sys::sysinfo::get(lion_protocol::SysInfoType::CPU_NUM);
                if (cpu_num.has_value())
                {
                    phoenix_protocol::MqttMessage msg;
                    msg.set_topic("sysinfo/cpu_num");

                    nlohmann::json payload;
                    payload["sysinfo"]["cpu_num"] = std::get<std::uint32_t>(cpu_num.value());
                    msg.set_payload(payload.dump());

                    phoenix_connector_->publisher_instance()->publish(CONFIG()->connector.phoenix.zmq_pub.topic, msg.SerializeAsString());
                }
            };

            std::function<void()> cpu_speed = [this]() {
                auto cpu_speed = sys::sysinfo::get(lion_protocol::SysInfoType::CPU_SPEED);
                if (cpu_speed.has_value())
                {
                    phoenix_protocol::MqttMessage msg;
                    msg.set_topic("sysinfo/cpu_speed");

                    nlohmann::json payload;
                    payload["sysinfo"]["cpu_speed"] = std::get<std::uint32_t>(cpu_speed.value());
                    msg.set_payload(payload.dump());

                    phoenix_connector_->publisher_instance()->publish(CONFIG()->connector.phoenix.zmq_pub.topic, msg.SerializeAsString());
                }
            };

            std::function<void()> storage_total = [this]() {
                auto storage_total = sys::sysinfo::get(lion_protocol::SysInfoType::STORAGE_TOTAL);
                if (storage_total.has_value())
                {
                    phoenix_protocol::MqttMessage msg;
                    msg.set_topic("sysinfo/storage_total");

                    nlohmann::json payload;
                    payload["sysinfo"]["storage_total"] = std::get<std::uint32_t>(storage_total.value());
                    msg.set_payload(payload.dump());

                    phoenix_connector_->publisher_instance()->publish(CONFIG()->connector.phoenix.zmq_pub.topic, msg.SerializeAsString());
                }
            };

            std::function<void()> storage_free = [this]() {
                auto storage_free = sys::sysinfo::get(lion_protocol::SysInfoType::STORAGE_FREE);
                if (storage_free.has_value())
                {
                    phoenix_protocol::MqttMessage msg;
                    msg.set_topic("sysinfo/storage_free");

                    nlohmann::json payload;
                    payload["sysinfo"]["storage_free"] = std::get<std::uint32_t>(storage_free.value());
                    msg.set_payload(payload.dump());

                    phoenix_connector_->publisher_instance()->publish(CONFIG()->connector.phoenix.zmq_pub.topic, msg.SerializeAsString());
                }
            };

            std::function<void()> uptime = [this]() {
                auto uptime = sys::sysinfo::get(lion_protocol::SysInfoType::UPTIME);
                if (uptime.has_value())
                {
                    phoenix_protocol::MqttMessage msg;
                    msg.set_topic("sysinfo/uptime");

                    nlohmann::json payload;
                    payload["sysinfo"]["uptime"] = std::get<std::string>(uptime.value());
                    msg.set_payload(payload.dump());

                    phoenix_connector_->publisher_instance()->publish(CONFIG()->connector.phoenix.zmq_pub.topic, msg.SerializeAsString());
                }
            };

            std::function<void()> temperature = [this]() {
                auto temperature = sys::sysinfo::get(lion_protocol::SysInfoType::TEMPERATURE);
                if (temperature.has_value())
                {
                    phoenix_protocol::MqttMessage msg;
                    msg.set_topic("sysinfo/temperature");

                    nlohmann::json payload;
                    payload["sysinfo"]["temperature"] = std::get<double>(temperature.value());
                    msg.set_payload(payload.dump());

                    phoenix_connector_->publisher_instance()->publish(CONFIG()->connector.phoenix.zmq_pub.topic, msg.SerializeAsString());
                }
            };

            task_manager_->add_task(arch);
            task_manager_->add_task(os);
            task_manager_->add_task(os_release);
            task_manager_->add_task(cpu_num);
            task_manager_->add_task(cpu_speed);
            task_manager_->add_task(storage_total);
            task_manager_->add_task(storage_free);
            task_manager_->add_task(uptime, std::chrono::minutes(10));
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
                        if (jpayload["type"].get<std::string>() == "shutdown")
                        {
                            if (sys::power::set(lion_protocol::PowerType::SHUTDOWN, jpayload["value"].get<bool>()) == -1)
                            {
                                LOG_ERROR(LOGGER(CONFIG()->application.name), "Failed to shutdown device");
                            }
                        }
                        else if (jpayload["type"].get<std::string>() == "reboot" && jpayload["value"].get<bool>())
                        {
                            if (sys::power::set(lion_protocol::PowerType::REBOOT, jpayload["value"].get<bool>()) == -1)
                            {
                                LOG_ERROR(LOGGER(CONFIG()->application.name), "Failed to reboot device");
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
