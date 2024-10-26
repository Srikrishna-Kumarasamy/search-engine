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

        // Position: words position based on the term id from lexicon
        void get_inverted_index_Metadatas_based_on_position(ifstream& bin_in_file,unsigned long int position,unsigned long int lastdocsize,vector<unsigned long int> &read_Last_docIds_data,unsigned long int &lengthOfChunksizeList,vector<unsigned long int> &read_chunksize_data)
        {
            for(unsigned long int i=0;i<position-1;i++)
                {
                    
                    unsigned long int totalsize=0UL;
                    bin_in_file.read(reinterpret_cast<char*>(&totalsize), sizeof(totalsize));
                    cout<<totalsize<<endl;
                    cout<<bin_in_file.tellg()<<endl;

                    //Skipping logic
                    bin_in_file.seekg( totalsize , ios::cur);
                    cout<<"Location "<<bin_in_file.tellg()<<endl;
                }

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


   void get_entire_block_DocIDs_and_Freq(ifstream& bin_in_file,vector<u_int8_t> &entireBlock_DocID ,vector<u_int8_t> &entireBlock_Freq, unsigned long int &lastdocsize, vector<unsigned long int> &read_chunksize_data  )
   {
       // To retreieve the doclist (Entire)
        for(unsigned long int i=0; i< lastdocsize ; i++ )
        {

            vector<u_int8_t> docId_compressed_list(read_chunksize_data[ (2 * i)]);
            bin_in_file.read(reinterpret_cast<char*>( docId_compressed_list.data()), docId_compressed_list.size());
            cout<<" DocID in compressed : "<<docId_compressed_list;

            vector<u_int8_t> freq_compressed_list( read_chunksize_data[ (2*i)+1 ] );
            bin_in_file.read(reinterpret_cast<char*>( freq_compressed_list.data()), freq_compressed_list.size());
            cout<<" Freq in compressed : "<<freq_compressed_list;
        }
   }
   
    void get_mini_block_DocIDs_and_Freq( ifstream& bin_in_file, unsigned long int docID_to_be_searched, vector<u_int8_t> &miniBlock_DocID ,vector<u_int8_t> &miniBlock_Freq,unsigned long int &lastdocsize, vector<unsigned long int> &read_chunksize_data,vector<unsigned long int> &read_Last_docIds_data)
    {
         // To retrieve the particular Block based on the DocID
        unsigned long int docID_to_be_searched;
        unsigned long int total_chunk_Size=0UL;
        for(unsigned long int i=0;i<lastdocsize;i++)
             {
        
             if( docID_to_be_searched < read_Last_docIds_data[i] )
                 {

                        // Need to swift for reading
                        bin_in_file.seekg(total_chunk_Size,ios::cur);

                        // Reading that particular block and passing for uncompression (DOCIDs List and Freqs List)
                        //vector<u_int8_t> miniBlock_DocID( read_chunksize_data[2*i] );
                        //vector<u_int8_t> miniBlock_Freq( read_chunksize_data[(2*i)+1] );
                        bin_in_file.read(reinterpret_cast<char*>( miniBlock_DocID.data()), miniBlock_DocID.size());
                        bin_in_file.read(reinterpret_cast<char*>( miniBlock_Freq.data()), miniBlock_Freq.size());
                        
                        break;
        }
        total_chunk_Size += read_chunksize_data[ (2*i) ] + read_chunksize_data [ (2*i)+1 ];
            }
    
    }

void inverted_index_DAAT()
{

}

 int main()
 {
    // Needed for traversal : Last DocIdSize, Last DocIDs List, ChunkSize length, ChunkSize list
    ifstream bin_in_file("data.bin", ios::binary);
    unsigned long int lastdocsize=0UL;
    vector<unsigned long int> read_Last_docIds_data;
    unsigned long int lengthOfChunksizeList=0UL;
    vector<unsigned long int> read_chunksize_data;
    get_inverted_index_Metadatas_based_on_position(bin_in_file,2UL,lastdocsize,read_Last_docIds_data,lengthOfChunksizeList,read_chunksize_data);
    
    
    


    //Closing any instance
    bin_in_file.close();

    return 0;
 }