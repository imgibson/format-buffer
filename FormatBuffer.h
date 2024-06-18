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

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <tchar.h>
    #undef WIN32_LEAN_AND_MEAN
#else
    #include <windows.h>
    #include <tchar.h>
#endif

#include <type_traits>

template <SIZE_T M>
class FormatBuffer final {
private:
    TCHAR m_buffer[M];

    static constexpr TCHAR kCharMap[16] = { _T('0'), _T('1'), _T('2'), _T('3'), _T('4'), _T('5'), _T('6'), _T('7'), _T('8'), _T('9'), _T('a'), _T('b'), _T('c'), _T('d'), _T('e'), _T('f') };

    template <typename T, typename Enable = void>
    struct FloatingPointHelper;

public:
    FormatBuffer() noexcept {
        clear();
    }

    template <typename... Types>
    FormatBuffer(LPCTSTR fmt, Types... args) noexcept {
        print(fmt, args...);
    }

    template <typename... Types>
    void print(LPCTSTR fmt, Types... args) noexcept {
        print(m_buffer, fmt, args...);
    }

    void clear() noexcept {
        m_buffer[0] = _T('\0');
    }

    template <SIZE_T N, typename... Types>
    static void print(TCHAR (&buf)[N], LPCTSTR fmt, Types... args) noexcept
        requires (N > 0) {
        TCHAR fill = _T(' ');
        SIZE_T i = 0;
        SIZE_T width = 0;
        [[maybe_unused]] const auto copyFromString = [&buf, &i](LPCTSTR str) noexcept -> void {
            do {
                buf[i++] = *str++;
            } while (*str != _T('\0') && i < N - 1);
        };
        const auto copyFromFormat = [&buf, &fmt, &i, &fill, &width]() noexcept -> bool {
            do {
                if (*fmt == _T('%') && *++fmt != _T('%')) {
                    fill = *fmt == _T('0') ? *fmt++ : _T(' ');
                    if (*fmt >= _T('1') && *fmt <= _T('9')) {
                        width = static_cast<SIZE_T>(*fmt++ - _T('0'));
                        while (*fmt >= _T('0') && *fmt <= _T('9')) {
                            width = width * 10 + static_cast<SIZE_T>(*fmt++ - _T('0'));
                        }
                    } else {
                        width = 0;
                    }
                    return true;
                }
                buf[i++] = *fmt++;
            } while (*fmt != _T('\0') && i < N - 1);
            return false;
        };
        (
            [&]<typename T>(T value) noexcept -> void {
                if (*fmt != _T('\0') && i < N - 1) {
                    const bool foundSpec = copyFromFormat();
                    if (foundSpec != false && *fmt != _T('\0')) {
                        const TCHAR spec = *fmt++;
                        if constexpr (std::is_same_v<T, INT64>) {
                            if (spec == _T('b')) {
                                TCHAR num[128];
                                toBase2<UINT64>(num, static_cast<std::make_unsigned_t<T>>(value), fill, width);
                                copyFromString(num);
                            } else if (spec == _T('d')) {
                                TCHAR num[128];
                                toBase10<INT64>(num, value, fill, width);
                                copyFromString(num);
                            } else if (spec == _T('x')) {
                                TCHAR num[128];
                                toBase16<UINT64>(num, static_cast<std::make_unsigned_t<T>>(value), fill, width);
                                copyFromString(num);
                            }
                        } else if constexpr (std::is_same_v<T, INT32> ||
                                             std::is_same_v<T, INT16> ||
                                             std::is_same_v<T, INT8>) {
                            if (spec == _T('b')) {
                                TCHAR num[128];
                                toBase2<UINT32>(num, static_cast<std::make_unsigned_t<T>>(value), fill, width);
                                copyFromString(num);
                            } else if (spec == _T('d')) {
                                TCHAR num[128];
                                toBase10<INT32>(num, value, fill, width);
                                copyFromString(num);
                            } else if (spec == _T('x')) {
                                TCHAR num[128];
                                toBase16<UINT32>(num, static_cast<std::make_unsigned_t<T>>(value), fill, width);
                                copyFromString(num);
                            }
                        } else if constexpr (std::is_same_v<T, UINT64>) {
                            if (spec == _T('b')) {
                                TCHAR num[128];
                                toBase2<UINT64>(num, value, fill, width);
                                copyFromString(num);
                            } else if (spec == _T('u')) {
                                TCHAR num[128];
                                toBase10<UINT64>(num, value, fill, width);
                                copyFromString(num);
                            } else if (spec == _T('x')) {
                                TCHAR num[128];
                                toBase16<UINT64>(num, value, fill, width);
                                copyFromString(num);
                            }
                        } else if constexpr (std::is_same_v<T, UINT32> ||
                                             std::is_same_v<T, UINT16> ||
                                             std::is_same_v<T, UINT8>) {
                            if (spec == _T('b')) {
                                TCHAR num[128];
                                toBase2<UINT32>(num, value, fill, width);
                                copyFromString(num);
                            } else if (spec == _T('u')) {
                                TCHAR num[128];
                                toBase10<UINT32>(num, value, fill, width);
                                copyFromString(num);
                            } else if (spec == _T('x')) {
                                TCHAR num[128];
                                toBase16<UINT32>(num, value, fill, width);
                                copyFromString(num);
                            }
                        } else if constexpr (std::is_same_v<T, DOUBLE> ||
                                             std::is_same_v<T, FLOAT>) {
                            if (spec == _T('a')) {
                                TCHAR num[32];
                                toHexadecimalFloatingPoint<T>(num, value);
                                copyFromString(num);
                            }
                        } else if constexpr (std::is_same_v<T, LPCTSTR> ||
                                             std::is_same_v<T, LPTSTR>) {
                            if (spec == _T('s')) {
                                copyFromString(value);
                            }
                        } else if constexpr (std::is_same_v<T, TCHAR>) {
                            if (spec == _T('c')) {
                                buf[i++] = value;
                            }
                        } else {
                            static_assert(std::is_same_v<T, void>);
                        }
                    }
                }
            }(args),
            ...);
        while (*fmt != _T('\0') && i < N - 1) {
            const bool foundSpec = copyFromFormat();
            if (foundSpec != false && *fmt != _T('\0')) {
                buf[i++] = *fmt++;
            }
        }
        buf[i] = _T('\0');
    }

