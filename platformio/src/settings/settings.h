#ifndef SETTINGS_H
#define SETTINGS_H

#include <vector>
#include <map>
#include <string>
#include "utils/json.h"
#include "utils/json_conversions.h"
#include "settings/settingsOption.h"

using json = nlohmann::json;

struct mqtt_topic
{
		String name = "";
		String topic_listen = "";
		String topic_publish = "";
};

struct Config_screenshot
{
		bool enabled = true;
		float temperature = -0.1;
		float tint = 1.0;
		float black = 0.0;
		float white = 1.0;
		float gamma = 0.9;
		float saturation = 1.0;
		float contrast = 1.0;
};

struct Config_mqtt
{
		bool enabled = false;
		String broker_ip = "";
		int broker_port = 1883;
		int keep_alive = 60;
		String username = "";
		String password = "";

		int retry_attemps = 5;

		String device_name = "squixl";

		std::vector<mqtt_topic> topics;

		String topic_listen = "squixl";
		String publish_topic = "squixl";

		bool has_ip()
		{
			return broker_ip.length() > 2;
		}
};

/**
 * @brief Settings struct for Haptics support
 */
struct Config_haptics
{
		bool enabled = true;
		bool trigger_on_boot = true;
		bool trigger_on_alarm = true;
		bool trigger_on_hour = true;
		bool trigger_on_event = false;
		bool trigger_on_wake = false;
		bool trigger_on_longpress = true;
		bool trigger_on_charge = true;
};

/**
 * @brief Settings struct for Haptics support
 */
struct Config_audio
{
		// Sound
		bool ui = true;
		bool alarm = true;
		bool on_hour = false;
		bool charge = true;
		int current_radio_station = 0;
};

struct wifi_station
{
		std::string ssid = "";
		std::string pass = "";
		uint8_t channel = 9;
};

struct Config_widget_battery
{
		// Fuel Gauge/Battery
		// This can be used to adjust what 100% is on your battery
		// This can be needed because the PMIC will stop charging the battery before it gets to 4.2V, usually around 4.1V,
		// So the fuel gauge will never actually get to a 100% state.
		// I'm not sure how to solve this othe than allow users to set an offset that visually shows full/no charging at 100%
		float perc_offset = 7.0;
		// User settable % to trigger fuel gauge wake from sleep trigger.
		// This can only be between 1% and 32%
		int low_perc = 25;
		// User settable V to trigger fuel gauge wake from sleep trigger.
		float low_volt_warn = 3.5;
		// User settable V to trigger power cutoff to the watch power switch.
		float low_volt_cutoff = 3.2;
};

/**
 * @brief Settings struct for open weather widget
 */
struct Config_widget_open_weather
{
		int poll_frequency = 30; // Open Weather poll interval - 30mins.
		String api_key = "";	 // API key for Open Weather
		bool enabled = true;
		bool units_metric = true;

		bool has_key()
		{
			return (api_key.length() > 1);
		}
};

/**
 * @brief Settings struct for open weather widget
 */
struct Config_widget_rss_feed
{
		int poll_frequency = 60;											// RSS poll interval - 60mins.
		String feed_url = "https://rss.slashdot.org/slashdot/slashdotmain"; // Default RSS Feed URL
		bool enabled = true;

		bool has_url()
		{
			return (feed_url.length() > 1);
		}
};

// Save data struct
struct Config
{
		int ver = 1;
		bool first_time = true;
		int screen_dim_mins = 10;

		int current_screen = 0;

		bool user_wallpaper = true;

		bool autostart_webserver = false;

		uint16_t case_color = 6371;

		bool ota_start = false;
		int wifi_tx_power = 44;

		bool wifi_check_for_updates = true;
		String mdns_name = "SQUiXL";

		std::vector<wifi_station> wifi_options;
		uint8_t current_wifi_station = 0;

		String country = "";
		String city = "";
		int utc_offset = 999;

		// Time
		bool time_24hour = false;
		bool time_dateformat = false; // False - DMY, True - MDY

		float volume = 15.0;

		int current_background = 0;
		int backlight_time_step_battery = 15; // in seconds
		int backlight_time_step_vbus = 30;	  // in seconds
		bool sleep_vbus = false;
		bool sleep_battery = true;

		// Battery/FG specific settings - see Struct above
		Config_widget_battery battery;

		// Open Weather specific settings - see Struct above
		Config_widget_open_weather open_weather;

		// RSS Feed specific settings - see Struct above
		Config_widget_rss_feed rss_feed;

		// Audio specific settings
		Config_audio audio;

		// MQTT specific settings - see Struct above
		Config_mqtt mqtt;

		// Haptics specific settings
		Config_haptics haptics;

		// Screenshot tool stuff
		Config_screenshot screenshot;

		json last_saved_data;

