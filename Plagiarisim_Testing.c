#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS 100
#define MAX_WORD_LEN 30 // maximum length of a word
#define MAX_LINE_LEN 1000 // maximum length of a line
#define NUM_FILES 4 // number of files to compare with main file

// a structure to store a word and its frequency
typedef struct {
    char word[MAX_WORD_LEN];
    int freq;
} word_freq;

// a function to compare two word_freq structures by frequency
int compare_by_freq(const void *a, const void *b) {
    word_freq *w1 = (word_freq *)a;
    word_freq *w2 = (word_freq *)b;
    return w2->freq - w1->freq; // descending order
}

// a function to check if a character is a delimiter
int is_delim(char c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == ',';
}

// a function to extract words from a file and store them in an array
int extract_words(char *filename, char words[][MAX_WORD_LEN]) {
    FILE *fp = fopen(filename, "r"); // open the file for reading
    if (fp == NULL) { // check if the file exists
        printf("Error: file %s not found\n", filename);
        return 0; // return 0 if the file does not exist
    }
    int count = 0; // count the number of words
    char line[MAX_LINE_LEN]; // store a line from the file
    while (fgets(line, MAX_LINE_LEN, fp) != NULL) { // read a line from the file
        int i = 0; // index of the current character in the line
        int j = 0; // index of the current character in the word
        while (line[i] != '\0') { // loop until the end of the line
            if (is_delim(line[i])) { // check if the current character is a delimiter
                if (j > 0) { // check if there is a word before the delimiter
                    words[count][j] = '\0'; // terminate the word with null character
                    count++; // increment the word count
                    j = 0; // reset the word index
                }
            } else { // otherwise, the current character is part of a word
                words[count][j] = line[i]; // copy the character to the word array
                j++; // increment the word index
            }
            i++; // increment the line index
        }
        if (j > 0) { // check if there is a word at the end of the line
            words[count][j] = '\0'; // terminate the word with null character
            count++; // increment the word count
        }
    }
    fclose(fp); // close the file
    return count; // return the number of words extracted from the file
}

// a function to find the common substring between two words 
char *find_common_substring(char *word1, char *word2) { 
    int len1 = strlen(word1); // get the length of the first word 
    int len2 = strlen(word2); // get the length of the second word 
    int min_len = len1 < len2 ? len1 : len2; // get the minimum length between the two words 
    char *common = NULL; // the common substring 
    for (int i = 0; i < min_len; i++) { // loop through all possible positions of the common substring 
        if (word1[i] == word2[i]) { // if the characters match at this position 
            if (common == NULL) { // if this is the first matching character 
                common = malloc(MAX_WORD_LEN); // allocate memory for the common substring 
                common[0] = word1[i]; // copy the character to the common substring 
                common[1] = '\0'; // terminate the common substring with null character 
            } else { // if this is not the first matching character 
                int len = strlen(common); // get the length of the common substring 
                common[len] = word1[i]; // append the character to the common substring 
                common[len + 1] = '\0'; // terminate the common substring with null character 
            }
        } else { // if the characters do not match at this position 
            break; // break out of the loop 
        } 
    }
    return common; // return the common substring
}

// a function to find the longest common substring of at least three words between two arrays of words
char *find_longest_common_substring(char words1[][MAX_WORD_LEN], int count1, char words2[][MAX_WORD_LEN], int count2, int *len) {
    char *longest = NULL; // the longest common substring 
    *len = 0; // the length of the longest common substring
    for (int i = 0; i < count1 - 2; i++){ // loop through all possible starting positions in the first array of words 
        for (int j = 0; j < count2 - 2; j++) {// loop through all possible starting positions in the second array of words 
            char *common1 = find_common_substring(words1[i], words2[j]); // find and store first common substring between two words
            char *common2 = find_common_substring(words1[i + 1], words2[j + 1]); // find and store second common substring between two words 
            char *common3 = find_common_substring(words1[i + 2], words2[j + 2]); // find and store third common substring between two words
            if (common1 != NULL && common2 != NULL && common3 != NULL) { // if all three common substrings are not null
                int common_len = strlen(common1) + strlen(common2) + strlen(common3); // get the length of the combined common substring 
                if (common_len > *len) { // if it is longer than the current longest
                    if (longest != NULL) { // free the previous longest substring
                        free(longest);
                    }
                    longest = malloc(MAX_WORD_LEN * 3); // allocate memory for the longest substring
                    sprintf(longest, "%s %s %s", common1, common2, common3); // copy the combined common substring to the longest substring
                    *len = common_len; // update the length of the longest substring
                }
            }
            if (common1 != NULL) { // free the first common substring
                free(common1);
            }
            if (common2 != NULL) { // free the second common substring 
                free(common2);
            }
            if (common3 != NULL) { // free the third common substring
                free(common3);
            }
        }
    }
    return longest;
}

