# Concurrency-Programing
Multi threaded program to perform string search on a file using POSIX thread library

Develop a program using POSIX threads to find the number of occurrences of an input search string in a given new-line delimited source. Program operates as follows:

string_search "searchString" < inputFile

The two parameters are an input file with the source and the string to search the source for. Assumed that search strings will be no longer than 80 characters. Searches should be parallelized to threads where each thread operates on a different data block (not necessarily terminated by a newline).The number of matches per block (partial result of a thread) may be stored in a global array that is aggregated in the main thread (after joining with other threads) to produce a final result number. Plotted a running time while using different numbers of threads and block sizes in order to find an optimal run configuration on a Linux machine.
Search strings may extend past data block boundaries, accounted for this.
