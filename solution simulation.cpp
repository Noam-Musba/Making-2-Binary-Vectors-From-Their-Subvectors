#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// Defines for array allocation, change ORIGINAL_WORD_SIZE, ORIGINAL_WINDOW_SIZE and SAFETY_NUM
// as length, 2log(length) and number of calls to see the changes.
#define ORIGINAL_WORD_SIZE 16384
//length    8   16  32  64  128 256 512 1024    2048    4096    8192    16384   32768   65536   131072  262144  524288  1048576
// log      6   8   10  12  14  16  18  20      22      24      26      28      30      32      34      36      38      40
#define ORIGINAL_WINDOW_SIZE 28
#define SAFETY_NUM 3
#define ENCODED_WORD_SIZE (ORIGINAL_WORD_SIZE * 2)
#define ENCODED_WINDOW_SIZE (ORIGINAL_WINDOW_SIZE * 2)
#define ORIGINAL_WINDOW_AMOUNT ((ORIGINAL_WORD_SIZE - ORIGINAL_WINDOW_SIZE + 1) * 2)
#define ENCODED_WINDOW_AMOUNT ((ENCODED_WORD_SIZE - ENCODED_WINDOW_SIZE + 1) * 2 * SAFETY_NUM)



using std::string;
using std::to_string;
using std::cout;
using std::endl;

/* Description: The function randomly generates two words from {0,1} in ORIGINAL_WORD_SIZE length.
 * Input: String array of two words to fill.
 * Return Value: Void.
 * */
void makeWords(string *words) {
    srand(time(nullptr));
    for (int i = 0; i < ORIGINAL_WORD_SIZE; i++) {
        words[0] += to_string((rand() % 2));
        words[1] += to_string((rand() % 2));
    }
}

/* Description: Encode the two words by adding '0'/'1' before every bit in the original first and second word respectively.
 * Input: The two original words, two empty words to be filled with padding.
 * Return Value: Void.
 * */
void encoder(string *original_words, string *encoded_words) {
    for (int i = 0; i < ORIGINAL_WORD_SIZE; i++) {
        encoded_words[0] += "0";
        encoded_words[0] += original_words[0][i];

        encoded_words[1] += "1";
        encoded_words[1] += original_words[1][i];
    }
}


/* Description: Fills an array on max size (the amount of windows in an encoded word)
 *              with windows at random locations from one of the encoded words at random.
 * Input: The encoded words, the array of windows to fill.
 * Return Value: Void.
 * */
void makeWindows(string *words, string *windows) {
    int for_cond = ENCODED_WINDOW_AMOUNT;
    int encoded_window_size = ENCODED_WINDOW_SIZE;
    bool if1_cond = (ENCODED_WORD_SIZE - ENCODED_WINDOW_SIZE + 1) > RAND_MAX;
    int if2_cond = (ENCODED_WORD_SIZE / RAND_MAX) - 1;
    int margins = (ENCODED_WORD_SIZE - ENCODED_WINDOW_SIZE + 1);
    int partition = ENCODED_WORD_SIZE / RAND_MAX;
    int last_part = RAND_MAX - ENCODED_WINDOW_SIZE + 1;
    int part, start, pick_word, pos;

    for (int i = 0; i < for_cond; i++) {
        part = 0;
        start = rand();
        if (if1_cond) {
            part = rand() % partition;
            if (part == if2_cond) {
                start = start % last_part;
            } else {
                start = start % RAND_MAX;
            }
        } else {
            start = start % margins; //the margins is for implementation reasons
        }
        pick_word = rand() % 2;
        pos = (part * RAND_MAX) + start;
        windows[i] = words[pick_word].substr(pos, encoded_window_size);
    }
}

/* Description: Decode the window, provided it came from the first word.
 * Input: The window to decode.
 * Return Value: The decoded window.
 * */
string decodeFromFirst(string window) {
    int i;
    int cond = ENCODED_WINDOW_SIZE;
    for (i = 0; i < cond; i++) {
        if (window[i] == '1') {
            break;
        }
    }

    string res;
    for (int j = (i % 2); j < cond; j += 2) {
        res += window[j];
    }

    return res;
}


/* Description: Decode the window, provided it came from the second word.
 * Input: The window to decode.
 * Return Value: The decoded window.
 * */
