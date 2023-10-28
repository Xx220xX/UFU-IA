
#ifndef T6_MLP_CPP_SHA256_H
#define T6_MLP_CPP_SHA256_H

#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

typedef unsigned char BYTE;
typedef unsigned int UI;
typedef unsigned long long ll;

class SHA256 {
private:

    std::vector<std::vector<UI>> M; // Message to be hashed
    std::vector<std::vector<UI>> H; // Hashed message
    std::vector<BYTE> bytes;          // Plain and padded message bytes
    UI W[64];                       // Message schedule
    ll l = 0;                         // Message length in bits
    int N;                            // Number of blocks in padded message

// Working variables
    UI a, b, c, d, e, f, g, h;

// Temporary words
    UI T1, T2;


/**
 * Take the given hexadecimal string and store the bytes in a global vector.
 * Also update the message length.
 */
    void store_message_bytes(const std::string &hex_str);

/**
 * Calculate the required padding of the message.
 * Return the required padding.
 */
    int calc_padding();

/**
 * Pad the message bytes according to the specification.
 */
    void pad_message();

/**
 * Parse the message into N 512-bit blocks split up into words.
 * Also update N.
 */
    void parse_message();

/**
 * Initialise the hash value H_0.
 */
    void init_hash();

/**
 * Rotate right function ROTR^n(x) in hash algorithm.
 */
    UI ROTR(const UI &n, const UI &x);

/**
 * Right shift function SHR^n(x) in hash algorithm.
 */
    UI SHR(const UI &n, const UI &x);

/**
 * Logical function Ch(x, y, z) in hash algorithm.
 */
    UI Ch(const UI &x, const UI &y, const UI &z);

/**
 * Logical function Maj(x, y, z) in hash algorithm.
 */
    UI Maj(const UI &x, const UI &y, const UI &z);

/**
 * Logical function (large) sigma^256_0(x) in hash algorithm.
 */
    UI lsigma0(const UI &x);

/**
 * Logical function (large) sigma^256_1(x) in hash algorithm.
 */
    UI lsigma1(const UI &x);

/**
 * Logical function (small) sigma^256_0(x) in hash algorithm.
 */
    UI ssigma0(const UI &x);

/**
 * Logical function (small) sigma^256_1(x) in hash algorithm.
 */
    UI ssigma1(const UI &x);

/**
 * Compute the hash value.
 */
    void compute_hash();

/**
 * Output the generated hash value as a hexadecimal string.
 */
    std::string output_hash();

/**
 * Clear all working vectors and variables.
 */
    void clear();

public:

    std::string hash(std::string &message);;

    ~SHA256();
};
std::string stringToHex(const std::string &input);
std::string hexToString(const std::string &hex);

#endif //T6_MLP_CPP_SHA256_H