// a function to split a string of words into an array of substrings 
void split_string(char *string, char substrings[][MAX_WORD_LEN], int *count) {
    char *token = strtok(string, " \n"); // get the first word from the string using space and newline as delimiters
    int i = 0; // the index of the array of substrings
    while (token != NULL) { // while there are more words in the string
        strcpy(substrings[i], token); // copy the word to the array of substrings 
        i++; // increment the index 
        token = strtok(NULL, " \n"); // get the next word from the string using space and newline as delimiters
    }
    *count = i; // set the count to the number of substrings
}

// a function to find the frequency of a string of words in an array of words
int find_frequency(char *string, char words[][MAX_WORD_LEN], int count, int string_len) {
    char substrings[MAX_WORDS][MAX_WORD_LEN]; // an array to store the substrings from the string
    int sub_count; // the number of substrings
    split_string(string, substrings, &sub_count); // split the string into substrings
    int freq = 0; // the frequency of the string
    for (int i = 0; i < count; i++) { // loop through the array of words
        char *word = words[i]; // get the current word 
        int match = 1; // a flag to indicate if the word matches the string
        for (int j = 0; j < sub_count; j++) { // loop through the array of substrings 
            char *substring = substrings[j]; // get the current substring 
            int len = strcspn(substring, " \n"); // get the length of the substring without spaces or newlines
            if (strncmp(word, substring, len) != 0) { // if the substring does not match at this position
                match = 0; // set the flag to false
                break; // break out of the inner loop
            }
            word += len + 1; // move to the next position after skipping a space or newline
        }
        if (match) { // if the flag is true 
            freq++; // increment the frequency 
        }
    }
    return freq; // return the frequency of the string
}

// a function to write the output to a file
void write_output(char *output_filename, char *main_filename, char (*other_filenames)[MAX_WORD_LEN], char (*substrings)[MAX_WORD_LEN], int freqs[]) { 
    FILE *fp = fopen(output_filename, "w"); // open the output file for writing 
    if (fp == NULL) { // check if the file can be opened
        printf("Error: file %s cannot be opened\n", output_filename); 
        return; // return if the file cannot be opened 
    }
    fprintf(fp, "Main file: %s\n", main_filename); // write the main file name to the output file 
    for (int i = 0; i < NUM_FILES; i++) { // loop through all other files 
        fprintf(fp, "Other file: %s\n", other_filenames[i]); // write the other file name to the output file 
        if (substrings[i] != NULL) { // if there is a longest common substring with this file 
            fprintf(fp, "Longest common substring: %s\n", substrings[i]); // write the longest common substring to the output file 
            fprintf(fp, "Frequency: %d\n", freqs[i]); // write the frequency of the longest common substring to the output file 
        } else { // if there is no longest common substring with this file 
            fprintf(fp, "No common substring\n"); // write no common substring to the output file 
        }
        fprintf(fp, "\n"); // write a blank line to separate each pair of files 
    }
    fclose(fp); // close the output file 
}

// a function to optimize and rewrite the main function by calling write_output function from main
int main() {
    char main_filename[MAX_WORD_LEN] = "main.txt";
    char other_filenames[NUM_FILES][MAX_WORD_LEN];
    char substrings[NUM_FILES][MAX_WORD_LEN]; // an array to store the longest common substrings with each file
    int freqs[NUM_FILES]; // an array to store the frequencies of the longest common substrings with each file
    char output_filename[MAX_WORD_LEN] = "similarity_tested.txt";

    char main_words[10000][MAX_WORD_LEN]; 
    int main_count = extract_words(main_filename, main_words); // extract the words from the main file

    for (int i = 0; i < NUM_FILES; i++) { // loop through all files to compare with main.txt
        sprintf(other_filenames[i], "text%d.txt", i + 1); // generate the current file name using the loop index
        char other_words[10000][MAX_WORD_LEN]; 
        int other_count = extract_words(other_filenames[i], other_words); // extract the words from the other file

        int len; // the length of the longest common substring
        char *substring = find_longest_common_substring(main_words, main_count, other_words, other_count, &len); // find the longest common substring

        if (substring != NULL) { // if there is a longest common substring with this file 
            strcpy(substrings[i], substring); // copy the substring to the substrings array
            freqs[i] = find_frequency(substring, other_words, other_count, len); // find the frequency of the substring and store it in the freqs array
            free(substring); // free the memory allocated for the substring
        } else { // if there is no longest common substring with this file 
            substrings[i][0] = '\0'; // set the first character of the substrings array to null terminator
            freqs[i] = 0; // set the frequency to zero
        }
    }

    write_output(output_filename, main_filename, other_filenames, substrings, freqs); // call the write_output function to write the results to a file

    return 0; 
}