		String case_color_in_hex()
		{
			uint16_t color565 = (uint16_t)case_color;

			uint8_t r5 = (color565 >> 11) & 0x1F;
			uint8_t g6 = (color565 >> 5) & 0x3F;
			uint8_t b5 = color565 & 0x1F;

			uint8_t r8 = (r5 << 3) | (r5 >> 2);
			uint8_t g8 = (g6 << 2) | (g6 >> 4);
			uint8_t b8 = (b5 << 3) | (b5 >> 2);

			char buf[8];
			snprintf(buf, sizeof(buf), "#%02X%02X%02X", r8, g8, b8);
			return String(buf);
		}
};

enum SettingType
{
	MAIN,
	WEB,
	WIDGET,
	THEME,
	SCREENIE,
};

struct setting_group
{
		String name = "";
		String description = "";
		std::vector<SettingsOptionBase *> groups = {};
		SettingType type = SettingType::MAIN;

		setting_group(String nm, SettingType t, String d = "") : name(nm), type(t), description(d) {};

		// void setup(String nm, SettingType t, String d = "")
		// {
		// 	name = nm;
		// 	type = t;
		// 	description = d;
		// };
};

class Settings
{

	public:
		std::vector<setting_group> settings_groups;
		Config config;

		Settings(void)
		{
			// // Setup settings groups
			settings_groups.push_back({"General Settings", SettingType::MAIN});

			settings_groups.push_back({"WiFi & Web Settings", SettingType::WEB});

			settings_groups.push_back({"Audio Settings", SettingType::MAIN});

			settings_groups.push_back({"Haptics Settings", SettingType::MAIN});

			settings_groups.push_back({"Open Weather Settings", SettingType::WIDGET, "Add your Open Weather API key here to be able to see your current weather details on your SQUiXL."});

			settings_groups.push_back({"MQTT Settings", SettingType::WEB});

			settings_groups.push_back({"Screenie", SettingType::SCREENIE});

			settings_groups.push_back({"RSS Feed Settings", SettingType::WIDGET, "Add your RSS Feed URL here to be able to see your favourte RSS feed on your SQUiXL."});
		}

		void init();
		bool load();
		bool save(bool force);
		bool backup();
		bool create();
		void print_file();
		bool has_wifi_creds(void);
		bool has_country_set(void);
		void update_wifi_credentials(String ssid, String pass);
		unsigned long reset_screen_dim_time(void);
		bool update_menu = false;
		bool ui_forced_save = false; //

		String color565ToWebHex(uint16_t color565);
		uint16_t webHexToColor565(const char *hex);

		// Add any SettingsOption values here for any settings you want to bind with a tw_Control
		SettingsOptionBool setting_time_24hour{&config.time_24hour, 0, "Time Mode", "12H", "24H"};
		SettingsOptionBool setting_time_dateformat{&config.time_dateformat, 0, "Date FMT", "DMY", "MDY"};
		SettingsOptionBool setting_wallpaper{&config.user_wallpaper, 0, "Wallpaper", "SYSTEM", "USER"};

		SettingsOptionColor565 setting_case_color{&config.case_color, 0, "Case Color"};
		SettingsOptionIntRange settings_backlight_timer_battery{&config.backlight_time_step_battery, 5, 30, 1, false, 0, "Battery Backlight Dimmer Time (secs)"};
		SettingsOptionIntRange settings_backlight_timer_vbus{&config.backlight_time_step_vbus, 5, 60, 1, false, 0, "5V Backlight Dimmer Time (secs)"};
		SettingsOptionBool setting_sleep_vbus{&config.sleep_vbus, 0, "Sleep On 5V", "NO", "YES"};
		SettingsOptionBool setting_sleep_battery{&config.sleep_battery, 0, "Sleep On Battery", "NO", "YES"};

		// Web and WiFi
		SettingsOptionBool setting_OTA_start{&config.ota_start, 1, "Enable OTA Updates", "NO", "YES"};
		SettingsOptionBool setting_wifi_check_updates{&config.wifi_check_for_updates, 1, "Notify Updates", "NO", "YES"};
		SettingsOptionString setting_web_mdns{&config.mdns_name, 1, "mDNS Name", 0, -1, "SQUiXL", false};
		SettingsOptionString setting_country{&config.country, 1, "Country Code", 0, 2};
		SettingsOptionString setting_city{&config.city, 1, "City"};
		SettingsOptionIntRange settings_utc_offset{&config.utc_offset, -12, 14, 1, false, 1, "UTC Offset"};
		SettingsOptionWiFiStations wifi_stations{&config.wifi_options, 1, "Wifi Stations"};

		// Audio
		SettingsOptionBool setting_audio_ui{&config.audio.ui, 2, "UI Sound", "NO", "YES"};
		SettingsOptionBool setting_audio_alarm{&config.audio.alarm, 2, "Alarm Sound", "NO", "YES"};
		SettingsOptionBool setting_audio_on_hour{&config.audio.on_hour, 2, "Beep Hour", "NO", "YES"};
		SettingsOptionBool setting_audio_charge{&config.audio.charge, 2, "Start Charge", "NO", "YES"};
		SettingsOptionFloatRange setting_audio_volume{&config.volume, 0, 21, 1, false, 2, "Volume"};

