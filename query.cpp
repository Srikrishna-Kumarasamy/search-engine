#include<iostream>
#include<fstream>
#include<string.h>
#include<sstream>
#include<set>

using namespace std;

ostream& operator<<(
    ostream& os,
    const vector<unsigned long int>& vec
) {
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        os << vec[i];
        if (i != vec.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

ostream& operator<<(
    ostream& os,
    const vector<u_int8_t>& vec
) {
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        os << bitset<sizeof(unsigned long int)>(vec[i]);
        if (i != vec.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

int find_document_size(
    ifstream& document_index, 
    unsigned long int target_document_id
) {
    string line;
    while(getline(document_index, line)) {
        stringstream words(line);
        unsigned long int document_id;
        int document_size;
        words>>document_id>>document_size;
        if(document_id == target_document_id) {
            return document_size;
        }
    }
    return -1;
}

int get_avg_document_size(ifstream& document_index) {
    document_index.seekg(0, std::ios::end);
    streampos fileSize = document_index.tellg();
    string lastLine, avg_document_size;
    char ch;
    for (std::streamoff i = 1; i < fileSize; ++i) {
        document_index.seekg(-i, std::ios::end);
        document_index.get(ch);
        if (ch == '\n') {
            getline(document_index, lastLine);
            stringstream words(lastLine);
            words>>avg_document_size;
            return stoi(avg_document_size);
        }
    }
    return -1;
}

class LexiconTerm {
    public:
    string term;
    string term_id;
    long number_of_documents;
    unsigned long int binary_position;

    bool operator<(
        const LexiconTerm& other
    ) const {
        return this->number_of_documents < other.number_of_documents;
    }
};

class DocumentScore {
    public:
    unsigned long int docID;
    double score;

    DocumentScore(
        unsigned long int docID,
        double score
    ) {
        this->docID=docID;
        this->score=score;
    }

    bool operator<(
        const DocumentScore& other
    ) const {
        return this->score < other.score;
    }
};

void get_query_terms_lexicon(
    ifstream& lexicon_file, 
    vector<string> query_terms, 
    vector<LexiconTerm>& lexicon_terms
) {
    string line;
    sort(query_terms.begin(), query_terms.end());
    for(auto& target_term: query_terms)
    {
        while(getline(lexicon_file, line))
        {
            stringstream words(line);
            LexiconTerm lexicon_term;
            words>>lexicon_term.term>>lexicon_term.term_id>>lexicon_term.number_of_documents>>lexicon_term.binary_position;
            if(lexicon_term.term == target_term)
            {
                lexicon_terms.push_back(lexicon_term);
                break;
            }
        }
    }
}

const double k1 = 1.5;
const double b = 0.75;

double calculateIDF(
    int N,
    int docFreq
) {
    return log((N - docFreq + 0.5) / (docFreq + 0.5) + 1);
}

double calculateBM25(
    int docLength, 
    int avgDocLength,  
    int N, 
    unsigned long int tf,
    unsigned long int df
) {
    double score = 0.0;
    double idf = calculateIDF(N, df);
    double numerator = (tf+1) * (k1 + 1);
    double denominator = (tf+1) + k1 * (1 - b + b * (docLength / static_cast<double>(avgDocLength)));
    score += idf * (numerator / denominator);
    return score;
}


int linear_search(
    vector<unsigned long int> arr,
    unsigned long int key
) {
    unsigned long int currentDocID=0;
    for(int i=0;i<arr.size();i++)
    {
        currentDocID+=arr[i];
        if(currentDocID==key)
        {
            return i;
        }
    }
    return -1;
}

vector<unsigned long> varbyte_decompress(
    const vector<uint8_t>& encoded_data
) {
    vector<unsigned long> decompressed_values;
    unsigned long current_value = 0;
    int shift = 0;
    for (uint8_t byte : encoded_data) {
        current_value |= (static_cast<unsigned long>(byte & 0x7F) << shift);
        if (byte & 0x80) {
            decompressed_values.push_back(current_value);
            current_value = 0;
            shift = 0;
        } else {
            shift += 7;
        }
    }
    return decompressed_values;
}

void get_inverted_index_Metadatas_based_on_position(
    ifstream& bin_in_file,
    unsigned long int &last_docids_list_size,
    vector<unsigned long int> &last_docids_of_batches,
    unsigned long int &chunksize_list_size,
    vector<unsigned long int> &batch_chunksize
) {
    unsigned long int totalsize=0UL;
    bin_in_file.read(reinterpret_cast<char*>(&totalsize), sizeof(totalsize));
    bin_in_file.read(reinterpret_cast<char*>(&last_docids_list_size), sizeof(last_docids_list_size)); 
    last_docids_of_batches.resize(last_docids_list_size);
    bin_in_file.read(reinterpret_cast<char*>(last_docids_of_batches.data()), last_docids_of_batches.size()*8);
    bin_in_file.read(reinterpret_cast<char*>(&chunksize_list_size), sizeof(chunksize_list_size)); 
    batch_chunksize.resize(chunksize_list_size);
    bin_in_file.read(reinterpret_cast<char*>(batch_chunksize.data()), batch_chunksize.size()*8);
}

void get_mini_block_DocIDs_and_Freq(
    ifstream& bin_in_file,
    unsigned long int docID_to_be_searched,
    vector<unsigned long int> &miniBlock_DocID,
    vector<unsigned long int> &miniBlock_Freq,
    unsigned long int &lastdocsize,
    vector<unsigned long int> &read_chunksize_data,
    vector<unsigned long int> &read_Last_docIds_data
) {
    unsigned long int total_chunk_Size = 0UL;
    for(unsigned long int i=0; i<lastdocsize; i++)
    {
        if(docID_to_be_searched <= read_Last_docIds_data[i]) {
            bin_in_file.seekg(total_chunk_Size,ios::cur);
            vector<u_int8_t> miniBlock_DocID_compressed(read_chunksize_data[2 * i]);
            vector<u_int8_t> miniBlock_Freq_compressed(read_chunksize_data[(2 * i) + 1]);
            bin_in_file.read(reinterpret_cast<char*>(miniBlock_DocID_compressed.data()), miniBlock_DocID_compressed.size());
            bin_in_file.read(reinterpret_cast<char*>(miniBlock_Freq_compressed.data()), miniBlock_Freq_compressed.size());
            miniBlock_DocID = varbyte_decompress(miniBlock_DocID_compressed);
            miniBlock_Freq= varbyte_decompress(miniBlock_Freq_compressed);
            break;
        }
        total_chunk_Size += read_chunksize_data[(2 * i)] + read_chunksize_data [(2 * i) + 1];
    }
}

priority_queue<DocumentScore> query_process(
    vector<string> target_terms,
    string search_type = "Disjunctive"
) {
    ifstream document_index = ifstream("document_index.txt");
    int avg_document_size = get_avg_document_size(document_index);
    document_index.seekg(0);
    ifstream lexicon_file = ifstream("lexicon.txt");
    vector<LexiconTerm> lexicon_terms;
    get_query_terms_lexicon(lexicon_file, target_terms, lexicon_terms);
    priority_queue<DocumentScore> max_heap_score;
    if (lexicon_terms.size() == 0)
        return max_heap_score;
    if(search_type == "Disjunctive")
    {
        cout<<"Disjunctive"<<endl;
        ifstream bin_in_file("data.bin", ios::binary);
        unordered_map<unsigned long int, double> document_score_map;
        for(int term_index = 0; term_index < lexicon_terms.size(); term_index++)
        {
            unsigned long int last_docids_list_size=0UL;
            vector<unsigned long int> last_docids_of_batches;
            unsigned long int chunksize_list_size=0UL;
            vector<unsigned long int> batch_chunksize;
            bin_in_file.seekg(lexicon_terms[term_index].binary_position);
            get_inverted_index_Metadatas_based_on_position(bin_in_file, last_docids_list_size, last_docids_of_batches, chunksize_list_size, batch_chunksize);
            for(unsigned long int block_index=0; block_index<last_docids_list_size; block_index++)
            {
                vector<u_int8_t> docId_compressed_list(batch_chunksize[(2 * block_index)]);
                bin_in_file.read(reinterpret_cast<char*>(docId_compressed_list.data()), docId_compressed_list.size());
                vector<u_int8_t> freq_compressed_list(batch_chunksize[(2 * block_index) + 1]);
                bin_in_file.read(reinterpret_cast<char*>(freq_compressed_list.data()), freq_compressed_list.size());
                vector<unsigned long int> decompressed_docID_block = varbyte_decompress(docId_compressed_list);
                vector<unsigned long int> decompressed_freq_block = varbyte_decompress(freq_compressed_list);
                unsigned long int currentDocID=0;
                for(int i=0; i < decompressed_docID_block.size(); i++)
                {
                    currentDocID = currentDocID + decompressed_docID_block[i];
                    int document_size = find_document_size(document_index, currentDocID);
                    double document_score = calculateBM25(document_size, avg_document_size, 8400000, decompressed_freq_block[i], lexicon_terms[term_index].number_of_documents);
                    if (document_score_map.find(currentDocID) != document_score_map.end()) {
                        document_score_map[currentDocID] = document_score_map[currentDocID] + document_score;
                    } else {
                        document_score_map[currentDocID] = document_score;
                    }
                }
            }
        }
        for (const auto& pair : document_score_map) {
            max_heap_score.push(DocumentScore(pair.first, pair.second));
        }
        return max_heap_score;
    }
    else {
        cout<<"Conjunctive"<<endl;
        sort(lexicon_terms.begin(), lexicon_terms.end());
        ifstream bin_in_file("data.bin", ios::binary);
        unsigned long int last_docids_list_size=0UL;
        vector<unsigned long int> last_docids_of_batches;
        unsigned long int chunksize_list_size=0UL;
        vector<unsigned long int> batch_chunksize;
        bin_in_file.seekg(lexicon_terms[0].binary_position);
        get_inverted_index_Metadatas_based_on_position(bin_in_file, last_docids_list_size, last_docids_of_batches, chunksize_list_size, batch_chunksize);
        double primaryTermScore=0.0, otherScores=0.0;
        for(unsigned long int i=0; i<last_docids_list_size; i++)
        {
            vector<u_int8_t> docId_compressed_list(batch_chunksize[(2 * i)]);
            bin_in_file.read(reinterpret_cast<char*>(docId_compressed_list.data()), docId_compressed_list.size());
            vector<u_int8_t> freq_compressed_list(batch_chunksize[(2 * i) + 1]);
            bin_in_file.read(reinterpret_cast<char*>(freq_compressed_list.data()), freq_compressed_list.size());
            vector<unsigned long int> decompressed_docID_block = varbyte_decompress(docId_compressed_list);
            vector<unsigned long int> decompressed_freq_block = varbyte_decompress(freq_compressed_list);
            unsigned long int currentDocID=0;
            for(int blockIndex=0; blockIndex < decompressed_docID_block.size(); blockIndex++)
            {
                currentDocID += decompressed_docID_block[blockIndex];
                int document_size = find_document_size(document_index, currentDocID);
                primaryTermScore = calculateBM25(document_size, avg_document_size, 8400000, decompressed_freq_block[blockIndex], lexicon_terms[0].number_of_documents);
                bool found_all_terms = true;
                if(lexicon_terms.size() > 1)
                {
                    for(unsigned long int index=1; index < lexicon_terms.size(); index++)
                    {
                        ifstream invertedList_term_pointer("data.bin", ios::binary);
                        unsigned long int last_docids_list_size=0UL;
                        vector<unsigned long int> last_docids_of_batches;
                        unsigned long int chunksize_list_size=0UL;
                        vector<unsigned long int> batch_chunksize;
                        invertedList_term_pointer.seekg(lexicon_terms[index].binary_position , ios::beg);
                        get_inverted_index_Metadatas_based_on_position(invertedList_term_pointer, last_docids_list_size, last_docids_of_batches, chunksize_list_size, batch_chunksize);
                        vector<unsigned long int> miniBlock_DocID;
                        vector<unsigned long int> miniBlock_Freq;
                        get_mini_block_DocIDs_and_Freq(invertedList_term_pointer, currentDocID, miniBlock_DocID, miniBlock_Freq, last_docids_list_size, batch_chunksize, last_docids_of_batches);
                        int mini_block_position = 0;
                        if((mini_block_position = linear_search(miniBlock_DocID, currentDocID)) != -1)
                        {
                            document_size=find_document_size(document_index,miniBlock_DocID[mini_block_position]);
                            otherScores+=calculateBM25( document_size,avg_document_size,8400000,miniBlock_Freq[mini_block_position], lexicon_terms[index].number_of_documents);
                        }
                        else{
                            found_all_terms = false;
                            break;
                        }
                        invertedList_term_pointer.close();
                    }
                    if(!found_all_terms)
                    {
                        otherScores=0.0;
                        continue;
                    }
                }
            max_heap_score.push(DocumentScore(currentDocID, primaryTermScore + otherScores));
            otherScores=0.0;
            }
        }
        return max_heap_score;
    }
}

void find_top_k_results(string filename,string query_id,int k, priority_queue<DocumentScore> results) {
    ofstream top_k_queries_entry(filename,std::ios::app);
    top_k_queries_entry<<query_id<<" : ";

    while(k > 0 && results.size() > 0)
    {
        DocumentScore resultDoc=results.top();
        results.pop();
        top_k_queries_entry<<resultDoc.docID<<" ";
        cout<<resultDoc.docID<<" "<<resultDoc.score<<endl;
        k--;
    }
    top_k_queries_entry<<endl;
    top_k_queries_entry.close();
}

int main()
{
    // while(true)
    // {
    //     cout<<"Enter the terms to be searched for : ";
    //     string query;
    //     getline(cin, query);
    //     stringstream words(query);
    //     string word;
    //     vector<string> query_terms;
    //     while(words>>word)
    //     {
    //         transform(word.begin(), word.end(), word.begin(), ::tolower);
    //         query_terms.push_back(word);
    //     }
    //     const int k = 10;
    //     cout<<"----------------"<<endl;
    //     find_top_k_results(k, query_process(query_terms));
    //     cout<<"----------------"<<endl;
    //     find_top_k_results(k, query_process(query_terms, "Conjunctive"));
    //     cout<<"----------------"<<endl;
    // }
     vector<string> evaluation_file_names = {"qrels.dev.tsv"};
     vector<string> input_file_names={"queries.dev.tsv"};

    // Traverse using a range-based for loop
    for (int index_file=0;index_file< evaluation_file_names.size();index_file++) 
    {
        
        string customQueryLine;
        set<string> customQuerySet;
        ifstream customQuery(evaluation_file_names[index_file]);
        while(getline(customQuery,customQueryLine))
    {
        if (customQueryLine.empty()) continue;
        istringstream line_stream(customQueryLine);
        string customQuery_id;
        getline(line_stream, customQuery_id, '\t');
        customQuerySet.insert(customQuery_id);
    }
    cout<<"Length of the customQuery set is :"<<customQuerySet.size();
    string outputFileName=format("Top_K_Results_BM25_{}.txt",evaluation_file_names[index_file]);
    ofstream fileClear(outputFileName, std::ios::trunc);
    fileClear.close();
    ifstream file(input_file_names[index_file]);
    string line;
    
    while (getline(file, line))
    {
        if (line.empty()) continue;
        istringstream line_stream(line);
        string query_id, query;
        getline(line_stream, query_id, '\t');
        if(customQuerySet.count(query_id)==0)
        continue;
        getline(line_stream, query);
        cout<<"Query : "<<query<<endl;
        stringstream words(query);
        string word;
        vector<string> query_terms;
        while(words>>word)
        {
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            query_terms.push_back(word);
        }
        const int k = 10;
        cout<<"Query Terms : "<<query_terms.size();
        find_top_k_results(outputFileName,query_id,k, query_process(query_terms));

    }

    }
    
    return 0;
}