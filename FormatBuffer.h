/*!
 * Format Buffer (the "software") provided by Anders Lind ("author") license agreements.
 * - This software is free for both personal and commercial use. You may install and use it on your computers free of charge.
 * - You may NOT modify, de-compile, disassemble or reverse engineer the software.
 * - You may use, copy, sell, redistribute or give the software to third part freely as long as the software is not modified.
 * - The software remains property of the authors also in case of dissemination to third parties.
 * - The software's name and logo are not to be used to identify other products or services.
 * - THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * - The authors reserve the rights to change the license agreements in future versions of the software
 */

#pragma once

#include <cstdint>
#include <type_traits>

template <std::size_t M>
class FormatBuffer final {
private:
    char m_buffer[M];

public:
    FormatBuffer() noexcept {
        clear();
    }

    template <typename... Types>
    FormatBuffer(const char* fmt, Types... args) noexcept {
        print(fmt, args...);
    }

    template <typename... Types>
    void print(const char* fmt, Types... args) noexcept {
        print(m_buffer, fmt, args...);
    }

    void clear() noexcept {
        m_buffer[0] = '\0';
    }

    template <std::size_t N, typename... Types>
    static void print(char (&buf)[N], const char* fmt, Types... args) noexcept {
        static_assert(N > 0);
        std::size_t i = 0;
        auto copyFromString = [&buf, &i](const char* str) noexcept -> void {
            do {
                buf[i++] = *str++;
            } while (*str != '\0' && i < (N - 1));
        };
        auto copyFromFormat = [&buf, &fmt, &i]() noexcept -> bool {
            do {
                if (*fmt == '%') {
                    ++fmt;
                    if (*fmt != '%') {
                        return true;
                    }
                }
                buf[i++] = *fmt++;
            } while (*fmt != '\0' && i < (N - 1));
            return false;
        };
        if (*fmt != '\0') {
            bool foundSpec = copyFromFormat();
            (
                [&]<typename T>(T value) noexcept -> void {
                    if (foundSpec != false && *fmt != '\0') {
                        char spec = *fmt++;
                        if constexpr (std::is_same_v<T, int64_t>) {
                            if (spec == 'b') {
                                char num[66];
                                toBase2<uint64_t>(num, value);
                                copyFromString(num);
                            } else if (spec == 'd') {
                                char num[22];
                                toBase10<int64_t>(num, value);
                                copyFromString(num);
                            } else if (spec == 'x') {
                                char num[18];
                                toBase16<uint64_t>(num, value);
                                copyFromString(num);
                            }
                        } else if constexpr (std::is_same_v<T, int32_t> ||
                                             std::is_same_v<T, int16_t> ||
                                             std::is_same_v<T, int8_t>) {
                            if (spec == 'b') {
                                char num[34];
                                toBase2<uint32_t>(num, static_cast<std::make_unsigned_t<T>>(value));
                                copyFromString(num);
                            } else if (spec == 'd') {
                                char num[16];
                                toBase10<int32_t>(num, value);
                                copyFromString(num);
                            } else if (spec == 'x') {
                                char num[10];
                                toBase16<uint32_t>(num, static_cast<std::make_unsigned_t<T>>(value));
                                copyFromString(num);
                            }
                        } else if constexpr (std::is_same_v<T, uint64_t>) {
                            if (spec == 'b') {
                                char num[66];
                                toBase2<uint64_t>(num, value);
                                copyFromString(num);
                            } else if (spec == 'u') {
                                char num[22];
                                toBase10<uint64_t>(num, value);
                                copyFromString(num);
                            } else if (spec == 'x') {
                                char num[18];
                                toBase16<uint64_t>(num, value);
                                copyFromString(num);
                            } 
                        } else if constexpr (std::is_same_v<T, uint32_t> ||
                                             std::is_same_v<T, uint16_t> ||
                                             std::is_same_v<T, uint8_t>) {
                            if (spec == 'b') {
                                char num[34];
                                toBase2<uint32_t>(num, value);
                                copyFromString(num);
                            } else if (spec == 'u') {
                                char num[16];
                                toBase10<uint32_t>(num, value);
                                copyFromString(num);
                            } else if (spec == 'x') {
                                char num[10];
                                toBase16<uint32_t>(num, value);
                                copyFromString(num);
                            }
                        } else if constexpr (std::is_same_v<T, const char*> ||
                                             std::is_same_v<T, char*>) {
                            if (spec == 's') {
                                copyFromString(value);
                            }
                        } else if constexpr (std::is_same_v<T, char>) {
                            if (spec == 'c') {
                                buf[i++] = value;
                            }
                        } else {
                            static_assert(std::is_same_v<T, void>);
                        }
                        if (*fmt != '\0' && i < (N - 1)) {
                            foundSpec = copyFromFormat();
                        }
                    }
                }(args),
                ...);
            if (*fmt != '\0') {
                if (foundSpec != false) {
                    buf[i++] = '%';
                }
                if (i < (N - 1)) {
                    copyFromString(fmt);
                }
            }
        }
        buf[i] = '\0';
    }

    const char* c_str() const noexcept {
        return m_buffer;
    }

private:
    static void reverse(char* buf, std::size_t length) noexcept {
        for (std::size_t i = 0, j = length - 1; i < j; ++i, --j) {
            char tmp = buf[i];
            buf[i] = buf[j];
            buf[j] = tmp;
        }
    }

    template <typename T>
    static void toBase2(char* buf, T value) noexcept {
        static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>);
        std::size_t i = 0;
        do {
            buf[i++] = static_cast<char>(value & 1) + '0';
            value >>= 1;
        } while (value != 0);
        reverse(buf, i);
        buf[i] = '\0';
    }

    template <typename T>
    static void toBase10(char* buf, T value) noexcept {
        static_assert(std::is_integral_v<T>);
        std::size_t i = 0;
        auto reduce = [&buf, &i](std::make_unsigned_t<T> num) -> void {
            do {
                buf[i++] = static_cast<char>(num % 10) + '0';
                num /= 10;
            } while (num != 0);
        };
        if constexpr (std::is_signed_v<T>) {
            T mask = value >> (sizeof(value) * 8 - 1);
            reduce((value + mask) ^ mask);
            buf[i] = '-';
            i += static_cast<std::size_t>(mask & 1);
        } else if constexpr (std::is_unsigned_v<T>) {
            reduce(value);
        } else {
            static_assert(std::is_same_v<T, void>);
        }
        reverse(buf, i);
        buf[i] = '\0';
    }

    template <typename T>
    static void toBase16(char* buf, T value) noexcept {
        static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>);
        const char kCharMap[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        std::size_t i = 0;
        do {
            buf[i++] = kCharMap[value & 15];
            value >>= 4;
        } while (value != 0);
        reverse(buf, i);
        buf[i] = '\0';
    }
};

template <std::size_t N, typename... Types>
void format(char (&buf)[N], const char* fmt, Types... args) noexcept {
    FormatBuffer<N>::print(buf, fmt, args...);
}
