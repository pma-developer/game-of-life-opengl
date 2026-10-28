#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <iostream> 
#include <curl/curl.h>
#include <regex>
#include "LifeData.h"
#include <stack>

#pragma once
class RleImporter
{
public:
    RleImporter();

    LifePattern get_pattern_from_url(const char* url);

private:
    CURL* curl_handle;

    struct response {
        char* memory;
        size_t size;
    };

    const LifePattern getPatternFromRle(std::string rleString);

    const LifePattern getLifePatternFromRleData(const size_t lifePatternWidth, const size_t lifePatternHeight, std::string lifePatternCellStates);

    bool getDataFromRle(std::string rleString, size_t& lifePatternWidth, size_t& lifePatternHeight, std::string& lifePatternRuleString, std::string& lifePatternCellStates);

    static size_t mem_cb(void* contents, size_t size, size_t nmemb, void* userp);
};

