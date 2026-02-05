#include "captive_portal.h"
#ifdef USE_CAPTIVE_PORTAL
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/components/wifi/wifi_component.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace captive_portal {

static const char *const TAG = "captive_portal";

// DiraSmart Logo embedded as base64
static const char LOGO_BASE64[] PROGMEM = "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";

// Favicon embedded as base64
static const char FAVICON_BASE64[] PROGMEM = "iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAABGdBTUEAALGPC/xhBQAAACBjSFJNAAB6JgAAgIQAAPoAAACA6AAAdTAAAOpgAAA6mAAAF3CculE8AAACjlBMVEUAAAD////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////5/P685PWL0e7N6/j9/v7N6/eI0O675PX+/v7Y7/lyx+tRuuZ2yetLuOXn9fvt+PzV7vn2+/2H0O53yevZ8Pnu+PyW1fBKuOWEzu3l9PvW7/lvxupSu+aP0+9cvuhev+hIt+VuxeqDzu2Y1vDv+Py65PVXvedjwunL6vf4/P614vQ5seOX1vA9suNzx+vc8fr8/v7J6vdiwehZvee/5vbC5/dVvOiq3vT2+/7r9/xOuebi8/qBze1Nuebb8fr0+v2l2/JHtuXL6NyV0rSr2rW64MPZ7t73+/hcv+hdv+i44/V7y+yU1O+x3bx2w4l3w4tqvoBiu3mDyZXO6tX8/fz6/f5gwOiAze2T1O/e8OPc8OHJ59CHy5liunmt27n5/PrY7t5uwINku3tywYen2bTk8+ix3b1guney3b3x+fPK6NLG5s6m2LNrvoF6xY3f8eSr2rhlvHv7/fu+4seo2bXQ6td8xo+Ax5J5xI2U0KT9/v34/Pmc1KpxwYVhuni44MLZ7t+64cSz3r7r9u7v+PHp9expvX/h8uaKzJyLzJzR69hju3rW7dyr2re74cXg8eXN6dSg1a56xY7M6dT5/f5fwOhhwei34vR/ze30+vbo9eu338Kc1Kttv4Gn2tSL0fCCze1GtuTP7Phnw+mk2/Km3PKn3PJTu+b2+/jW7drK6fGM0e/7/f7i8/uHz+5wx+pxx+pwxupuxuqM0e7t9/w6UlDEAAAAJHRSTlMAACWH1vjXiSYCT9L+1FHp61LR0yiL1dj3+tmIjCcp7FRT2gEn7MyDAAAAAWJLR0QB/wIt3gAAAAd0SU1FB+cHFREzD2tCS8EAAAHISURBVDjLY2BgZGRiZmFVwQCsbOwcjIwMDIycXNw8KlgBDy8fJyMDIxe/Ck4gIMjIwCSkg...";

// HTML Template for captive portal
static const char CAPTIVE_PORTAL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>%BRAND% - WiFi Setup</title>
  <link rel="icon" type="image/png" href="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAABGdBTUEAALGPC/xhBQAAACBjSFJNAAB6JgAAgIQAAPoAAACA6AAAdTAAAOpgAAA6mAAAF3CculE8AAACjlBMVEUAAAD////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////5/P685PWL0e7N6/j9/v7N6/eI0O675PX+/v7Y7/lyx+tRuuZ2yetLuOXn9fvt+PzV7vn2+/2H0O53yevZ8Pnu+PyW1fBKuOWEzu3l9PvW7/lvxupSu+aP0+9cvuhev+hIt+VuxeqDzu2Y1vDv+Py65PVXvedjwunL6vf4/P614vQ5seOX1vA9suNzx+vc8fr8/v7J6vdiwehZvee/5vbC5/dVvOiq3vT2+/7r9/xOuebi8/qBze1Nuebb8fr0+v2l2/JHtuXL6NyV0rSr2rW64MPZ7t73+/hcv+hdv+i44/V7y+yU1O+x3bx2w4l3w4tqvoBiu3mDyZXO6tX8/fz6/f5gwOiAze2T1O/e8OPc8OHJ59CHy5liunmt27n5/PrY7t5uwINku3tywYen2bTk8+ix3b1guney3b3x+fPK6NLG5s6m2LNrvoF6xY3f8eSr2rhlvHv7/fu+4seo2bXQ6td8xo+Ax5J5xI2U0KT9/v34/Pmc1KpxwYVhuni44MLZ7t+64cSz3r7r9u7v+PHp9expvX/h8uaKzJyLzJzR69hju3rW7dyr2re74cXg8eXN6dSg1a56xY7M6dT5/f5fwOhhwei34vR/ze30+vbo9eu338Kc1Kttv4Gn2tSL0fCCze1GtuTP7Phnw+mk2/Km3PKn3PJTu+b2+/jW7drK6fGM0e/7/f7i8/uHz+5wx+pxx+pwxupuxuqM0e7t9/w6UlDEAAAAJHRSTlMAACWH1vjXiSYCT9L+1FHp61LR0yiL1dj3+tmIjCcp7FRT2gEn7MyDAAAAAWJLR0QB/wIt3gAAAAd0SU1FB+cHFREzD2tCS8EAAAHISURBVDjLY2BgZGRiZmFVwQCsbOwcjIwMDIycXNw8KlgBDy8fJyMDIxe/Ck4gIMjIwCSkg">
  <style>
    *{box-sizing:border-box;margin:0;padding:0}
    body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);min-height:100vh;display:flex;flex-direction:column;align-items:center;justify-content:center;padding:20px}
    .container{background:#fff;border-radius:16px;box-shadow:0 10px 40px rgba(0,0,0,.2);padding:30px;max-width:400px;width:100%}
    .logo{text-align:center;margin-bottom:20px}
    .logo img{max-width:200px;height:auto}
    .logo h1{color:%COLOR%;font-size:22px;margin-top:10px;font-weight:600}
    h2{color:#333;font-size:16px;margin-bottom:15px;text-align:center;font-weight:500}
    .info{background:linear-gradient(135deg,#e3f2fd 0%,#bbdefb 100%);border-radius:10px;padding:15px;margin-bottom:20px;font-size:12px}
    .info p{margin:4px 0;color:#1565c0}
    .info strong{color:#0d47a1}
    .networks{margin-bottom:15px;max-height:200px;overflow-y:auto}
    .network{display:flex;align-items:center;padding:12px 15px;border:2px solid #e0e0e0;border-radius:10px;margin-bottom:8px;cursor:pointer;transition:all .2s}
    .network:hover{background:#f5f5f5;border-color:%COLOR%;transform:translateX(5px)}
    .network.selected{background:linear-gradient(135deg,#e3f2fd 0%,#bbdefb 100%);border-color:%COLOR%}
    .network-name{flex:1;font-weight:500;color:#333}
    .network-signal{color:#666;font-size:11px;margin-right:8px}
    .network-lock{color:#666;font-size:14px}
    input[type="text"],input[type="password"]{width:100%;padding:14px;border:2px solid #e0e0e0;border-radius:10px;font-size:14px;margin-bottom:12px;transition:border-color .2s}
    input:focus{outline:none;border-color:%COLOR%}
    .btn{width:100%;padding:14px;border:none;border-radius:10px;font-size:15px;font-weight:600;cursor:pointer;transition:all .2s;margin-bottom:10px;text-transform:uppercase;letter-spacing:1px}
    .btn-primary{background:linear-gradient(135deg,%COLOR% 0%,#1976d2 100%);color:#fff;box-shadow:0 4px 15px rgba(3,169,244,.4)}
    .btn-primary:hover{transform:translateY(-2px);box-shadow:0 6px 20px rgba(3,169,244,.5)}
    .btn-danger{background:linear-gradient(135deg,#f44336 0%,#d32f2f 100%);color:#fff;box-shadow:0 4px 15px rgba(244,67,54,.4)}
    .btn-danger:hover{transform:translateY(-2px);box-shadow:0 6px 20px rgba(244,67,54,.5)}
    .btn-warning{background:linear-gradient(135deg,#ff9800 0%,#f57c00 100%);color:#fff;box-shadow:0 4px 15px rgba(255,152,0,.4)}
    .btn-warning:hover{transform:translateY(-2px);box-shadow:0 6px 20px rgba(255,152,0,.5)}
    .btn-group{display:flex;gap:10px;margin-top:15px}
    .btn-group .btn{flex:1;padding:12px 8px;font-size:12px}
    .divider{border-top:2px solid #e0e0e0;margin:20px 0;position:relative}
    .divider span{position:absolute;top:-12px;left:50%;transform:translateX(-50%);background:#fff;padding:0 15px;color:#999;font-size:11px;text-transform:uppercase;letter-spacing:1px}
    .hidden{display:none}
    .scanning{text-align:center;padding:30px;color:#666}
    .scanning::after{content:'';animation:dots 1.5s infinite}
    @keyframes dots{0%,20%{content:'.'}40%{content:'..'}60%,100%{content:'...'}}
    .modal{position:fixed;top:0;left:0;right:0;bottom:0;background:rgba(0,0,0,.6);display:flex;align-items:center;justify-content:center;z-index:100;backdrop-filter:blur(5px)}
    .modal-content{background:#fff;border-radius:16px;padding:30px;max-width:320px;text-align:center;box-shadow:0 20px 60px rgba(0,0,0,.3)}
    .modal-content h3{margin-bottom:10px;color:#333}
    .modal-content p{color:#666;margin-bottom:20px;font-size:14px}
    .footer{text-align:center;margin-top:15px;font-size:11px;color:#999}
  </style>
</head>
<body>
  <div class="container">
    <div class="logo">
      <img src="data:image/png;base64,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" alt="DiraSmart">
      <h1>%BRAND%</h1>
    </div>

    <div class="info">
      <p><strong>Device:</strong> <span id="dev-name">Loading...</span></p>
      <p><strong>MAC:</strong> <span id="dev-mac">Loading...</span></p>
      <p><strong>Version:</strong> <span id="dev-ver">Loading...</span></p>
    </div>

    <h2>Select WiFi Network</h2>

    <div id="networks" class="networks">
      <div class="scanning">Scanning networks</div>
    </div>

    <form id="wifi-form" class="hidden">
      <input type="text" id="ssid" name="ssid" placeholder="Network Name (SSID)" required>
      <input type="password" id="psk" name="psk" placeholder="Password">
      <button type="submit" class="btn btn-primary">Connect</button>
    </form>

    <div class="divider"><span>Device Actions</span></div>

    <div class="btn-group">
      <button class="btn btn-warning" onclick="confirmReboot()">Reboot</button>
      <button class="btn btn-danger" onclick="confirmReset()">Factory Reset</button>
    </div>

    <div class="footer">Powered by ESPHome</div>
  </div>

  <div id="modal" class="modal hidden">
    <div class="modal-content">
      <h3 id="modal-title">Confirm</h3>
      <p id="modal-text">Are you sure?</p>
      <div class="btn-group">
        <button class="btn btn-primary" id="modal-confirm">Yes</button>
        <button class="btn" style="background:#e0e0e0;color:#333" onclick="closeModal()">Cancel</button>
      </div>
    </div>
  </div>

  <script>
    let selectedSSID='';
    function loadConfig(){
      fetch('/config.json').then(r=>r.json()).then(d=>{
        document.getElementById('dev-name').textContent=d.name||'Unknown';
        document.getElementById('dev-mac').textContent=d.mac||'Unknown';
        document.getElementById('dev-ver').textContent=d.version||'Unknown';
        const nets=document.getElementById('networks');
        if(d.aps&&d.aps.length>0){
          nets.innerHTML=d.aps.map(ap=>`<div class="network" onclick="selectNetwork('${ap.ssid.replace(/'/g,"\\'")}')"><span class="network-name">${ap.ssid}</span><span class="network-signal">${ap.rssi}dBm</span>${ap.lock?'<span class="network-lock">🔒</span>':''}</div>`).join('');
        }else{nets.innerHTML='<p style="text-align:center;color:#666;padding:20px">No networks found. <a href="javascript:loadConfig()" style="color:%COLOR%">Refresh</a></p>';}
      }).catch(()=>{document.getElementById('networks').innerHTML='<p style="text-align:center;color:#f44336;padding:20px">Error loading. <a href="javascript:loadConfig()">Retry</a></p>';});
    }
    function selectNetwork(ssid){
      selectedSSID=ssid;
      document.querySelectorAll('.network').forEach(n=>n.classList.remove('selected'));
      event.currentTarget.classList.add('selected');
      document.getElementById('ssid').value=ssid;
      document.getElementById('wifi-form').classList.remove('hidden');
      document.getElementById('psk').focus();
    }
    document.getElementById('wifi-form').onsubmit=function(e){
      e.preventDefault();
      const ssid=document.getElementById('ssid').value;
      const psk=document.getElementById('psk').value;
      fetch('/wifisave?ssid='+encodeURIComponent(ssid)+'&psk='+encodeURIComponent(psk)).then(()=>{alert('WiFi saved! Device will restart...');});
    };
    function showModal(title,text,action){
      document.getElementById('modal-title').textContent=title;
      document.getElementById('modal-text').textContent=text;
      document.getElementById('modal-confirm').onclick=action;
      document.getElementById('modal').classList.remove('hidden');
    }
    function closeModal(){document.getElementById('modal').classList.add('hidden');}
    function confirmReboot(){showModal('Reboot Device','Are you sure you want to reboot?',function(){fetch('/reboot').then(()=>alert('Rebooting...'));closeModal();});}
    function confirmReset(){showModal('Factory Reset','This will erase all settings. Continue?',function(){fetch('/reset').then(()=>alert('Factory reset complete. Rebooting...'));closeModal();});}
    loadConfig();
  </script>
</body>
</html>
)rawliteral";

void CaptivePortal::handle_index(AsyncWebServerRequest *request) {
  String html = FPSTR(CAPTIVE_PORTAL_HTML);
  html.replace("%BRAND%", this->brand_name_.c_str());
  html.replace("%COLOR%", this->primary_color_.c_str());
  request->send(200, "text/html", html);
}

void CaptivePortal::handle_config(AsyncWebServerRequest *request) {
  String json = "{";
  json += "\"name\":\"" + String(App.get_name().c_str()) + "\",";
  char mac[18];
  get_mac_address_pretty_into_buffer(mac);
  json += "\"mac\":\"" + String(mac) + "\",";
#ifdef ESPHOME_PROJECT_VERSION
  json += "\"version\":\"" ESPHOME_PROJECT_VERSION "\",";
#else
  json += "\"version\":\"unknown\",";
#endif
  json += "\"aps\":[";
  bool first = true;
  for (auto &scan : wifi::global_wifi_component->get_scan_result()) {
    if (scan.get_is_hidden()) continue;
    if (!first) json += ",";
    first = false;
    json += "{\"ssid\":\"" + String(scan.get_ssid().c_str()) + "\",";
    json += "\"rssi\":" + String(scan.get_rssi()) + ",";
    json += "\"lock\":" + String(scan.get_with_auth() ? "true" : "false") + "}";
  }
  json += "]}";
  AsyncWebServerResponse *response = request->beginResponse(200, "application/json", json);
  response->addHeader("Cache-Control", "no-cache");
  request->send(response);
}

void CaptivePortal::handle_wifisave(AsyncWebServerRequest *request) {
  std::string ssid = request->arg("ssid").c_str();
  std::string psk = request->arg("psk").c_str();
  ESP_LOGI(TAG, "WiFi credentials received - SSID: '%s'", ssid.c_str());
  request->send(200, "text/plain", "OK");
  this->set_timeout(200, [ssid, psk]() {
    wifi::global_wifi_component->save_wifi_sta(ssid, psk);
  });
}

void CaptivePortal::handle_reboot(AsyncWebServerRequest *request) {
  ESP_LOGI(TAG, "Reboot requested via captive portal");
  request->send(200, "text/html", "<!DOCTYPE html><html><body style='font-family:sans-serif;text-align:center;padding:50px'><h2>Rebooting...</h2><p>Please wait.</p></body></html>");
  this->set_timeout(200, []() { App.safe_reboot(); });
}

void CaptivePortal::handle_reset(AsyncWebServerRequest *request) {
  ESP_LOGW(TAG, "Factory reset requested via captive portal");
  request->send(200, "text/html", "<!DOCTYPE html><html><body style='font-family:sans-serif;text-align:center;padding:50px'><h2>Factory Reset</h2><p>All settings erased. Restarting...</p></body></html>");
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
  this->base_->init();
  if (!this->initialized_) {
    this->base_->add_handler(this);
  }
#ifdef USE_ARDUINO
  this->dns_server_ = make_unique<DNSServer>();
  this->dns_server_->setErrorReplyCode(DNSReplyCode::NoError);
  this->dns_server_->start(53, "*", wifi::global_wifi_component->wifi_soft_ap_ip());
#endif
  this->initialized_ = true;
  this->active_ = true;
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
  ESP_LOGCONFIG(TAG, "Custom Captive Portal:");
  ESP_LOGCONFIG(TAG, "  Brand: %s", this->brand_name_.c_str());
}

CaptivePortal *global_captive_portal = nullptr;

}  // namespace captive_portal
}  // namespace esphome
#endif
