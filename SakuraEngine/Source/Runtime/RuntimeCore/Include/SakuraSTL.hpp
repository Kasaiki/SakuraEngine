﻿/*
 * @CopyRight: MIT License
 * Copyright (c) 2020 SaeruHikari
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THESOFTWARE.
 * 
 * 
 * @Description: stl containers of sakura engine
 * @Version: 0.1.0
 * @Autor: SaeruHikari
 * @Date: 2020-08-04 10:11:48
 * @LastEditTime: 2020-08-10 00:06:26
 */ 
#pragma once
#include "Base/Definations.h"
#include "RuntimeCore/Memory.h"
#include <spdlog/fmt/fmt.h>
#include <chrono>
#include <string>
#include <string_view>
#include <any>
#include <variant>
#include <array>
#include <unordered_map>
#include <gsl/span>

namespace sakura
{
    template <typename ElementType, std::size_t Extent = gsl::dynamic_extent>
    using span = gsl::span<ElementType, Extent>;

    using u8string = std::string;
    using u8string_view = std::string_view;

    template <class _Kty, class _Ty, class _Hasher = std::hash<_Kty>, class _Keyeq = std::equal_to<_Kty>,
        class _Alloc = std::allocator<std::pair<const _Kty, _Ty>>>
    using unordered_map = std::unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>;

	template<typename T>
    using vector = std::vector<T>;
	
    template <class _Ty, class _Dx = std::default_delete<_Ty>>
    using unique_ptr = std::unique_ptr<_Ty, _Dx>;

    using string_view = std::string_view;

    template <class... _Types>
    using variant = std::variant<_Types...>;

	template<class _Ty, size_t _Size>
    using array = std::array<_Ty, _Size>;

    template<class _Ty1, class _Ty2>
    using pair = std::pair<_Ty1, _Ty2>;

	template<class _Fty>
    using function = std::function<_Fty>;

    using std::make_pair;
    using std::move;
    using std::forward;
    using std::to_string;
    using std::to_wstring;
}
using char8_t = char;

#ifdef SAKURA_TARGET_PLATFORM_WIN
#include <memory_resource>
namespace sakura
{
    using string = std::pmr::string;
    using wstring = std::pmr::wstring;
}
#else
namespace sakura
{
    using string = std::string;
    using wstring = std::wstring;
}
#endif

#include <ctime>

namespace sakura
{
    using time_t = std::time_t;
    using nullptr_t = std::nullptr_t;
    using ostream = std::ostream;
    using exception = std::exception;

	template<typename... Ts>
    using initializer_list = std::initializer_list<Ts...>;

	template<typename T>
    using atomic = std::atomic<T>;
}

namespace sakura
{
    template<typename string_type>
	void string_replace(
        string_type& src,
        const string_type& to_replace, const string_type& replacement)
	{
        typename string_type::size_type pos = 0;
        typename string_type::size_type srclen = to_replace.size();
        typename string_type::size_type dstlen = replacement.size();

		while ((pos = src.find(to_replace, pos)) != string_type::npos)
		{
            src.replace(pos, srclen, replacement);
			pos += dstlen;
		}
	}

    template <typename T, uint32_t size>
    struct storage
    {
        alignas(T) char c[size * sizeof(T)];
    };
    template <typename T>
    struct storage<T, 0>
    {};
}

namespace fmt
{
/*
    template <> struct formatter<sakura::u8string> : fmt::formatter<std::string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto format(sakura::u8string c, FormatContext& ctx) {
            string_view name = (const char*)c.c_str();
            return formatter<string_view>::format(name, ctx);
        }
    };
*/
    template <> struct formatter<sakura::double4> : fmt::formatter<std::string_view> {
        // parse is inherited from formatter<string_view>.
        template <typename FormatContext>
        auto format(sakura::double4 d4, FormatContext& ctx) {
            return fmt::format_to(
                ctx.out(),
                "({0:.4f}, {0:.4f}, {0:.4f}, {0:.4f})", d4.x, d4.y, d4.z, d4.w
            );
        }
    };
	
}

#include <iostream>

namespace std
{
    //std::string_view RuntimeCoreAPI to_string_view(const sakura::u8string& s) noexcept;
    std::string_view RuntimeCoreAPI to_string_view(const sakura::string& s) noexcept;
    std::wstring_view RuntimeCoreAPI to_string_view(const sakura::wstring& s) noexcept;
}

namespace sakura
{
    template<typename Flag, typename... Flags>
    constexpr decltype(auto) Or(Flag&& flag, Flags&&... flags)
    {
        return std::forward<Flag>(flag) || std::forward<Flags...>(flags...);
    }

    template<typename Flag, typename... Flags>
    constexpr decltype(auto) And(Flag&& flag, Flags&&... flags)
    {
        return std::forward<Flag>(flag) && std::forward<Flags...>(flags...);
    }

    template<typename Arg0, typename Arg1>
    constexpr decltype(auto) max(Arg0&& arg0, Arg1&& arg1)
    {
        return std::max(
            std::forward<Arg0>(arg0), std::forward<Arg1>(arg1));
    }
    template<typename Arg0, typename Arg1, typename... Args>
    constexpr decltype(auto) max(Arg0&& arg0, Arg1&& arg1, Args&&... args)
    {
        return sakura::max(
            sakura::max(std::forward<Arg0>(arg0), std::forward<Arg1>(arg1)),
            std::forward<Args...>(args...)
        );
    }
	
