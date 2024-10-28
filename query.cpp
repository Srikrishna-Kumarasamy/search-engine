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
                double numerator = tf * (k1 + 1);
                double denominator = tf + k1 * (1 - b + b * (docLength / static_cast<double>(avgDocLength)));
                score += idf * (numerator / denominator);
            

            return score;
        }


    int binary_search(vector<unsigned long int> arr,unsigned long int key)
    {
            int left = 0;
        int right = arr.size() - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;  // Avoids overflow

            // Check if the target is at mid
            if (arr[mid] == key) {
                return mid;  // Target found
            }

            // If target is greater, ignore the left half
            if (arr[mid] < key) {
                left = mid + 1;
            }
            // If target is smaller, ignore the right half
            else {
                right = mid - 1;
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
        void get_inverted_index_Metadatas_based_on_position(ifstream& bin_in_file,unsigned long int position,unsigned long int &lastdocsize,vector<unsigned long int> &read_Last_docIds_data,unsigned long int &lengthOfChunksizeList,vector<unsigned long int> &read_chunksize_data)
        {
            // for(unsigned long int i=0;i<position-1;i++)
            //     {
                    
            //         unsigned long int totalsize=0UL;
            //         bin_in_file.read(reinterpret_cast<char*>(&totalsize), sizeof(totalsize));
            //         cout<<totalsize<<endl;
            //         cout<<bin_in_file.tellg()<<endl;

            //         //Skipping logic
            //         bin_in_file.seekg( totalsize , ios::cur);
            //         cout<<"Location "<<bin_in_file.tellg()<<endl;
            //     }

                // To retrieve Total size
                unsigned long int totalsize=0UL;
                bin_in_file.read(reinterpret_cast<char*>(&totalsize), sizeof(totalsize));
                cout<<totalsize<<endl;

                // To Retrieve the last doc size
                bin_in_file.read(reinterpret_cast<char*>(&lastdocsize), sizeof(lastdocsize)); 
                cout<<lastdocsize<<endl;

                // To Retrieve the Last Doc IDs list 
                read_Last_docIds_data.resize(lastdocsize);
                bin_in_file.read(reinterpret_cast<char*>( read_Last_docIds_data.data()), read_Last_docIds_data.size()*8);
                cout<<read_Last_docIds_data<<endl;

                // To Retrieve the size of ChunkSize
                bin_in_file.read(reinterpret_cast<char*>(&lengthOfChunksizeList), sizeof(lengthOfChunksizeList)); 
                cout<<lengthOfChunksizeList<<endl;

                // To Retrieve the ChunkSize List 
               read_chunksize_data.resize(lengthOfChunksizeList);
                bin_in_file.read(reinterpret_cast<char*>( read_chunksize_data.data()), read_chunksize_data.size()*8);
                cout<<read_chunksize_data<<endl;

            }


     priority_queue<DocumentScore> query_process(  vector<string> target_terms )
   {

        ifstream document_index = ifstream("document_index.txt");
        int avg_document_size=get_avg_document_size(document_index);
        document_index.seekg(0);
        
        ifstream lexicon_file = ifstream("lexicon.txt");
      
        vector<LexiconTerm> lexicon_terms;
        get_query_terms_lexicon(lexicon_file, target_terms, lexicon_terms);
        sort(lexicon_terms.begin(), lexicon_terms.end());

            // File pointer for Primary term
            ifstream bin_in_file("data.bin", ios::binary);
            unsigned long int lastdocsize=0UL;
            vector<unsigned long int> read_Last_docIds_data;
            unsigned long int lengthOfChunksizeList=0UL;
            vector<unsigned long int> read_chunksize_data;
            // Movement of pointer for primary term
            bin_in_file.seekg(lexicon_terms[0].binary_position,ios::cur);
            get_inverted_index_Metadatas_based_on_position(bin_in_file,16UL,lastdocsize,read_Last_docIds_data,lengthOfChunksizeList,read_chunksize_data);
           

        priority_queue<DocumentScore> max_heap_score;
       // To retreieve the doclist (Entire)
        double primaryTermScore=0.0,otherScores=0.0;
        
        for(unsigned long int i=0; i< lastdocsize ; i++ )
        {

            vector<u_int8_t> docId_compressed_list(read_chunksize_data[ (2 * i)]);
            bin_in_file.read(reinterpret_cast<char*>( docId_compressed_list.data()), docId_compressed_list.size());
            cout<<" DocID in compressed : "<<docId_compressed_list;

            vector<u_int8_t> freq_compressed_list( read_chunksize_data[ (2*i)+1 ] );
            bin_in_file.read(reinterpret_cast<char*>( freq_compressed_list.data()), freq_compressed_list.size());
            cout<<" Freq in compressed : "<<freq_compressed_list;

            //Call miniBlock DocID based on DocID search
            vector<unsigned long int> decompressed_docID_block= varbyte_decompress(docId_compressed_list);
            vector<unsigned long int> decompressed_freq_block= varbyte_decompress(freq_compressed_list);
            //Inside block traversal
            for(int blockIndex=0;blockIndex <  decompressed_docID_block.size();blockIndex++)
            {
                //For the indexed word score calcualtion here
                int document_size=find_document_size(document_index,decompressed_docID_block[blockIndex] );
                primaryTermScore=calculateBM25( document_size,avg_document_size,8400000,decompressed_freq_block[blockIndex], lexicon_terms[0].number_of_documents);
                // Score calculation if there are more terms
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
                        get_inverted_index_Metadatas_based_on_position(invertedList_term_pointer,16UL,lastdocsize,read_Last_docIds_data,lengthOfChunksizeList,read_chunksize_data);
                        cout<<lastdocsize<<" "<<read_Last_docIds_data<<" "<<lengthOfChunksizeList<<" "<<read_chunksize_data<<endl;
                        
                        // BLock search based on DocID
                        vector<unsigned long int> miniBlock_DocID;
                        vector<unsigned long int> miniBlock_Freq;
                        get_mini_block_DocIDs_and_Freq(invertedList_term_pointer, decompressed_docID_block[blockIndex] ,miniBlock_DocID,miniBlock_Freq,lastdocsize,read_chunksize_data,read_Last_docIds_data);

                        // Condition check using binary search
                        int mini_block_position=0;
                        if((mini_block_position=binary_search(miniBlock_DocID,decompressed_docID_block[blockIndex]))!=-1)
                        {
                            // score calucaltion here
                            document_size=find_document_size(document_index,decompressed_docID_block[blockIndex] );
                            otherScores+=calculateBM25( document_size,avg_document_size,8400000,decompressed_freq_block[blockIndex], lexicon_terms[index].number_of_documents);
               
                        }
                        else{
                            break;
                        }


                        invertedList_term_pointer.close();
                    }
                    //skipping as based on conjuntive there are no other terms with same DocID
                    if(otherScores==primaryTermScore)
                    continue;

                }
            max_heap_score.push(DocumentScore(decompressed_docID_block[blockIndex],primaryTermScore+otherScores));
            otherScores=0.0;

            }

           

        }

        return max_heap_score;
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


        int main()
        {
            vector<string> target_terms = {"apple", "pragya", "semi-reduced"};

            priority_queue<DocumentScore> result= query_process(target_terms);
            for(int i=0;i<5;i++)
            {
                DocumentScore resultDoc=result.top();
                cout<<"Document ID = "<<resultDoc.docID<<" Score = "<<resultDoc.score;
                result.pop();
            }
            return 0;
        }