    LPCTSTR c_str() const noexcept {
        return m_buffer;
    }

private:
    static void reverse(LPTSTR buf, SIZE_T length) noexcept {
        for (SIZE_T i = 0, j = length - 1; i < j; ++i, --j) {
            const TCHAR tmp = buf[i];
            buf[i] = buf[j];
            buf[j] = tmp;
        }
    }

    template <typename T, SIZE_T N>
    static void toBase2(TCHAR (&buf)[N], T value, TCHAR fill = _T(' '), SIZE_T width = 0) noexcept
        requires (N > 64 && std::is_integral_v<T> && std::is_unsigned_v<T>) {
        SIZE_T len = 0;
        do {
            buf[len++] = static_cast<TCHAR>(value & 1) + _T('0');
        } while ((value >>= 1) != 0);
        if (width > N - 1) {
            width = N - 1;
        }
        if (width > len) {
            SIZE_T count = width - len;
            do {
                buf[len++] = fill;
            } while (--count > 0);
        }
        reverse(buf, len);
        buf[len] = _T('\0');
    }

    template <typename T, SIZE_T N>
    static void toBase10(TCHAR (&buf)[N], T value, TCHAR fill = _T(' '), SIZE_T width = 0) noexcept
        requires (N > 20 && std::is_integral_v<T>) {
        SIZE_T len = 0;
        const auto reduce = [&buf, &len](std::make_unsigned_t<T> num) -> void {
            do {
                buf[len++] = static_cast<TCHAR>(num % 10) + _T('0');
            } while ((num /= 10) != 0);
        };
        if constexpr (std::is_signed_v<T>) {
            const T mask = value >> (sizeof(value) * 8 - 1);
            reduce((value + mask) ^ mask);
            buf[len] = _T('-');
            len += static_cast<SIZE_T>(mask & 1);
        } else if constexpr (std::is_unsigned_v<T>) {
            reduce(value);
        } else {
            static_assert(std::is_same_v<T, void>);
        }
        if (width > N - 1) {
            width = N - 1;
        }
        if (width > len) {
            SIZE_T count = width - len;
            do {
                buf[len++] = fill;
            } while (--count > 0);
        }
        reverse(buf, len);
        buf[len] = _T('\0');
    }

    template <typename T, SIZE_T N>
    static void toBase16(TCHAR (&buf)[N], T value, TCHAR fill = _T(' '), SIZE_T width = 0) noexcept
        requires (N > 16 && std::is_integral_v<T> && std::is_unsigned_v<T>) {
        SIZE_T len = 0;
        do {
            buf[len++] = kCharMap[value & 15];
        } while ((value >>= 4) != 0);
        if (width > N - 1) {
            width = N - 1;
        }
        if (width > len) {
            SIZE_T count = width - len;
            do {
                buf[len++] = fill;
            } while (--count > 0);
        }
        reverse(buf, len);
        buf[len] = _T('\0');
    }

