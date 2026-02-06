#include "captive_portal.h"
#ifdef USE_CAPTIVE_PORTAL
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/components/wifi/wifi_component.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace captive_portal {

static const char *const TAG = "captive_portal";

void CaptivePortal::handle_index(AsyncWebServerRequest *request) {
  String html = F("<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'><title>");
  html += this->brand_name_.c_str();
  html += F("</title><style>*{box-sizing:border-box;margin:0;padding:0}body{font-family:sans-serif;background:#");
  html += this->primary_color_.c_str() + 1; // Skip the # character
  html += F(";min-height:100vh;display:flex;align-items:center;justify-content:center;padding:20px}.c{background:#fff;border-radius:12px;padding:25px;max-width:380px;width:100%;box-shadow:0 4px 20px rgba(0,0,0,.3)}h1{color:#");
  html += this->primary_color_.c_str() + 1;
  html += F(";text-align:center;margin-bottom:15px;font-size:24px}h2{font-size:14px;margin:15px 0 10px;color:#333}.info{background:#f5f5f5;padding:10px;border-radius:8px;font-size:12px;margin-bottom:15px}.info b{color:#333}.net{padding:10px;border:1px solid #ddd;border-radius:6px;margin:5px 0;cursor:pointer}.net:hover{background:#f0f0f0}input{width:100%;padding:12px;border:1px solid #ddd;border-radius:6px;margin:8px 0;font-size:14px}.btn{width:100%;padding:12px;border:none;border-radius:6px;font-size:14px;cursor:pointer;margin:5px 0}.p{background:#");
  html += this->primary_color_.c_str() + 1;
  html += F(";color:#fff}.w{background:#ff9800;color:#fff}.d{background:#f44336;color:#fff}.g{display:flex;gap:10px}.g .btn{flex:1}</style></head><body><div class='c'><h1>");
  html += this->brand_name_.c_str();
  html += F("</h1><div class='info'><b>Device:</b> ");
  html += App.get_name().c_str();
  html += F("<br><b>MAC:</b> ");
  char mac[18];
  get_mac_address_pretty_into_buffer(mac);
  html += mac;
  html += F("</div><h2>WiFi Networks</h2><div id='n'>Scanning...</div><form action='/wifisave'><input name='ssid' placeholder='SSID' id='s'><input name='psk' type='password' placeholder='Password'><button class='btn p' type='submit'>Connect</button></form><h2>Device Actions</h2><div class='g'><a href='/reboot' class='btn w' onclick=\"return confirm('Reboot?')\">Reboot</a><a href='/reset' class='btn d' onclick=\"return confirm('Factory Reset?')\">Reset</a></div></div><script>fetch('/config.json').then(r=>r.json()).then(d=>{let h='';d.aps.forEach(a=>{h+='<div class=\"net\" onclick=\"document.getElementById(\\'s\\').value=\\''+a.ssid+'\\';\">'+a.ssid+' ('+a.rssi+'dBm)</div>';});document.getElementById('n').innerHTML=h||'No networks';}).catch(()=>{document.getElementById('n').innerHTML='Error';});</script></body></html>");
  request->send(200, "text/html", html);
}

void CaptivePortal::handle_config(AsyncWebServerRequest *request) {
  String json = "{\"name\":\"";
  json += App.get_name().c_str();
  json += "\",\"aps\":[";
  bool first = true;
  for (auto &scan : wifi::global_wifi_component->get_scan_result()) {
    if (scan.get_is_hidden()) continue;
    if (!first) json += ",";
    first = false;
    json += "{\"ssid\":\"";
    json += scan.get_ssid().c_str();
    json += "\",\"rssi\":";
    json += String(scan.get_rssi());
    json += "}";
  }
  json += "]}";
  request->send(200, "application/json", json);
}

void CaptivePortal::handle_wifisave(AsyncWebServerRequest *request) {
  std::string ssid = request->arg("ssid").c_str();
  std::string psk = request->arg("psk").c_str();
  ESP_LOGI(TAG, "WiFi credentials received - SSID: '%s'", ssid.c_str());
  request->send(200, "text/html", "<!DOCTYPE html><html><body style='font-family:sans-serif;text-align:center;padding:50px'><h2>WiFi Saved!</h2><p>Device will restart...</p></body></html>");
  this->set_timeout(200, [ssid, psk]() {
    wifi::global_wifi_component->save_wifi_sta(ssid, psk);
  });
}

void CaptivePortal::handle_reboot(AsyncWebServerRequest *request) {
  ESP_LOGI(TAG, "Reboot requested via captive portal");
  request->send(200, "text/html", "<!DOCTYPE html><html><body style='font-family:sans-serif;text-align:center;padding:50px'><h2>Rebooting...</h2></body></html>");
  this->set_timeout(200, []() { App.safe_reboot(); });
}

void CaptivePortal::handle_reset(AsyncWebServerRequest *request) {
  ESP_LOGW(TAG, "Factory reset requested via captive portal");
  request->send(200, "text/html", "<!DOCTYPE html><html><body style='font-family:sans-serif;text-align:center;padding:50px'><h2>Factory Reset</h2><p>Restarting...</p></body></html>");
  this->set_timeout(200, []() {
    global_preferences->reset();
    App.safe_reboot();
  });
}

void CaptivePortal::setup() {}

void CaptivePortal::loop() {
#ifdef USE_ARDUINO
  if (this->dns_server_ != nullptr)
    this->dns_server_->processNextRequest();
#endif
}

void CaptivePortal::start() {
  ESP_LOGI(TAG, "Starting captive portal...");
  this->base_->init();
  if (!this->initialized_) {
    AsyncWebServer *server = this->base_->get_server();

    server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
      this->handle_index(request);
    });

    server->on("/config.json", HTTP_GET, [this](AsyncWebServerRequest *request) {
      this->handle_config(request);
    });

    server->on("/wifisave", HTTP_GET, [this](AsyncWebServerRequest *request) {
      this->handle_wifisave(request);
    });

    server->on("/reboot", HTTP_GET, [this](AsyncWebServerRequest *request) {
      this->handle_reboot(request);
    });

    server->on("/reset", HTTP_GET, [this](AsyncWebServerRequest *request) {
      this->handle_reset(request);
    });

    server->onNotFound([this](AsyncWebServerRequest *request) {
      if (this->active_) {
        this->handle_index(request);
      } else {
        request->send(404, "text/plain", "Not found");
      }
    });
  }
