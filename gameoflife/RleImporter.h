#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <iostream> 
#include <curl/curl.h>
#include <regex>
#include "LifePattern.h"

#pragma once
class RleImporter
{
public:

    RleImporter() {}

    LifePattern get_pattern_from_url(const char* url)
    {
        CURLcode res;

        curl_handle = curl_easy_init();
        if (curl_handle)
        {
            curl_easy_setopt(curl_handle, CURLOPT_URL, url);

            struct response chunk;
            chunk.memory = static_cast<char*>(malloc(0));
            chunk.size = 0;

            curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, mem_cb);
            curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);

            res = curl_easy_perform(curl_handle);
            if (res != CURLE_OK)
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
            
            std::string rleString(chunk.memory);
            LifePattern result = getPatternFromRle(rleString);

            printf(chunk.memory);

            free(chunk.memory);
            curl_easy_cleanup(curl_handle);

            return result;
        }

        throw std::runtime_error("curl was not initialized properly");
    }

private:
    CURL* curl_handle;

    struct response {
        char* memory;
        size_t size;
    };

    const LifePattern getPatternFromRle(std::string rleString)
    {
        size_t lifePatternWidth;
        size_t lifePatternHeight;
        std::string lifePatternRuleString;
        std::string lifePatternCellStates;

        if (!getDataFromRle(rleString, lifePatternWidth, lifePatternHeight, lifePatternRuleString, lifePatternCellStates)) {
            return LifePattern(0, 0, 0);
        }

        return getLifePatternFromRleData(lifePatternWidth, lifePatternHeight, lifePatternCellStates);
    }

    const LifePattern getLifePatternFromRleData(const size_t lifePatternWidth, const size_t lifePatternHeight, std::string lifePatternCellStates)
    {

        std::unique_ptr<char[]> body(new char[lifePatternWidth * lifePatternHeight]);

        int currentTagNumber = -1;
        size_t curOutputLineIndex = 0;
        size_t curOutputCharIndex = 0;

        for (size_t i = 0; i < lifePatternCellStates.length(); i++) {
            char curChar = lifePatternCellStates[i];

            if (curOutputLineIndex > lifePatternHeight - 1) {
                std::cout << "invalid RLE string, exceeded line height" << std::endl;
                return LifePattern(lifePatternWidth, lifePatternHeight, std::move(body));
            }

            if (std::isdigit(curChar)) {
                currentTagNumber = currentTagNumber == -1 ? curChar - '0' : (currentTagNumber * 10) + (curChar - '0');
            }
            else {
                currentTagNumber = currentTagNumber == -1 ? 1 : currentTagNumber;

                if (curChar == '!') {
                    return LifePattern(lifePatternWidth, lifePatternHeight, std::move(body));
                }
                else if (curChar == '$') {
                    curOutputLineIndex += currentTagNumber;
                    curOutputCharIndex = 0;
                }
                else if (curChar == 'b') {
                    curOutputCharIndex += currentTagNumber;
                    if (curOutputCharIndex > lifePatternWidth - 1) {
                        std::cout << "invalid RLE string, exceeded line width on \'b\' write" << std::endl;
                        return LifePattern(lifePatternWidth, lifePatternHeight, std::move(body));
                    }
                }
                else if (curChar == 'o') {
                    if (curOutputCharIndex + currentTagNumber - 1 > lifePatternWidth - 1) {
                        std::cout << "invalid RLE string, exceeded line width on \'o\' write" << std::endl;
                        return LifePattern(lifePatternWidth, lifePatternHeight, std::move(body));
                    }

                    for (size_t j = 0; j < currentTagNumber; j++) {
                        body[curOutputCharIndex + j + (curOutputLineIndex * lifePatternWidth)] = (char)255;
                    }
                    curOutputCharIndex += currentTagNumber;
                }
                currentTagNumber = -1;
            }
        }

        return LifePattern(lifePatternWidth, lifePatternHeight, std::move(body));
    }

    bool getDataFromRle(std::string rleString, size_t& lifePatternWidth, size_t& lifePatternHeight, std::string& lifePatternRuleString, std::string& lifePatternCellStates)
    {
        std::regex regexPattern(R"(x = (\d+), y = (\d+), rule = (\S+))");
        std::smatch match;
        if (std::regex_search(rleString, match, regexPattern)) {
            lifePatternWidth = std::stoi(match[1].str());
            lifePatternHeight = std::stoi(match[2].str());
            lifePatternRuleString = match[3].str();

            lifePatternCellStates = match.suffix();
        }
        else {
            std::cout << "Wrong string format. \"x = (\\d+), y = (\\d+), rule = (\\S+)\" not found in RLE string." << std::endl;
            return false;
        }


        std::string garbage = "\r\n";
        size_t garbagePosition = lifePatternCellStates.find(garbage);
        while (garbagePosition != std::string::npos) {
            lifePatternCellStates.erase(garbagePosition, garbage.length());
            garbagePosition = lifePatternCellStates.find(garbage);
        }
    }

    void printAs2DArray(char* array, int width, int height) {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                std::cout << array[i * width + j] << ' ';
            }
            std::cout << std::endl;
        }
    }

    static size_t
    mem_cb(void* contents, size_t size, size_t nmemb, void* userp)
    {
        size_t realsize = size * nmemb;
        response* mem = (response*)userp;

        char* ptr = static_cast<char*>(realloc(mem->memory, mem->size + realsize + 1));
        if (!ptr) {
            printf("not enough memory (realloc returned NULL)\n");
            return 0;
        }

        mem->memory = ptr;
        memcpy(&(mem->memory[mem->size]), contents, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;

        return realsize;
    }
};

