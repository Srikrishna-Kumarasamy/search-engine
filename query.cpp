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


   void calculate_score(ifstream& bin_in_file,unsigned long int &lastdocsize, vector<unsigned long int> &read_chunksize_data  ,vector<unsigned long int> queryTermPositions)
   {
       // To retreieve the doclist (Entire)
       unsigned long int score=0;
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

                // Score calculation if there are more terms
                if(queryTermPositions.size()>1)
                {
                    for(unsigned long int index=0;index < queryTermPositions.size();index++)
                    {
                        ifstream invertedList_term_pointer("data.bin", ios::binary);
                        unsigned long int lastdocsize=0UL;
                        vector<unsigned long int> read_Last_docIds_data;
                        unsigned long int lengthOfChunksizeList=0UL;
                        vector<unsigned long int> read_chunksize_data;
                        // To move the pointer for reading in the binary file
                        bin_in_file.seekg( queryTermPositions[index] , ios::cur);
                        // For reading Metadata
                        get_inverted_index_Metadatas_based_on_position(invertedList_term_pointer,16UL,lastdocsize,read_Last_docIds_data,lengthOfChunksizeList,read_chunksize_data);
                        cout<<lastdocsize<<" "<<read_Last_docIds_data<<" "<<lengthOfChunksizeList<<" "<<read_chunksize_data<<endl;
                        
                        // BLock search based on DocID
                        vector<unsigned long int> miniBlock_DocID;
                        vector<unsigned long int> miniBlock_Freq;
                        get_mini_block_DocIDs_and_Freq(invertedList_term_pointer, decompressed_docID_block[blockIndex] ,miniBlock_DocID,miniBlock_Freq,lastdocsize,read_chunksize_data,read_Last_docIds_data);

                        // Condition check
                    

                        invertedList_term_pointer.close();
                    }
                }
            }

           

        }
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
            // Needed for traversal : Last DocIdSize, Last DocIDs List, ChunkSize length, ChunkSize list


            ifstream bin_in_file("data.bin", ios::binary);
            unsigned long int lastdocsize=0UL;
            vector<unsigned long int> read_Last_docIds_data;
            unsigned long int lengthOfChunksizeList=0UL;
            vector<unsigned long int> read_chunksize_data;
            get_inverted_index_Metadatas_based_on_position(bin_in_file,16UL,lastdocsize,read_Last_docIds_data,lengthOfChunksizeList,read_chunksize_data);
            cout<<lastdocsize<<" "<<read_Last_docIds_data<<" "<<lengthOfChunksizeList<<" "<<read_chunksize_data<<endl;
            
            // Decompress block by block for the the entire term or based on the condition
            vector<u_int8_t> entireBlock_DocID,entireBlock_Freq;

            


            


            //Closing any instance
            bin_in_file.close();

            return 0;
        }