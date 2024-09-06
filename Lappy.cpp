#include <iostream>
#include <windows.h>
#include <Shlobj.h>
#include <vector>
#include <WinSock2.h>
#include <stdio.h>
#include <process.h>
#include <string>
#include <fstream>
#include <WinInet.h>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

std::string createTempDirectory() {
    std::string tempDirectory;
    std::string tempPath = "C:\\TEMP\\boost";
    std::string directoryName = "boost";

    fs::path path(tempPath);
    if (!fs::exists(path)) {
        fs::create_directories(path);
    }

    auto it = fs::directory_iterator{ path };
    auto i = 0;
    for (; it != fs::directory_iterator{}; ++it, ++i) {
        if (it->path().filename() == directoryName) {
            tempDirectory = it->path().string();
            break;
        }
    }

    if (tempDirectory.empty()) {
        tempDirectory = tempPath + "\\" + directoryName + "\\" + i;
        fs::create_directories(fs::path(tempDirectory));
    }

    return tempDirectory;
}

std::string getIPAddress() {
    using boost::asio::ip::tcp;

    boost::asio::io_context ctx;
    tcp::resolver resolver(ctx);
    tcp::resolver::query query(boost::asio::ip::tcp::v4(), "whatsmyip.com", "80");
    tcp::resolver::iterator iterator = resolver.resolve(query);

    tcp::resolver::iterator end;
    tcp::socket socket(ctx);
    socket.connect(*iterator++, boost::asio::ip::tcp::resolver::endpoint_hints{});
    std::stringstream ss;
    char buffer[4096];
    while (true) {
        auto read_len = socket.read_some(boost::asio::buffer(buffer, sizeof(buffer)));
        if (read_len <= 0) break;

        auto startPos = buffer;
        auto pos = startPos;
        while (pos - startPos < read_len && buffer[pos] != '\n') ++pos;
        ss << std::string(startPos, pos);

        if (pos - startPos >= read_len - 1) break;
    }

    socket.close();
    std::string line;
    std::getline(ss, line, '\r');
    std::string ipAddress = line.substr(line.find(": ") + 2);

    return ipAddress;
}

std::string downloadFile(const std::string& url, const std::string& fileName) {
    using boost::asio::ip::tcp;

    boost::asio::io_context ctx;
    tcp::resolver resolver(ctx);
    tcp::resolver::query query(boost::asio::ip::tcp::v4(), url, "http");
    tcp::resolver::iterator iterator = resolver.resolve(query);

    tcp::resolver::iterator end;
    tcp::socket socket(ctx);
    socket.connect(*iterator++, boost::asio::ip::tcp::resolver::endpoint_hints{});

    std::ofstream file(fileName, std::ios::binary);
    boost::asio::streambuf buffer;
    char c;
    while (socket.read_some(boost::asio::buffer(&c, 1), boost::asio::transfer_at_least(1), &buffer)) {
        file.write(buffer.data(), buffer.size());
    }

    socket.close();
    file.close();

    return fileName;
}

void installBoost() {
    std::string tempDirectory = createTempDirectory();
    std::string url = "https://boostorg.jfrog.io/artifactory/main/release/boost_1_78_0/boost_1_78_0_0.exe";
    std::string fileName = downloadFile(url, "boost_1_78_0.exe");

    std::system((std::string("start ") + fileName + " /passive /d=" + tempDirectory).c_str());
}

std::string getBoostIncludePath() {
    std::string includePath;
    for (const auto& entry : fs::directory_iterator(tempDirectory)) {
        if (entry.path().filename() == "boost") {
            includePath = entry.path().string();
            break;
        }
    }

    std::string includePathEnd = includePath + "\\boost_1_78_0\\libs";
    if (fs::exists(fs::path(includePathEnd))) {
        includePath += "\\libs";
    }

    return includePath;
}

std::string getBoostLibPath() {
    std::string libPath;
    for (const auto& entry : fs::directory_iterator(tempDirectory)) {
        if (entry.path().filename() == "boost") {
            libPath = entry.path().string();
            break;
        }
    }

    std::string libPathEnd = libPath + "\\lib";
    if (fs::exists(fs::path(libPathEnd))) {
        libPath += "\\lib";
    }

    return libPath;
}

void linkBoost(const std::string& objectFile, const std::string& libPath, const std::string& includePath) {
    std::string linkerOptions = "/link /NOLOGO /LIBPATH:\"" + libPath + "\" /INCLUDE:\"" + includePath + "\" ";
    system((std::string("cl ") + linkerOptions + objectFile + " /subsystem:windows /entry:mainCRTStartup /FE:Lappy.exe /link /NOLOGO").c_str());
}

void main() {
    installBoost();

    std::string includePath = getBoostIncludePath();
    std::string libPath = getBoostLibPath();

    std::string linkerOptions = "/c /EHsc /nologo /W3 /GR /TP /I" + includePath + " /Fe Lappy.obj ";
    system((std::string("cl ") + linkerOptions + "Lappy.cpp").c_str());

    std::string directories[] = { "system32", "Temp", "SysWOW64", "C:\\Users\\%username%" };

    std::string LappyPath = "Lappy.exe";
    for (const std::string& dir : directories) {
        std::string fullPath = dir + "\\" + LappyPath;
        CreateDirectory(fullPath.c_str(), NULL);
    }

    CopyFile("Lappy.exe", "Temp\\Lappy.exe", FALSE);
    CopyFile("Lappy.exe", "SysWOW64\\Lappy.exe", FALSE);

    CreateProcess(NULL, ("Lappy.exe").c_str(), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
}
