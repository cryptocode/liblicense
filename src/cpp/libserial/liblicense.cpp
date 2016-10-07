#include "liblicense.h"
#include "sha1.h"
#include <functional>

std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> v;
    auto i = 0;
    auto pos = s.find(delim);

    while (pos != std::string::npos)
    {
        v.push_back(s.substr(i, pos-i));
        i = ++pos;
        pos = s.find(delim, pos);

        if (pos == std::string::npos)
            v.push_back(s.substr(i, s.length()));
    }

    return v;
}

LibLicense::Verification LibLicense::verify(std::string licensekey, std::function<std::string(std::string)> subkeyFunc)
{
    // Step 0: Basic formatting
    std::size_t lastdash = licensekey.find_last_of("-");

    bool prefixed = licensekey.find_first_of(":") != std::string::npos;

    if (licensekey.size() < 16 ||
        lastdash == std::string::npos ||
        licensekey.front() == '-' ||
        licensekey.back() == '-')
    {
        return Verification::FormatFailure;
    }

    // First groups is the seed, last one checksum. When removed, we're left with actual subkeys.
    std::vector<std::string> subkeys = prefixed ? split(split(licensekey, ':').at(1), '-') : split(licensekey, '-');

    int groupsize = -1;
    for (auto& subkey : subkeys)
    {
        int len = subkey.length();

        if (groupsize != -1 && len != groupsize)
        {
            return Verification::FormatFailure;
        }

       groupsize = len;
    }

    subkeys.pop_back();
    std::string seed = subkeys.at(0);
    subkeys.erase(subkeys.begin());

    // Step 1: The checksum must be correct

    // Everything except the checksum (including the last dash)
    std::string keyNoCheckSum = licensekey.substr(0, lastdash+1);

    // The checksum in the license key
    std::string checksum = licensekey.substr(lastdash+1);

    // Compute checksum
    std::string computedCheckSum = sha1(keyNoCheckSum).substr(0,groupsize);

    if (checksum.compare(computedCheckSum) != 0)
    {
        return Verification::ChecksumFailure;
    }

    // Step 2: Match at least one subkey
    auto match = sha1(subkeyFunc(seed)).substr(0,groupsize);
    for (auto& subkey : subkeys)
    {
        if (subkey.compare(match) == 0)
            return Verification::Success;
    }

    return Verification::KeyMatchFailure;
}

std::string LibLicense::generate(std::string licenseHolderInfo, bool prefixed,
                                 int groupsize, std::vector<std::function<std::string(std::string)>> funcs)
{
    std::string key = "";

    std::string prefix_sha = sha1(licenseHolderInfo);
    std::string seed = prefix_sha.substr(0,groupsize);

    for (std::function<std::string(std::string)>&  func : funcs)
    {
        key += sha1(func(seed)).substr(0,groupsize) + "-";
    }

    std::string finalKey;
    if (prefixed)
        finalKey = licenseHolderInfo + ":";

    finalKey += seed + "-" + key;

    // Add checksum. This includes any prefix so the license holder information
    // cannot be spoofed.
    finalKey += sha1(finalKey).substr(0,groupsize);

    return finalKey;
}
