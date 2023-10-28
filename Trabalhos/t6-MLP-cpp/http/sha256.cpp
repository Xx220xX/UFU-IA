

#include "sha256.h"
/**
 * SHA-256 implemented according to the specification:
 * http://csrc.nist.gov/publications/fips/fips180-4/fips-180-4.pdf
 */


// Constants used in hash algorithm
const UI K[] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
                0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
                0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
                0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
                0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
                0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};


//int main() {
//    // Read each line as a hexadecimal string to be hashed
//
//    for (std::string line; std::getline(std::cin, line);) {
//        // Store the plain bytes of the message
//    }
//}
void SHA256::store_message_bytes(const std::string &hex_str) {
    int str_length = hex_str.length();
    for (int i = 0, j = 0; i < str_length / 2; ++i, j += 2) {
        BYTE byte = std::stoi(hex_str.substr(j, 2), nullptr, 16);
        bytes.push_back(byte);
        l += 8;
    }
}

int SHA256::calc_padding() {
    int k = 0;
    while ((l + 1 + k) % 512 != 448) ++k;
    return k;
}

void SHA256::pad_message() {
    int k = calc_padding();
    // Append 1 followed by zeroes before the least significant bit,
    // assuming full bytes as input
    bytes.push_back(0x80);
    k = k - 7;

    // Append the remaining zeroes of k
    for (int i = 0; i < (k / 8); ++i) {
        bytes.push_back(0);
    }

    // Finally append the length in binary to the message as the least
    // significant bits, assuming a 64-bit number
    for (int i = 1; i < 9; ++i) {
        bytes.push_back(l >> (64 - i * 8));
    }
}

void SHA256::parse_message() {
    UI n = 0;
    for (int i = 0; n < bytes.size() / 64; ++n) {
        std::vector<UI> block(16);
        for (int j = 0; j < 16; ++j) {
            UI word = 0;
            for (int k = 0; k < 4; ++k, ++i) {
                word <<= 8;
                word |= bytes[i];
            }
            block[j] = word;
        }
        M.push_back(block);
    }
    N = n;
}

void SHA256::init_hash() {
    std::vector<UI> h0 = {0x6a09e667,
                          0xbb67ae85,
                          0x3c6ef372,
                          0xa54ff53a,
                          0x510e527f,
                          0x9b05688c,
                          0x1f83d9ab,
                          0x5be0cd19};
    H.push_back(h0);
}

UI SHA256::ROTR(const UI &n, const UI &x) {
    return (x >> n) | (x << (32 - n));
}

UI SHA256::SHR(const UI &n, const UI &x) {
    return x >> n;
}

UI SHA256::Ch(const UI &x, const UI &y, const UI &z) {
    return (x & y) ^ (~x & z);
}

UI SHA256::Maj(const UI &x, const UI &y, const UI &z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

UI SHA256::lsigma0(const UI &x) {
    return ROTR(2, x) ^ ROTR(13, x) ^ ROTR(22, x);
}

UI SHA256::lsigma1(const UI &x) {
    return ROTR(6, x) ^ ROTR(11, x) ^ ROTR(25, x);
}

UI SHA256::ssigma0(const UI &x) {
    return ROTR(7, x) ^ ROTR(18, x) ^ SHR(3, x);
}

UI SHA256::ssigma1(const UI &x) {
    return ROTR(17, x) ^ ROTR(19, x) ^ SHR(10, x);
}

void SHA256::compute_hash() {
    std::vector<UI> hash_block(8);
    for (int i = 1; i <= N; ++i) {
        // Prepare message schedule
        for (int t = 0; t <= 15; ++t)
            W[t] = M[i - 1][t]; // M^i in spec
        for (int t = 16; t <= 63; ++t)
            W[t] = ssigma1(W[t - 2]) + W[t - 7] + ssigma0(W[t - 15]) + W[t - 16];

        // Initialise working variables with previous hash value
        a = H[i - 1][0];
        b = H[i - 1][1];
        c = H[i - 1][2];
        d = H[i - 1][3];
        e = H[i - 1][4];
        f = H[i - 1][5];
        g = H[i - 1][6];
        h = H[i - 1][7];

        // Perform logical operations
        for (int t = 0; t <= 63; ++t) {
            T1 = h + lsigma1(e) + Ch(e, f, g) + K[t] + W[t];
            T2 = lsigma0(a) + Maj(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;
        }

        // Compute intermediate hash values by assigning them to H^i
        hash_block[0] = a + H[i - 1][0];
        hash_block[1] = b + H[i - 1][1];
        hash_block[2] = c + H[i - 1][2];
        hash_block[3] = d + H[i - 1][3];
        hash_block[4] = e + H[i - 1][4];
        hash_block[5] = f + H[i - 1][5];
        hash_block[6] = g + H[i - 1][6];
        hash_block[7] = h + H[i - 1][7];
        H.push_back(hash_block);
    }
}

std::string SHA256::output_hash() {
    std::ostringstream out;
    // Concatenate the final hash blocks
    for (int i = 0; i < 8; ++i)
        out << std::hex << std::setw(8) << std::setfill('0') << H[N][i];
    return out.str();
}

void SHA256::clear() {
    M.clear();
    H.clear();
    bytes.clear();
    l = 0;
}

std::string SHA256::hash(std::string &message) {

    std::string hexmessage = stringToHex(message);
    store_message_bytes(hexmessage);

    // Pad the bytes (if necessary)
    pad_message();

    // Parse message into word blocks
    parse_message();

    // Set the inital hash value
    init_hash();
    // Compute the hash value
    compute_hash();
    // Output the generated hash value
    std::string out = output_hash();
    // Reset message to hash a new one
    clear();
    return out;
}

std::string stringToHex(const std::string &input) {
    std::ostringstream ss;
    for (unsigned char ch: input) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ch);
    }
    return ss.str();
}

std::string hexToString(const std::string &hex) {
    if (hex.length() % 2 != 0) {
        throw std::invalid_argument("A string hexadecimal deve ter um número par de dígitos.");
    }

    std::string result;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        char byte = static_cast<char>(std::stoul(byteString, nullptr, 16));
        result.push_back(byte);
    }

    return result;
}

SHA256::~SHA256() {
    clear();
}