string decodeFromSecond(string window) {
    int i;
    int cond = ENCODED_WINDOW_SIZE;
    for (i = 0; i < cond; i++) {
        if (window[i] == '0') {
            break;
        }
    }

    string res;
    for (int j = (i % 2); j < cond; j += 2) {
        res += window[j];
    }

    return res;
}

/* Description: Check and mark if the encoded window either came from one of the two words, or if it can not say for sure.
 * Input: The encoded window, two bools to mark the origin of the window.
 * Return Value: Void.
 * */
void checkWindowOrigin(string encoded_windows, bool *from_first, bool *from_second) {
    *from_first = false;
    *from_second = false;

    int cond = ENCODED_WINDOW_SIZE - 1;
    for (int i = 0; i < cond; i++) {
        if (encoded_windows[i] == '0' && encoded_windows[i + 1] == '0') {
            *from_first = true;
        } else if (encoded_windows[i] == '1' && encoded_windows[i + 1] == '1') {
            *from_second = true;
        }
    }

    if ((*from_first) && (*from_second)) {
        cout << "Found a window that came from neither one - " << encoded_windows << endl;
        exit(0);
    }

    if ((!(*from_first)) && (!(*from_second))) {
        //We cant say for sure from where it came from
    }
}

/* Description: Standard binary search in string array, no modifications, recursion version.
 * Input: The window_array that contains all of the windows from the original word,
 *        the start and end of the array, the window to look for.
 * Return Value: The location of the window in the window_array.
 * */
int binarySearch(string *windows_array, int smallest, int largest, const string *window) {
    if (largest >= smallest) {
        int mid = smallest + (largest - smallest) / 2;

        if (windows_array[mid] == *window) {
            return mid;
        }
        if (windows_array[mid] > *window) {
            return binarySearch(windows_array, smallest, mid - 1, window);
        }
        return binarySearch(windows_array, mid + 1, largest, window);
    }
    return -1;
}

/* Description: Takes the first array (already sorted), and make a copy of it into the second array without duplicates,
 *              saves the quantity of each window in a parallel int array.
 * Input: The array of windows, the none-duplication array to fill, and the quantity array to fill.
 * Return Value: The true size of both the none-duplication array and the quantity array.(the arrays are bigger,
 *               but the rest beyond the size measured is not needed.
 * */
int quantifyArray(string *sorted_original_windows, string *sorted_original_windows_nd, int *quantity) {
    int counter;
    int size = 0;
    int cond = ORIGINAL_WINDOW_AMOUNT / 2;
    for (int i = 0; i < cond; i++) {
        if (i == cond - 1) {
            sorted_original_windows_nd[size] = sorted_original_windows[i];
            quantity[size] = 1;
            size += 1;
            break;
        }

        counter = 1;
        while (sorted_original_windows[i] == sorted_original_windows[i + 1]) {
            counter++;
            i++;
            if (i >= cond - 1) {
                break;
            }
        }
        sorted_original_windows_nd[size] = sorted_original_windows[i];
        quantity[size] = counter;
        size += 1;
    }
    return size;
}

/* Description: Distribute and decode the decoded windows into two groups according to word of origin, and count the
 *              size of each of these groups.
 * Input: The original words, the encoded windows, two counters for each group size.
 * Return Value: Void.
 * */
