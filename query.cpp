#include<iostream>
#include<fstream>
#include<string.h>
#include<sstream>

using namespace std;

ostream& operator<<(ostream& os, const vector<unsigned long int>& vec) {
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
ostream& operator<<(ostream& os, const vector<u_int8_t>& vec) {
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        os << bitset<8>(vec[i]);
        if (i != vec.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}
int find_document_size(ifstream& document_index, unsigned long int target_document_id) {
    string line;
    while(getline(document_index, line)) {
        stringstream words(line);
        unsigned long int document_id;
        int document_size;
        words>>document_id>>document_size;
        // cout<<document_id<<" "<<document_size<<endl;
        if(document_id == target_document_id) {
            return document_size;
        }
    }
    return -1;
}

int get_avg_document_size(ifstream& document_index) {
    document_index.seekg(0, std::ios::end);
    streampos fileSize = document_index.tellg();
    string lastLine;
    char ch;
    for (std::streamoff i = 1; i < fileSize; ++i) {
        document_index.seekg(-i, std::ios::end);
        document_index.get(ch);
        if (ch == '\n') {
            getline(document_index, lastLine);
            return stoi(lastLine);
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

    bool operator<(const LexiconTerm& other) const {
        return this->number_of_documents < other.number_of_documents;
    }
};

    class DocumentScore{
        public:
        unsigned long int docID;
        double score;
        DocumentScore(unsigned long int docID,double score)
        {
            this->docID=docID;
            this->score=score;
        }
        // Define operator< for max-heap (higher score has higher priority)
    bool operator<(const DocumentScore& other) const {
        return this->score < other.score;  // Max-heap based on score
    }

    };

void get_query_terms_lexicon(ifstream& lexicon_file, vector<string> query_terms, vector<LexiconTerm>& lexicon_terms) {
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

            // BM25 parameters
        const double k1 = 1.5;
        const double b = 0.75;

        // Function to calculate IDF
        double calculateIDF(int N, int docFreq) {
            return log((N - docFreq + 0.5) / (docFreq + 0.5) + 1);
        }

        // Function to calculate BM25 score for a document given a query
        //df => Number of Documents containing that term
        // tf=> Term frequency
        // N => Total Number of Doucments in the collection
        // docLength => current document length (word count)
        // avgDocLength => Averge length of the entire documents (word count)
        double calculateBM25(int docLength, int avgDocLength,  int N, unsigned long int tf,unsigned long int df) {
            double score = 0.0;
                // Calculate IDF for the term
                double idf = calculateIDF(N, df);

                // Calculate BM25 term score
                double numerator = (tf+1) * (k1 + 1);
                double denominator = (tf+1) + k1 * (1 - b + b * (docLength / static_cast<double>(avgDocLength)));
                score += idf * (numerator / denominator);
               // cout<<"score "<<"idf "<<idf<<" num "<<numerator<<" denom "<<denominator<<endl;

            return score;
        }


    int linear_search(vector<unsigned long int> arr,unsigned long int key)
    {
         unsigned long int currentDocID=0;
         for(int i=0;i<arr.size();i++)
         {
            currentDocID+=arr[i];
            if(currentDocID==key)
            {
                return i;
            }
         }

        return -1;  // Target not found
    }

    vector<unsigned long> varbyte_decompress(const vector<uint8_t>& encoded_data) {
            vector<unsigned long> decompressed_values;
            unsigned long current_value = 0;
            int shift = 0;

            for (uint8_t byte : encoded_data) {
                // Extract the lower 7 bits of the byte and shift into the current value
                current_value |= (static_cast<unsigned long>(byte & 0x7F) << shift);

                // Check if MSB is 1, indicating the end of this integer
                if (byte & 0x80) {
                    decompressed_values.push_back(current_value); // Add the decompressed integer to the result
                    current_value = 0;  // Reset for the next integer
                    shift = 0;          // Reset shift for the next integer
                } else {
                    shift += 7;         // Increment shift by 7 to handle the next part of the number
                }
            }

            return decompressed_values;
    }

        // Position: words position based on the term id from lexicon
        void get_inverted_index_Metadatas_based_on_position(ifstream& bin_in_file,unsigned long int &lastdocsize,vector<unsigned long int> &read_Last_docIds_data,unsigned long int &lengthOfChunksizeList,vector<unsigned long int> &read_chunksize_data)
        {
           
                // To retrieve Total size
                unsigned long int totalsize=0UL;
                bin_in_file.read(reinterpret_cast<char*>(&totalsize), sizeof(totalsize));
                // cout<<totalsize<<endl;

                // To Retrieve the last doc size
                bin_in_file.read(reinterpret_cast<char*>(&lastdocsize), sizeof(lastdocsize)); 
                // cout<<lastdocsize<<endl;

                // To Retrieve the Last Doc IDs list 
                read_Last_docIds_data.resize(lastdocsize);
                bin_in_file.read(reinterpret_cast<char*>( read_Last_docIds_data.data()), read_Last_docIds_data.size()*8);
                // cout<<read_Last_docIds_data<<endl;

                // To Retrieve the size of ChunkSize
                bin_in_file.read(reinterpret_cast<char*>(&lengthOfChunksizeList), sizeof(lengthOfChunksizeList)); 
                // cout<<lengthOfChunksizeList<<endl;

                // To Retrieve the ChunkSize List 
               read_chunksize_data.resize(lengthOfChunksizeList);
                bin_in_file.read(reinterpret_cast<char*>( read_chunksize_data.data()), read_chunksize_data.size()*8);
                // cout<<read_chunksize_data<<endl;

            }

     void get_mini_block_DocIDs_and_Freq( ifstream& bin_in_file, unsigned long int docID_to_be_searched, vector<unsigned long int> &miniBlock_DocID 
                            ,vector<unsigned long int> &miniBlock_Freq,unsigned long int &lastdocsize, vector<unsigned long int> &read_chunksize_data,vector<unsigned long int> &read_Last_docIds_data)
            {
                // To retrieve the particular Block based on the DocID
                // unsigned long int docID_to_be_searched;
                unsigned long int total_chunk_Size=0UL;
                for(unsigned long int i=0;i<lastdocsize;i++)
                    {
                
                    if( docID_to_be_searched < read_Last_docIds_data[i] )
                        {

                                // Need to swift for reading
                                bin_in_file.seekg(total_chunk_Size,ios::cur);

                                // Reading that particular block and passing for uncompression (DOCIDs List and Freqs List)
                                vector<u_int8_t> miniBlock_DocID_compressed( read_chunksize_data[2*i] );
                                vector<u_int8_t> miniBlock_Freq_compressed( read_chunksize_data[(2*i)+1] );
                                bin_in_file.read(reinterpret_cast<char*>( miniBlock_DocID_compressed.data()), miniBlock_DocID.size());
                                bin_in_file.read(reinterpret_cast<char*>( miniBlock_Freq_compressed.data()), miniBlock_Freq.size());
                                
                                miniBlock_DocID = varbyte_decompress(miniBlock_DocID_compressed);
                                miniBlock_Freq= varbyte_decompress(miniBlock_Freq_compressed);

                                break;
                }
                total_chunk_Size += read_chunksize_data[ (2*i) ] + read_chunksize_data [ (2*i)+1 ];
                    }
                
            
            }


     priority_queue<DocumentScore> query_process(  vector<string> target_terms )
   {

        ifstream document_index = ifstream("document_index.txt");
        int avg_document_size=get_avg_document_size(document_index);
        document_index.seekg(0);
        // cout<<avg_document_size<<endl;
        ifstream lexicon_file = ifstream("lexicon.txt");
      
        vector<LexiconTerm> lexicon_terms;
        get_query_terms_lexicon(lexicon_file, target_terms, lexicon_terms);
        sort(lexicon_terms.begin(), lexicon_terms.end());
        // cout<<lexicon_terms[0].term<<endl;
            // File pointer for Primary term
            ifstream bin_in_file("data.bin", ios::binary);
            unsigned long int lastdocsize=0UL;
            vector<unsigned long int> read_Last_docIds_data;
            unsigned long int lengthOfChunksizeList=0UL;
            vector<unsigned long int> read_chunksize_data;
            // Movement of pointer for primary term
            // cout<<lexicon_terms[0].binary_position<<endl;
            bin_in_file.seekg(lexicon_terms[0].binary_position,ios::cur);
            // cout<<"Firsdt done "<<lexicon_terms[0].binary_position;
            get_inverted_index_Metadatas_based_on_position(bin_in_file,lastdocsize,read_Last_docIds_data,lengthOfChunksizeList,read_chunksize_data);
            // cout<<read_chunksize_data.size()<<endl;
        //    cout<<" second odne "<<endl;
        priority_queue<DocumentScore> max_heap_score;
       // To retreieve the doclist (Entire)
        double primaryTermScore=0.0,otherScores=0.0;
        // cout<<"2222"<<endl;
        for(unsigned long int i=0; i< lastdocsize ; i++ )
        {
            // cout<<"3333"<<endl;
            vector<u_int8_t> docId_compressed_list(read_chunksize_data[ (2 * i)]);
            // cout<<"12312312"<<endl;
            bin_in_file.read(reinterpret_cast<char*>( docId_compressed_list.data()), docId_compressed_list.size());
            cout<<" DocID in compressed : "<<docId_compressed_list<<endl;
            // cout<<"here"<<endl;
            // cout<<read_chunksize_data<<endl;
            vector<u_int8_t> freq_compressed_list( read_chunksize_data[ (2*i)+1 ] );
            // cout<<"here 12312312"<<read_chunksize_data[ (2*i)+1 ]<<endl;
            bin_in_file.read(reinterpret_cast<char*>( freq_compressed_list.data()), freq_compressed_list.size());
            // cout<<" Freq in compressed : "<<freq_compressed_list;
            // cout<<"5555"<<endl;
            //Call miniBlock DocID based on DocID search
            vector<unsigned long int> decompressed_docID_block= varbyte_decompress(docId_compressed_list);
            vector<unsigned long int> decompressed_freq_block= varbyte_decompress(freq_compressed_list);
            // cout<<"done"<<endl;
            //Inside block traversal
            // cout<<"3"<<endl;
            unsigned long int currentDocID=0;
            cout<<decompressed_docID_block<<endl;
            cout<<decompressed_freq_block<<endl;
            for(int blockIndex=0;blockIndex <  decompressed_docID_block.size();blockIndex++)
            {
                currentDocID+=decompressed_docID_block[blockIndex];
                //For the indexed word score calcualtion here
                int document_size=find_document_size(document_index,currentDocID);
                primaryTermScore=calculateBM25( document_size,avg_document_size,8400000,decompressed_freq_block[blockIndex], lexicon_terms[0].number_of_documents);
                // Score calculation if there are more terms
                // cout<<"1"<<endl;
                bool found_all_terms = true;
                if(lexicon_terms.size()>1)
                {
                    for(unsigned long int index=1;index < lexicon_terms.size();index++)
                    {
                        ifstream invertedList_term_pointer("data.bin", ios::binary);
                        unsigned long int lastdocsize=0UL;
                        vector<unsigned long int> read_Last_docIds_data;
                        unsigned long int lengthOfChunksizeList=0UL;
                        vector<unsigned long int> read_chunksize_data;
                        // To move the pointer for reading in the binary file
                        bin_in_file.seekg( lexicon_terms[index].binary_position , ios::cur);
                        // For reading Metadata
                        get_inverted_index_Metadatas_based_on_position(invertedList_term_pointer,lastdocsize,read_Last_docIds_data,lengthOfChunksizeList,read_chunksize_data);
                        // cout<<lastdocsize<<" "<<read_Last_docIds_data<<" "<<lengthOfChunksizeList<<" "<<read_chunksize_data<<endl;
                        
                        // BLock search based on DocID
                        vector<unsigned long int> miniBlock_DocID;
                        vector<unsigned long int> miniBlock_Freq;
                        get_mini_block_DocIDs_and_Freq(invertedList_term_pointer,currentDocID ,miniBlock_DocID 
                                                        ,miniBlock_Freq ,lastdocsize,read_chunksize_data,read_Last_docIds_data);

                        // Condition check using binary search
                        int mini_block_position=0;
                        if((mini_block_position=linear_search(miniBlock_DocID,currentDocID))!=-1)
                        {
                            // score calucaltion here
                            document_size=find_document_size(document_index,miniBlock_DocID[mini_block_position] );
                            otherScores+=calculateBM25( document_size,avg_document_size,8400000,miniBlock_Freq[mini_block_position], lexicon_terms[index].number_of_documents);
               
                        }
                        else{
                            found_all_terms = false;
                            break;
                        }

                        invertedList_term_pointer.close();
                    }
                    //skipping as based on conjuntive there are no other terms with same DocID
                    // if(!found_all_terms)
                    // {
                    //     continue;
                    // }
                    if(otherScores == primaryTermScore)
                    {
                        continue;
                    }

                }
                // cout<<"2"<<endl;
            // cout<<"second"<<endl;
            // cout<<decompressed_docID_block[blockIndex]<<" ";
            max_heap_score.push(DocumentScore(currentDocID,primaryTermScore+otherScores));
            otherScores=0.0;

            }

           

        }

        return max_heap_score;
         }
   
           

        int main()
        {
            vector<string> target_terms = {"01/01/1999"};
            // vector<string> target_terms = {"deffinately"};

            priority_queue<DocumentScore> result= query_process(target_terms);
            int k = 10;
            while(k > 0 && result.size() > 0)
            {
                DocumentScore resultDoc=result.top();
                result.pop();
                cout<<"Document ID = "<<resultDoc.docID<<" Score = "<<resultDoc.score<<endl;
                k--;
            }
            return 0;
        }