    template <typename T, SIZE_T N>
    static void toHexadecimalFloatingPoint(TCHAR (&buf)[N], T number) noexcept
        requires (N > 24 && std::is_floating_point_v<T>) {
        using Helper = FloatingPointHelper<T>;
        const auto sign = Helper::sign(number);
        const auto exponent = Helper::exponent(number);
        const auto fraction = Helper::fraction(number);
        SIZE_T len = 0;
        const auto copyFromString = [&buf, &len](LPCTSTR str) noexcept -> void {
            do {
                buf[len++] = *str++;
            } while (*str != _T('\0'));
        };
        const auto copyBase16 = [&buf, &len]<typename T>(T value) noexcept -> void {
            do {
                buf[len++] = kCharMap[value & 15];
            } while ((value >>= 4) != 0);
        };
        const auto copyBase10 = [&buf, &len]<typename T>(T value) noexcept -> void {
            do {
                buf[len++] = static_cast<TCHAR>(value % 10) + _T('0');
            } while ((value /= 10) != 0);
        };
        const auto appendZeros = []<typename T>(T fraction) noexcept -> T {
            while ((fraction & Helper::kAppendZeroMask) == 0) {
                fraction <<= 4;
            }
            return fraction;
        };
        const auto removeZeros = []<typename T>(T fraction) noexcept -> T {
            while ((fraction & 15) == 0) {
                fraction >>= 4;
            }
            return fraction;
        };
        if (exponent == 0) {
            if (fraction == 0) {
                copyFromString(_T("-0x0p0") + sign);
            } else {
                copyFromString(_T("-0x0.") + sign);
                copyBase16.template operator()(appendZeros.template operator()(fraction));
                copyFromString(_T("p-"));
                const SIZE_T off = len;
                copyBase10.template operator()(Helper::kExponentBias - 1);
                reverse(&buf[off], len - off);
            }
        } else if (exponent == Helper::kExponentMax) {
            if (fraction == 0) {
                copyFromString(_T("-inf") + sign);
            } else {
                copyFromString(_T("-nan") + sign);
            }
        } else {
            if (fraction == 0) {
                copyFromString(_T("-0x1") + sign);
            } else {
                copyFromString(_T("-0x1.") + sign);
                const SIZE_T off = len;
                copyBase16.template operator()(removeZeros.template operator()(fraction));
                reverse(&buf[off], len - off);
            }
            if (exponent < Helper::kExponentBias) {
                copyFromString(_T("p-"));
                const SIZE_T off = len;
                copyBase10.template operator()(Helper::kExponentBias - exponent);
                reverse(&buf[off], len - off);
            } else {
                copyFromString(_T("p"));
                const SIZE_T off = len;
                copyBase10.template operator()(exponent - Helper::kExponentBias);
                reverse(&buf[off], len - off);
            }
        }
        buf[len] = _T('\0');
    }
};

template <SIZE_T M>
template <typename T>
struct FormatBuffer<M>::FloatingPointHelper<T, std::enable_if_t<std::is_same_v<T, FLOAT>>> {
    static constexpr UINT32 kExponentMax = 255ul;
    static constexpr UINT32 kExponentBias = 127ul;
    static constexpr UINT32 kAppendZeroMask = 15728640ul;

    static UINT32 sign(FLOAT number) noexcept {
        const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&number);
        return 1ul - ((bytes[3] & 0x80ul) >> 7);
    }

    static UINT32 exponent(FLOAT number) noexcept {
        const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&number);
        return (static_cast<UINT32>(bytes[2]) & 0x80ul) >> 7 |
               (static_cast<UINT32>(bytes[3]) & 0x7ful) << 1;
    }

    static UINT32 fraction(FLOAT number) noexcept {
        const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&number);
        return static_cast<UINT32>(bytes[0]) << 1 |
               static_cast<UINT32>(bytes[1]) << 9 |
               (static_cast<UINT32>(bytes[2]) & 0x7ful) << 17;
    }
};

template <SIZE_T M>
template <typename T>
struct FormatBuffer<M>::FloatingPointHelper<T, std::enable_if_t<std::is_same_v<T, DOUBLE>>> {
    static constexpr UINT32 kExponentMax = 2047ul;
    static constexpr UINT32 kExponentBias = 1023ul;
    static constexpr UINT64 kAppendZeroMask = 4222124650659840ull;

    static UINT32 sign(DOUBLE number) noexcept {
        const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&number);
        return 1ul - ((bytes[7] & 0x80ul) >> 7);
    }

    static UINT32 exponent(DOUBLE number) noexcept {
        const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&number);
        return (static_cast<UINT32>(bytes[6]) & 0xf0ul) >> 4 |
               (static_cast<UINT32>(bytes[7]) & 0x7ful) << 4;
    }

    static UINT64 fraction(DOUBLE number) noexcept {
        const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&number);
        return static_cast<UINT64>(bytes[0]) |
               static_cast<UINT64>(bytes[1]) << 8 |
               static_cast<UINT64>(bytes[2]) << 16 |
               static_cast<UINT64>(bytes[3]) << 24 |
               static_cast<UINT64>(bytes[4]) << 32 |
               static_cast<UINT64>(bytes[5]) << 40 |
               (static_cast<UINT64>(bytes[6]) & 0x0full) << 48;
    }
};

template <SIZE_T N, typename... Types>
void format(TCHAR (&buf)[N], LPCTSTR fmt, Types... args) noexcept {
    FormatBuffer<N>::print(buf, fmt, args...);
}