    template<typename Arg0, typename Arg1>
    constexpr decltype(auto) min(Arg0&& arg0, Arg1&& arg1)
    {
        return std::min(
            std::forward<Arg0>(arg0), std::forward<Arg1>(arg1));
    }
    template<typename Arg0, typename Arg1, typename... Args>
    constexpr decltype(auto) min(Arg0&& arg0, Arg1&& arg1, Args&&... args)
    {
        return sakura::min(
            sakura::min(std::forward<Arg0>(arg0), std::forward<Arg1>(arg1)),
            std::forward<Args...>(args...)
        );
    }

    namespace detail
    {

        template<typename R, typename ...As>
        struct __function_traits_base {
            using function_type = std::function<R(As...)>;

            using result_type = R;

            using argument_types = std::tuple<As...>;
        };

        template<typename F>
        struct __function_traits;
        template<typename F>
        struct __function_traits<std::reference_wrapper<F>> : public __function_traits<F> {};
        template<typename R, typename ...As>
        struct __function_traits<R(*)(As...)> : public __function_traits_base<R, As...> {};
        template<typename R, typename C, typename ...As>
        struct __function_traits<R(C::*)(As...)> : public __function_traits_base<R, As...> {};
        template<typename R, typename C, typename ...As>
        struct __function_traits<R(C::*)(As...) const> : public __function_traits_base<R, As...> {};
        template<typename F>
        struct __function_traits : public __function_traits<decltype(&F::operator())> {};


        template <int ind, typename... T>
        class helper {
        public:
            static void set_tuple(std::tuple<T...>& t, const std::vector<std::any>& v) {
                std::get<ind>(t) = std::any_cast<typename std::tuple_element<ind, std::tuple<T...> >::type>(v[ind]);
                helper<(ind - 1), T...>::set_tuple(t, v);
            }
        };

        template <typename... T>
        class helper<0, T...> {
        public:
            static void set_tuple(std::tuple<T...>& t, const std::vector<std::any>& v) {
                std::get<0>(t) = std::any_cast<typename std::tuple_element<0, std::tuple<T...> >::type>(v[0]);
            }
        };
    }

    template<typename F>
    struct function_traits : public detail::__function_traits<std::decay_t<F>> {};

    template<typename Invoker, typename F, size_t ...i, typename T>
    auto call_function(Invoker invoker, F f, std::index_sequence<i...>, T&& t) {
        return (invoker.*f)(std::get<i>(t)...);
    }

    template<typename Invoker, typename F, typename... T>
    auto call_function(Invoker invoker, F f, const std::tuple<T...>& t) {
        return call_function(invoker, f, std::make_index_sequence<sizeof...(T)>(), t);
    }

    template <typename... T>
    std::tuple<T...> create_tuple(const std::vector<std::any>& init) {
        std::tuple<T...> res;
        detail::helper<sizeof...(T) - 1, T...>::set_tuple(res, init);
        return res;
    }

    template <class, template <class, class...> class>
    struct is_instance_of : public std::false_type {};
    template <class...Ts, template <class, class...> class U>
    struct is_instance_of<U<Ts...>, U> : public std::true_type {};
    template <typename C, template <class, class...> class T>
    inline constexpr bool is_instance_of_v = is_instance_of<C, T>::value;

    template<class T, typename U>
    constexpr std::ptrdiff_t member_offset(U T::* member)
    {
        return reinterpret_cast<std::ptrdiff_t>(
            &(reinterpret_cast<T const volatile*>(0)->*member)
        );
    }
    template<typename T>
    struct declaring_type;
    template<class T, typename U>
    struct declaring_type<U T::*>{using type = T;};

    template <class F> struct function_view;
	template<typename Ret, typename... Args>
	struct function_view_base
	{
        constexpr static size_t args_count = sizeof...(Args);
        using ret_t = Ret;
        template <size_t N>
        struct arg
        {
            using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
        };
        template <size_t N> using arg_t = typename arg<N>::type;
		
	};
    template <typename Ret, typename... Args>
    struct function_view<Ret(*)(Args...)> : public function_view_base<Ret, Args...>
	{
        
    };
    template <typename Ret, typename Cls, typename... Args>
    struct function_view<Ret(Cls::*)(Args...)> : public function_view_base<Ret, Args...>
	{
        using declaring_t = std::decay_t<Cls>;
    };
    template <typename Ret, typename Cls, typename... Args>
    using method_view = function_view<Ret(Cls::*)(Args...)>;
    
    template <class T, std::size_t = sizeof(T)>
    std::true_type is_complete_impl(T *);
    std::false_type is_complete_impl(...);
    template <class T>
    using is_complete = decltype(is_complete_impl(std::declval<T*>()));
    template <class T>
    inline constexpr bool is_complete_v = is_complete<T>::value;

    namespace detail
    {
        template <typename T, std::size_t ... Is>
        constexpr std::array<T, sizeof...(Is)>
        create_array(T value, std::index_sequence<Is...>)
        {
            // cast Is to void to remove the warning: unused value
            return {{(static_cast<void>(Is), value)...}};
        }
    }

    template <typename T, std::size_t N>
    constexpr std::array<T, N> create_array(T&& value)
    {
        return detail::create_array(std::forward<T>(value), std::make_index_sequence<N>());
    }
}