void distributeWindowsToOrigin(string *original_words, string *encoded_windows, int *counter,
                               string *decoded_windows_from_first,
                               string *decoded_windows_from_second) {

    string first;
    string second;
    bool from_first;
    bool from_second;
    int cond = ENCODED_WINDOW_AMOUNT;

    static string original_first_windows[ORIGINAL_WINDOW_AMOUNT / 2];
    static string original_second_windows[ORIGINAL_WINDOW_AMOUNT / 2];
    static string original_first_windows_nd[ORIGINAL_WINDOW_AMOUNT / 2];
    static string original_second_windows_nd[ORIGINAL_WINDOW_AMOUNT / 2];
    static int quantity_first[ORIGINAL_WINDOW_AMOUNT / 2] = {0};
    static int quantity_second[ORIGINAL_WINDOW_AMOUNT / 2] = {0};
    int first_size_nd = 0;
    int second_size_nd = 0;
    int easy_cond = ORIGINAL_WINDOW_AMOUNT / 2;
    int orignal_window_size = ORIGINAL_WINDOW_SIZE;

    for (int i = 0; i < easy_cond; ++i) {
        original_first_windows[i] = original_words[0].substr(i, orignal_window_size);
        original_second_windows[i] = original_words[1].substr(i, orignal_window_size);
    }

    int array_size1 = sizeof(original_first_windows) / sizeof(original_first_windows[0]);
    int array_size2 = sizeof(original_second_windows) / sizeof(original_second_windows[0]);

    sort(original_first_windows, original_first_windows + array_size1);
    sort(original_second_windows, original_second_windows + array_size2);
    first_size_nd = quantifyArray(original_first_windows, original_first_windows_nd, quantity_first);
    second_size_nd = quantifyArray(original_second_windows, original_second_windows_nd, quantity_second);

    for (int i = 0; i < cond; i++) {
        checkWindowOrigin(encoded_windows[i], &from_first, &from_second);
        if (from_first) {
            first = decodeFromFirst(encoded_windows[i]);

            int location = binarySearch(original_first_windows_nd, 0, first_size_nd - 1, &first);
            if (location == -1) {
                // problem - there is a window that wasn't in the original word, should not be possible
                cout << "We found a window that wasn't in the original first word" << endl;
                cout << first << endl;
                exit(0);
            }

            if (quantity_first[location] > 0 && location < first_size_nd) {
                decoded_windows_from_first[counter[0]] = first;
                counter[0] += 1;
                quantity_first[location] -= 1;
                continue;
            }
            continue;
        }
        if (from_second) {
            second = decodeFromSecond(encoded_windows[i]);
            int location = binarySearch(original_second_windows_nd, 0, second_size_nd - 1, &second);
            if (location == -1) {
                // problem - there is a window that wasn't in the original word, should not be possible
                cout << "We found a window that wasn't in the original second word" << endl;
                exit(0);
            }
            if (quantity_second[location] > 0 && location < second_size_nd) {
                decoded_windows_from_second[counter[1]] = second;
                counter[1] += 1;
                quantity_second[location] -= 1;
                continue;
            }
        }
    }
}

/* Description: Calculate and print the failure rate of both words for personal use for individual simulation.
 * Input: The amount of windows found in the first and second word.
 * Return Value: Void.
 * */
/*
void printResults(const int * counter){

    cout <<"The amount of windows found in first are - " << counter[0] << endl;
    cout <<  "The failure rate of the first word is - " << 1.0 - ((double)counter[0]/((double)ORIGINAL_WINDOW_AMOUNT / 2.0)) << endl;
    cout <<"The amount of windows found in second are- " << counter[1] << endl;
    cout <<  "The failure rate of the second word is - " << 1.0 - ((double)counter[1]/((double)ORIGINAL_WINDOW_AMOUNT / 2.0)) << endl;
}*/

/* Description: Using the funcs above will generate random words, encode them, cut windows at random from the encoded
 * words, then try and organize the encoded windows into two groups according to the origin of the decoded window.
 * The length of the words, windows and the amount of windows read is controlled by the DEFINEs at the top of the file.
 * */
int main() {
    double stats[2] = {0};
    time_t sum = time(nullptr);
    int simultaion = 100;
    for (int i = 0; i < simultaion; ++i) {
        static string decoded_windows_from_first[ENCODED_WINDOW_AMOUNT];
        static string decoded_windows_from_second[ENCODED_WINDOW_AMOUNT];

        static string original_words[2];
        makeWords(original_words);

        static string encoded_words[2];
        encoder(original_words, encoded_words);

        static string encoded_windows[ENCODED_WINDOW_AMOUNT];
        makeWindows(encoded_words, encoded_windows);

        int counter[2] = {0};
        distributeWindowsToOrigin(original_words, encoded_windows, counter, decoded_windows_from_first,
                                  decoded_windows_from_second);

        stats[0] += 1.0 - ((double) counter[0] / ((double) ORIGINAL_WINDOW_AMOUNT / 2.0));
        stats[1] += 1.0 - ((double) counter[1] / ((double) ORIGINAL_WINDOW_AMOUNT / 2.0));
    }
    time_t alltime = time(nullptr);

    cout << "The time for " << simultaion << " runs is - " << alltime - sum << endl;
    cout << endl << "The failure rate for the first word for " << simultaion << " runs is - "
         << stats[0] / (double) simultaion << endl;
    cout << endl << "The failure rate for the second word for " << simultaion << " runs is - "
         << stats[1] / (double) simultaion << endl;

    return 0;
}
