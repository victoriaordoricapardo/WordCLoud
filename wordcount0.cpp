#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <wordcloud.hpp>


using namespace std;

int main() {
    // Define the input file names
    vector<string> filenames = {"1henryiv.txt", "2henryiv.txt", "hamlet.txt", "macbeth.txt", "merchant.txt", "tempest.txt"};

    // Define the output file name
    string outputFilename = "wordcloud.png";

    // Define a map to store the word count for each word
    unordered_map<string, int> wordCount;

    // Loop over each file and count the words
    for (string filename : filenames) {
        ifstream inputFile(filename);

        if (inputFile.is_open()) {
            string line;
            while (getline(inputFile, line)) {
                istringstream iss(line);

                string word;
                while (iss >> word) {
                    // Convert the word to lowercase
                    transform(word.begin(), word.end(), word.begin(), ::tolower);

                    // Remove any non-alphabetic characters from the word
                    word.erase(remove_if(word.begin(), word.end(), [](char c) { return !isalpha(c); }), word.end());

                    // Increment the word count in the map
                    if (word.length() > 0) {
                        wordCount[word]++;
                    }
                }
            }

            inputFile.close();
        }
        else {
            cout << "Unable to open file " << filename << endl;
        }
    }

    // Create a new WordCloud object
    WordCloud wc;

    // Loop over the words in the wordCount map and add them to the WordCloud
    for (auto const& [word, count] : wordCount) {
        wc.addWord(word, count);
    }

    // Generate the word cloud and save it to a file
    wc.generate(outputFilename);

    // Write the word count to a CSV file
    ofstream outputFile("wordcount.csv");

    if (outputFile.is_open()) {
        outputFile << "Word,Count\n";
        for (auto const& [word, count] : wordCount) {
            outputFile << word << "," << count << "\n";
        }

        outputFile.close();
    }
    else {
        cout << "Unable to open file wordcount.csv" << endl;
    }

    cout << "Word count saved to wordcount.csv" << endl;

    return 0;
}
