package com.liblicense;

public class Main {

    public static void main(String[] args)
    {
        LibLicense license = new LibLicense();
        LibLicense.Verification res = license.verify(
                "asdfasdfasdf:79437f-44a77a-6b1d0b-0a599a-d43eb5-de169d-18bd64-da39a3-05a923",
                seed -> "_"+seed+"_");

        if (res == LibLicense.Verification.ChecksumFailure)
            System.out.println("Checksum failed");
        else if (res == LibLicense.Verification.FormatFailure)
            System.out.println("Invalid format");
        else if ( res == LibLicense.Verification.KeyMatchFailure)
            System.out.println("Invalid subkey");
        else
            System.out.println("Successfully verified");
    }
}
