#include <stdio.h>
#include <ros/ros.h> 
#include <curl/curl.h>
#include "../thirdparty/rapidjson/document.h"
#include "../thirdparty/rapidjson/prettywriter.h"
#include "../include/remote.h"

using namespace rapidjson;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

static size_t dummy_callback(void *buffer, size_t size, size_t nmemb, void *userp)
{
   return size * nmemb;
}

int sensor_config(std::string sensor_ipaddr, std::string parameter, std::string value)
{
	long http_code;
	CURL *curl = curl_easy_init();
	std::string URL_RESTFUL_API = "http://" + sensor_ipaddr + parameter;

	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, URL_RESTFUL_API.c_str());
    	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, value.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dummy_callback);
		if (curl_easy_perform(curl) == CURLE_OK){
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
			if(http_code == 200)
			{
				ROS_INFO("Set %s, Value: %s ... done", URL_RESTFUL_API.c_str(), value.c_str());
			}
			else
			{
				ROS_INFO("Set %s, Value: %s ... failed!", URL_RESTFUL_API.c_str(), value.c_str());
			}
		}
		else
		{
			ROS_INFO("http put error! please check lidar connection!");
		}
	}
	curl_easy_cleanup(curl);
    curl_global_cleanup();

	return 0;
}

int get_telemetry_data(std::string sensor_ipaddr)
{
	CURL *curl;
	CURLcode res;
	std::string readBuffer;
	std::string URL_API_FIRMWARE = "http://" + sensor_ipaddr + "/api/v1/system/firmware";
	std::string URL_API_MONITOR = "http://" + sensor_ipaddr + "/api/v1/system/monitor";
	std::string URL_API_OVERVIEW = "http://" + sensor_ipaddr + "/api/v1/sensor/overview";

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	if(curl) {
		readBuffer = "";
		curl_easy_setopt(curl, CURLOPT_URL, URL_API_FIRMWARE.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		const char* json = const_cast<char*>(readBuffer.c_str());
		Document jsondoc;
		jsondoc.Parse(json);
		assert(jsondoc.IsObject());
		ROS_INFO("-------------------------------------------------");
		ROS_INFO("model:		%s", jsondoc["model"].GetString());
		ROS_INFO("sn:		%s", jsondoc["sn"].GetString());
		ROS_INFO("ver hw:		%s", jsondoc["hw"].GetString());
		ROS_INFO("ver fpga:	%s", jsondoc["fpga"].GetString());
		ROS_INFO("ver core:	%s", jsondoc["core"].GetString());
		ROS_INFO("ver aux:	%s", jsondoc["aux"].GetString());
	}

	curl = curl_easy_init();
	if(curl) {
		readBuffer = "";
		curl_easy_setopt(curl, CURLOPT_URL, URL_API_MONITOR.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		const char* json = const_cast<char*>(readBuffer.c_str());
		Document jsondoc;
		jsondoc.Parse(json);
		assert(jsondoc.IsObject());
		ROS_INFO("load average:	%.2f", jsondoc["load_average"].GetDouble());
		ROS_INFO("men useage:	%.2f", jsondoc["mem_useage"].GetDouble());
		ROS_INFO("uptime:		%.2f sec", jsondoc["uptime"].GetDouble());
	}

	curl = curl_easy_init();
	if(curl) {
		readBuffer = "";
		curl_easy_setopt(curl, CURLOPT_URL, URL_API_OVERVIEW.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		const char* json = const_cast<char*>(readBuffer.c_str());
		Document jsondoc;
		jsondoc.Parse(json);
		assert(jsondoc.IsObject());
		ROS_INFO("scanfreq:	%d hz", jsondoc["scanfreq"].GetInt());
		ROS_INFO("motor rpm:	%d (%.2fhz)", jsondoc["motor_rpm"].GetInt(), (jsondoc["motor_rpm"].GetInt() / 60.f));
		ROS_INFO("laser enable:	%d", jsondoc["laser_enable"].GetBool());
		ROS_INFO("scan start:	%d deg", jsondoc["scan_range"]["start"].GetInt());
		ROS_INFO("scan stop:	%d deg", jsondoc["scan_range"]["stop"].GetInt());
		ROS_INFO("flt level:	%d", jsondoc["filter"]["level"].GetInt());
		ROS_INFO("flt min_angle:	%d", jsondoc["filter"]["min_angle"].GetInt());
		ROS_INFO("flt max_angle:	%d", jsondoc["filter"]["max_angle"].GetInt());
		ROS_INFO("flt window:	%d", jsondoc["filter"]["window"].GetInt());
		ROS_INFO("flt neighbors:	%d", jsondoc["filter"]["neighbors"].GetInt());
		ROS_INFO("host ip:	%s", jsondoc["host"]["ip"].GetString());
		ROS_INFO("host port:	%d", jsondoc["host"]["port"].GetInt());
		ROS_INFO("-------------------------------------------------");
	}

	return 0;
}