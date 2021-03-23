#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

/* Defines for array allocation, change ORIGINAL_WORD_SIZE, SPACES_BETWEEN_FIRST and SAFETY_NUM
 * as length for ORIGINAL_WORD_SIZE and number of calls as SAFETY_NUM, the rest are by the table to see the changes.
 * ORIGINAL_WORD_SIZE    16  32  64  128 256 512 1024    2048    4096    8192    16384   32768   65536   131072  262144  524288  1048576
 * SPACES_BETWEEN_FIRST  2   3   3   3   4   4   4       4       4       5       5       5       5       5       6       6       6
 *
 * The values written above are the maximum redundancy and the minimal window size, for less redundancy by the price of
 * window size, see the table below, higher values -> smaller redundancy -> bigger window size.
 * NOTE - The mentioned values above are the lowest possible values for each word size, you can only increase them.
 *        (lower values will cause the next step of the algorithm, the building of the word from all of its windows to
 *        be unsuccessful)
 * table:
 * SPACES_BETWEEN_WORDS     2,3   3,4   4,5   5,6   6,7   7,8   8,9   9,10   10,11  11,12  12,13  13,14  14,15  15,16
 * ORIGINAL_WINDOW_SIZE     8     15    24    35    48    63    80    99     120    143    168    195    224    255
 * ENCCODED_WINDOW_SIZE     12    20    30    42    56    72    90    110    132    156    182    210    240    272
 */

#define ORIGINAL_WORD_SIZE 1048576
#define SPACES_BETWEEN_FIRST 13
#define SAFETY_NUM 3


#define SPACES_BETWEEN_SECOND (SPACES_BETWEEN_FIRST + 1)
#define ENCODED_WINDOW_SIZE ((SPACES_BETWEEN_FIRST + 1) * (SPACES_BETWEEN_SECOND + 1))
#define ORIGINAL_WINDOW_SIZE_FIRST (ENCODED_WINDOW_SIZE - (ENCODED_WINDOW_SIZE/(SPACES_BETWEEN_FIRST + 1)))
#define ORIGINAL_WINDOW_SIZE_SECOND (ENCODED_WINDOW_SIZE - (ENCODED_WINDOW_SIZE/(SPACES_BETWEEN_SECOND + 1)))
#define ENCODED_WORD_SIZE_FIRST (ORIGINAL_WORD_SIZE + (ORIGINAL_WORD_SIZE/SPACES_BETWEEN_FIRST))
#define ENCODED_WORD_SIZE_SECOND (ORIGINAL_WORD_SIZE + (ORIGINAL_WORD_SIZE/SPACES_BETWEEN_SECOND))
#define ORIGINAL_WINDOW_AMOUNT_FIRST (ORIGINAL_WORD_SIZE - ORIGINAL_WINDOW_SIZE_FIRST + 1)
#define ORIGINAL_WINDOW_AMOUNT_SECOND (ORIGINAL_WORD_SIZE - ORIGINAL_WINDOW_SIZE_SECOND + 1)
#define ENCODED_WINDOW_AMOUNT_FIRST ((ENCODED_WORD_SIZE_FIRST - ENCODED_WINDOW_SIZE + 1) * SAFETY_NUM)
#define ENCODED_WINDOW_AMOUNT_SECOND ((ENCODED_WORD_SIZE_SECOND - ENCODED_WINDOW_SIZE + 1) * SAFETY_NUM)


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
        if (i % SPACES_BETWEEN_FIRST == 0) {
            encoded_words[0] += "0";
        }
        encoded_words[0] += original_words[0][i];

        if (i % SPACES_BETWEEN_SECOND == 0) {
            encoded_words[1] += "1";
        }
        encoded_words[1] += original_words[1][i];
    }
}


/* Description: Fills an array on max size (the amount of windows in an encoded word)
 *              with windows at random locations from one of the encoded words at random.
 * Input: The encoded words, the array of windows to fill.
 * Return Value: Void.
 * */
