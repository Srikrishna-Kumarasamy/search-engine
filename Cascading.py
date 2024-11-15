import h5py
import numpy as np
import pandas as pd

def writeResultInFile(filename,queryID,l):
    with open(filename,"a") as f:
        f.write(queryID+" : "+str(" ".join([documentid[0] for documentid in l]))+"\n")


# Similarity score generation
def getSimilarityScore(queryEmbed,documentEmbed):
    queryNormalized= queryEmbed / np.linalg.norm(queryEmbed)
    documentNormalized = documentEmbed / np.linalg.norm(documentEmbed)
    # print(type(documentNormalized),len(documentNormalized))
    # print(type(queryNormalized),len(queryNormalized))
    # print(len(queryNormalized),queryEmbed)
    return np.dot(queryNormalized,documentNormalized) 

# Read text file and to get ranked results from BM25
def load_h5_embeddings(file_path, id_key='id', embedding_key='embedding'):
    """
    Load IDs and embeddings from an HDF5 file.

    Parameters:
    - id_key: Dataset name for the IDs inside the HDF5 file.
    - embedding_key: Dataset name for the embeddings inside the HDF5 file.

    Returns:
    - ids: Numpy array of IDs (as strings).
    - embeddings: Numpy array of embeddings (as float32).
    """
    print(f"Loading data from {file_path}...")
    with h5py.File(file_path, 'r') as f:
        ids = np.array(f[id_key]).astype(str)
        embeddings = np.array(f[embedding_key]).astype(np.float32)  

    print(f"Loaded {len(ids)} embeddings.")
    return ids, embeddings

# usage:
file_path = '/Users/rockramsri/Documents/GitHub/Web search engine works/Project 3/MSMARCO-Embeddings/msmarco_passages_embeddings_subset.h5'
ids, embeddings = load_h5_embeddings(file_path)
query_ids,query_embeddings = load_h5_embeddings("/Users/rockramsri/Documents/GitHub/Web search engine works/Project 3/MSMARCO-Embeddings/msmarco_queries_dev_eval_embeddings.h5")

# 
inputFileNames = ["Top_K_Results_BM25_qrels.dev.tsv.txt","Top_K_Results_BM25_qrels.eval.one.tsv.txt","Top_K_Results_BM25_qrels.eval.two.tsv.txt"]
outputFileNames = ["Top_K_Results_BM25_rerank_qrels.dev.tsv.txt","Top_K_Results_BM25_rerank_qrels.eval.one.tsv.txt","Top_K_Results_BM25_rerank_qrels.eval.two.tsv.txt"]
# emptying file if there
for inputFile_name in range(0,len(inputFileNames)):
    with open(outputFileNames[inputFile_name], "w") as file:
        pass  # The file is now empty

# creating the ouput
for inputFile_name in range(0,len(inputFileNames)):
    with open(inputFileNames[inputFile_name],"r") as bm25_rank_file:
        #df_query_dev=pd.read_csv("/Users/rockramsri/Documents/GitHub/Web search engine works/Project 3/queries.dev.tsv",names=["Query_id", "Query"],delimiter="\t")
        for index,document_ids in enumerate(bm25_rank_file):
            l=[]
            query_id_string= document_ids[:document_ids.index(":")].strip()
            if document_ids[document_ids.index(":")+1:].strip():
                document_id_list= document_ids[document_ids.index(":")+1:].strip().replace("\n","").split(" ")
                for doc_id in document_id_list:
                    l.append([doc_id, getSimilarityScore(query_embeddings[ np.where(query_ids == query_id_string ) ][0] ,embeddings[ np.where(ids == str(doc_id.strip())) ][0])])
                if len(l)!=0:
                    l.sort(key=lambda x:x[1],reverse=True)
            # Append list in txt file for further analsysis
            writeResultInFile(outputFileNames[inputFile_name],query_id_string,l)

 