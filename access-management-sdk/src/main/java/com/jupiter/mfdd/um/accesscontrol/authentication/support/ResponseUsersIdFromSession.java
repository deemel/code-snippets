package com.jupiter.mfdd.um.accesscontrol.authentication.support;

import javax.naming.InvalidNameException;
import javax.naming.ldap.LdapName;

public class ResponseUsersIdFromSession {
    private String id;
    private String realm;
    private String dn;
    private String successURL;
    private String fullLoginURL;
    private LdapName ldapName;

    public String getId() {
        return id;
    }

    public String getRealm() {
        return realm;
    }

    public String getDn() {
        return dn;
    }

    public String getSuccessURL() {
        return successURL;
    }

    public String getFullLoginURL() {
        return fullLoginURL;
    }

    public void setId(String id) {
        this.id = id;
    }

    public void setRealm(String realm) {
        this.realm = realm;
    }

    public void setDn(String dn) {
        this.dn = dn;
    }

    public void setSuccessURL(String successURL) {
        this.successURL = successURL;
    }

    public void setFullLoginURL(String fullLoginURL) {
        this.fullLoginURL = fullLoginURL;
    }

    public LdapName getLdapName() throws InvalidNameException {
        if (ldapName == null)
            ldapName = new LdapName(dn);

        return ldapName;
    }

    public String toString() {
        String ldapName = null;
        try {
            ldapName = getLdapName().toString();
        } catch (InvalidNameException e) {
        }

        return "id <" + id + ">, realm <" + realm + ">, dn <" + dn
                + ">, successURL <" + successURL + ">, fullLoginURL <"
                + fullLoginURL + ">, ldapName <" + ldapName + ">";
    }
}
