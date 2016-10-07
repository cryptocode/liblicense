package com.liblicense;

import java.security.MessageDigest;
import java.util.ArrayList;
import java.util.Arrays;

/**
 * LibLicense implementation for Java
 */
public class LibLicense
{
    public enum Verification
    {
        Success,
        FormatFailure,
        ChecksumFailure,
        KeyMatchFailure
    };

    private String sha1(String input)
    {
        try
        {
            MessageDigest digest = MessageDigest.getInstance("SHA-1");
            digest.update(input.getBytes("utf8"));
            byte[] digestBytes = digest.digest();
            return javax.xml.bind.DatatypeConverter.printHexBinary(digestBytes).toLowerCase();
        }
        catch (Exception ex)
        {
            return null;
        }
    }

    /**
     * Verifies the license key
     * @param licensekey License key. It may be a prefixed or non-prefixed key.
     * @param subkeyVerifier
     * @return
     */
    public Verification verify(String licensekey, VerifierFunction subkeyVerifier)
    {
        int lastdash = licensekey.lastIndexOf('-');
        boolean prefixed = licensekey.lastIndexOf(':') != -1;

        if (licensekey.length() < 16 ||
            lastdash == -1 ||
            licensekey.charAt(0) == '-' ||
            licensekey.charAt(licensekey.length()-1) == '-')
        {
            return Verification.FormatFailure;
        }

        ArrayList<String> subkeys = new ArrayList<>(10);
        String subkeyString = licensekey;
        if (prefixed)
        {
            subkeyString = licensekey.split(":")[1];
        }

        subkeys.addAll(Arrays.asList(subkeyString.split("-")));

        // All subkeys must have the same length
        int groupsize = -1;
        for (String subkey : subkeys)
        {
            int len = subkey.length();

            if (groupsize != -1 && len != groupsize)
                return Verification.FormatFailure;

            groupsize = len;
        }

        // First group is seed, last one checksum. When removed, we're left with actual subkeys.
        subkeys.remove(subkeys.size()-1);
        String seed = subkeys.remove(0);

        // Step 1: The checksum must be correct
        String keyNoCheckSum = licensekey.substring(0, lastdash+1); //include the dash in checksum
        String checksum = licensekey.substring(lastdash+1);
        String computedCheckSum = sha1(keyNoCheckSum).substring(0,groupsize);

        if (!checksum.equals(computedCheckSum))
            return Verification.ChecksumFailure;

        // Step 2: Match at least one subkey
        String match = sha1(subkeyVerifier.verify(seed)).substring(0,groupsize);
        for (String subkey : subkeys)
        {
            if (subkey.equals(match))
                return Verification.Success;
        }

        return Verification.KeyMatchFailure;
    }
}
