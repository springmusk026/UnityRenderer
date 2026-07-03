#pragma once

// =============================================================================
// Compile-time string obfuscation (XOR-based)
// Provides the YK() macro used throughout the IL2CPP helper.
//
// In release builds this encrypts string literals at compile time and decrypts
// them at runtime, making strings harder to find with static analysis tools.
// In debug builds it's a passthrough for easier debugging.
// =============================================================================

#ifdef NDEBUG

namespace obfuscate {

    constexpr char KEY = 0x5A;

    template<int N>
    struct ObfuscatedString {
        char data[N];

        constexpr ObfuscatedString(const char (&str)[N]) : data{} {
            for (int i = 0; i < N; i++) {
                data[i] = str[i] ^ KEY;
            }
        }
    };

    template<int N>
    class Decryptor {
    public:
        constexpr Decryptor(const ObfuscatedString<N>& obs) : encrypted{} {
            for (int i = 0; i < N; i++) {
                encrypted[i] = obs.data[i];
            }
        }

        const char* decrypt() const {
            // Lazy decrypt into mutable buffer
            if (!decrypted[0] && encrypted[0]) {
                for (int i = 0; i < N; i++) {
                    decrypted[i] = encrypted[i] ^ KEY;
                }
            } else if (!encrypted[0]) {
                // Handle empty string / null terminator case
                for (int i = 0; i < N; i++) {
                    decrypted[i] = encrypted[i] ^ KEY;
                }
            }
            return decrypted;
        }

        operator const char*() const {
            return decrypt();
        }

    private:
        char encrypted[N];
        mutable char decrypted[N]{};
    };

} // namespace obfuscate

#define YK(str) ([]() -> const char* { \
    constexpr static auto obs = obfuscate::ObfuscatedString(str); \
    static obfuscate::Decryptor dec(obs); \
    return dec.decrypt(); \
}())

#else

// Debug mode: passthrough, no obfuscation
#define YK(str) str

#endif