#ifdef USE_ARDUINO
  this->dns_server_ = make_unique<DNSServer>();
  this->dns_server_->setErrorReplyCode(DNSReplyCode::NoError);
  this->dns_server_->start(53, "*", wifi::global_wifi_component->wifi_soft_ap_ip());
#endif
  this->initialized_ = true;
  this->active_ = true;
  ESP_LOGI(TAG, "Captive portal active");
}

void CaptivePortal::handleRequest(AsyncWebServerRequest *req) {
  String url = req->url();
  if (url == "/config.json") { this->handle_config(req); }
  else if (url == "/wifisave") { this->handle_wifisave(req); }
  else if (url == "/reboot") { this->handle_reboot(req); }
  else if (url == "/reset") { this->handle_reset(req); }
  else { this->handle_index(req); }
}

CaptivePortal::CaptivePortal(web_server_base::WebServerBase *base) : base_(base) { global_captive_portal = this; }
float CaptivePortal::get_setup_priority() const { return setup_priority::WIFI + 1.0f; }
void CaptivePortal::dump_config() {
  ESP_LOGCONFIG(TAG, "Captive Portal:");
  ESP_LOGCONFIG(TAG, "  Brand: %s", this->brand_name_.c_str());
}

CaptivePortal *global_captive_portal = nullptr;

}  // namespace captive_portal
}  // namespace esphome
#endif
