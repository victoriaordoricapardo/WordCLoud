#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include "mpi.h"

using namespace std;
using namespace std::chrono;

// Function to read the text file and create a vector of words
vector<string> read_file(const string& filename) {
    vector<string> words;
    ifstream infile(filename);
    string line;
    while (getline(infile, line)) {
        stringstream ss(line);
        string word;
        while (ss >> word) {
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

int main(int argc, char *argv[]) {
    int rank, size;
    int MAX_BUFFER_SIZE = 10000;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Read in each of the six files and count the frequency of each word
    unordered_map<string, int> global_word_count;
    high_resolution_clock::time_point start_time;

    if (rank == 0) {
        start_time = high_resolution_clock::now();

        for (int i = 1; i <= 6; i++) {
            vector<string> words = read_file("shakespeare" + to_string(i) + ".txt");
            int local_word_count_size = words.size();

            // Determine the size of the subtasks
            int chunk_size = local_word_count_size / size;
            int remainder = local_word_count_size % size;
            int* counts = new int[size];
            int* displs = new int[size];
            displs[0] = 0;

            // Calculate the counts and displacements for each process
            for (int j = 0; j < size; j++) {
                counts[j] = chunk_size;
                if (j < remainder) counts[j]++;
                if (j > 0) displs[j] = displs[j-1] + counts[j-1];
            }

            // Send the subtasks to each process
            for (int j = 1; j < size; j++) {
                MPI_Send(&words[displs[j]], counts[j], MPI_STRING, j, 0, MPI_COMM_WORLD);
            }

            // Calculate the frequency of words in the local subtask
            unordered_map<string, int> local_word_count = count_words(vector<string>(words.begin(), words.begin() + counts[0]));

            // Receive the word counts from each process and merge them into the global word count
            for (int j = 1; j < size; j++) {
                unordered_map<string, int> received_word_count;
                char received_buffer[MAX_BUFFER_SIZE];
                MPI_Status status;
                MPI_Recv(received_buffer, MAX_BUFFER_SIZE, MPI_CHAR, j, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                int count;
                MPI_Get_count(&status, MPI_CHAR, &count);
                stringstream ss;
                ss.write(received_buffer, count);
                string received_string = ss.str();
                stringstream ss2(received_string);
                string word;
                int freq;
                while (ss2 >> word >> freq) {
                    received_word_count[word] += freq;
                }
                for (const auto& word_count : received_word_count) {
                            }

        // Gather word counts from all processes to root process
        int total_words = 0;
        MPI_Reduce(&local_word_count, &global_word_count, max_words, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&local_word_count, &total_words, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            // Output the word count matrix to CSV file
            ofstream output_file("word_counts_mpi.csv");
            output_file << "Word,Frequency" << endl;
            for (const auto& word_count : global_word_count) {
                output_file << word_count.first << "," << word_count.second << endl;
            }

            // Output the execution time
            double execution_time = MPI_Wtime() - start_time;
            cout << "Execution Time: " << execution_time << " seconds" << endl;
            cout << "Total Words Processed: " << total_words << endl;
        }
        
        MPI_Finalize();
        return 0;
}

                   