void makeWindows(string *words, string *windows) {
    int part, start, pick_word, pos;
    int for_cond = ENCODED_WINDOW_AMOUNT_FIRST + ENCODED_WINDOW_AMOUNT_SECOND;
    int encoded_window_size = ENCODED_WINDOW_SIZE;

    bool first_if1_cond = (ENCODED_WORD_SIZE_FIRST - ENCODED_WINDOW_SIZE + 1) > RAND_MAX;
    int first_if2_cond = (ENCODED_WORD_SIZE_FIRST / RAND_MAX) - 1;
    int first_partition = ENCODED_WORD_SIZE_FIRST / RAND_MAX;
    int first_last_part = RAND_MAX - ENCODED_WINDOW_SIZE + 1;
    int first_margins = (ENCODED_WORD_SIZE_FIRST - ENCODED_WINDOW_SIZE + 1);

    bool second_if1_cond = (ENCODED_WORD_SIZE_SECOND - ENCODED_WINDOW_SIZE + 1) > RAND_MAX;
    int second_if2_cond = (ENCODED_WORD_SIZE_SECOND / RAND_MAX) - 1;
    int second_partition = ENCODED_WORD_SIZE_SECOND / RAND_MAX;
    int second_last_part = RAND_MAX - ENCODED_WINDOW_SIZE + 1;
    int second_margins = (ENCODED_WORD_SIZE_SECOND - ENCODED_WINDOW_SIZE + 1);

    for (int i = 0; i < for_cond; i++) {
        part = 0;
        start = rand();
        pick_word = rand() % 2;
        if (pick_word == 0) {
            if (first_if1_cond) {
                part = rand() % first_partition;
                if (part == first_if2_cond) {
                    start = start % first_last_part;
                }
            } else
                start = start % first_margins;
        } else {
            if (second_if1_cond) {
                part = rand() % second_partition;
                if (part == second_if2_cond) {
                    start = start % second_last_part;
                }
            } else
                start = start % second_margins;
        }
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
    int location = -1;
    bool found_one = false;
    int jumps = SPACES_BETWEEN_FIRST + 1;
    for (i = 0; i < jumps; i++) {
        for (int j = i; window[j] == '0'; j += jumps) {
            if (j + jumps >= cond) {
                if (found_one) {
                    return "bad string";
                }
                location = i % jumps;
                found_one = true;
                break;
            }
        }
    }

    string res;
    for (int j = 0; j < cond; j++) {
        if (j % jumps == location) {
            continue;
        }
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
    int location = -1;
    bool found_one = false;
    int jumps = SPACES_BETWEEN_SECOND + 1;
    for (i = 0; i < jumps; i++) {
        for (int j = i; window[j] == '1'; j += jumps) {
            if (j + jumps >= cond) {
                if (found_one) {
                    return "bad string";
                }
                location = i % jumps;
                found_one = true;
                break;
            }
        }
    }

    string res;
    for (int j = 0; j < cond; j++) {
        if (j % jumps == location) {
            continue;
        }
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

    int cond = ENCODED_WINDOW_SIZE;
    int i;
    int jumps_first = SPACES_BETWEEN_FIRST + 1;
    int jumps_second = SPACES_BETWEEN_SECOND + 1;
    for (i = 0; i < jumps_second; i++) {
        for (int j = i; encoded_windows[j] == '1'; j += jumps_second) {
            if (j + jumps_second >= cond) {
                *from_second = true;
                break;
            }
        }
        if (i == jumps_first) break;
        for (int j = i; encoded_windows[j] == '0'; j += jumps_first) {
            if (j + jumps_first >= cond) {
                *from_first = true;
                break;
            }
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
 * Input: The array of windows, the none-duplication array to fill, and the quantity array to fill, the final int "second"
 *        is to determine if we are in the first or second word, as it effects the calculation.
 * Return Value: The true size of both the none-duplication array and the quantity array.(the arrays are bigger,
 *               but the rest beyond the size measured is not needed.
 * */
int quantifyArray(string *sorted_original_windows, string *sorted_original_windows_nd, int *quantity, int second) {
    int counter;
    int size = 0;
    int cond = ORIGINAL_WINDOW_AMOUNT_FIRST - second;
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
    int cond = ENCODED_WINDOW_AMOUNT_FIRST + ENCODED_WINDOW_AMOUNT_SECOND;

    static string original_first_windows[ORIGINAL_WINDOW_AMOUNT_FIRST];
    static string original_second_windows[ORIGINAL_WINDOW_AMOUNT_SECOND];
    static string original_first_windows_nd[ORIGINAL_WINDOW_AMOUNT_FIRST];
    static string original_second_windows_nd[ORIGINAL_WINDOW_AMOUNT_SECOND];
    static int quantity_first[ORIGINAL_WINDOW_AMOUNT_FIRST] = {0};
    static int quantity_second[ORIGINAL_WINDOW_AMOUNT_SECOND] = {0};
    int first_size_nd = 0;
    int second_size_nd = 0;
    int easy_cond = ORIGINAL_WINDOW_AMOUNT_FIRST;
    int orignal_window_size_first = ORIGINAL_WINDOW_SIZE_FIRST;
    int orignal_window_size_second = ORIGINAL_WINDOW_SIZE_SECOND;

    for (int i = 0; i < easy_cond; ++i) {
        original_first_windows[i] = original_words[0].substr(i, orignal_window_size_first);
        if (i == easy_cond - 1) break;
        original_second_windows[i] = original_words[1].substr(i, orignal_window_size_second);
    }

    int array_size1 = sizeof(original_first_windows) / sizeof(original_first_windows[0]);
    int array_size2 = sizeof(original_second_windows) / sizeof(original_second_windows[0]);

    sort(original_first_windows, original_first_windows + array_size1);
    sort(original_second_windows, original_second_windows + array_size2);
    first_size_nd = quantifyArray(original_first_windows, original_first_windows_nd, quantity_first, 0);
    second_size_nd = quantifyArray(original_second_windows, original_second_windows_nd, quantity_second, 1);
    for (int i = 0; i < cond; i++) {
        checkWindowOrigin(encoded_windows[i], &from_first, &from_second);
        if (from_first) {
            first = decodeFromFirst(encoded_windows[i]);
            if (first == "bad string"){
                continue;
            }

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
            if (second == "bad string"){
                continue;
            }

            int location = binarySearch(original_second_windows_nd, 0, second_size_nd - 1, &second);
            if (location == -1) {
                // problem - there is a window that wasn't in the original word, should not be possible
                cout << "We found a window that wasn't in the original second word" << endl;
                cout << second << endl;
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

/* Description: Using the funcs above will generate random words, encode them, cut windows at random from the encoded
 * words, then try and organize the encoded windows into two groups according to the origin of the decoded window.
 * The length of the words, windows and the amount of windows read is controlled by the DEFINEs at the top of the file.
 * */
int main() {
    double stats[2] = {0};
    time_t sum = time(nullptr);
    int simultaion = 20;
    for (int i = 0; i < simultaion; ++i) {
        static string decoded_windows_from_first[ENCODED_WINDOW_AMOUNT_FIRST + ENCODED_WINDOW_AMOUNT_SECOND];
        static string decoded_windows_from_second[ENCODED_WINDOW_AMOUNT_FIRST + ENCODED_WINDOW_AMOUNT_SECOND];

        static string original_words[2];
        makeWords(original_words);

        static string encoded_words[2];
        encoder(original_words, encoded_words);

        static string encoded_windows[ENCODED_WINDOW_AMOUNT_FIRST + ENCODED_WINDOW_AMOUNT_SECOND];
        makeWindows(encoded_words, encoded_windows);

        int counter[2] = {0};
        distributeWindowsToOrigin(original_words, encoded_windows, counter, decoded_windows_from_first,
                                  decoded_windows_from_second);

        stats[0] += 1.0 - ((double) counter[0] / ((double) ORIGINAL_WINDOW_AMOUNT_FIRST / 1.0));
        stats[1] += 1.0 - ((double) counter[1] / ((double) ORIGINAL_WINDOW_AMOUNT_SECOND / 1.0));
    }
    time_t alltime = time(nullptr);

    cout << "The time for " << simultaion << " runs is - " << alltime - sum << endl;
    cout << endl << "The failure rate for the first word for " << simultaion << " runs is - "
         << stats[0] / (double) simultaion << endl;
    cout << endl << "The failure rate for the second word for " << simultaion << " runs is - "
         << stats[1] / (double) simultaion << endl;
    cout << "average is - " << (stats[0]+ stats[1])/ (simultaion*2) << endl;

    return 0;
}
