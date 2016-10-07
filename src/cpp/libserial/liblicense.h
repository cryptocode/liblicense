#ifndef LIBLICENSE_H
#define LIBLICENSE_H

#include <string>
#include <vector>
#include <functional>

/**
 * @brief LibLicense implements a partial license key verification
 *
 * LICENSE KEY FORMAT
 *
 *  prefix:xxxxxx-xxxxxx-xxxxxx-xxxxxx-...-xxxxxx-xxxxxx-xxxxxx
 *  |_____||____| |____________________________________| |____|
 *     |       |                      |                   |
 *  OPTIONAL  SEED                 N SUBKEYS             CHECKSUM
 *   UTF8                          OF SIZE X
 *  PREFIX
 *
 * Textually, a license is an optional prefix followed by a colon, followed by
 * N groups of X hex digits separated by dashes.
 *
 * The key, including the prefix, is case sensitive. That is, changing case on
 * one or more characters will cause checksum verification to fail.
 *
 * The size of N and X are chosen by the developer and N basically sets the limit
 * for the number of changes to which subkey is checked (which is done to prevent
 * keygen analysis). The size of X decides the level of protection against brute force
 * analysis.
 *
 * If X=40, the brute force requirement is equal to finding a SHA1 collision
 * for all active subkeys. An *active* subkey is a subkey which is checked
 * by the application.
 *
 * The prefix, typically license holder information, can be displayed
 * in GUI's and may include newlines and special characters, except colon.
 *
 * Sample license keys with various prefixes, group sizes and number of subkeys:
 *
 *    bf0ccd-18facb-ceaac1-fd111d-b145ff-a008ce
 *
 *    name@thecompany.com:af0cdd-98fa8b-dec0c1-f044ec-a9714b
 *
 *    Mr User;User Street 492;New York, USA;name@thecompany.com;Order-17149:4f0c11-922a5c-138542-1e1ffd-f014e1-bb8147
 *
 *    name@thecompany.com:147e759436310c5d-a833084b3af9e85d-31d90c417f1f55e0-37e97801df3f006b-23e6f345c70edfcf-7398de574c6770af-3e699590953ae2b6-828eb3d101f54fe3-da39a3ee5e6b4b0d-9561749b30128e94
 *
 * PREFIXING, SEEDS AND CHECKSUMS
 *
 * The checksum depends on the seed and subkeys. For prefixed license keys,
 * the checksum also depends on the prefix value.
 *
 * This means that if the license key was generated with the prefix option,
 * it is not possible to remove or change the prefix in the license key.
 *
 * The checksum ensures the integrity of the license key. Without it,
 * and attacker could brute force each subkey. With the checksum, any
 * modification to any part of the license key will render it useless.
 *
 * As the seed is a sha1 of the prefix, it's possible to verify whether or not
 * a license key is based on a certain prefix, even for non-prefixed keys.
 *
 * To discourage fraudulent license key purchases, consider requiring identifying
 * information, such as a combination of verified email address, hashed credit card
 * information and name/company/address.
 *
 * The prefix can encode license expiration as well, which cannot be changed
 * by the user without invalidating the checksum. For instance, the following
 * key encodes the fact that the user has received a demo license lasting
 * until Jan 1 2017:
 *
 *    joe@thecompany.com;DEMO;2017/1/1:ffd41a-d11a77-c4ffaa-1f58da-ab2f4c
 *
 * The application simply reads this information and checks against the date,
 * after making sure the checksum/subkey(s) are valid.
 *
 * LICENSE KEY STRENGTH
 *
 * The most secure license scheme is to "call home" to a server and ask for
 * credentials and see if there's an associated license available.
 *
 * However, this fails when the network is down, requires a server
 * infrastructure and an activation support function.
 *
 * A local-only solution is robust and user friendly, but prone to cracking.
 * This is because the algorithm to check license keys is embedded in the
 * executable which can thus be reverse engineered.
 *
 * PLKV makes it infeasible to reverse engineer the license key generator,
 * without gaining access to the actual key generator (which is kept secret)
 * since the application only checks parts of the license key. That is,
 * the key generation scheme cannot be deduced even if the attacker gets
 * hold of the source code for a specific version of the software.
 *
 * By associating user-identity to the license key, publishing keys are
 * discouraged - the identifying information cannot be changed without
 * invalidating the checksum.
 *
 * DISTRIBUTION AND DISPLAY
 *
 * The license key can be distributed in a file, via mail or any other
 * means, as long as the formatting is pristine. For instance, if the
 * prefix include newlines, it is prone to translation to CR-LF pairs
 * on Windows. To keep license checks robust, newlines should be replaced
 * with a special character such as ; which is then replaced with a
 * newline for display purposes.
 *
 * PARTIAL HASH STRUCTURE:
 *
 * The first group is the first X bytes of the hex representation of the
 * SHA1 hash of the license prefix. This also defines the minimum licenseholder
 * prefix length.
 *
 * This group is called the SEED.
 *
 * The next N groups are algorithmically chosen SUBKEYS, usually based on at least
 * the SEED, but also potentially other information.
 *
 * These subkeys are checked by the application. Different versions of the
 * application should check different subkeys to prevent keygen analysis.
 *
 * What algorithm is used to produce the subkeys is entirely up to the
 * application. The input should typically be at least the seed. Other possible
 * inputs are checksums of binaries that are included with each
 * release (but changes between releases), secret keys stored in protected
 * OS-level keychains, and so on.
 *
 * This setup means that old license keys will work in new versions of programs, as
 * long as the subkey algorithm(s) are unchanged. Once the subkey checks are exhausted,
 * the application can choose to 1) restart on the first subkey or 2) introduce
 * new algorithms. The last option would render the old license keys invalid, and should
 * only be done on major-version upgrades.
 *
 * The last group is the first X bytes of the hex-SHA1 of the entire non-checksummed key,
 * forming a checksum.
 *
 * VERFICATION:
 *
 * The checksum group should be checked at application startup. This makes sure the
 * license key is well formed and consistent.
 *
 * At application-dependent places in the application, the subkey check(s) occurs.
 * Not placing the subkey check at startup further complicates reverse engineering efforts.
 */
class LibLicense
{
public:

    enum Verification
    {
        Success,
        FormatFailure,
        ChecksumFailure,
        KeyMatchFailure
    };

    /**
     * @brief verify license key
     * @param licensekey License key, prefixed or non-prefixed.
     * @param subkeyVerifier Subkey verifier. Call verify(...) multiple times if you want to check multiple subkeys.
     * @return Verification value
     */
    Verification verify(std::string licensekey, std::function<std::string(std::string)> subkeyVerifier);


    /**
     * @brief generate license key
     * @param prefix Prefix (such as license holder info). At least 6 chars. Any printable character except ':' is allowed.
     * @param funcs List of functions to produce subkeys of at least 3 bytes. At least 3 functions must be supplied
     * (include only one in user binary)
     * @return License key, or empty string if input is invalid (such as too short prefix)
     */
    std::string generate(std::string prefix, bool prefixed, int groupsize, std::vector<std::function<std::string(std::string)>> subkeyGenerators);
};

#endif // LIBLICENSE_H
