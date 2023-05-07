#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <regex>

using namespace std;
using namespace std::chrono;

// Function to read the text file and create a vector of words
vector<string> read_file(const string& filename) {
    vector<string> words;
    ifstream infile(filename);
    string line;
    while (getline(infile, line)) {
        // Remove HTML tags
        line = regex_replace(line, regex("<[^>]*>"), "");
        // Split line into words
        stringstream ss(line);
        string word;
        while (ss >> word) {
            // Remove unwanted words
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            if (word == "margin" || word == "https" || word == "rem" || word == "px" || word == "em" || word == "font" || word == "color" || word == "width" || word == "height" || word == "display") {
                continue;
            }
            words.push_back(word);
        }
    }
    return words;
}

// Function to count the frequency of each word
unordered_map<string, int> count_words(const vector<string>& words) {
    unordered_map<string, int> word_count;
    for (const string& word : words) {
        word_count[word]++;
    }
    return word_count;
}

int main() {
    high_resolution_clock::time_point start_time = high_resolution_clock::now();

    // Read in each of the six files and count the frequency of each word
    unordered_map<string, int> global_word_count;
    for (int i = 1; i <= 6; i++) {
        vector<string> words = read_file("shakespeare" + to_string(i) + ".txt");
        unordered_map<string, int> local_word_count = count_words(words);
        for (const auto& word_count : local_word_count) {
            global_word_count[word_count.first] += word_count.second;
        }
    }

    high_resolution_clock::time_point end_time = high_resolution_clock::now();

    // Output the word count matrix to a CSV file
    ofstream outfile("word_count.csv");
    outfile << "word,frequency" << endl;
    for (const auto& word_count : global_word_count) {
        outfile << word_count.first << "," << word_count.second << endl;
    }
    outfile.close();

    // Output the execution time
    duration<double> execution_time = duration_cast<duration<double>>(end_time - start_time);
    cout << "Execution Time: " << execution_time.count() << " seconds" << endl;

    return 0;
}
