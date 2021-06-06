/*
    Made by ZKC#1337
    Github Repository : https://github.com/NotKrystian/Helium-Desktop/
    Currently only supports 1 Hotspot
    Requirements to run :
    - VCRedistx86 (If Release is set to x86)
    - VCRedistx64 (If Release is set to x64)


    How to compile :

    - Open cmd
    - git clone https://github.com/Microsoft/vcpkg.git
    - cd vcpkg
    - bootstrap-vcpkg.sh
    - vcpkg integrate install
    - vcpkg install libcurl
    - vcpkg install jsoncpp
    - vcpkg install wintoastlib


*/

#include <wintoast/wintoastlib.h>
#include <string>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <Windows.h>
#include <curl/curl.h>
#include <json/json.h>
#include <filesystem>

namespace
{
    std::size_t callback(
        const char* in,
        std::size_t size,
        std::size_t num,
        std::string* out)
    {
        const std::size_t totalBytes(size * num);
        out->append(in, totalBytes);
        return totalBytes;
    }
}


size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

std::wstring s2ws(const std::string& s) {
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

using namespace WinToastLib;


void witness();
void beacon();
void consensus();
void rewards();
void notification(LPWSTR text1);
void notification2(LPWSTR text1);

void HideConsole()
{
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

void ShowConsole()
{
    ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
}

class CustomHandler : public IWinToastHandler {
public:
    void toastActivated() const {
        std::wcout << L"The user clicked in this notification" << std::endl;
        ShowConsole();
        notification2(L"Click here to hide console");
    }

    void toastActivated(int actionIndex) const {
        std::wcout << L"The user clicked on notification using action #" << actionIndex << std::endl;
    }

    void toastDismissed(WinToastDismissalReason state) const {
        switch (state) {
        case UserCanceled:
            std::wcout << L"The user dismissed this notification" << std::endl;
            
            break;
        case TimedOut:
            std::wcout << L"The notification has timed out" << std::endl;
            
            break;
        case ApplicationHidden:
            std::wcout << L"The application hid the notification using ToastNotifier.hide()" << std::endl;
            
            break;
        default:
            std::wcout << L"notification not activated" << std::endl;
            
            break;
        }
    }

    void toastFailed() const {
        std::wcout << L"Error showing current notification" << std::endl;
        
    }
};

class CustomHandler2 : public IWinToastHandler {
public:
    void toastActivated() const {
        std::wcout << L"The user clicked in this notification" << std::endl;
        HideConsole();
    }

    void toastActivated(int actionIndex) const {
        std::wcout << L"The user clicked on notification using action #" << actionIndex << std::endl;
        HideConsole();
    }

    void toastDismissed(WinToastDismissalReason state) const {
        switch (state) {
        case UserCanceled:
            std::wcout << L"The user dismissed this notification" << std::endl;
            HideConsole();

            break;
        case TimedOut:
            std::wcout << L"The notification has timed out" << std::endl;

            break;
        case ApplicationHidden:
            std::wcout << L"The application hid the Notification using ToastNotifier.hide()" << std::endl;
            HideConsole();

            break;
        default:
            std::wcout << L"Notification not activated" << std::endl;
            HideConsole();
            break;
        }
    }

    void toastFailed() const {
        std::wcout << L"Error showing current notification" << std::endl;
        HideConsole();
    }
};


enum Results {
	ToastClicked,					
	ToastDismissed,					
	ToastTimeOut,					
	ToastHided,						
	ToastNotActivated,				
	ToastFailed,					
	SystemNotSupported,				
	UnhandledOption,				
	MultipleTextNotSupported,		
	InitializationFailure,			
	ToastNotLaunched				
};

using namespace std;

#include <iostream>
#include <fstream>  

int wmain()
{
    CURL* curl;
    FILE* fp;
    CURLcode res;
    char* url = "https://hypocrite.cc/Helium.jpg";  // Downloads helium logo for notification bar
    char outfilename[FILENAME_MAX] = "C://helium.jpg";
    curl = curl_easy_init();
    if (curl) {
        fp = fopen(outfilename, "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    }


    int f = 1;
    int witnesses = 0;
    int beacons = 0;
    int consensuss = 0;
    int rewardsg = 0;
    if (!std::filesystem::exists("C://config.txt")) { // If Config.txt isn't found on PC, app will make one for you
        ofstream outfile("C://config.txt");
        outfile.close();
        cout << "Paste your hotspot id in config.txt in C:/ and restart app"; // Hotspot ID can be found in explorer.helium.com under your hotspot name
        ShellExecuteA(NULL, "open", "C:\\", NULL, NULL, SW_SHOWDEFAULT);
        Sleep(100000);
        exit(0);
    }
    ifstream a;
    string hotspot;
    a.open("C://config.txt");
    getline(a, hotspot);
    a.close();
    HideConsole(); // Hides main window - Can be resurfaced after pressing on notification

    while (f == 1) {
        const std::string url("https://api.helium.io/v1/hotspots/" + hotspot + "/activity/count"); // Collects hotspot data from the helium api
        CURL* curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        long httpCode(0);
        std::unique_ptr<std::string> httpData(new std::string());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());
        curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        curl_easy_cleanup(curl);
        std::cout << "\nGot successful response from " << url << std::endl;
        Json::Value jsonData;
        Json::Reader jsonReader;
        jsonReader.parse(*httpData.get(), jsonData);
        cout << "Successfully parsed JSON data" << std::endl;
        cout << "\nJSON data received:" << std::endl;
        cout << jsonData.toStyledString() << std::endl;
        int witnessess = (jsonData["data"]["poc_receipts_v1"].asUInt64());
        int beaconss = (jsonData["data"]["poc_request_v1"].asUInt64());
        int consensusss = (jsonData["data"]["consensus_group_v1"].asUInt64());
        int rewardsgs = (jsonData["data"]["rewards_v2"].asUInt64());
        cout << "\tTotal Witnesses : " << witnessess << std::endl;
        cout << "\tTotal Beacons : " << beaconss << std::endl;
        cout << "\tTotal Consensus : " << consensusss << std::endl;
        cout << "\tTotal Rewards : " << rewardsgs << std::endl;
        if (witnessess != witnesses) {
            witness();
            witnesses = witnessess; // Updates witness standings in app
        }
        if (beaconss != beacons) {
            beacon();
            beacons = beaconss; // Updates beacons standings in app
        }
        if (consensusss != consensuss) {
            consensus();
            consensuss = consensusss; // Updates consensus standings in app
        }
        if (rewardsgs != rewardsg) {
            rewards();
            rewardsg = rewardsgs; // Updates rewards standings in app
        }
        Sleep(50000); // Waits 50 seconds before checking again
    }



}


void witness() {
    LPWSTR witness = L"Your hotspot has been a witness!";
    notification(witness);
}

void beacon() {
    LPWSTR beacon = L"Your hotspot has beaconed!";
    notification(beacon);
}

void consensus() {
    LPWSTR consensus = L"Your hotspot has been elected for consensus!";
    notification(consensus);

}

void rewards() {
    string hotspots;
    ifstream af;
    af.open("C://config.txt");
    getline(af, hotspots);
    af.close();
    const std::string url("https://api.helium.io/v1/hotspots/" + hotspots + "/rewards/sum?min_time=-1%20day&bucket=day"); // Collects hotspot's rewards for the last 24 hours
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    long httpCode(0);
    std::unique_ptr<std::string> httpData(new std::string());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());
    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);
    if (httpCode == 200)
    {
        std::cout << "\nGot successful response from " << url << std::endl;
        Json::Value jsonData;
        Json::Reader jsonReader;
        if (jsonReader.parse(*httpData.get(), jsonData))
        {
            std::cout << "Successfully parsed JSON data" << std::endl;
            std::cout << "\nJSON data received:" << std::endl;
            std::cout << jsonData.toStyledString() << std::endl;
            const std::string dateString(jsonData["data"][0]["total"].asString()); // Grabs hotspot rewards from RESPONSE
            std::cout << "\tHNT Total : " << dateString << std::endl;
            std::cout << std::endl;
            float amount = stod(dateString);
            string finals = to_string(amount);
            string RewardMessage = "Your hotspot has recieved rewards! " + finals + " HNT mined in the past 24 hours";
            wstring sym = s2ws(RewardMessage);
            LPWSTR sRewardMessage = &sym[0];
            LPWSTR reward = sRewardMessage;
            notification(reward);
        }
    }
}

void notification(LPWSTR text1) {

    LPWSTR appName = L"Helium Hotspot API",
        appUserModelID = L"Helium Hotspots",
        text = NULL,
        imagePath = L"C:/helium.jpg",
        attribute = L"Helium Hotspots";
    std::vector<std::wstring> actions;
    INT64 expiration = 0;


    bool onlyCreateShortcut = false;
    WinToastTemplate::AudioOption audioOption = WinToastTemplate::AudioOption::Default;


    WinToast::instance()->setAppName(appName);
    WinToast::instance()->setAppUserModelId(appUserModelID);

    if (!text)
        text = text1;

    WinToast::instance()->initialize();

    bool withImage = (imagePath != NULL);
    WinToastTemplate templ(withImage ? WinToastTemplate::ImageAndText02 : WinToastTemplate::Text02);
    templ.setTextField(text, WinToastTemplate::FirstLine);
    templ.setAudioOption(audioOption);
    templ.setAttributionText(attribute);

    for (auto const& action : actions)
        templ.addAction(action);
    if (expiration)
        templ.setExpiration(expiration);
    if (withImage)
        templ.setImagePath(imagePath);

    WinToast::instance()->showToast(templ, new CustomHandler());

}

void notification2(LPWSTR text1) {

    LPWSTR appName = L"Helium Hotspot API",
        appUserModelID = L"Helium Hotspots",
        text = NULL,
        imagePath = L"C:/helium.jpg",
        attribute = L"Helium Hotspots";
    std::vector<std::wstring> actions;
    INT64 expiration = 0;


    bool onlyCreateShortcut = false;
    WinToastTemplate::AudioOption audioOption = WinToastTemplate::AudioOption::Default;


    WinToast::instance()->setAppName(appName);
    WinToast::instance()->setAppUserModelId(appUserModelID);

    if (!text)
        text = text1;

    WinToast::instance()->initialize();

    bool withImage = (imagePath != NULL);
    WinToastTemplate templ(withImage ? WinToastTemplate::ImageAndText02 : WinToastTemplate::Text02);
    templ.setTextField(text, WinToastTemplate::FirstLine);
    templ.setAudioOption(audioOption);
    templ.setAttributionText(attribute);

    for (auto const& action : actions)
        templ.addAction(action);
    if (expiration)
        templ.setExpiration(expiration);
    if (withImage)
        templ.setImagePath(imagePath);

    WinToast::instance()->showToast(templ, new CustomHandler2());

}



