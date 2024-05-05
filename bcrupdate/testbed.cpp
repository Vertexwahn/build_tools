#include <iostream>
#include <string>
#include <curl/curl.h>

// Callback function to receive response data
size_t write_callback(void *contents, size_t size, size_t nmemb, std::string *buffer) {
    size_t total_size = size * nmemb;
    buffer->append((char *)contents, total_size);
    return total_size;
}

int main() {
    CURL *curl;
    CURLcode res;
    std::string url = "https://api.github.com/graphql";
    std::string query = R"(
        {
          repository(owner: "rails", name: "rails") {
            refs(refPrefix: "refs/tags/", last: 100, orderBy: {field: TAG_COMMIT_DATE, direction: ASC}) {
              edges {
                node {
                  name
                  target {
                    oid
                    ... on Tag {
                      message
                      commitUrl
                      tagger {
                        name
                        email
                        date
                      }
                    }
                  }
                }
              }
            }
          }
        }
    )";
    std::string response;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Set headers
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Authorization: Bearer your_access_token"); // Replace with your GitHub access token
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Failed to perform request: " << curl_easy_strerror(res) << std::endl;
        } else {
            std::cout << "Response:\n" << response << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    return 0;
}
