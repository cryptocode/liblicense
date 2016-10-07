package com.liblicense;

@FunctionalInterface
public interface VerifierFunction
{
    String verify(String subkey);
}
