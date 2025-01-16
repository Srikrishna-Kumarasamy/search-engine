
# Search Engine (Inverted Index)

## Introduction

This project provides a program to create and query an inverted index, a critical data structure in information retrieval that maps terms to documents containing them. Using efficient data processing, optimized sorting, and compression, the program supports fast and accurate document retrieval.

The search engine includes:
- An index creation module
- Query ranking using the BM25 algorithm
- A user interface for querying through a browser

## Features

1. **Index Formation**: Builds an inverted index from a set of input documents.
2. **Efficient Sorting and Compression**: Uses heap sort and VarByte encoding for optimized data storage and retrieval.
3. **Query Handling**: Processes user queries with BM25 ranking, returning the top-ranked documents by relevance.

## Requirements

- **Operating System**: Linux or Windows with a C++ compiler (GCC 7.5 or higher recommended).
- **Libraries**: Standard C++ library, optional multithreading library for large datasets.
- **Dataset**: Directory of text files for indexing.
- **File Permissions**: Ensure read and write permissions for input, intermediate, and output directories.

## Installation and Setup

1. **Prepare Input Files**: Store the text files to be indexed in a directory.
2. **Compile**: Navigate to the project directory and compile the files:
   ```bash
   cd search-engine
   g++ -o form_inverted form_inverted.cpp
   g++ -o io_efficient_merge_sort io_efficient_merge_sort.cpp
   g++ -o query query.cpp
   ```
3. **Run the Program**:
   - **Create Index**: `./form_inverted`
   - **Merge and Compress**: `./io_efficient_merge_sort`
   - **Query**: `./query "your search terms"`

## User Interface (Optional)

1. Compile the query program with `g++ -std=c++20 query.cpp -o query`.
2. Start a PHP server on localhost: `php -S localhost:8000`.
3. Access the UI at `http://localhost:8000`.

## Internal Mechanics

### Modules

1. **form_inverted.cpp**: Parses documents to create the intermediate inverted index.
2. **io_efficient_merge_sort.cpp**: Merges and compresses the index, saving it in binary format.
3. **query.cpp**: Processes queries and ranks documents using BM25.

## Performance

The program is designed for efficient indexing and querying with metrics available for runtime analysis, file size estimation, and query processing speed.

## Limitations

- **Memory Constraints**: Large datasets may impact performance.
- **Index File Size**: High document volume can lead to substantial index files.
- **Character Constraints**: Terms containing semicolons are ignored.

## Future Directions

Potential improvements include enhanced ranking using deep learning, parallel processing, and advanced compression techniques.

## Example Usage

```bash
./query "sample search terms"
```

Expected output: A ranked list of document IDs based on BM25 relevance scores.
