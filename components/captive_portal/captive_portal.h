#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/preferences.h"
#include "esphome/components/web_server_base/web_server_base.h"

#ifdef USE_ARDUINO
#include <DNSServer.h>
#endif

namespace esphome {
namespace captive_portal {

class CaptivePortal : public AsyncWebHandler, public Component {
 public:
  CaptivePortal(web_server_base::WebServerBase *base);
  void setup() override;
  void dump_config() override;
  void loop() override;
  float get_setup_priority() const override;
  void start();
  bool is_active() const { return this->active_; }
  bool canHandle(AsyncWebServerRequest *request) { return this->active_; }
  void handleRequest(AsyncWebServerRequest *req);

  void set_logo_url(const std::string &url) { this->logo_url_ = url; }
  void set_brand_name(const std::string &name) { this->brand_name_ = name; }
  void set_primary_color(const std::string &color) { this->primary_color_ = color; }

 protected:
  void handle_index(AsyncWebServerRequest *request);
  void handle_config(AsyncWebServerRequest *request);
  void handle_wifisave(AsyncWebServerRequest *request);
  void handle_reboot(AsyncWebServerRequest *request);
  void handle_reset(AsyncWebServerRequest *request);

  web_server_base::WebServerBase *base_;
  bool initialized_{false};
  bool active_{false};

  std::string logo_url_;
  std::string brand_name_{"DiraSmart"};
  std::string primary_color_{"#03A9F4"};

#ifdef USE_ARDUINO
  std::unique_ptr<DNSServer> dns_server_;
#endif
};

extern CaptivePortal *global_captive_portal;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

}  // namespace captive_portal
}  // namespace esphome
