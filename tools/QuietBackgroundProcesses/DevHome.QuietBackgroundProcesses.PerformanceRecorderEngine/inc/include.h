#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

std::vector<std::wstring> split_string(std::wstring const & str, std::wstring const & delimiter = L",")
{
    std::vector<std::wstring> values;
    size_t pos = 0;
    size_t prevPos = 0;
    std::wstring token;
    while (true)
    {
        pos = str.find(delimiter, prevPos);
        if (pos == std::wstring::npos)
        {
            token = str.substr(prevPos);
            values.push_back(token);
            break;
        }
        token = str.substr(prevPos, pos - prevPos);
        values.push_back(token);
        std::wcout << token << std::endl;
        prevPos = pos + 1;
    }
    return values;
}

std::vector<std::wstring> parse_args(int argc, wchar_t** argv)
{
    std::vector<std::wstring> args;
    for (auto const & arg : wil::make_range(argv, argc))
    {
        args.push_back(arg);
    }
    return args;
}

std::set<std::wstring> parse_arg_switches(std::vector<std::wstring> args, bool q = false)
{
    std::set<std::wstring> switches;
    for (const auto & arg : args)
    {
        if (arg.substr(0,1) == L"-")
        {
            switches.insert(arg);
            if (!q)
            {
                std::wcout << L"arg switch: " << arg << std::endl;
            }
        }
    }
    return switches;
}

std::vector<std::wstring> parse_arg_strings(std::vector<std::wstring> args, bool q = false)
{
    std::vector<std::wstring> argstrings;
    for (const auto & arg : args)
    {
        if (arg.substr(0,1) != L"-")
        {
            argstrings.push_back(arg);
            if (!q)
            {
                std::wcout << L"arg string: " << arg << std::endl;
            }
        }
    }
    return argstrings;
}

std::map<std::wstring, std::wstring> parse_arg_strings_as_map(std::vector<std::wstring> args, bool q = false)
{
    std::optional<std::wstring> argname;

    std::map<std::wstring, std::wstring> argmap;

    auto argsAfterFirst = wil::make_range(std::next(args.begin()), args.end()); // gross way to skip first arg, which is the .exe
    for (const auto & arg : argsAfterFirst)
    {
        if (arg.substr(0,1) == L"-")
        {
            argname = arg;
        }
        else
        {
            if (argname)
            {
                argmap.insert(std::make_pair(argname.value(), arg));
                if (!q)
                {
                    std::wcout << L"arg pair: " << argname.value() << L" = " << arg << std::endl;
                }
            }
            argname = std::nullopt;
        }
    }
    return argmap;
}

template <typename T>
std::optional<T> try_get_named_arg_as(std::map<std::wstring, std::wstring> const & argmap, std::wstring const & argumentname);

template <>
std::optional<std::wstring> try_get_named_arg_as<std::wstring>(std::map<std::wstring, std::wstring> const & argmap, std::wstring const & argumentname)
{
    if (argmap.find(argumentname) == argmap.end())
    {
        return std::nullopt;
    }
    return argmap.at(argumentname);
}

template <>
std::optional<int> try_get_named_arg_as<int>(std::map<std::wstring, std::wstring> const & argmap, std::wstring const & argumentname)
{
    if (auto str = try_get_named_arg_as<std::wstring>(argmap, argumentname))
    {
        return _wtoi(str.value().c_str());
    }
    return std::nullopt;
}

std::optional<std::wstring> try_get_named_arg(std::map<std::wstring, std::wstring> const & argmap, std::wstring const & argumentname)
{
    return try_get_named_arg_as<std::wstring>(argmap, argumentname);
}

std::optional<std::vector<std::wstring>> try_get_named_array_arg(std::map<std::wstring, std::wstring> & argmap, std::wstring const & argumentname)
{
    auto value = try_get_named_arg(argmap, argumentname);
    if (!value)
    {
        return std::nullopt;
    }
    return split_string(value.value(), L",");
}