		// haptics
		SettingsOptionBool setting_haptics_enabled{&config.haptics.enabled, 3, "Enabled", "NO", "YES"};
		SettingsOptionBool setting_haptics_trig_boot{&config.haptics.trigger_on_boot, 3, "On Boot", "NO", "YES"};
		SettingsOptionBool setting_haptics_trig_wake{&config.haptics.trigger_on_wake, 3, "On Wake", "NO", "YES"};
		SettingsOptionBool setting_haptics_trig_alarm{&config.haptics.trigger_on_alarm, 3, "On Alarm", "NO", "YES"};
		SettingsOptionBool setting_haptics_trig_hour{&config.haptics.trigger_on_hour, 3, "On Hour", "NO", "YES"};
		SettingsOptionBool setting_haptics_trig_event{&config.haptics.trigger_on_event, 3, "On Event", "NO", "YES"};
		SettingsOptionBool setting_haptics_trig_longpress{&config.haptics.trigger_on_longpress, 3, "LongPress", "NO", "YES"};
		SettingsOptionBool setting_haptics_trig_charge{&config.haptics.trigger_on_charge, 3, "Start Charge", "NO", "YES"};

		// Open Weather
		SettingsOptionBool widget_ow_enabled{&config.open_weather.enabled, 4, "Enabled", "NO", "YES"};
		SettingsOptionString widget_ow_apikey{&config.open_weather.api_key, 4, "API KEY", 0, -1, "", false};
		SettingsOptionIntRange widget_ow_poll_interval{&config.open_weather.poll_frequency, 10, 300, 10, false, 4, "Poll Interval (Min)"};
		SettingsOptionBool widget_ow_units{&config.open_weather.units_metric, 4, "Temperature Units", "Fahrenheit", "Celsius"};

		// MQTT
		SettingsOptionBool mqtt_enabled{&config.mqtt.enabled, 5, "Enabled", "NO", "YES"};
		SettingsOptionString mqtt_broker_ip{&config.mqtt.broker_ip, 5, "Broker IP"};
		SettingsOptionInt mqtt_broker_port{&config.mqtt.broker_port, 5, 2000, false, 5, "Broker Port"};
		SettingsOptionString mqtt_username{&config.mqtt.username, 5, "Username", 0, -1, "", false};
		SettingsOptionString mqtt_password{&config.mqtt.password, 5, "Password", 0, -1, "", false};
		SettingsOptionString mqtt_device_name{&config.mqtt.device_name, 5, "Device Name"};
		SettingsOptionString mqtt_topic_listen{&config.mqtt.topic_listen, 5, "Listen Topic"};

		// Screenshot
		SettingsOptionBool screenshot_enabled{&config.screenshot.enabled, 6, "Enabled", "NO", "YES"};
		SettingsOptionFloatRange screenshot_wb_temp{&config.screenshot.temperature, -1.0f, 1.0f, 0.1f, false, 6, "WB White Balance - Temperature"};
		SettingsOptionFloatRange screenshot_wb_tint{&config.screenshot.tint, -1.0f, 1.0f, 0.1f, false, 6, "White Balance -  Tint"};
		SettingsOptionFloatRange screenshot_black{&config.screenshot.black, 0.0f, 1.0f, 0.1f, false, 6, "Levels - Black"};
		SettingsOptionFloatRange screenshot_white{&config.screenshot.white, 0.0f, 1.0f, 0.1f, false, 6, "Levels - White"};
		SettingsOptionFloatRange screenshot_gamma{&config.screenshot.gamma, 0.0f, 2.0f, 0.1f, false, 6, "Levels - Gamma"};
		SettingsOptionFloatRange screenshot_saturation{&config.screenshot.saturation, 0.0f, 2.0f, 0.1f, false, 6, "Saturation"};
		SettingsOptionFloatRange screenshot_contrast{&config.screenshot.contrast, 0.0f, 2.0f, 0.1f, false, 6, "Contrast"};

		// RSS Feed
		SettingsOptionBool widget_rss_enabled{&config.rss_feed.enabled, 7, "Enabled", "NO", "YES"};
		SettingsOptionString widget_rss_feed_url{&config.rss_feed.feed_url, 7, "Feed URL", 0, -1, "", false};
		SettingsOptionIntRange widget_rss_poll_interval{&config.rss_feed.poll_frequency, 10, 300, 60, false, 7, "Poll Interval (Min)"};

	private:
		static constexpr const char *filename = "/settings.json";
		static constexpr const char *tmp_filename = "/tmp_settings.json";
		static constexpr const char *log = "/log.txt";
		static constexpr const char *backup_prefix = "settings_back_";
		static const int max_backups = 10;
		static long backupNumber(const String);

		unsigned long max_time_between_saves = 10000; // every 10 seconds
		unsigned long last_save_time = 0;
};

extern Settings settings;

#endif // SETTINGS_H
