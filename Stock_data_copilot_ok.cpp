#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <curl/curl.h>
#include <algorithm>
#include <vector>

// Function to write the data to a file
size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int main() {
    std::ifstream stock_list("stock_data2.csv");
    if (!stock_list) {
        std::cerr << "Unable to open file stock_data.csv" << std::endl;
        return 1;
    }

    std::string stock;
   // std::string output_dir = "C:\\Users\\add\\Downloads\\Python\\Data\\stock";
    std::string output_dir = "C:\\Users\\Thanimwas\\Downloads\\Data\\stock";
    //std::string output_dir = "C:\\mnt\\Users\\add\\Downloads\\Python\\Data\\stock";
    //std::string output_dir = "stock";
    //std::string consolidated_file = output_dir + "\\consolidated_stocks.csv";
    std::string consolidated_file = output_dir + "\\stock_data.csv";
    std::ofstream out_file(consolidated_file, std::ios::out );//| std::ios::out); // Open in append mode// เปิดไฟล์สำหรับเขียนและ append ข้อมูล  std::ios::out  / std::ios::app

    if (!out_file) {
        std::cerr << "Unable to open file " << consolidated_file << std::endl;
        return 1;
    }

    // Write header to the consolidated file
    out_file << "<TICKER>     \t<OPEN>    \t<HIGH>      \t<LOW>      \t<CLOSE>    \t<Adj Close>   \t<VOL>" << std::endl;

    // Loop through each stock in the CSV file
    while (getline(stock_list, stock)) {
        // Extract the stock symbol from the CSV line
        std::string stock_symbol = stock.substr(0, stock.find(','));

        // Convert stock symbol to uppercase and add the '.BK' extension
        std::string stock_upper = stock_symbol + ".BK";
        std::transform(stock_upper.begin(), stock_upper.end(), stock_upper.begin(), ::toupper);

        // Construct the URL for the Yahoo Finance API
        std::string url = "https://query1.finance.yahoo.com/v7/finance/download/" + stock_upper;
        url += "?period1=" + std::to_string(std::time(nullptr) - 86400); // From yesterday
        url += "&period2=" + std::to_string(std::time(nullptr)); // To today
        url += "&interval=1d&events=history&includeAdjustedClose=true";

        std::cout << "Downloading data for " << stock_upper << "..." << std::endl;

        // Initialize CURL and set the options
        CURL* curl = curl_easy_init();
        if (curl) {
            // Disable CURL's default verification of the site's SSL certificate
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Not recommended for production
           

            // Set up a temporary file to hold the downloaded data
            std::string temp_file_name = output_dir + "\\" + stock_symbol + "_temp.csv";
            FILE* temp_fp = fopen(temp_file_name.c_str(), "wb");

            if(temp_fp != NULL) {
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, temp_fp);
                CURLcode res = curl_easy_perform(curl);
                // Check for errors
                if(res != CURLE_OK) {
                    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                }
                fclose(temp_fp); // Close the temporary file

                // Open the temporary file for reading
                std::ifstream temp_in_file(temp_file_name, std::ios::in);
                std::string line;
                std::vector<std::string> rows;

                // Read the downloaded data and skip the header
                if (temp_in_file.is_open()) {
                    getline(temp_in_file, line); // Skip header
                    while (getline(temp_in_file, line)) {
                        rows.push_back(line);
                    }
                }
                temp_in_file.close();

                // Write the last line of data to the consolidated file
                if (!rows.empty()) {
                    out_file << stock_symbol << "\t" << rows.back() << std::endl;
                }

                // Remove the temporary file
                std::remove(temp_file_name.c_str());
            } else {
                std::cerr << "Failed to open temporary file for writing: " << temp_file_name << std::endl;
            }
            curl_easy_cleanup(curl); // Clean up CURL resources
        } else {
            std::cerr << "Failed to initialize CURL." << std::endl;
        }
    }
    out_file.close(); // Close the consolidated file
    return 0;
}


//g++ -o stock Stock_data_copilot_ok.cpp -lcurl  คำสั่งรันบนท Ubuntu
//g++ -o stock_new Stock_data_copilot_ok.cpp -lcurl  คำสั่งรันบนท Ubuntu

// รันบน Terminal อื่น
// g++ -o stock Stock_data_copilot_ok.cpp -I"C:/Curl/curl-8.7.1_7-win64-mingw/include" -L"C:/Curl/curl-8.7.1_7-win64-mingw/lib" -lcurl คำสั่งรัน
//cd /mnt/c/Users/add/Downloads/Python/Bata/C++  เข้า drirectory ของ Ubantu
//cd /mnt/c/Users/thanimwas/Downloads/C++/Test  เข้า drirectory ของ Ubantu
//cd /mnt/c/Users/add/Downloads/C++/Detection





//g++ curl_ok.cpp -o curl_ok -I"C:/Curl/curl-8.7.1_7-win64-mingw/include" -L"C:/Curl/curl-8.7.1_7-win64-mingw/lib" -lcurl
//./curl_ok