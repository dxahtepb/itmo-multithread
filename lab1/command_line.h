#pragma once
#include <unordered_map>
#include <algorithm>

struct Options {
    Options() = default;

    [[maybe_unused]]
    std::string get_option(const std::string& name) const {
        return params_.at(name);
    }

    [[maybe_unused]]
    std::string get_option_or_default(const std::string& name, const std::string& default_) const {
        auto element = params_.find(name);
        return (element != params_.end()) ? element->second : default_;
    }

    [[maybe_unused]]
    int get_int_option(const std::string& name) const {
        return std::stoi(params_.at(name));
    }

    [[maybe_unused]]
    int get_int_option_or_default(const std::string& name, int default_) const {
        auto element = params_.find(name);
        return (element != params_.end()) ? std::stoi(element->second) : default_;
    }

    [[maybe_unused]]
    Options& add_option(std::string option, std::string value) {
        params_.emplace(std::move(option), std::move(value));
        return *this;
    }

private:
    std::unordered_map<std::string, std::string> params_;
};

struct CommandLineArguments {
    CommandLineArguments() = default;

    CommandLineArguments(int argc, char* argv[]) : params_vec_{argv, argv + argc} {
        //empty
    }

    std::string at(size_t idx) const {
        return params_vec_.at(idx);
    }

    Options get_options() const {
        Options options{};
        auto iterator = std::find(params_vec_.begin(), params_vec_.end(), "--options");
        if (iterator != params_vec_.end() && ++iterator != params_vec_.end()) {
            for (; iterator != params_vec_.end(); ++iterator) {
                auto name = *iterator;
                if (++iterator == params_vec_.end()) {
                    break;
                }
                auto value = *iterator;
                options.add_option(name, value);
            }
        }
        return options;
    }

private:
    std::vector<std::string> params_vec_;
};
