package com.jupiter.boss.sdk.accesscontrol.authorization;

import java.util.Enumeration;
import java.util.Locale;
import java.util.ResourceBundle;

public class AuthXResponseInternationalizationTest {

    public static void displayValues(ResourceBundle bundle) {
        Enumeration<String> keys = bundle.getKeys();
        while (keys.hasMoreElements()) {
            String key = keys.nextElement();
            displayResponses(bundle, key);
        }
    }

    private static void displayResponses(ResourceBundle bundle, String key) {
        String[] responses = bundle.getString(key).split(":");
        int i = 0;
        for (String response : responses) {
            System.out.println((i == 0 ? "Reason:" : "Message:") + response);
            i++;
        }
    }

    public static void main(String[] args) {
        System.out.println("1");
        ResourceBundle bundle1 = ResourceBundle
                .getBundle("com.jupiter.boss.sdk.accesscontrol.authorization.AuthXResponseCodeBundle");
        displayValues(bundle1);

        System.out.println("2");
        Locale defaultLocale = Locale.getDefault();
        ResourceBundle bundle2 = ResourceBundle
                .getBundle(
                        "com.jupiter.boss.sdk.accesscontrol.authorization.AuthXResponseCodeBundle",
                        defaultLocale);
        displayValues(bundle2);

        System.out.println("3");
        Locale swedishLocale = new Locale("sv", "SE");
        ResourceBundle bundle3 = ResourceBundle
                .getBundle(
                        "com.jupiter.boss.sdk.accesscontrol.authorization.AuthXResponseCodeBundle",
                        swedishLocale);
        displayValues(bundle3);

        System.out.println("4");
        Locale nonexistentLocale = new Locale("xx", "XX");
        ResourceBundle bundle4 = ResourceBundle
                .getBundle(
                        "com.jupiter.boss.sdk.accesscontrol.authorization.AuthXResponseCodeBundle",
                        nonexistentLocale);
        displayValues(bundle4);
    }

}
