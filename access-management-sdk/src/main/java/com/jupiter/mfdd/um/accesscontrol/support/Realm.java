package com.jupiter.mfdd.um.accesscontrol.support;

import org.apache.commons.lang3.StringUtils;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;

public class Realm {
    public static final String REALM_SEPARATOR = "/";
    private static final int MAX_REALM_SEPARATOR = 2;

    private String realm;
    private String partRealm;
    private String partSubrealm;

    public Realm(String realm) throws AuthXException {
        validate(realm);

        this.realm = realm;

        if (this.realm.equals(REALM_SEPARATOR))
            partRealm = "";
        else if (this.realm.lastIndexOf(REALM_SEPARATOR) == 0)
            partRealm = this.realm.substring(1);
        else {
            int i = this.realm.lastIndexOf(REALM_SEPARATOR, 1);

            partRealm = this.realm.substring(1, i);
            partSubrealm = this.realm.substring(i + 1);
        }
    }

    public String getRealm() {
        return realm;
    }

    public String getPartRealm() {
        return partRealm;
    }

    public String getPartSubrealm() {
        return partSubrealm;
    }

    public static void validate(String realm) throws AuthXException {
        String exceptionMessage = "Invalid realm <" + realm + ">.";

        if (StringUtils.isEmpty(realm))
            throw new AuthXException(exceptionMessage);

        if (!realm.startsWith(REALM_SEPARATOR))
            throw new AuthXException(exceptionMessage);

        if (StringUtils.countMatches(realm, REALM_SEPARATOR) > MAX_REALM_SEPARATOR)
            throw new AuthXException(exceptionMessage);

        if ((realm.length() > REALM_SEPARATOR.length())
                && (realm.endsWith(REALM_SEPARATOR)))
            throw new AuthXException(exceptionMessage);
    }
}
