#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;

class Term {
    public:
    string term;
    string doc_frequency;
    int intermediate_file_index;

    Term(string term, string doc_frequency, int intermediate_file_index)
    {
        this->term = term;
        this->doc_frequency = doc_frequency;
        this->intermediate_file_index = intermediate_file_index;
    }

    bool operator<(const Term& other) const
    {
        if (this->term == other.term) return this->intermediate_file_index > other.intermediate_file_index;
        return this->term > other.term;
    }
};

Term split_doc_id_frequency(int file_index, string line) {
    size_t colonPos = line.find(':');
    Term term = Term(
        line.substr(0, colonPos),
        line.substr(colonPos + 2),
        file_index
    );
    return term;
}

int main() 
{
    float BATCH_SIZE = 10000;
    int num_of_intermediate_files = ceil(8841823/BATCH_SIZE);
    ofstream inverted_index = ofstream("final_inverted_index.txt");
    vector<ifstream> intermediate_files(num_of_intermediate_files);
    priority_queue<Term, vector<Term>> min_heap;

    for(int i=0; i < num_of_intermediate_files; i++) {
        string file_name = "intermediate_inverted_indices/intermediate_inverted_index_" + to_string(i) + ".txt";
        intermediate_files[i].open(file_name);
    }

    for(int i=0; i < num_of_intermediate_files; i++) {
        string line;
        if (getline(intermediate_files[i], line)) {
            Term term = split_doc_id_frequency(i, line);
            min_heap.push(term);
        }
    }
    
    string prev = min_heap.top().term;
    string doc_freq_combined = "";
    long terms_processed = 0;
    while(!min_heap.empty()) {
        Term min_term = min_heap.top();
        min_heap.pop();
        if (min_term.term != prev)
        {
            inverted_index<<prev<<": "<<doc_freq_combined<<endl;
            doc_freq_combined = min_term.doc_frequency + " ";
        }
        else doc_freq_combined = doc_freq_combined + min_term.doc_frequency;
        string line;
        if (getline(intermediate_files[min_term.intermediate_file_index], line)) {
            Term term = split_doc_id_frequency(min_term.intermediate_file_index, line);
            min_heap.push(term);
        }
        else 
        {
            cout<<min_term.intermediate_file_index<<endl;
            intermediate_files[min_term.intermediate_file_index].close();
        }
        prev = min_term.term;
        terms_processed++;
        if (terms_processed % 10000 == 0) cout<<"Terms Processed : "<<terms_processed<<endl;
    }
    inverted_index<<prev<<": "<<doc_freq_combined<<endl;
    inverted_index.close();
    return 0;
}